#ifndef __RUNTIME_DATE_H_INCL__
#define __RUNTIME_DATE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <string>
#include <cstdint>

namespace runtime {
/// \brief Date type for model runtime (stub implementation)
///
/// This is a stub implementation that stores dates as ISO 8601 strings.
/// Full implementation with date arithmetic, formatting, parsing, and
/// timezone support will be provided later.
class Date
{
public:
    /// \brief Default constructor (initializes to empty date)
    Date() = default;

    /// \brief Construct from ISO 8601 string (YYYY-MM-DD)
    /// \param iso8601Date The date string in ISO 8601 format
    Date(const std::string& iso8601Date);

    /// \brief Construct from year, month, day
    /// \param year The year
    /// \param month The month (1-12)
    /// \param day The day (1-31)
    Date(int year, int month, int day);

    /// \brief Virtual destructor
    virtual ~Date() = default;

    /// \brief Get the date as ISO 8601 string
    /// \return The date string (YYYY-MM-DD)
    const std::string& GetValue() const;

    /// \brief Set the date from ISO 8601 string
    /// \param iso8601Date The date string in ISO 8601 format
    void SetValue(const std::string& iso8601Date);

    /// \brief Check if date is valid
    /// \return True if date is valid
    bool IsValid() const;

    /// \brief Implicit conversion to std::string
    operator std::string() const;

private:
    std::string value_; // ISO 8601 format: YYYY-MM-DD
};
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_DATE_H_INCL__
