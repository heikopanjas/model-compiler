#include "SharedLibrary.h"

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

#include <utility>

namespace {
#if defined(_WIN32)
runtime::String GetWindowsErrorMessage()
{
    const DWORD errorCode = GetLastError();
    if (errorCode == 0)
    {
        return {};
    }

    char*       messageBuffer = nullptr;
    const DWORD size          = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);

    runtime::String message;
    if (size > 0 && nullptr != messageBuffer)
    {
        message.SetValue(std::string(messageBuffer, size));
        LocalFree(messageBuffer);
    }

    return message;
}
#endif
} // namespace

SharedLibrary::SharedLibrary() : handle_(nullptr), lastError_() {}

SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept : handle_(other.handle_), lastError_(std::move(other.lastError_))
{
    other.handle_ = nullptr;
}

SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept
{
    if (this != &other)
    {
        Unload();
        handle_       = other.handle_;
        lastError_    = std::move(other.lastError_);
        other.handle_ = nullptr;
    }
    return *this;
}

SharedLibrary::~SharedLibrary()
{
    Unload();
}

bool SharedLibrary::Load(const std::filesystem::path& path)
{
    Unload();

#if defined(_WIN32)
    handle_ = LoadLibraryW(path.wstring().c_str());
    if (nullptr == handle_)
    {
        lastError_ = GetWindowsErrorMessage();
        return false;
    }
#else
    dlerror();
    handle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (nullptr == handle_)
    {
        const char* error = dlerror();
        lastError_        = (nullptr != error) ? error : "unknown shared library load error";
        return false;
    }
#endif

    lastError_.SetValue("");
    return true;
}

void SharedLibrary::Unload()
{
    if (nullptr == handle_)
    {
        return;
    }

#if defined(_WIN32)
    FreeLibrary(static_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif

    handle_ = nullptr;
}

void SharedLibrary::Release()
{
    handle_ = nullptr;
    lastError_.SetValue("");
}

bool SharedLibrary::IsLoaded() const
{
    return nullptr != handle_;
}

void* SharedLibrary::GetSymbol(const char* symbolName) const
{
    if (nullptr == handle_ || nullptr == symbolName)
    {
        return nullptr;
    }

#if defined(_WIN32)
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle_), symbolName));
#else
    dlerror();
    void*       symbol = dlsym(handle_, symbolName);
    const char* error  = dlerror();
    return (nullptr == error) ? symbol : nullptr;
#endif
}

const runtime::String& SharedLibrary::GetLastError() const
{
    return lastError_;
}
