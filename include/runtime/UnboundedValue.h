#ifndef __RUNTIME_UNBOUNDED_VALUE_H_INCL__
#define __RUNTIME_UNBOUNDED_VALUE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

namespace runtime {
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
    using value_type = T;

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
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_UNBOUNDED_VALUE_H_INCL__
