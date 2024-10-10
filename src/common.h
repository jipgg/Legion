#pragma once
#include <blaze/Blaze.h>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <span>
#include <optional>
#include <filesystem>
#define EXPECTS(precondition) assert(precondition and "expects")
#define ENSURES(postcondition) assert(postcondition and "ensures")
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using uint = unsigned int;
using ulong = unsigned long;
using ushort = unsigned short;
using usize = std::size_t;
using V2 = blaze::StaticVector<float, 2>;
using M3x3 = blaze::StaticMatrix<float, 3, 2>;
using V2i16 = blaze::StaticVector<i16, 2>;
using V2i = blaze::StaticVector<int, 2>;
constexpr int bits_in_byte_count = 8;
//concepts
template <class T>
concept Comparable_to_nullptr = std::is_convertible_v<decltype(std::declval<T>() != nullptr), bool>;
//free functions
#ifdef _WIN32
void attach_console();
#endif
[[nodiscard]] std::optional<std::string> read_file(const std::filesystem::path& path);
[[nodiscard]] std::string compile_source(std::string_view string);
[[nodiscard]] constexpr usize calc_total_bytes_needed(usize bit_count) {
    return (bit_count + (bits_in_byte_count - 1)) / bits_in_byte_count;
}
//classes
class Dynamic_bitset {
    usize bitsize_;
    usize capacity_;
    u8* data_;
public:
    Dynamic_bitset(usize bit_count = 0) noexcept;
    ~Dynamic_bitset() noexcept;
    Dynamic_bitset(const Dynamic_bitset& a);
    Dynamic_bitset& operator=(const Dynamic_bitset& a);
    Dynamic_bitset(Dynamic_bitset&& a) noexcept;
    Dynamic_bitset& operator=(Dynamic_bitset&& a) noexcept;
    void set(usize i);
    void set();
    void reset();
    void reset(usize i);
    void reserve(usize bit_count);
    void append(bool bit = 0);
    void shrink_to_fit();
    void resize(usize bit_count);
    usize count() const;
    [[nodiscard]] usize size() const;
    [[nodiscard]] usize capacity() const;
    [[nodiscard]] bool test(usize i) const;
    [[nodiscard]] bool none() const;
    [[nodiscard]] bool all() const;
private:
    void reallocate(usize new_capacity);
};
//templated classes
template <class Key, class Val>
class Sparse_set {
    std::unordered_map<Key, size_t> sparse_; //maybe make a sparse array for this for better cache locality
    std::vector<Val> dense_;
public:
    constexpr void emplace_back(Key key, Val&& val) {
        dense_.emplace_back(std::forward<Val&&>(val));
        const size_t index = dense_.size() - 1;
        sparse_.insert(std::make_pair(key, index));
    }
    constexpr void push_back(Key key, const Val& val) {
        emplace_back(key, Val{val});
    }
    constexpr void erase(Key key) {
        auto it = sparse_.find(key);
        if (it == sparse_.end()) {
            return;
        }
        const size_t idx = it->second;
        std::swap(dense_[idx], dense_.back());
        dense_.pop_back();
        sparse_.erase(key);
    }
    [[nodiscard]] constexpr Val& at(Key key) {
        return dense_.at(sparse_.at(key));
    }
    [[nodiscard]] constexpr Val& operator[](Key key) {
        return dense_[sparse_[key]];
    }
    [[nodiscard]] constexpr std::optional<std::reference_wrapper<Val>> at_if(Key key) {
        auto found = sparse_.find(key);
        return found == sparse_.end() ? std::nullopt : dense_.at(found->second);
    }
    [[nodiscard]] constexpr const Val& at(const Key key) const {
        return dense_.at(sparse_.at(key));
    }
    constexpr void clear() {
        dense_.clear();
        sparse_.clear();
    }
    std::span<Val> span() {
        return dense_;
    }
    size_t count(Key k) const {
        return sparse_.count(k);
    }
};
