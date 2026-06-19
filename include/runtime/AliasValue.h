#ifndef __RUNTIME_ALIAS_VALUE_H_INCL__
#define __RUNTIME_ALIAS_VALUE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <utility>

namespace runtime {
/// \brief Wrapper for alias fields (forwarding reads/writes to another field)
///
/// Template parameter:
/// - WrapperT: The type of the field wrapper being aliased (e.g., BoundedValue, UnboundedValue)
///
/// Stores a reference to the target field wrapper and forwards all operations to it.
/// Writing to an alias triggers the target field's invariant validation.
/// This provides read-write access to inherited or other class fields.
template<typename WrapperT> class AliasValue
{
public:
    /// \brief Construct with reference to target field wrapper
    /// \param target Reference to the field wrapper being aliased
    explicit AliasValue(WrapperT& target) : target_(target) {}

    /// \brief Implicit conversion - forwards to target's conversion
    /// \return The target's value
    operator auto() const
    {
        return static_cast<const typename WrapperT::value_type&>(target_);
    }

    /// \brief Assignment operator - forwards to target's assignment (triggers validation!)
    /// \param value The new value to assign
    /// \return Reference to this wrapper
    template<typename U> AliasValue& operator=(U&& value)
    {
        target_ = std::forward<U>(value);
        return *this;
    }

    /// \brief Explicit get method
    /// \return The target's value
    auto Get() const
    {
        return target_.Get();
    }

private:
    WrapperT& target_;
};
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_ALIAS_VALUE_H_INCL__
