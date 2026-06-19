#include "GeneratorInstance.h"

GeneratorInstance::GeneratorInstance() : generator_(nullptr), destroyGenerator_(nullptr) {}

GeneratorInstance::GeneratorInstance(runtime::ICodeGenerator* generator, const DestroyGeneratorFunction destroyGenerator) :
    generator_(generator), destroyGenerator_(destroyGenerator)
{
}

GeneratorInstance::GeneratorInstance(GeneratorInstance&& other) noexcept : generator_(other.generator_), destroyGenerator_(other.destroyGenerator_)
{
    other.generator_        = nullptr;
    other.destroyGenerator_ = nullptr;
}

GeneratorInstance& GeneratorInstance::operator=(GeneratorInstance&& other) noexcept
{
    if (this != &other)
    {
        Reset();
        generator_              = other.generator_;
        destroyGenerator_       = other.destroyGenerator_;
        other.generator_        = nullptr;
        other.destroyGenerator_ = nullptr;
    }
    return *this;
}

GeneratorInstance::~GeneratorInstance()
{
    Reset();
}

bool GeneratorInstance::IsValid() const
{
    return nullptr != generator_;
}

runtime::ICodeGenerator* GeneratorInstance::operator->() const
{
    return generator_;
}

runtime::ICodeGenerator* GeneratorInstance::Get() const
{
    return generator_;
}

void GeneratorInstance::Reset()
{
    if (nullptr != generator_ && nullptr != destroyGenerator_)
    {
        destroyGenerator_(generator_);
    }

    generator_        = nullptr;
    destroyGenerator_ = nullptr;
}
