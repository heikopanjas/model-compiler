#ifndef __GENERATOR_LANGUAGE_INFO_H_INCL__
#define __GENERATOR_LANGUAGE_INFO_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <filesystem>
#include "runtime/String.h"

/// \brief Public metadata for a loaded generator plugin
struct GeneratorLanguageInfo
{
    runtime::String       languageId;
    runtime::String       displayName;
    runtime::String       fileExtension;
    bool                  experimental;
    std::filesystem::path path;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATOR_LANGUAGE_INFO_H_INCL__
