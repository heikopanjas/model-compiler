#ifndef __RUNTIME_DYNAMIC_VALUE_H_INCL__
#define __RUNTIME_DYNAMIC_VALUE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <functional>
#include <utility>

namespace runtime {
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
    using value_type  = T;

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
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_DYNAMIC_VALUE_H_INCL__
