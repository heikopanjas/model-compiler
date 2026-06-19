#ifndef __RUNTIME_OPTIONAL_BOUNDED_VALUE_H_INCL__
#define __RUNTIME_OPTIONAL_BOUNDED_VALUE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <optional>
#include <stdexcept>

namespace runtime {
/// \brief Wrapper for optional fields with invariant constraints
///
/// Template parameters:
/// - T: The value type
/// - ParentT: The containing class type
/// - Checkers: Variadic list of static checker function pointers
///
/// Stores an optional value of type T and validates invariants on assignment.
/// Supports std::optional-like interface with has_value(), value(), etc.
template<typename T, typename ParentT, auto... Checkers> class OptionalBoundedValue
{
public:
    using value_type = T;

    /// \brief Construct with parent reference - no value
    /// \param parent Reference to the containing parent object
    explicit OptionalBoundedValue(ParentT& parent) : parent_(parent), value_() {}

    /// \brief Construct with parent reference and initial value
    /// \param parent Reference to the containing parent object
    /// \param value Initial value
    OptionalBoundedValue(ParentT& parent, const T& value) : parent_(parent), value_(value) {}

    /// \brief Assignment operator - validates invariants before assigning
    /// \param value The new value to assign
    /// \return Reference to this wrapper
    /// \throws std::invalid_argument if any invariant is violated
    OptionalBoundedValue& operator=(const T& value)
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

    /// \brief Check if value is present
    /// \return True if value is present
    bool has_value() const
    {
        return value_.has_value();
    }

    /// \brief Get the value (throws if not present)
    /// \return The wrapped value
    const T& value() const
    {
        return value_.value();
    }

    /// \brief Get the value (throws if not present)
    /// \return The wrapped value
    T& value()
    {
        return value_.value();
    }

    /// \brief Dereference operator
    /// \return The wrapped value
    const T& operator*() const
    {
        return *value_;
    }

    /// \brief Arrow operator
    /// \return Pointer to the wrapped value
    const T* operator->() const
    {
        return &(*value_);
    }

    /// \brief Reset to empty state
    void reset()
    {
        value_.reset();
        parent_.UpdateModificationDate();
    }

    // Allow direct access to optional value
    std::optional<T> value_;

private:
    ParentT& parent_;
};
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_OPTIONAL_BOUNDED_VALUE_H_INCL__
