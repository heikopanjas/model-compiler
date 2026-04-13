#ifndef __BBFM_GENERATED_TEST_COMPUTED_SIMPLE_H_INCL__
#define __BBFM_GENERATED_TEST_COMPUTED_SIMPLE_H_INCL__

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

/// \brief Rectangle struct
struct Rectangle : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Rectangle()
        : width_(*this),
          height_(*this),
          area_(*this, [](const Rectangle& parent) { return parent.width_.value_ * parent.height_.value_; }),
          perimeter_(*this, [](const Rectangle& parent) { return (parent.width_.value_ + parent.height_.value_) * 2; })
    {
    }

    /// \brief Virtual destructor
    virtual ~Rectangle() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<int64_t, Rectangle> width_;
    bbfm::runtime::UnboundedValue<int64_t, Rectangle> height_;
    bbfm::runtime::DynamicValue<int64_t, Rectangle> area_;
    bbfm::runtime::DynamicValue<int64_t, Rectangle> perimeter_;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_GENERATED_TEST_COMPUTED_SIMPLE_H_INCL__
