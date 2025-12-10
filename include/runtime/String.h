#ifndef __BBFM_RUNTIME_STRING_H_INCL__
#define __BBFM_RUNTIME_STRING_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <string>
#include <cstdint>

namespace bbfm {
namespace runtime {
/// \brief String type for BBFM runtime (stub implementation)
///
/// This is a stub implementation that wraps std::string.
/// Full implementation with encoding support, validation, and
/// additional string operations will be provided later.
class String
{
public:
    /// \brief Default constructor
    String() = default;

    /// \brief Construct from C++ string
    /// \param value The string value
    String(const std::string& value);

    /// \brief Construct from C string
    /// \param value The C string value
    String(const char* value);

    /// \brief Virtual destructor
    virtual ~String() = default;

    /// \brief Get the underlying string value
    /// \return Reference to the string value
    const std::string& GetValue() const;

    /// \brief Set the string value
    /// \param value The new string value
    void SetValue(const std::string& value);

    /// \brief Get the length of the string
    /// \return The string length
    size_t GetLength() const;

    /// \brief Check if string is empty
    /// \return True if empty
    bool IsEmpty() const;

    /// \brief Implicit conversion to std::string
    operator std::string() const;

    /// \brief Implicit conversion to const char*
    operator const char*() const;

private:
    std::string value_;
};
} // namespace runtime
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_RUNTIME_STRING_H_INCL__
