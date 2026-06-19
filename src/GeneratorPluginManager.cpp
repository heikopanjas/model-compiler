#include "GeneratorPluginManager.h"

#include "Console.h"

#include "runtime/Array.h"
#include "runtime/String.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <utility>

namespace {
runtime::String NormalizeLanguage(const runtime::String& language)
{
    std::string normalized;
    normalized.reserve(language.GetLength());

    for (const char ch : language.GetValue())
    {
        normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }

    const runtime::String result(normalized);
    if ("c++" == result || "cpp" == result)
    {
        return runtime::String("c++");
    }

    return result;
}

std::string GetLibraryExtension()
{
#if defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

bool IsSharedLibraryPath(const std::filesystem::path& path)
{
    return path.extension() == GetLibraryExtension();
}

runtime::String ValueOrEmpty(const runtime::CapabilitiesDictionary& dictionary, const char* key)
{
    const char* value = dictionary.GetValue(key);
    return (nullptr != value) ? runtime::String(value) : runtime::String();
}

bool StringToBool(const runtime::String& value)
{
    const runtime::String normalized = NormalizeLanguage(value);
    return "true" == normalized || "yes" == normalized || "1" == normalized;
}

runtime::String DefaultExtensionForLanguage(const runtime::String& languageId)
{
    if ("c++" == languageId)
    {
        return runtime::String(".h");
    }

    if ("rust" == languageId)
    {
        return runtime::String(".rs");
    }

    if ("swift" == languageId)
    {
        return runtime::String(".swift");
    }

    return runtime::String(".txt");
}

bool ContainsSearchDirectory(const runtime::Array<std::filesystem::path>& directories, const std::filesystem::path& directory)
{
    for (size_t i = 0; i < directories.GetCount(); ++i)
    {
        if (directories.GetValueAt(i) == directory)
        {
            return true;
        }
    }

    return false;
}
} // namespace

// --- GeneratorPluginManager --------------------------------------------------

GeneratorPluginManager::GeneratorPluginManager() : searchDirectories_(), plugins_() {}

void GeneratorPluginManager::AddSearchDirectory(const std::filesystem::path& directory)
{
    if (directory.empty() == true)
    {
        return;
    }

    const std::filesystem::path normalized = std::filesystem::absolute(directory);
    if (ContainsSearchDirectory(searchDirectories_, normalized) == false)
    {
        searchDirectories_.AddValue(normalized);
    }
}

bool GeneratorPluginManager::LoadPlugins()
{
    for (size_t dirIndex = 0; dirIndex < searchDirectories_.GetCount(); ++dirIndex)
    {
        const std::filesystem::path& directory = searchDirectories_.At(dirIndex);
        if (std::filesystem::is_directory(directory) == false)
        {
            continue;
        }

        runtime::Array<std::filesystem::path> candidatePaths;
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const std::filesystem::path path = entry.path();
            if (IsSharedLibraryPath(path) == true)
            {
                candidatePaths.AddValue(path);
            }
        }

        std::vector<std::filesystem::path> sortablePaths;
        for (size_t i = 0; i < candidatePaths.GetCount(); ++i)
        {
            sortablePaths.push_back(candidatePaths.GetValueAt(i));
        }
        std::sort(sortablePaths.begin(), sortablePaths.end());

        for (const auto& path : sortablePaths)
        {
            LoadPlugin(path);
        }
    }

    return plugins_.GetCount() > 0;
}

bool GeneratorPluginManager::HasLanguage(const runtime::String& language) const
{
    return nullptr != FindPlugin(language);
}

runtime::String GeneratorPluginManager::GetDisplayName(const runtime::String& language) const
{
    const Plugin* plugin = FindPlugin(language);
    return (nullptr != plugin) ? plugin->displayName : runtime::String();
}

runtime::String GeneratorPluginManager::GetFileExtension(const runtime::String& language) const
{
    const Plugin* plugin = FindPlugin(language);
    return (nullptr != plugin) ? plugin->fileExtension : runtime::String();
}

bool GeneratorPluginManager::IsExperimental(const runtime::String& language) const
{
    const Plugin* plugin = FindPlugin(language);
    return (nullptr != plugin) ? plugin->experimental : false;
}

runtime::String GeneratorPluginManager::FormatSupportedLanguages() const
{
    runtime::String result;

    for (size_t i = 0; i < plugins_.GetCount(); ++i)
    {
        if (i > 0)
        {
            result = result + runtime::String(", ");
        }
        result = result + plugins_.At(i).displayName;
    }

    return result;
}

runtime::Array<GeneratorLanguageInfo> GeneratorPluginManager::GetLanguages() const
{
    runtime::Array<GeneratorLanguageInfo> languages;

    for (size_t i = 0; i < plugins_.GetCount(); ++i)
    {
        const Plugin& plugin = plugins_.At(i);
        languages.AddValue({plugin.languageId, plugin.displayName, plugin.fileExtension, plugin.experimental, plugin.path});
    }

    return languages;
}

GeneratorInstance GeneratorPluginManager::CreateGenerator(
    const runtime::String& language, const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::Array<runtime::String>& namespaces,
    const runtime::String& classPrefix) const
{
    const Plugin* plugin = FindPlugin(language);
    if (nullptr == plugin || nullptr == plugin->createGenerator)
    {
        return {};
    }

    std::vector<const char*> namespacePointers;
    namespacePointers.reserve(namespaces.GetCount());
    for (size_t i = 0; i < namespaces.GetCount(); ++i)
    {
        namespacePointers.push_back(namespaces.GetValueAt(i).GetValue().c_str());
    }

    const GeneratorCreateContext context = {
        ast,
        analyzer,
        namespacePointers.empty() == false ? namespacePointers.data() : nullptr,
        namespacePointers.size(),
        classPrefix.GetValue().c_str(),
    };

    runtime::ICodeGenerator* generator = plugin->createGenerator(&context);
    if (nullptr == generator)
    {
        return {};
    }

    return GeneratorInstance(generator, plugin->destroyGenerator);
}

bool GeneratorPluginManager::LoadPlugin(const std::filesystem::path& path)
{
    SharedLibrary library;
    if (library.Load(path) == false)
    {
        Console::ReportStatus(runtime::String("WARNING: Could not load generator plugin '") + runtime::String(path.string()) + runtime::String("': ") + library.GetLastError());
        return false;
    }

    const auto getCapabilities = reinterpret_cast<GetCapabilitiesFunction>(library.GetSymbol(GET_CAPABILITIES_SYMBOL));
    if (nullptr == getCapabilities)
    {
        library.Release();
        return false;
    }

    const auto destroyCapabilities = reinterpret_cast<DestroyCapabilitiesFunction>(library.GetSymbol(DESTROY_CAPABILITIES_SYMBOL));
    const auto createGenerator     = reinterpret_cast<CreateGeneratorFunction>(library.GetSymbol(CREATE_GENERATOR_SYMBOL));
    const auto destroyGenerator    = reinterpret_cast<DestroyGeneratorFunction>(library.GetSymbol(DESTROY_GENERATOR_SYMBOL));

    if (nullptr == destroyCapabilities || nullptr == createGenerator || nullptr == destroyGenerator)
    {
        Console::ReportStatus(runtime::String("WARNING: Ignoring generator plugin '") + runtime::String(path.string()) + runtime::String("' because it does not export all required symbols."));
        return false;
    }

    runtime::CapabilitiesDictionary* capabilities = getCapabilities();
    if (nullptr == capabilities)
    {
        Console::ReportStatus(runtime::String("WARNING: Ignoring generator plugin '") + runtime::String(path.string()) + runtime::String("' because GetCapabilities returned null."));
        return false;
    }

    const runtime::String displayName = ValueOrEmpty(*capabilities, "Language");
    if (displayName.IsEmpty() == true)
    {
        destroyCapabilities(capabilities);
        Console::ReportStatus(runtime::String("WARNING: Ignoring generator plugin '") + runtime::String(path.string()) + runtime::String("' because it does not declare Language."));
        return false;
    }

    const runtime::String apiVersion = ValueOrEmpty(*capabilities, "ApiVersion");
    if (apiVersion != runtime::String(std::to_string(GENERATOR_PLUGIN_API_VERSION)))
    {
        destroyCapabilities(capabilities);
        Console::ReportStatus(runtime::String("WARNING: Ignoring generator plugin '") + runtime::String(path.string()) + runtime::String("' because its API version is unsupported."));
        return false;
    }

    const runtime::String languageId    = NormalizeLanguage(displayName);
    runtime::String       fileExtension = ValueOrEmpty(*capabilities, "FileExtension");
    if (fileExtension.IsEmpty() == true)
    {
        fileExtension = DefaultExtensionForLanguage(languageId);
    }

    const bool experimental = StringToBool(ValueOrEmpty(*capabilities, "Experimental"));
    destroyCapabilities(capabilities);

    if (nullptr != FindPlugin(languageId))
    {
        Console::ReportStatus(runtime::String("WARNING: Ignoring duplicate generator plugin for language '") + displayName + runtime::String("'."));
        return false;
    }

    Plugin plugin;
    plugin.path             = path;
    plugin.library          = std::move(library);
    plugin.languageId       = languageId;
    plugin.displayName      = displayName;
    plugin.fileExtension    = fileExtension;
    plugin.experimental     = experimental;
    plugin.createGenerator  = createGenerator;
    plugin.destroyGenerator = destroyGenerator;

    plugins_.AddValue(std::move(plugin));
    return true;
}

const GeneratorPluginManager::Plugin* GeneratorPluginManager::FindPlugin(const runtime::String& language) const
{
    const runtime::String normalized = NormalizeLanguage(language);
    for (size_t i = 0; i < plugins_.GetCount(); ++i)
    {
        const Plugin& plugin = plugins_.At(i);
        if (plugin.languageId == normalized)
        {
            return &plugin;
        }
    }

    return nullptr;
}
