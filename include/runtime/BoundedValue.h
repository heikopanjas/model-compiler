#ifndef __RUNTIME_BOUNDED_VALUE_H_INCL__
#define __RUNTIME_BOUNDED_VALUE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/AliasValue.h"
#include "runtime/DynamicValue.h"
#include "runtime/OptionalBoundedValue.h"
#include "runtime/OptionalUnboundedValue.h"
#include "runtime/UnboundedValue.h"

#include <stdexcept>

namespace runtime {
/// \brief Wrapper for required fields with invariant constraints (with compile-time checker binding)
///
/// Template parameters:
/// - T: The value type
/// - ParentT: The containing class type
/// - Checkers: Variadic list of static checker function pointers
///
/// Stores a value of type T and validates invariants on assignment.
/// Checker functions are bound at compile-time via template parameters.
/// Updates parent object's modificationDate on success.
template<typename T, typename ParentT, auto... Checkers> class BoundedValue
{
public:
    using value_type = T;

    /// \brief Construct with parent reference
    /// \param parent Reference to the containing parent object
    explicit BoundedValue(ParentT& parent) : parent_(parent), value_() {}

    /// \brief Construct with parent reference and initial value
    /// \param parent Reference to the containing parent object
    /// \param value Initial value
    BoundedValue(ParentT& parent, const T& value) : parent_(parent), value_(value) {}

    /// \brief Assignment operator - validates invariants before assigning
    /// \param value The new value to assign
    /// \return Reference to this wrapper
    /// \throws std::invalid_argument if any invariant is violated
    BoundedValue& operator=(const T& value)
    {
        if constexpr (sizeof...(Checkers) > 0)
        {
            // Call all checker functions and ensure all return true
            const bool allValid = (Checkers(parent_, value) && ...);
            if (allValid == false)
            {
                throw std::invalid_argument("Invariant validation failed");
            }
        }

        // All checks passed - assign value and update modification date
        value_ = value;
        parent_.UpdateModificationDate();
        return *this;
    }

    /// \brief Implicit conversion to T for reading
    /// \return The wrapped value
    operator const T&() const
    {
        return value_;
    }

    /// \brief Explicit get method
    /// \return The wrapped value
    const T& Get() const
    {
        return value_;
    }

    // Allow direct access to value for internal use (e.g., checker temporary modifications)
    T value_;

private:
    ParentT& parent_;
};
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_BOUNDED_VALUE_H_INCL__
