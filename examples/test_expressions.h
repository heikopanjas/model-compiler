#ifndef __GENERATED_TEST_EXPRESSIONS_H_INCL__
#define __GENERATED_TEST_EXPRESSIONS_H_INCL__

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
    runtime::BoundedValue<int64_t, Point, &Point::Require_x_validX, &Point::Require_x_withinBounds> x_;
    runtime::BoundedValue<int64_t, Point, &Point::Require_y_validY, &Point::Require_y_withinBounds> y_;
    /// \brief Require invariant 'validX' for field 'x'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to x
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_x_validX(const Point& object, const int64_t& newValue)
    {
        return newValue >= 0;
    }

    /// \brief Require invariant 'withinBounds' for field 'x'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to x
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_x_withinBounds(const Point& object, const int64_t& newValue)
    {
        return newValue <= 10000 && object.y_.value_ <= 10000;
    }

    /// \brief Require invariant 'validY' for field 'y'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to y
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_y_validY(const Point& object, const int64_t& newValue)
    {
        return newValue >= 0;
    }

    /// \brief Require invariant 'withinBounds' for field 'y'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to y
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_y_withinBounds(const Point& object, const int64_t& newValue)
    {
        return object.x_.value_ <= 10000 && newValue <= 10000;
    }

};

/// \brief Shape struct
struct Shape : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Shape()
        : width_(*this),
          height_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Shape() = default;

    // User-defined fields
    runtime::BoundedValue<int64_t, Shape, &Shape::Require_width_positiveWidth, &Shape::Require_width_validRatio> width_;
    runtime::BoundedValue<int64_t, Shape, &Shape::Require_height_positiveHeight, &Shape::Require_height_validRatio> height_;
    /// \brief Require invariant 'positiveWidth' for field 'width'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to width
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_width_positiveWidth(const Shape& object, const int64_t& newValue)
    {
        return newValue > 0;
    }

    /// \brief Require invariant 'validRatio' for field 'width'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to width
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_width_validRatio(const Shape& object, const int64_t& newValue)
    {
        return newValue * object.height_.value_ >= 100;
    }

    /// \brief Require invariant 'positiveHeight' for field 'height'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to height
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_height_positiveHeight(const Shape& object, const int64_t& newValue)
    {
        return newValue > 0;
    }

    /// \brief Require invariant 'validRatio' for field 'height'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to height
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_height_validRatio(const Shape& object, const int64_t& newValue)
    {
        return object.width_.value_ * newValue >= 100;
    }

};

/// \brief Rectangle struct
struct Rectangle : public Shape
{
    /// \brief Constructor - initializes all fields with parent reference
    Rectangle()
        : topLeft_(*this),
          bottomRight_(*this),
          maxArea_(*this),
          computedWidth_(*this, [](const Rectangle& parent) { return parent.bottomRight_.value_.GetX() - parent.topLeft_.value_.GetX(); }),
          computedHeight_(*this, [](const Rectangle& parent) { return parent.bottomRight_.value_.GetY() - parent.topLeft_.value_.GetY(); }),
          computedArea_(*this, [](const Rectangle& parent) { return parent.computedWidth_ * parent.computedHeight_; })
    {
    }

    /// \brief Virtual destructor
    virtual ~Rectangle() = default;

    // User-defined fields
    runtime::UnboundedValue<Point, Rectangle> topLeft_;
    runtime::UnboundedValue<Point, Rectangle> bottomRight_;
    runtime::BoundedValue<int64_t, Rectangle, &Rectangle::Require_maxArea_validArea> maxArea_;
    runtime::DynamicValue<int64_t, Rectangle> computedWidth_;
    runtime::DynamicValue<int64_t, Rectangle> computedHeight_;
    runtime::DynamicValue<int64_t, Rectangle> computedArea_;
    /// \brief Require invariant 'validArea' for field 'maxArea'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to maxArea
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_maxArea_validArea(const Rectangle& object, const int64_t& newValue)
    {
        return object.width_.value_ * object.height_.value_ <= newValue;
    }

};

/// \brief Temperature struct
struct Temperature : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Temperature()
        : celsius_(*this),
          fahrenheit_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Temperature() = default;

    // User-defined fields
    runtime::BoundedValue<double, Temperature, &Temperature::Require_celsius_conversion, &Temperature::Require_celsius_aboveAbsoluteZero> celsius_;
    runtime::BoundedValue<double, Temperature, &Temperature::Require_fahrenheit_conversion> fahrenheit_;
    /// \brief Require invariant 'conversion' for field 'celsius'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to celsius
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_celsius_conversion(const Temperature& object, const double& newValue)
    {
        return object.fahrenheit_.value_ == newValue * 1.800000 + 32.000000;
    }

    /// \brief Require invariant 'aboveAbsoluteZero' for field 'celsius'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to celsius
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_celsius_aboveAbsoluteZero(const Temperature& object, const double& newValue)
    {
        return newValue >= -273.150000;
    }

    /// \brief Require invariant 'conversion' for field 'fahrenheit'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to fahrenheit
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_fahrenheit_conversion(const Temperature& object, const double& newValue)
    {
        return newValue == object.celsius_.value_ * 1.800000 + 32.000000;
    }

};

/// \brief Document struct
struct Document : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Document()
        : title_(*this),
          minLength_(*this),
          maxLength_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Document() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, Document> title_;
    runtime::BoundedValue<int64_t, Document, &Document::Require_minLength_validRange, &Document::Require_minLength_notEmpty> minLength_;
    runtime::BoundedValue<int64_t, Document, &Document::Require_maxLength_validRange> maxLength_;
    /// \brief Require invariant 'validRange' for field 'minLength'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to minLength
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_minLength_validRange(const Document& object, const int64_t& newValue)
    {
        return newValue < 10 || object.maxLength_.value_ > 100;
    }

    /// \brief Require invariant 'notEmpty' for field 'minLength'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to minLength
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_minLength_notEmpty(const Document& object, const int64_t& newValue)
    {
        return !(newValue == 0);
    }

    /// \brief Require invariant 'validRange' for field 'maxLength'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to maxLength
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_maxLength_validRange(const Document& object, const int64_t& newValue)
    {
        return object.minLength_.value_ < 10 || newValue > 100;
    }

};

// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATED_TEST_EXPRESSIONS_H_INCL__
