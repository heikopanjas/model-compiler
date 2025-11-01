#ifndef __COMMON_H_INCL__
#define __COMMON_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

/// \brief Macro to suppress unused parameter warnings
///
/// Use this macro to explicitly mark function parameters that are intentionally
/// unused. This prevents compiler warnings while documenting the intent.
/// Works across MSVC, GCC, and Clang.
#define UNREFERENCED_PARAMETER(param) (void)(param)

// Restore previous alignment
#pragma pack(pop)

#endif // __COMMON_H_INCL__
