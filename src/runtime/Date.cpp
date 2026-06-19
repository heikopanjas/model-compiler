#include "runtime/Date.h"

#include <cctype>
#include <iomanip>
#include <sstream>

namespace runtime {
namespace {
bool IsDigitAt(const std::string& value, const size_t index)
{
    return std::isdigit(static_cast<unsigned char>(value[index])) != 0;
}

int ParseTwoDigits(const std::string& value, const size_t index)
{
    return ((value[index] - '0') * 10) + (value[index + 1] - '0');
}

bool HasIso8601DateShape(const std::string& value)
{
    if (value.size() != 10)
    {
        return false;
    }

    if (value[4] != '-' || value[7] != '-')
    {
        return false;
    }

    return IsDigitAt(value, 0) && IsDigitAt(value, 1) && IsDigitAt(value, 2) && IsDigitAt(value, 3) && IsDigitAt(value, 5) && IsDigitAt(value, 6) &&
           IsDigitAt(value, 8) && IsDigitAt(value, 9);
}
} // namespace

Date::Date(const std::string& iso8601Date) : value_(iso8601Date) {}

Date::Date(const int year, const int month, const int day)
{
    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(4) << year << '-' << std::setw(2) << month << '-' << std::setw(2) << day;
    value_ = stream.str();
}

const std::string& Date::GetValue() const
{
    return value_;
}

void Date::SetValue(const std::string& iso8601Date)
{
    value_ = iso8601Date;
}

bool Date::IsValid() const
{
    if (HasIso8601DateShape(value_) == false)
    {
        return false;
    }

    const int month = ParseTwoDigits(value_, 5);
    const int day   = ParseTwoDigits(value_, 8);
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

Date::operator std::string() const
{
    return value_;
}
} // namespace runtime
