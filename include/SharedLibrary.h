#ifndef __SHARED_LIBRARY_H_INCL__
#define __SHARED_LIBRARY_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <filesystem>
#include "runtime/String.h"

/// \brief RAII wrapper for platform shared-library loading
class SharedLibrary
{
public:
    /// \brief Default constructor
    SharedLibrary();

    SharedLibrary(const SharedLibrary& other)            = delete;
    SharedLibrary& operator=(const SharedLibrary& other) = delete;

    /// \brief Move constructor
    /// \param other The library handle to move from
    SharedLibrary(SharedLibrary&& other) noexcept;

    /// \brief Move assignment operator
    /// \param other The library handle to move from
    /// \return Reference to this library
    SharedLibrary& operator=(SharedLibrary&& other) noexcept;

    /// \brief Destructor
    virtual ~SharedLibrary();

    /// \brief Load a shared library from disk
    /// \param path Path to the shared library
    /// \return True if loading succeeded
    bool Load(const std::filesystem::path& path);

    /// \brief Unload the current shared library
    void Unload();

    /// \brief Release ownership of the loaded handle without unloading it
    void Release();

    /// \brief Check whether a library is loaded
    /// \return True if a handle is loaded
    bool IsLoaded() const;

    /// \brief Look up a symbol in the loaded library
    /// \param symbolName The exported symbol name
    /// \return Symbol address, or nullptr if missing
    void* GetSymbol(const char* symbolName) const;

    /// \brief Get the last loader error message
    /// \return Human-readable error message
    const runtime::String& GetLastError() const;

private:
    void*       handle_;
    runtime::String lastError_;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __SHARED_LIBRARY_H_INCL__
