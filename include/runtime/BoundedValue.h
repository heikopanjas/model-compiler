#ifndef __BBFM_RUNTIME_BOUNDED_VALUE_H_INCL__
#define __BBFM_RUNTIME_BOUNDED_VALUE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

namespace bbfm { namespace runtime {
// Forward declaration
class Fabric;

/// \brief Wrapper for computed/derived fields (dynamically calculated values)
///
/// Template parameters:
/// - T: The computed value type
/// - ParentT: The containing class type
///
/// Stores a computation function (lambda) and evaluates it on access.
/// The value is computed on-demand each time it is accessed.
/// Access computed fields directly (parent.field_) - implicit conversion applies.
template<typename T, typename ParentT> class DynamicValue
{
public:
    using ComputeFunc = std::function<T(const ParentT&)>;

    /// \brief Construct with parent reference and computation function
    /// \param parent Reference to the containing parent object
    /// \param computeFunc Lambda or function that computes the value
    DynamicValue(ParentT& parent, ComputeFunc computeFunc) : parent_(parent), computeFunc_(std::move(computeFunc)) {}

    /// \brief Implicit conversion to T - computes value on access
    /// \return The computed value
    operator T() const
    {
        return computeFunc_(parent_);
    }

    /// \brief Explicit get method - computes value on access
    /// \return The computed value
    T Get() const
    {
        return computeFunc_(parent_);
    }

private:
    ParentT&    parent_;
    ComputeFunc computeFunc_;
};

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
            bool allValid = (Checkers(parent_, value) && ...);
            if (!allValid)
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

/// \brief Wrapper for required fields without invariant constraints
///
/// Template parameters:
/// - T: The value type
/// - ParentT: The containing class type
///
/// Stores a value of type T and updates parent modificationDate on assignment.
template<typename T, typename ParentT> class UnboundedValue
{
public:
    /// \brief Construct with parent reference
    /// \param parent Reference to the containing parent object
    explicit UnboundedValue(ParentT& parent) : parent_(parent), value_() {}

    /// \brief Construct with parent reference and initial value
    /// \param parent Reference to the containing parent object
    /// \param value Initial value
    UnboundedValue(ParentT& parent, const T& value) : parent_(parent), value_(value) {}

    /// \brief Assignment operator - updates modification date
    /// \param value The new value to assign
    /// \return Reference to this wrapper
    UnboundedValue& operator=(const T& value)
    {
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

    // Allow direct access to value
    T value_;

private:
    ParentT& parent_;
};

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
            bool allValid = (Checkers(parent_, value) && ...);
            if (!allValid)
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

/// \brief Wrapper for optional fields without invariant constraints
///
/// Template parameters:
/// - T: The value type
/// - ParentT: The containing class type
///
/// Stores an optional value of type T and updates parent modificationDate on assignment.
template<typename T, typename ParentT> class OptionalUnboundedValue
{
public:
    /// \brief Construct with parent reference - no value
    /// \param parent Reference to the containing parent object
    explicit OptionalUnboundedValue(ParentT& parent) : parent_(parent), value_() {}

    /// \brief Construct with parent reference and initial value
    /// \param parent Reference to the containing parent object
    /// \param value Initial value
    OptionalUnboundedValue(ParentT& parent, const T& value) : parent_(parent), value_(value) {}

    /// \brief Assignment operator - updates modification date
    /// \param value The new value to assign
    /// \return Reference to this wrapper
    OptionalUnboundedValue& operator=(const T& value)
    {
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
}} // namespace bbfm::runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_RUNTIME_BOUNDED_VALUE_H_INCL__
