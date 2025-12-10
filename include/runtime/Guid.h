#ifndef __BBFM_RUNTIME_GUID_H_INCL__
#define __BBFM_RUNTIME_GUID_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <string>
#include <cstdint>

namespace bbfm {
namespace runtime {
/// \brief Guid (UUID) type for BBFM runtime (stub implementation)
///
/// This is a stub implementation that stores GUIDs as strings.
/// Full implementation with UUID generation, validation, parsing,
/// and various UUID format support will be provided later.
class Guid
{
public:
    /// \brief Default constructor (initializes to empty GUID)
    Guid() = default;

    /// \brief Construct from UUID string
    /// \param uuid The UUID string (various formats supported)
    Guid(const std::string& uuid);

    /// \brief Virtual destructor
    virtual ~Guid() = default;

    /// \brief Get the GUID as string
    /// \return The GUID string
    const std::string& GetValue() const;

    /// \brief Set the GUID from string
    /// \param uuid The UUID string
    void SetValue(const std::string& uuid);

    /// \brief Check if GUID is valid (non-empty)
    /// \return True if GUID is valid
    bool IsValid() const;

    /// \brief Generate a new random GUID
    /// \return A new Guid instance
    static Guid Generate();

    /// \brief Implicit conversion to std::string
    operator std::string() const;

private:
    std::string value_; // UUID string representation
};
} // namespace runtime
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_RUNTIME_GUID_H_INCL__
