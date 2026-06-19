#ifndef __RUNTIME_DICTIONARY_H_INCL__
#define __RUNTIME_DICTIONARY_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/String.h"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace runtime {
namespace detail {
inline bool IsNullOrEmptyKey(const char* key)
{
    return nullptr == key || '\0' == key[0];
}

inline std::string NormalizeKey(const char* key)
{
    return std::string(key);
}

inline std::string NormalizeKey(const String& key)
{
    return key.GetValue();
}
} // namespace detail

/// \brief Simple key/value dictionary for runtime values
template<typename TKey, typename TValue> class Dictionary
{
public:
    Dictionary() = default;

    Dictionary(const Dictionary& other)                = default;
    Dictionary& operator=(const Dictionary& other)     = default;
    Dictionary(Dictionary&& other) noexcept            = default;
    Dictionary& operator=(Dictionary&& other) noexcept = default;

    virtual ~Dictionary() = default;

    void SetValue(TKey key, TValue value)
    {
        const std::string keyString = detail::NormalizeKey(key);
        if (keyString.empty() == true)
        {
            return;
        }

        for (auto& entry : entries_)
        {
            if (entry.first == keyString)
            {
                entry.second = std::move(value);
                return;
            }
        }

        entries_.emplace_back(keyString, std::move(value));
    }

    TValue GetValue(TKey key) const
    {
        const std::string keyString = detail::NormalizeKey(key);
        for (const auto& entry : entries_)
        {
            if (entry.first == keyString)
            {
                return entry.second;
            }
        }

        return TValue{};
    }

    const TValue* GetValuePtr(TKey key) const
    {
        const std::string keyString = detail::NormalizeKey(key);
        for (const auto& entry : entries_)
        {
            if (entry.first == keyString)
            {
                return &entry.second;
            }
        }

        return nullptr;
    }

    TValue& GetOrCreate(TKey key)
    {
        const std::string keyString = detail::NormalizeKey(key);
        for (auto& entry : entries_)
        {
            if (entry.first == keyString)
            {
                return entry.second;
            }
        }

        entries_.emplace_back(keyString, TValue{});
        return entries_.back().second;
    }

    bool ContainsKey(TKey key) const
    {
        return nullptr != GetValuePtr(key);
    }

    size_t GetCount() const
    {
        return entries_.size();
    }

    const String GetKeyAt(const size_t index) const
    {
        if (index >= entries_.size())
        {
            return String();
        }

        return String(entries_[index].first);
    }

    const TValue& GetValueAt(const size_t index) const
    {
        static const TValue EMPTY{};
        if (index >= entries_.size())
        {
            return EMPTY;
        }

        return entries_[index].second;
    }

    const TValue* GetValuePtrAt(const size_t index) const
    {
        if (index >= entries_.size())
        {
            return nullptr;
        }

        return &entries_[index].second;
    }

private:
    std::vector<std::pair<std::string, TValue>> entries_;
};

/// \brief String-to-string dictionary used by generator plugin capabilities
template<> class Dictionary<const char*, const char*>
{
public:
    Dictionary() = default;

    Dictionary(const Dictionary& other)                = default;
    Dictionary& operator=(const Dictionary& other)     = default;
    Dictionary(Dictionary&& other) noexcept            = default;
    Dictionary& operator=(Dictionary&& other) noexcept = default;

    virtual ~Dictionary() = default;

    void SetValue(const char* key, const char* value)
    {
        if (detail::IsNullOrEmptyKey(key) == true)
        {
            return;
        }

        const std::string keyString(key);
        const std::string valueString(nullptr != value ? value : "");

        for (auto& entry : entries_)
        {
            if (entry.first == keyString)
            {
                entry.second = valueString;
                return;
            }
        }

        entries_.emplace_back(keyString, valueString);
    }

    const char* GetValue(const char* key) const
    {
        if (nullptr == key)
        {
            return nullptr;
        }

        const std::string keyString(key);
        for (const auto& entry : entries_)
        {
            if (entry.first == keyString)
            {
                return entry.second.c_str();
            }
        }

        return nullptr;
    }

    bool ContainsKey(const char* key) const
    {
        return nullptr != GetValue(key);
    }

    size_t GetCount() const
    {
        return entries_.size();
    }

    const char* GetKeyAt(const size_t index) const
    {
        if (index >= entries_.size())
        {
            return nullptr;
        }

        return entries_[index].first.c_str();
    }

    const char* GetValueAt(const size_t index) const
    {
        if (index >= entries_.size())
        {
            return nullptr;
        }

        return entries_[index].second.c_str();
    }

private:
    std::vector<std::pair<std::string, std::string>> entries_;
};

using CapabilitiesDictionary = Dictionary<const char*, const char*>;
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_DICTIONARY_H_INCL__
