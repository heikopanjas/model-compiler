#ifndef __GENERATED_PODCAST_H_INCL__
#define __GENERATED_PODCAST_H_INCL__

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

namespace model {

/// \brief MediaType enumeration
enum class MediaType
{
    AUDIO,
    VIDEO
};

/// \brief Asset struct
struct Asset : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Asset()
        : url_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Asset() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, Asset> url_;
};

/// \brief AudioAsset struct
struct AudioAsset : public Asset
{
    /// \brief Constructor - initializes all fields with parent reference
    AudioAsset()
        : format_(*this),
          fileSize_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~AudioAsset() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, AudioAsset> format_;
    runtime::BoundedValue<int64_t, AudioAsset, &AudioAsset::Require_fileSize_maxFileSize> fileSize_;
    /// \brief Require invariant 'maxFileSize' for field 'fileSize'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to fileSize
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_fileSize_maxFileSize(const AudioAsset& object, const int64_t& newValue)
    {
        return newValue <= 500000000;
    }

};

/// \brief PictureAsset struct
struct PictureAsset : public Asset
{
    /// \brief Constructor - initializes all fields with parent reference
    PictureAsset()
        : width_(*this),
          height_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~PictureAsset() = default;

    // User-defined fields
    runtime::BoundedValue<int64_t, PictureAsset, &PictureAsset::Require_width_minWidth> width_;
    runtime::BoundedValue<int64_t, PictureAsset, &PictureAsset::Require_height_minHeight> height_;
    /// \brief Require invariant 'minWidth' for field 'width'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to width
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_width_minWidth(const PictureAsset& object, const int64_t& newValue)
    {
        return newValue >= 3000;
    }

    /// \brief Require invariant 'minHeight' for field 'height'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to height
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_height_minHeight(const PictureAsset& object, const int64_t& newValue)
    {
        return newValue >= 3000;
    }

};

/// \brief Podcast struct
struct Podcast : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Podcast()
        : title_(*this),
          description_(*this),
          author_(*this),
          feedUrl_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Podcast() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, Podcast> title_;
    runtime::UnboundedValue<runtime::String, Podcast> description_;
    runtime::OptionalUnboundedValue<runtime::String, Podcast> author_;
    runtime::UnboundedValue<runtime::String, Podcast> feedUrl_; // Unique constraint
    std::vector<Episode> episodes_;
};

/// \brief Episode struct
struct Episode : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Episode()
        : title_(*this),
          publicationDate_(*this),
          duration_(*this),
          mediaType_(*this),
          audio_(*this),
          transcript_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Episode() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, Episode> title_;
    runtime::UnboundedValue<runtime::Date, Episode> publicationDate_;
    runtime::UnboundedValue<double, Episode> duration_;
    runtime::UnboundedValue<MediaType, Episode> mediaType_;
    runtime::UnboundedValue<AudioAsset, Episode> audio_;
    runtime::OptionalUnboundedValue<Transcript, Episode> transcript_;
};

/// \brief Transcript struct
struct Transcript : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Transcript()
        : text_(*this),
          language_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Transcript() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, Transcript> text_;
    runtime::UnboundedValue<runtime::String, Transcript> language_;
    std::vector<Region> regions_;
};

/// \brief Tag struct
struct Tag : public runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Tag()
        : name_(*this),
          timestamp_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Tag() = default;

    // User-defined fields
    runtime::UnboundedValue<runtime::String, Tag> name_;
    runtime::BoundedValue<double, Tag, &Tag::Require_timestamp_validTimestamp> timestamp_;
    /// \brief Require invariant 'validTimestamp' for field 'timestamp'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to timestamp
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_timestamp_validTimestamp(const Tag& object, const double& newValue)
    {
        return newValue >= 0;
    }

};

/// \brief Region struct
struct Region : public Tag
{
    /// \brief Constructor - initializes all fields with parent reference
    Region()
        : startTime_(timestamp_),
          endTime_(*this),
          duration_(*this, [](const Region& parent) { return (parent.endTime_.value_ - parent.startTime_); })
    {
    }

    /// \brief Virtual destructor
    virtual ~Region() = default;

    // User-defined fields
    runtime::BoundedValue<double, Region, &Region::Require_endTime_validEndTime, &Region::Require_endTime_validRegion> endTime_;
    runtime::DynamicValue<double, Region> duration_;
    runtime::AliasValue<runtime::BoundedValue<double, Tag, &Tag::Require_timestamp_validTimestamp>> startTime_;
    /// \brief Require invariant 'validEndTime' for field 'endTime'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to endTime
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_endTime_validEndTime(const Region& object, const double& newValue)
    {
        return newValue >= 0;
    }

    /// \brief Require invariant 'validRegion' for field 'endTime'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to endTime
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_endTime_validRegion(const Region& object, const double& newValue)
    {
        return newValue > object.startTime_;
    }

};


} // namespace model
// Restore previous alignment
#pragma pack(pop)

#endif // __GENERATED_PODCAST_H_INCL__
