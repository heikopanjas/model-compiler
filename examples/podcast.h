#ifndef __BBFM_GENERATED_PODCAST_H_INCL__
#define __BBFM_GENERATED_PODCAST_H_INCL__

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
struct Asset : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Asset()
        : url_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Asset() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Asset> url_;
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
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, AudioAsset> format_;
    bbfm::runtime::BoundedValue<int64_t, AudioAsset, &AudioAsset::Require_fileSize_maxFileSize> fileSize_;
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
    bbfm::runtime::BoundedValue<int64_t, PictureAsset, &PictureAsset::Require_width_minWidth> width_;
    bbfm::runtime::BoundedValue<int64_t, PictureAsset, &PictureAsset::Require_height_minHeight> height_;
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
struct Podcast : public bbfm::runtime::Fabric
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
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Podcast> title_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Podcast> description_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, Podcast> author_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Podcast> feedUrl_; // Unique constraint
    std::vector<model::Episode> episodes_;
};

/// \brief Episode struct
struct Episode : public bbfm::runtime::Fabric
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
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Episode> title_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::Date, Episode> publicationDate_;
    bbfm::runtime::UnboundedValue<double, Episode> duration_;
    bbfm::runtime::UnboundedValue<model::MediaType, Episode> mediaType_;
    bbfm::runtime::UnboundedValue<model::AudioAsset, Episode> audio_;
    bbfm::runtime::OptionalUnboundedValue<model::Transcript, Episode> transcript_;
};

/// \brief Transcript struct
struct Transcript : public bbfm::runtime::Fabric
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
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Transcript> text_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Transcript> language_;
    std::vector<model::Region> regions_;
};

/// \brief Tag struct
struct Tag : public bbfm::runtime::Fabric
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
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Tag> name_;
    bbfm::runtime::BoundedValue<double, Tag, &Tag::Require_timestamp_validTimestamp> timestamp_;
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
    bbfm::runtime::BoundedValue<double, Region, &Region::Require_endTime_validEndTime, &Region::Require_endTime_validRegion> endTime_;
    bbfm::runtime::DynamicValue<double, Region> duration_;
    bbfm::runtime::AliasValue<bbfm::runtime::BoundedValue<double, Tag, &Tag::Require_timestamp_validTimestamp>> startTime_;
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

#endif // __BBFM_GENERATED_PODCAST_H_INCL__
