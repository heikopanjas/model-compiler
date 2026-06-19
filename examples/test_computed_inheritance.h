#ifndef __GENERATED_TEST_COMPUTED_INHERITANCE_H_INCL__
#define __GENERATED_TEST_COMPUTED_INHERITANCE_H_INCL__

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

/// \brief Shape struct
struct Shape : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Shape()
        : width_(*this),
          height_(*this),
          area_(*this, [](const Shape& parent) { return parent.width_.value_ * parent.height_.value_; })
    {
    }

    /// \brief Virtual destructor
    virtual ~Shape() = default;

    // User-defined fields
    runtime::UnboundedValue<int64_t, Shape> width_;
    runtime::UnboundedValue<int64_t, Shape> height_;
    runtime::DynamicValue<int64_t, Shape> area_;
};

/// \brief ColoredShape struct
struct ColoredShape : public Shape
{
    /// \brief Constructor - initializes all fields with parent reference
    ColoredShape()
        : depth_(*this),
          volume_(*this, [](const ColoredShape& parent) { return parent.width_.value_ * parent.height_.value_ * parent.depth_.value_; }),
          doubleArea_(*this, [](const ColoredShape& parent) { return parent.area_ * 2; })
    {
    }

    /// \brief Virtual destructor
    virtual ~ColoredShape() = default;

    // User-defined fields
    runtime::UnboundedValue<int64_t, ColoredShape> depth_;
    runtime::DynamicValue<int64_t, ColoredShape> volume_;
    runtime::DynamicValue<int64_t, ColoredShape> doubleArea_;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATED_TEST_COMPUTED_INHERITANCE_H_INCL__
