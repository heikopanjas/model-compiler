#ifndef __RUNTIME_STRING_H_INCL__
#define __RUNTIME_STRING_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace runtime {
/// \brief String type for model runtime
///
/// Stores string values using the PIMPL idiom so future encoding,
/// validation, and storage details can evolve without changing this
/// public header.
class String
{
public:
    /// \brief Default constructor
    String();

    /// \brief Construct from C++ string
    /// \param value The string value
    String(const std::string& value);

    /// \brief Construct from C string
    /// \param value The C string value
    String(const char* value);

    /// \brief Copy constructor
    /// \param other The String to copy from
    String(const String& other);

    /// \brief Copy assignment operator
    /// \param other The String to copy from
    /// \return Reference to this String
    String& operator=(const String& other);

    /// \brief Move constructor
    /// \param other The String to move from
    String(String&& other) noexcept;

    /// \brief Move assignment operator
    /// \param other The String to move from
    /// \return Reference to this String
    String& operator=(String&& other) noexcept;

    /// \brief Virtual destructor
    virtual ~String();

    /// \brief Get the underlying string value
    /// \return Reference to the string value
    const std::string& GetValue() const;

    /// \brief Set the string value
    /// \param value The new string value
    void SetValue(const std::string& value);

    /// \brief Set the string value from a C string
    /// \param value The new C string value
    void SetValue(const char* value);

    /// \brief Get the length of the string
    /// \return The string length
    size_t GetLength() const;

    /// \brief Check if string is empty
    /// \return True if empty
    bool IsEmpty() const;

    /// \brief Append another string
    /// \param other The string to append
    /// \return Reference to this String
    String& Append(const String& other);

    /// \brief Append a C string
    /// \param value The C string to append
    /// \return Reference to this String
    String& Append(const char* value);

    /// \brief Concatenate two strings
    /// \param lhs Left-hand string
    /// \param rhs Right-hand string
    /// \return Concatenated string
    friend String operator+(String lhs, const String& rhs);

    /// \brief Compare for equality
    /// \param other The string to compare with
    /// \return True if values are equal
    bool operator==(const String& other) const;

    /// \brief Compare for inequality
    /// \param other The string to compare with
    /// \return True if values differ
    bool operator!=(const String& other) const;

    /// \brief Compare for equality with a C string
    /// \param value The C string to compare with
    /// \return True if values are equal
    bool operator==(const char* value) const;

    /// \brief Compare for inequality with a C string
    /// \param value The C string to compare with
    /// \return True if values differ
    bool operator!=(const char* value) const;

    /// \brief Compare for less-than ordering
    /// \param other The string to compare with
    /// \return True if this string is lexicographically less
    bool operator<(const String& other) const;

    /// \brief Implicit conversion to std::string
    operator std::string() const;

    /// \brief Implicit conversion to const char*
    operator const char*() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

inline bool operator==(const char* lhs, const String& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const char* lhs, const String& rhs)
{
    return rhs != lhs;
}
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_STRING_H_INCL__
