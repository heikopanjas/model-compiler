#include "TypeSymbol.h"

#include "runtime/AST.h"

TypeSymbol::TypeSymbol() : kind(Kind::PRIMITIVE), name(), enumDecl(nullptr), classDecl(nullptr) {}

TypeSymbol::TypeSymbol(const runtime::String& typeName) : kind(Kind::PRIMITIVE), name(typeName), enumDecl(nullptr), classDecl(nullptr) {}

TypeSymbol::TypeSymbol(const runtime::EnumDeclaration* enumDeclaration) :
    kind(Kind::ENUM), name(enumDeclaration->GetName()), enumDecl(enumDeclaration), classDecl(nullptr)
{
}

TypeSymbol::TypeSymbol(const runtime::ClassDeclaration* classDeclaration) :
    kind(Kind::CLASS), name(classDeclaration->GetName()), enumDecl(nullptr), classDecl(classDeclaration)
{
}
