#ifndef __TYPE_SYMBOL_H_INCL__
#define __TYPE_SYMBOL_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/String.h"

namespace runtime {
class ClassDeclaration;
class EnumDeclaration;
}


/// \brief Symbol table entry for a declared type
struct TypeSymbol
{
    enum class Kind
    {
        ENUM,
        CLASS,
        PRIMITIVE
    };

    Kind                    kind;
    runtime::String             name;
    const runtime::EnumDeclaration*  enumDecl;  // Non-null if kind == ENUM
    const runtime::ClassDeclaration* classDecl; // Non-null if kind == CLASS

    /// \brief Default constructor for dictionary empty values
    TypeSymbol();

    /// \brief Construct a primitive type symbol
    /// \param typeName The primitive type name
    explicit TypeSymbol(const runtime::String& typeName);

    /// \brief Construct an enum type symbol
    /// \param enumDeclaration Pointer to the enum declaration
    explicit TypeSymbol(const runtime::EnumDeclaration* enumDeclaration);

    /// \brief Construct a class type symbol
    /// \param classDeclaration Pointer to the class declaration
    explicit TypeSymbol(const runtime::ClassDeclaration* classDeclaration);
};

// Restore previous alignment
#pragma pack(pop)

#endif // __TYPE_SYMBOL_H_INCL__
