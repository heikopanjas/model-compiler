#include "runtime/Fabric.h"

#include <chrono>

namespace runtime {
Fabric::Fabric()
{
    // Initialize with default values (stub implementation)
    cardinality_      = 1;
    creationDate_     = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
    modificationDate_ = creationDate_;
}

const Guid& Fabric::GetTypeId() const
{
    return typeId_;
}

const Guid& Fabric::GetId() const
{
    return id_;
}

int64_t Fabric::GetCardinality() const
{
    return cardinality_;
}

double Fabric::GetCreationDate() const
{
    return creationDate_;
}

double Fabric::GetModificationDate() const
{
    return modificationDate_;
}

const String& Fabric::GetComment() const
{
    return comment_;
}

void Fabric::SetComment(const String& comment)
{
    comment_ = comment;
    UpdateModificationDate();
}

void Fabric::UpdateModificationDate()
{
    modificationDate_ = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
}
} // namespace runtime
