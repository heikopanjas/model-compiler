#ifndef __GENERATED_TEST_COMPUTED_MEMBER_ACCESS_H_INCL__
#define __GENERATED_TEST_COMPUTED_MEMBER_ACCESS_H_INCL__

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

/// \brief Point struct
struct Point : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Point()
        : x_(*this),
          y_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Point() = default;

    // User-defined fields
    runtime::UnboundedValue<int64_t, Point> x_;
    runtime::UnboundedValue<int64_t, Point> y_;
};

/// \brief Rectangle struct
struct Rectangle : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Rectangle()
        : topLeft_(*this),
          bottomRight_(*this),
          width_(*this, [](const Rectangle& parent) { return parent.bottomRight_.value_.GetX() - parent.topLeft_.value_.GetX(); }),
          height_(*this, [](const Rectangle& parent) { return parent.bottomRight_.value_.GetY() - parent.topLeft_.value_.GetY(); }),
          area_(*this, [](const Rectangle& parent) { return parent.width_ * parent.height_; })
    {
    }

    /// \brief Virtual destructor
    virtual ~Rectangle() = default;

    // User-defined fields
    runtime::UnboundedValue<Point, Rectangle> topLeft_;
    runtime::UnboundedValue<Point, Rectangle> bottomRight_;
    runtime::DynamicValue<int64_t, Rectangle> width_;
    runtime::DynamicValue<int64_t, Rectangle> height_;
    runtime::DynamicValue<int64_t, Rectangle> area_;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATED_TEST_COMPUTED_MEMBER_ACCESS_H_INCL__
