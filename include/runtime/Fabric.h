#ifndef __RUNTIME_FABRIC_H_INCL__
#define __RUNTIME_FABRIC_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/Guid.h"
#include "runtime/String.h"
#include <cstdint>

namespace runtime {
/// \brief Base class for all model types providing universal metadata
///
/// Every model type automatically inherits from Fabric, which provides
/// six universal metadata fields that are present on all instances:
/// - typeId: Type identifier (same for all instances of a type)
/// - id: Instance identifier (unique per instance)
/// - cardinality: Cardinality of the instance
/// - creationDate: Creation timestamp
/// - modificationDate: Modification timestamp
/// - comment: User comment/notes
class Fabric
{
public:
    /// \brief Default constructor
    Fabric();

    /// \brief Virtual destructor
    virtual ~Fabric() = default;

    /// \brief Get the type identifier
    /// \return Reference to the type ID
    const Guid& GetTypeId() const;

    /// \brief Get the instance identifier
    /// \return Reference to the instance ID
    const Guid& GetId() const;

    /// \brief Get the cardinality
    /// \return The cardinality value
    int64_t GetCardinality() const;

    /// \brief Get the creation date
    /// \return The creation timestamp
    double GetCreationDate() const;

    /// \brief Get the modification date
    /// \return The modification timestamp
    double GetModificationDate() const;

    /// \brief Get the comment
    /// \return Reference to the comment string
    const String& GetComment() const;

    /// \brief Set the comment
    /// \param comment The new comment value
    void SetComment(const String& comment);

    /// \brief Update the modification date to current time
    void UpdateModificationDate();

protected:
    Guid    typeId_;           // Type identifier (same for all instances of this type)
    Guid    id_;               // Instance identifier (unique per instance)
    int64_t cardinality_;      // Cardinality of the instance
    double  creationDate_;     // Creation timestamp (seconds since epoch)
    double  modificationDate_; // Modification timestamp (seconds since epoch)
    String  comment_;          // User comment/notes
};
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_FABRIC_H_INCL__
