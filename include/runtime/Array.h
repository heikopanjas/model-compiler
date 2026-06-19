#ifndef __RUNTIME_ARRAY_H_INCL__
#define __RUNTIME_ARRAY_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace runtime {
/// \brief Simple indexed array for runtime values
template<typename TValue> class Array
{
public:
    Array() = default;

    Array(const Array& other)                = default;
    Array& operator=(const Array& other)     = default;
    Array(Array&& other) noexcept            = default;
    Array& operator=(Array&& other) noexcept = default;

    virtual ~Array() = default;

    void AddValue(TValue value)
    {
        values_.push_back(std::move(value));
    }

    bool SetValueAt(const size_t index, TValue value)
    {
        if (index >= values_.size())
        {
            return false;
        }

        values_[index] = std::move(value);
        return true;
    }

    TValue GetValueAt(const size_t index) const
    {
        if (index >= values_.size())
        {
            return TValue{};
        }

        return values_[index];
    }

    TValue& At(const size_t index)
    {
        return values_.at(index);
    }

    const TValue& At(const size_t index) const
    {
        return values_.at(index);
    }

    size_t GetCount() const
    {
        return values_.size();
    }

    void Clear()
    {
        values_.clear();
    }

    typename std::vector<TValue>::iterator begin()
    {
        return values_.begin();
    }

    typename std::vector<TValue>::iterator end()
    {
        return values_.end();
    }

    typename std::vector<TValue>::const_iterator begin() const
    {
        return values_.begin();
    }

    typename std::vector<TValue>::const_iterator end() const
    {
        return values_.end();
    }

private:
    std::vector<TValue> values_;
};

/// \brief ABI-friendly string array used by generator plugin metadata
template<> class Array<const char*>
{
public:
    Array() = default;

    Array(const Array& other)                = default;
    Array& operator=(const Array& other)     = default;
    Array(Array&& other) noexcept            = default;
    Array& operator=(Array&& other) noexcept = default;

    virtual ~Array() = default;

    void AddValue(const char* value)
    {
        values_.emplace_back(nullptr != value ? value : "");
    }

    bool SetValueAt(const size_t index, const char* value)
    {
        if (index >= values_.size())
        {
            return false;
        }

        values_[index] = (nullptr != value ? value : "");
        return true;
    }

    const char* GetValueAt(const size_t index) const
    {
        if (index >= values_.size())
        {
            return nullptr;
        }

        return values_[index].c_str();
    }

    size_t GetCount() const
    {
        return values_.size();
    }

    void Clear()
    {
        values_.clear();
    }

private:
    std::vector<std::string> values_;
};

using StringArray = Array<const char*>;
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_ARRAY_H_INCL__
