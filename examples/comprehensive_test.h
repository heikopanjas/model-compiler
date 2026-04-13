#ifndef __BBFM_GENERATED_COMPREHENSIVE_TEST_H_INCL__
#define __BBFM_GENERATED_COMPREHENSIVE_TEST_H_INCL__

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

/// \brief MediaType enumeration
enum class MediaType
{
    AUDIO,
    VIDEO
};

/// \brief Status enumeration
enum class Status
{
    DRAFT,
    PUBLISHED,
    ARCHIVED
};

/// \brief PrimitiveTypes struct
struct PrimitiveTypes : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    PrimitiveTypes()
        : stringField_(*this),
          intField_(*this),
          realField_(*this),
          boolField_(*this),
          timestampField_(*this),
          timespanField_(*this),
          dateField_(*this),
          guidField_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~PrimitiveTypes() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, PrimitiveTypes> stringField_;
    bbfm::runtime::UnboundedValue<int64_t, PrimitiveTypes> intField_;
    bbfm::runtime::UnboundedValue<double, PrimitiveTypes> realField_;
    bbfm::runtime::UnboundedValue<bool, PrimitiveTypes> boolField_;
    bbfm::runtime::UnboundedValue<double, PrimitiveTypes> timestampField_;
    bbfm::runtime::UnboundedValue<double, PrimitiveTypes> timespanField_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::Date, PrimitiveTypes> dateField_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::Guid, PrimitiveTypes> guidField_;
};

/// \brief FieldModifiers struct
struct FieldModifiers : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    FieldModifiers()
        : mandatoryField_(*this),
          explicitMandatory_(*this),
          optionalField_(*this),
          explicitOptional_(*this),
          uniqueField_(*this),
          mandatoryUnique_(*this),
          optionalUnique_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~FieldModifiers() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, FieldModifiers> mandatoryField_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, FieldModifiers> explicitMandatory_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, FieldModifiers> optionalField_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, FieldModifiers> explicitOptional_;
    std::vector<bbfm::runtime::String> optionalArray_;
    std::vector<bbfm::runtime::String> requiredArray_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, FieldModifiers> uniqueField_; // Unique constraint
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, FieldModifiers> mandatoryUnique_; // Unique constraint
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, FieldModifiers> optionalUnique_; // Unique constraint
};

/// \brief FieldNamesMatchingTypes struct
struct FieldNamesMatchingTypes : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    FieldNamesMatchingTypes()
        : string_(*this),
          int_(*this),
          real_(*this),
          bool_(*this),
          timestamp_(*this),
          timespan_(*this),
          date_(*this),
          guid_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~FieldNamesMatchingTypes() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, FieldNamesMatchingTypes> string_;
    bbfm::runtime::UnboundedValue<int64_t, FieldNamesMatchingTypes> int_;
    bbfm::runtime::UnboundedValue<double, FieldNamesMatchingTypes> real_;
    bbfm::runtime::UnboundedValue<bool, FieldNamesMatchingTypes> bool_;
    bbfm::runtime::UnboundedValue<double, FieldNamesMatchingTypes> timestamp_;
    bbfm::runtime::UnboundedValue<double, FieldNamesMatchingTypes> timespan_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::Date, FieldNamesMatchingTypes> date_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::Guid, FieldNamesMatchingTypes> guid_;
};

/// \brief Asset struct
struct Asset : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Asset()
        : url_(*this),
          title_(*this),
          description_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Asset() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Asset> url_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Asset> title_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, Asset> description_;
};

/// \brief AudioAsset struct
struct AudioAsset : public Asset
{
    /// \brief Constructor - initializes all fields with parent reference
    AudioAsset()
        : format_(*this),
          fileSize_(*this),
          duration_(*this),
          bitrate_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~AudioAsset() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, AudioAsset> format_;
    bbfm::runtime::UnboundedValue<int64_t, AudioAsset> fileSize_;
    bbfm::runtime::UnboundedValue<double, AudioAsset> duration_;
    bbfm::runtime::OptionalUnboundedValue<int64_t, AudioAsset> bitrate_;
};

/// \brief VideoAsset struct
struct VideoAsset : public Asset
{
    /// \brief Constructor - initializes all fields with parent reference
    VideoAsset()
        : width_(*this),
          height_(*this),
          duration_(*this),
          codec_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~VideoAsset() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<int64_t, VideoAsset> width_;
    bbfm::runtime::UnboundedValue<int64_t, VideoAsset> height_;
    bbfm::runtime::UnboundedValue<double, VideoAsset> duration_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, VideoAsset> codec_;
};

/// \brief InvariantTests struct
struct InvariantTests : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    InvariantTests()
        : count_(*this),
          price_(*this),
          name_(*this),
          active_(*this),
          createdAt_(*this),
          duration_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~InvariantTests() = default;

    // User-defined fields
    bbfm::runtime::BoundedValue<int64_t, InvariantTests, &InvariantTests::Require_count_validCount, &InvariantTests::Require_count_maxCount, &InvariantTests::Require_count_positiveCount, &InvariantTests::Require_count_belowLimit, &InvariantTests::Require_count_exactCount, &InvariantTests::Require_count_notZero> count_;
    bbfm::runtime::BoundedValue<double, InvariantTests, &InvariantTests::Require_price_validPrice, &InvariantTests::Require_price_maxPrice, &InvariantTests::Require_price_positivePrice, &InvariantTests::Require_price_belowThousand> price_;
    bbfm::runtime::BoundedValue<bbfm::runtime::String, InvariantTests, &InvariantTests::Require_name_nonEmptyName, &InvariantTests::Require_name_specificName> name_;
    bbfm::runtime::BoundedValue<bool, InvariantTests, &InvariantTests::Require_active_isActive, &InvariantTests::Require_active_notInactive> active_;
    bbfm::runtime::BoundedValue<double, InvariantTests, &InvariantTests::Require_createdAt_validTimestamp, &InvariantTests::Require_createdAt_futureTimestamp> createdAt_;
    bbfm::runtime::BoundedValue<double, InvariantTests, &InvariantTests::Require_duration_validDuration, &InvariantTests::Require_duration_maxDuration> duration_;
    /// \brief Require invariant 'validCount' for field 'count'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to count
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_count_validCount(const InvariantTests& object, const int64_t& newValue)
    {
        return newValue >= 0;
    }

    /// \brief Require invariant 'maxCount' for field 'count'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to count
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_count_maxCount(const InvariantTests& object, const int64_t& newValue)
    {
        return newValue <= 1000;
    }

    /// \brief Require invariant 'positiveCount' for field 'count'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to count
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_count_positiveCount(const InvariantTests& object, const int64_t& newValue)
    {
        return newValue > 0;
    }

    /// \brief Require invariant 'belowLimit' for field 'count'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to count
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_count_belowLimit(const InvariantTests& object, const int64_t& newValue)
    {
        return newValue < 999;
    }

    /// \brief Require invariant 'exactCount' for field 'count'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to count
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_count_exactCount(const InvariantTests& object, const int64_t& newValue)
    {
        return newValue == 42;
    }

    /// \brief Require invariant 'notZero' for field 'count'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to count
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_count_notZero(const InvariantTests& object, const int64_t& newValue)
    {
        return newValue != 0;
    }

    /// \brief Require invariant 'validPrice' for field 'price'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to price
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_price_validPrice(const InvariantTests& object, const double& newValue)
    {
        return newValue >= 0.000000;
    }

    /// \brief Require invariant 'maxPrice' for field 'price'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to price
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_price_maxPrice(const InvariantTests& object, const double& newValue)
    {
        return newValue <= 999.990000;
    }

    /// \brief Require invariant 'positivePrice' for field 'price'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to price
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_price_positivePrice(const InvariantTests& object, const double& newValue)
    {
        return newValue > 0.000000;
    }

    /// \brief Require invariant 'belowThousand' for field 'price'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to price
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_price_belowThousand(const InvariantTests& object, const double& newValue)
    {
        return newValue < 1000.000000;
    }

    /// \brief Require invariant 'nonEmptyName' for field 'name'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to name
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_name_nonEmptyName(const InvariantTests& object, const bbfm::runtime::String& newValue)
    {
        return newValue != """";
    }

    /// \brief Require invariant 'specificName' for field 'name'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to name
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_name_specificName(const InvariantTests& object, const bbfm::runtime::String& newValue)
    {
        return newValue == ""test"";
    }

    /// \brief Require invariant 'isActive' for field 'active'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to active
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_active_isActive(const InvariantTests& object, const bool& newValue)
    {
        return newValue == true;
    }

    /// \brief Require invariant 'notInactive' for field 'active'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to active
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_active_notInactive(const InvariantTests& object, const bool& newValue)
    {
        return newValue != false;
    }

    /// \brief Require invariant 'validTimestamp' for field 'createdAt'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to createdAt
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_createdAt_validTimestamp(const InvariantTests& object, const double& newValue)
    {
        return newValue >= 0.000000;
    }

    /// \brief Require invariant 'futureTimestamp' for field 'createdAt'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to createdAt
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_createdAt_futureTimestamp(const InvariantTests& object, const double& newValue)
    {
        return newValue > 1704067200.000000;
    }

    /// \brief Require invariant 'validDuration' for field 'duration'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to duration
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_duration_validDuration(const InvariantTests& object, const double& newValue)
    {
        return newValue > 0.000000;
    }

    /// \brief Require invariant 'maxDuration' for field 'duration'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to duration
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_duration_maxDuration(const InvariantTests& object, const double& newValue)
    {
        return newValue <= 7200.000000;
    }

};

/// \brief ImageConstraints struct
struct ImageConstraints : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    ImageConstraints()
        : width_(*this),
          height_(*this),
          format_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~ImageConstraints() = default;

    // User-defined fields
    bbfm::runtime::BoundedValue<int64_t, ImageConstraints, &ImageConstraints::Require_width_validWidth, &ImageConstraints::Require_width_minWidth> width_;
    bbfm::runtime::BoundedValue<int64_t, ImageConstraints, &ImageConstraints::Require_height_validHeight, &ImageConstraints::Require_height_minHeight> height_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, ImageConstraints> format_;
    /// \brief Require invariant 'validWidth' for field 'width'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to width
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_width_validWidth(const ImageConstraints& object, const int64_t& newValue)
    {
        return newValue <= 3840;
    }

    /// \brief Require invariant 'minWidth' for field 'width'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to width
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_width_minWidth(const ImageConstraints& object, const int64_t& newValue)
    {
        return newValue >= 100;
    }

    /// \brief Require invariant 'validHeight' for field 'height'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to height
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_height_validHeight(const ImageConstraints& object, const int64_t& newValue)
    {
        return newValue <= 2160;
    }

    /// \brief Require invariant 'minHeight' for field 'height'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to height
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_height_minHeight(const ImageConstraints& object, const int64_t& newValue)
    {
        return newValue >= 100;
    }

};

/// \brief Author struct
struct Author : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Author()
        : name_(*this),
          email_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Author() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Author> name_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, Author> email_;
};

/// \brief Publisher struct
struct Publisher : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Publisher()
        : name_(*this),
          website_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Publisher() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Publisher> name_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, Publisher> website_;
};

/// \brief Book struct
struct Book : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Book()
        : title_(*this),
          isbn_(*this),
          publicationDate_(*this),
          primaryAuthor_(*this),
          publisher_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Book() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Book> title_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Book> isbn_; // Unique constraint
    bbfm::runtime::UnboundedValue<bbfm::runtime::Date, Book> publicationDate_;
    bbfm::runtime::UnboundedValue<Author, Book> primaryAuthor_;
    bbfm::runtime::OptionalUnboundedValue<Publisher, Book> publisher_;
    std::vector<Author> authors_;
    std::vector<bbfm::runtime::String> tags_;
};

/// \brief Podcast struct
struct Podcast : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Podcast()
        : title_(*this),
          description_(*this),
          author_(*this),
          rssUrl_(*this),
          coverArt_(*this),
          status_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Podcast() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Podcast> title_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Podcast> description_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, Podcast> author_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Podcast> rssUrl_; // Unique constraint
    bbfm::runtime::OptionalUnboundedValue<Asset, Podcast> coverArt_;
    std::vector<Episode> episodes_;
    bbfm::runtime::UnboundedValue<Status, Podcast> status_;
};

/// \brief Episode struct
struct Episode : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Episode()
        : title_(*this),
          description_(*this),
          publicationDate_(*this),
          duration_(*this),
          mediaType_(*this),
          audioFile_(*this),
          transcript_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Episode() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Episode> title_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, Episode> description_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::Date, Episode> publicationDate_;
    bbfm::runtime::UnboundedValue<double, Episode> duration_;
    bbfm::runtime::UnboundedValue<MediaType, Episode> mediaType_;
    bbfm::runtime::UnboundedValue<AudioAsset, Episode> audioFile_;
    bbfm::runtime::OptionalUnboundedValue<Transcript, Episode> transcript_;
    std::vector<ChapterMarker> chapterMarkers_;
};

/// \brief Transcript struct
struct Transcript : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    Transcript()
        : text_(*this),
          language_(*this),
          format_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~Transcript() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Transcript> text_;
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, Transcript> language_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, Transcript> format_;
};

/// \brief ChapterMarker struct
struct ChapterMarker : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    ChapterMarker()
        : title_(*this),
          startTime_(*this),
          endTime_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~ChapterMarker() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, ChapterMarker> title_;
    bbfm::runtime::BoundedValue<double, ChapterMarker, &ChapterMarker::Require_startTime_validStart> startTime_;
    bbfm::runtime::OptionalUnboundedValue<double, ChapterMarker> endTime_;
    /// \brief Require invariant 'validStart' for field 'startTime'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to startTime
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_startTime_validStart(const ChapterMarker& object, const double& newValue)
    {
        return newValue >= 0.000000;
    }

};

/// \brief UserPreferences struct
struct UserPreferences : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    UserPreferences()
        : userId_(*this),
          emailNotifications_(*this),
          pushNotifications_(*this),
          darkMode_(*this),
          isActive_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~UserPreferences() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::Guid, UserPreferences> userId_;
    bbfm::runtime::UnboundedValue<bool, UserPreferences> emailNotifications_;
    bbfm::runtime::UnboundedValue<bool, UserPreferences> pushNotifications_;
    bbfm::runtime::OptionalUnboundedValue<bool, UserPreferences> darkMode_;
    bbfm::runtime::UnboundedValue<bool, UserPreferences> isActive_;
};

/// \brief FeatureFlags struct
struct FeatureFlags : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    FeatureFlags()
        : enableBetaFeatures_(*this),
          allowGuestAccess_(*this),
          maintenanceMode_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~FeatureFlags() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bool, FeatureFlags> enableBetaFeatures_;
    bbfm::runtime::UnboundedValue<bool, FeatureFlags> allowGuestAccess_;
    bbfm::runtime::BoundedValue<bool, FeatureFlags, &FeatureFlags::Require_maintenanceMode_safeMode> maintenanceMode_;
    /// \brief Require invariant 'safeMode' for field 'maintenanceMode'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to maintenanceMode
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_maintenanceMode_safeMode(const FeatureFlags& object, const bool& newValue)
    {
        return newValue == false;
    }

};

/// \brief MediaItem struct
struct MediaItem : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    MediaItem()
        : title_(*this),
          description_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~MediaItem() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, MediaItem> title_;
    bbfm::runtime::OptionalUnboundedValue<bbfm::runtime::String, MediaItem> description_;
};

/// \brief StreamableMedia struct
struct StreamableMedia : public MediaItem
{
    /// \brief Constructor - initializes all fields with parent reference
    StreamableMedia()
        : url_(*this),
          duration_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~StreamableMedia() = default;

    // User-defined fields
    bbfm::runtime::UnboundedValue<bbfm::runtime::String, StreamableMedia> url_;
    bbfm::runtime::UnboundedValue<double, StreamableMedia> duration_;
};

/// \brief PodcastEpisode struct
struct PodcastEpisode : public StreamableMedia
{
    /// \brief Constructor - initializes all fields with parent reference
    PodcastEpisode()
        : episodeNumber_(*this),
          seasonNumber_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~PodcastEpisode() = default;

    // User-defined fields
    bbfm::runtime::BoundedValue<int64_t, PodcastEpisode, &PodcastEpisode::Require_episodeNumber_validEpisode> episodeNumber_;
    bbfm::runtime::OptionalUnboundedValue<int64_t, PodcastEpisode> seasonNumber_;
    /// \brief Require invariant 'validEpisode' for field 'episodeNumber'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to episodeNumber
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_episodeNumber_validEpisode(const PodcastEpisode& object, const int64_t& newValue)
    {
        return newValue > 0;
    }

};

/// \brief BaseWithInvariant struct
struct BaseWithInvariant : public bbfm::runtime::Fabric
{
    /// \brief Constructor - initializes all fields with parent reference
    BaseWithInvariant()
        : width_(*this),
          height_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~BaseWithInvariant() = default;

    // User-defined fields
    bbfm::runtime::BoundedValue<int64_t, BaseWithInvariant, &BaseWithInvariant::Require_width_validWidth> width_;
    bbfm::runtime::BoundedValue<int64_t, BaseWithInvariant, &BaseWithInvariant::Require_height_validHeight> height_;
    /// \brief Require invariant 'validWidth' for field 'width'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to width
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_width_validWidth(const BaseWithInvariant& object, const int64_t& newValue)
    {
        return newValue >= 100;
    }

    /// \brief Require invariant 'validHeight' for field 'height'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to height
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_height_validHeight(const BaseWithInvariant& object, const int64_t& newValue)
    {
        return newValue >= 100;
    }

};

/// \brief DerivedWithInvariant struct
struct DerivedWithInvariant : public BaseWithInvariant
{
    /// \brief Constructor - initializes all fields with parent reference
    DerivedWithInvariant()
        : depth_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~DerivedWithInvariant() = default;

    // User-defined fields
    bbfm::runtime::BoundedValue<int64_t, DerivedWithInvariant, &DerivedWithInvariant::Require_depth_validDepth> depth_;
    /// \brief Require invariant 'validDepth' for field 'depth'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to depth
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_depth_validDepth(const DerivedWithInvariant& object, const int64_t& newValue)
    {
        return newValue >= 10;
    }

};

/// \brief FurtherDerivedWithInvariant struct
struct FurtherDerivedWithInvariant : public DerivedWithInvariant
{
    /// \brief Constructor - initializes all fields with parent reference
    FurtherDerivedWithInvariant()
        : color_(*this)
    {
    }

    /// \brief Virtual destructor
    virtual ~FurtherDerivedWithInvariant() = default;

    // User-defined fields
    bbfm::runtime::BoundedValue<bbfm::runtime::String, FurtherDerivedWithInvariant, &FurtherDerivedWithInvariant::Require_color_nonEmptyColor> color_;
    /// \brief Require invariant 'nonEmptyColor' for field 'color'
    /// \param object Reference to the containing object
    /// \param newValue The new value being assigned to color
    /// \return True if invariant is satisfied, false otherwise
    static bool Require_color_nonEmptyColor(const FurtherDerivedWithInvariant& object, const bbfm::runtime::String& newValue)
    {
        return newValue != """";
    }

};

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_GENERATED_COMPREHENSIVE_TEST_H_INCL__
