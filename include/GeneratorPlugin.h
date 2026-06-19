#ifndef __GENERATOR_PLUGIN_H_INCL__
#define __GENERATOR_PLUGIN_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/AST.h"
#include "runtime/Dictionary.h"
#include "runtime/ICodeGenerator.h"
#include "runtime/SemanticAnalyzer.h"

#include <cstddef>

#if defined(_WIN32)
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

constexpr int GENERATOR_PLUGIN_API_VERSION = 1;

/// \brief C-compatible generator creation context
///
/// Exported plugin functions must not accept STL containers. The host provides
/// namespace values as a pointer/count pair and class prefix as a C string.
struct GeneratorCreateContext
{
    const runtime::AST*              ast;
    const runtime::SemanticAnalyzer* analyzer;
    const char* const*      namespaces;
    size_t                  namespaceCount;
    const char*             classPrefix;
};

using GetCapabilitiesFunction     = runtime::CapabilitiesDictionary* (*)();
using DestroyCapabilitiesFunction = void                (*)(runtime::CapabilitiesDictionary* capabilities);
using CreateGeneratorFunction     = runtime::ICodeGenerator*         (*)(const GeneratorCreateContext* context);
using DestroyGeneratorFunction    = void                   (*)(runtime::ICodeGenerator* generator);

const constexpr char* GET_CAPABILITIES_SYMBOL     = "GetCapabilities";
const constexpr char* DESTROY_CAPABILITIES_SYMBOL = "DestroyCapabilities";
const constexpr char* CREATE_GENERATOR_SYMBOL     = "CreateGenerator";
const constexpr char* DESTROY_GENERATOR_SYMBOL    = "DestroyGenerator";

// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATOR_PLUGIN_H_INCL__
