#include "Guid.h"

#include <algorithm>
#include <cstring>
#include <vector>

// Platform-specific includes for UUID generation
#if defined(_WIN32)
    #include <objbase.h>
    #pragma comment(lib, "ole32.lib")
#elif defined(__APPLE__)
    #include <CoreFoundation/CFUUID.h>
#else
    #include <uuid/uuid.h>
#endif

namespace {
// Convert a single hex character to its 4-bit value.
// Returns -1 on invalid input.
int HexCharToNibble(char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    if ('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return -1;
}

// Parse a hex string into bytes. Returns empty vector on invalid input.
std::vector<uint8_t> ParseHexBytes(const char* hex, int count)
{
    std::vector<uint8_t> result;
    result.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        int hi = HexCharToNibble(hex[i * 2]);
        int lo = HexCharToNibble(hex[i * 2 + 1]);
        if (0 > hi || 0 > lo)
        {
            return {};
        }
        result.push_back(static_cast<uint8_t>((hi << 4) | lo));
    }
    return result;
}

// Parse UUID string into 16 bytes. Supports:
//   Canonical:  xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx  (36 chars)
//   Braced:     {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx} (38 chars)
//   No dashes:  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx       (32 chars)
// Returns a 16-byte vector on success, empty vector on failure.
std::vector<uint8_t> ParseUuidString(const std::string& str)
{
    const char* p   = str.c_str();
    size_t      len = str.size();

    // Strip braces if present
    if (2 <= len && '{' == p[0] && '}' == p[len - 1])
    {
        p   += 1;
        len -= 2;
    }

    if (36 == len)
    {
        // Canonical format: 8-4-4-4-12 with dashes at positions 8, 13, 18, 23
        if ('-' != p[8] || '-' != p[13] || '-' != p[18] || '-' != p[23])
        {
            return {};
        }

        // Parse each segment: 4-2-2-2-6 bytes
        struct
        {
            const char* ptr;
            int         count;
        } segments[] = {
            {     p, 4}, // bytes 0-3
            { p + 9, 2}, // bytes 4-5
            {p + 14, 2}, // bytes 6-7
            {p + 19, 2}, // bytes 8-9
            {p + 24, 6}, // bytes 10-15
        };

        std::vector<uint8_t> result;
        result.reserve(16);

        for (const auto& seg : segments)
        {
            auto chunk = ParseHexBytes(seg.ptr, seg.count);
            if (chunk.empty())
            {
                return {};
            }
            result.insert(result.end(), chunk.begin(), chunk.end());
        }
        return result;
    }
    else if (32 == len)
    {
        // No-dash format: 32 contiguous hex chars
        return ParseHexBytes(p, 16);
    }

    return {};
}

// Format 16 bytes as canonical lowercase UUID string.
std::string FormatUuid(const uint8_t* bytes)
{
    static const char HEX[] = "0123456789abcdef";
    std::string       result(36, '-');
    int               pos = 0;

    for (int i = 0; i < 16; ++i)
    {
        result[pos++] = HEX[bytes[i] >> 4];
        result[pos++] = HEX[bytes[i] & 0x0F];

        // Insert dashes after bytes 3, 5, 7, 9
        if (3 == i || 5 == i || 7 == i || 9 == i)
        {
            ++pos; // skip pre-placed dash
        }
    }

    return result;
}

// UUID byte count.
constexpr int UUID_SIZE = 16;

// Static zero array for nil comparisons.
static const uint8_t NIL_BYTES[UUID_SIZE] = {};

// Generate 16 random UUID bytes using platform-native API.
void GenerateNative(uint8_t* out)
{
#if defined(_WIN32)
    GUID guid;
    CoCreateGuid(&guid);
    std::memcpy(out, &guid, 16);
#elif defined(__APPLE__)
    CFUUIDRef   uuid  = CFUUIDCreate(nullptr);
    CFUUIDBytes bytes = CFUUIDGetUUIDBytes(uuid);
    out[0]            = bytes.byte0;
    out[1]            = bytes.byte1;
    out[2]            = bytes.byte2;
    out[3]            = bytes.byte3;
    out[4]            = bytes.byte4;
    out[5]            = bytes.byte5;
    out[6]            = bytes.byte6;
    out[7]            = bytes.byte7;
    out[8]            = bytes.byte8;
    out[9]            = bytes.byte9;
    out[10]           = bytes.byte10;
    out[11]           = bytes.byte11;
    out[12]           = bytes.byte12;
    out[13]           = bytes.byte13;
    out[14]           = bytes.byte14;
    out[15]           = bytes.byte15;
    CFRelease(uuid);
#else
    uuid_generate(out);
#endif
}
} // anonymous namespace

namespace bbfm { namespace runtime {
// --- Impl ----------------------------------------------------------------

class Guid::Impl
{
public:
    std::vector<uint8_t> bytes_;
    mutable std::string  cachedString_;
    mutable bool         dirty_;

    Impl() : bytes_(UUID_SIZE, 0), dirty_(true) {}

    explicit Impl(const uint8_t* bytes) : bytes_(bytes, bytes + UUID_SIZE), dirty_(true) {}

    Impl(const Impl& other)            = default;
    Impl& operator=(const Impl& other) = default;

    const std::string& ToString() const
    {
        if (dirty_)
        {
            cachedString_ = FormatUuid(bytes_.data());
            dirty_        = false;
        }
        return cachedString_;
    }

    bool IsNil() const
    {
        return 0 == std::memcmp(bytes_.data(), NIL_BYTES, UUID_SIZE);
    }
};

// --- Guid public methods -----------------------------------------------------

Guid::Guid()
{
    uint8_t bytes[16];
    GenerateNative(bytes);
    impl_ = std::make_unique<Impl>(bytes);
}

Guid::Guid(const std::string& uuid) : impl_(nullptr)
{
    auto bytes = ParseUuidString(uuid);
    if (!bytes.empty())
    {
        impl_ = std::make_unique<Impl>(bytes.data());
    }
}

Guid::Guid(const uint8_t* bytes) : impl_(std::make_unique<Impl>(bytes)) {}

Guid::Guid(const Guid& other) : impl_(nullptr != other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr) {}

Guid& Guid::operator=(const Guid& other)
{
    if (this != &other)
    {
        impl_ = (nullptr != other.impl_) ? std::make_unique<Impl>(*other.impl_) : nullptr;
    }
    return *this;
}

Guid::Guid(Guid&& other) noexcept : impl_(std::move(other.impl_)) {}

Guid& Guid::operator=(Guid&& other) noexcept
{
    if (this != &other)
    {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

Guid::~Guid() = default;

std::string Guid::GetValue() const
{
    if (nullptr == impl_)
    {
        return {};
    }
    return impl_->ToString();
}

void Guid::SetValue(const std::string& uuid)
{
    auto bytes = ParseUuidString(uuid);
    if (!bytes.empty())
    {
        impl_ = std::make_unique<Impl>(bytes.data());
    }
    else
    {
        impl_.reset();
    }
}

std::string Guid::ToString() const
{
    return GetValue();
}

bool Guid::IsValid() const
{
    return nullptr != impl_;
}

bool Guid::IsNil() const
{
    if (nullptr == impl_)
    {
        return true;
    }
    return impl_->IsNil();
}

const uint8_t* Guid::GetBytes() const
{
    if (nullptr == impl_)
    {
        return nullptr;
    }
    return impl_->bytes_.data();
}

Guid Guid::Nil()
{
    uint8_t bytes[16] = {};
    return Guid(bytes);
}

Guid::operator std::string() const
{
    return GetValue();
}

bool Guid::operator==(const Guid& other) const
{
    const uint8_t* a = (nullptr != impl_) ? impl_->bytes_.data() : NIL_BYTES;
    const uint8_t* b = (nullptr != other.impl_) ? other.impl_->bytes_.data() : NIL_BYTES;
    return 0 == std::memcmp(a, b, UUID_SIZE);
}

bool Guid::operator!=(const Guid& other) const
{
    return !(*this == other);
}

bool Guid::operator<(const Guid& other) const
{
    const uint8_t* a = (nullptr != impl_) ? impl_->bytes_.data() : NIL_BYTES;
    const uint8_t* b = (nullptr != other.impl_) ? other.impl_->bytes_.data() : NIL_BYTES;
    return 0 > std::memcmp(a, b, UUID_SIZE);
}
}} // namespace bbfm::runtime

// --- std::hash specialization ------------------------------------------------

size_t std::hash<bbfm::runtime::Guid>::operator()(const bbfm::runtime::Guid& guid) const
{
    static const uint8_t NIL[UUID_SIZE] = {};
    const uint8_t*       bytes          = (nullptr != guid.impl_) ? guid.impl_->bytes_.data() : NIL;

    uint64_t lo = 0;
    uint64_t hi = 0;
    std::memcpy(&lo, bytes, 8);
    std::memcpy(&hi, bytes + 8, 8);

    size_t h1 = std::hash<uint64_t>{}(lo);
    size_t h2 = std::hash<uint64_t>{}(hi);

    // Combine hashes (boost-style hash_combine)
    return h1 ^ (h2 + 0x9e37'79b9 + (h1 << 6) + (h1 >> 2));
}
