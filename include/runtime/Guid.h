#ifndef __BBFM_RUNTIME_GUID_H_INCL__
#define __BBFM_RUNTIME_GUID_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace bbfm { namespace runtime {
/// \brief Cross-platform GUID (UUID) type for BBFM runtime
///
/// Stores a 128-bit universally unique identifier using the PIMPL idiom
/// to hide platform-specific implementation details. Supports generation
/// via native OS APIs (CoCreateGuid on Windows, CFUUIDCreate on macOS,
/// uuid_generate on Linux).
///
/// A default-constructed Guid generates a new random UUID automatically.
/// Use Guid::Nil() for the explicit nil UUID (all zeros, IsValid() == true,
/// IsNil() == true).
class Guid
{
public:
    /// \brief Default constructor (generates a new random UUID)
    Guid();

    /// \brief Construct from UUID string
    /// \param uuid The UUID string (supports canonical, braced, and no-dash formats)
    Guid(const std::string& uuid);

    /// \brief Copy constructor (deep copy)
    /// \param other The Guid to copy from
    Guid(const Guid& other);

    /// \brief Copy assignment operator (deep copy)
    /// \param other The Guid to copy from
    /// \return Reference to this Guid
    Guid& operator=(const Guid& other);

    /// \brief Move constructor
    /// \param other The Guid to move from
    Guid(Guid&& other) noexcept;

    /// \brief Move assignment operator
    /// \param other The Guid to move from
    /// \return Reference to this Guid
    Guid& operator=(Guid&& other) noexcept;

    /// \brief Virtual destructor
    virtual ~Guid();

    /// \brief Get the GUID as canonical string
    /// \return The GUID in lowercase canonical format (xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)
    std::string GetValue() const;

    /// \brief Set the GUID from string
    /// \param uuid The UUID string (supports canonical, braced, and no-dash formats)
    void SetValue(const std::string& uuid);

    /// \brief Get the GUID as canonical string
    /// \return The GUID in lowercase canonical format
    std::string ToString() const;

    /// \brief Check if GUID has been initialized with a value
    /// \return True if GUID holds a valid value (always true unless moved-from or set to invalid)
    bool IsValid() const;

    /// \brief Check if this is a nil (all-zero) UUID
    /// \return True if all 16 bytes are zero
    bool IsNil() const;

    /// \brief Get raw 16-byte UUID representation
    /// \return Pointer to 16 bytes, or nullptr if moved-from
    const uint8_t* GetBytes() const;

    /// \brief Return the nil (all-zero) UUID
    /// \return A valid Guid with all bytes set to zero
    static Guid Nil();

    /// \brief Implicit conversion to std::string
    operator std::string() const;

    /// \brief Equality comparison
    /// \param other The Guid to compare with
    /// \return True if both Guids have identical bytes
    bool operator==(const Guid& other) const;

    /// \brief Inequality comparison
    /// \param other The Guid to compare with
    /// \return True if Guids differ
    bool operator!=(const Guid& other) const;

    /// \brief Less-than comparison (for ordered containers)
    /// \param other The Guid to compare with
    /// \return True if this Guid is lexicographically less than other
    bool operator<(const Guid& other) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    /// \brief Private constructor from raw bytes (used by Nil)
    /// \param bytes Pointer to 16 bytes of UUID data
    Guid(const uint8_t* bytes);

    friend struct std::hash<Guid>;
};
}} // namespace bbfm::runtime

// std::hash specialization for Guid
namespace std {
template<> struct hash<bbfm::runtime::Guid>
{
    size_t operator()(const bbfm::runtime::Guid& guid) const;
};
} // namespace std

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_RUNTIME_GUID_H_INCL__
