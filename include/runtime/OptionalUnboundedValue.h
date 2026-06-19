#ifndef __RUNTIME_OPTIONAL_UNBOUNDED_VALUE_H_INCL__
#define __RUNTIME_OPTIONAL_UNBOUNDED_VALUE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <optional>

namespace runtime {
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
    using value_type = T;

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
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_OPTIONAL_UNBOUNDED_VALUE_H_INCL__
