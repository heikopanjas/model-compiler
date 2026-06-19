#ifndef __GENERATED_TEST_NAMESPACE_H_INCL__
#define __GENERATED_TEST_NAMESPACE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <optional>
#include <functional>
#include "runtime/Fabric.h"
#include "runtime/BoundedValue.h"
#include "runtime/String.h"
#include "runtime/Date.h"
#include "runtime/Guid.h"

namespace models {

/// \brief Status enumeration
enum class Status
{
    ACTIVE,
    INACTIVE
};

/// \brief TestClass struct
struct TestClass : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    TestClass()
        : name_(*this),
          status_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~TestClass() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, TestClass> name_;
    runtime::UnboundedValue<models::Status, TestClass> status_;
};


} // namespace models
// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATED_TEST_NAMESPACE_H_INCL__
