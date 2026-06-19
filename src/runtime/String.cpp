#include "runtime/String.h"

#include <utility>

namespace {
const std::string& EmptyString()
{
    static const std::string EMPTY;
    return EMPTY;
}
} // namespace

namespace runtime {
// --- Impl --------------------------------------------------------------------

class String::Impl
{
public:
    Impl() = default;

    explicit Impl(const std::string& value) : value_(value) {}

    explicit Impl(const char* value) : value_(nullptr != value ? value : "") {}

    Impl(const Impl& other)            = default;
    Impl& operator=(const Impl& other) = default;

    const std::string& GetValue() const
    {
        return value_;
    }

    void SetValue(const std::string& value)
    {
        value_ = value;
    }

private:
    std::string value_;
};

// --- String public methods ---------------------------------------------------

String::String() : impl_(std::make_unique<Impl>()) {}

String::String(const std::string& value) : impl_(std::make_unique<Impl>(value)) {}

String::String(const char* value) : impl_(std::make_unique<Impl>(value)) {}

String::String(const String& other) : impl_(nullptr != other.impl_ ? std::make_unique<Impl>(*other.impl_) : std::make_unique<Impl>()) {}

String& String::operator=(const String& other)
{
    if (this != &other)
    {
        impl_ = (nullptr != other.impl_) ? std::make_unique<Impl>(*other.impl_) : std::make_unique<Impl>();
    }
    return *this;
}

String::String(String&& other) noexcept : impl_(std::move(other.impl_)) {}

String& String::operator=(String&& other) noexcept
{
    if (this != &other)
    {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

String::~String() = default;

const std::string& String::GetValue() const
{
    if (nullptr == impl_)
    {
        return EmptyString();
    }
    return impl_->GetValue();
}

void String::SetValue(const std::string& value)
{
    if (nullptr == impl_)
    {
        impl_ = std::make_unique<Impl>(value);
        return;
    }

    impl_->SetValue(value);
}

size_t String::GetLength() const
{
    return GetValue().size();
}

bool String::IsEmpty() const
{
    return GetValue().empty();
}

void String::SetValue(const char* value)
{
    SetValue(nullptr != value ? std::string(value) : std::string());
}

String& String::Append(const String& other)
{
    if (IsEmpty() == true)
    {
        *this = other;
        return *this;
    }

    std::string combined  = GetValue();
    combined             += other.GetValue();
    SetValue(combined);
    return *this;
}

String& String::Append(const char* value)
{
    if (nullptr == value)
    {
        return *this;
    }

    return Append(String(value));
}

String operator+(String lhs, const String& rhs)
{
    lhs.Append(rhs);
    return lhs;
}

bool String::operator==(const String& other) const
{
    return GetValue() == other.GetValue();
}

bool String::operator!=(const String& other) const
{
    return GetValue() != other.GetValue();
}

bool String::operator==(const char* value) const
{
    if (nullptr == value)
    {
        return IsEmpty();
    }

    return GetValue() == value;
}

bool String::operator!=(const char* value) const
{
    return (*this == value) == false;
}

bool String::operator<(const String& other) const
{
    return GetValue() < other.GetValue();
}

String::operator std::string() const
{
    return GetValue();
}

String::operator const char*() const
{
    return GetValue().c_str();
}
} // namespace runtime
