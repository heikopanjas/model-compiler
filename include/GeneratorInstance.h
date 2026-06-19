#ifndef __GENERATOR_INSTANCE_H_INCL__
#define __GENERATOR_INSTANCE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "GeneratorPlugin.h"
#include "runtime/ICodeGenerator.h"

/// \brief RAII handle for a generator created by a plugin
class GeneratorInstance
{
public:
    /// \brief Default constructor
    GeneratorInstance();

    /// \brief Construct from generator pointer and destroy callback
    /// \param generator Generator instance from the plugin
    /// \param destroyGenerator Plugin destroy callback
    GeneratorInstance(runtime::ICodeGenerator* generator, DestroyGeneratorFunction destroyGenerator);

    GeneratorInstance(const GeneratorInstance& other)            = delete;
    GeneratorInstance& operator=(const GeneratorInstance& other) = delete;

    /// \brief Move constructor
    /// \param other The instance to move from
    GeneratorInstance(GeneratorInstance&& other) noexcept;

    /// \brief Move assignment operator
    /// \param other The instance to move from
    /// \return Reference to this instance
    GeneratorInstance& operator=(GeneratorInstance&& other) noexcept;

    /// \brief Destructor
    virtual ~GeneratorInstance();

    /// \brief Check whether the instance holds a generator
    /// \return True when a generator is present
    bool IsValid() const;

    /// \brief Access the generator
    /// \return Generator pointer
    runtime::ICodeGenerator* operator->() const;

    /// \brief Access the generator
    /// \return Generator pointer
    runtime::ICodeGenerator* Get() const;

private:
    void Reset();

    runtime::ICodeGenerator*          generator_;
    DestroyGeneratorFunction destroyGenerator_;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATOR_INSTANCE_H_INCL__
