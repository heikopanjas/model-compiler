#ifndef __GENERATOR_PLUGIN_MANAGER_H_INCL__
#define __GENERATOR_PLUGIN_MANAGER_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "GeneratorInstance.h"
#include "GeneratorLanguageInfo.h"
#include "GeneratorPlugin.h"
#include "runtime/AST.h"
#include "runtime/SemanticAnalyzer.h"
#include "SharedLibrary.h"

#include <filesystem>
#include "runtime/Array.h"
#include "runtime/String.h"

/// \brief Loads generator plugins and creates code generators by language
class GeneratorPluginManager
{
public:
    /// \brief Default constructor
    GeneratorPluginManager();

    /// \brief Add a directory to search for generator plugins
    /// \param directory Directory containing generator plugin shared libraries
    void AddSearchDirectory(const std::filesystem::path& directory);

    /// \brief Load generator plugins from configured search directories
    /// \return True if at least one generator plugin was loaded
    bool LoadPlugins();

    /// \brief Check whether a generator for a language is loaded
    /// \param language Language requested by the user, e.g. "c++", "rust", "swift"
    /// \return True if a matching generator is loaded
    bool HasLanguage(const runtime::String& language) const;

    /// \brief Get the display language name
    /// \param language Language requested by the user
    /// \return Display language name, or empty string if not found
    runtime::String GetDisplayName(const runtime::String& language) const;

    /// \brief Get the output file extension for a language
    /// \param language Language requested by the user
    /// \return File extension, or empty string if not found
    runtime::String GetFileExtension(const runtime::String& language) const;

    /// \brief Check whether a loaded language is marked experimental
    /// \param language Language requested by the user
    /// \return True if the language is experimental
    bool IsExperimental(const runtime::String& language) const;

    /// \brief Format all loaded languages for diagnostics
    /// \return Comma-separated display names
    runtime::String FormatSupportedLanguages() const;

    /// \brief Get metadata for all loaded generator languages
    /// \return Vector of loaded generator metadata
    runtime::Array<GeneratorLanguageInfo> GetLanguages() const;

    /// \brief Create a generator instance for a language
    /// \param language Language requested by the user
    /// \param ast Validated runtime::AST
    /// \param analyzer Semantic analyzer
    /// \param namespaces Combined namespace list
    /// \param classPrefix Class prefix
    /// \return Generator instance, or invalid instance if creation failed
    GeneratorInstance CreateGenerator(
        const runtime::String& language, const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::Array<runtime::String>& namespaces,
        const runtime::String& classPrefix) const;

private:
    struct Plugin
    {
        std::filesystem::path    path;
        SharedLibrary            library;
        runtime::String          languageId;
        runtime::String          displayName;
        runtime::String          fileExtension;
        bool                     experimental;
        CreateGeneratorFunction  createGenerator;
        DestroyGeneratorFunction destroyGenerator;
    };

    bool          LoadPlugin(const std::filesystem::path& path);
    const Plugin* FindPlugin(const runtime::String& language) const;

    runtime::Array<std::filesystem::path> searchDirectories_;
    runtime::Array<Plugin>                  plugins_;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATOR_PLUGIN_MANAGER_H_INCL__
