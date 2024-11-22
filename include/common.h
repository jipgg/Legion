//common
#pragma once
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <optional>
#include <utility>
#include <iostream>
#include <functional>
#include <span>
#include <string_view>
#include <filesystem>
#include <stdexcept>
#include <mutex>
#include <array>
#include <blaze/Blaze.h>
using Vec2f = blaze::StaticVector<float, 2, false, blaze::aligned, blaze::unpadded>;
using Vec2d = blaze::StaticVector<double, 2, false, blaze::aligned, blaze::unpadded>;
using vec2i16 = blaze::StaticVector<int16_t, 2, false, blaze::aligned, blaze::unpadded>;
using Vec3d = blaze::StaticVector<double, 3, false, blaze::aligned, blaze::unpadded>;
using Vec3f = blaze::StaticVector<float, 3>;
using Vec2i = blaze::StaticVector<int, 2, false, blaze::aligned, blaze::unpadded>;
using Mat3d = blaze::StaticMatrix<double, 3, 3>;
using Mat3f = blaze::StaticMatrix<float, 3, 3>;
struct ScopeGuard {
    using DeferFunction = std::function<void()>;
    DeferFunction block;
    ~ScopeGuard() {block();};
};
template <class T>
struct MutexVector {
    std::vector<T> vec;
    std::mutex mtx;
    void lazy_clear(int to_reserve = 1) {
        vec.resize(0);
        vec.reserve(to_reserve);
    }
};
//concepts
template <class Ty>
concept ComparableToNullptr = std::is_convertible_v<decltype(std::declval<Ty>() != nullptr), bool>;
template <class Ty>
concept Enum = std::is_enum_v<Ty>;
//free functions
#ifdef _WIN32
void attach_console();
void enable_ansi_escape_sequences();
#endif
[[nodiscard]] std::optional<std::string> read_file(const std::filesystem::path& path);
[[nodiscard]] std::string compile_source(std::string_view string);
[[nodiscard]] constexpr std::array<uint8_t, 4> to_lil_endian_bytes(uint32_t v) {
    constexpr int byte_bitsize = 8;
    return {
        static_cast<uint8_t>(v >> (0 * byte_bitsize) & UINT8_MAX),
        static_cast<uint8_t>(v >> (1 * byte_bitsize) & UINT8_MAX),
        static_cast<uint8_t>(v >> (2 * byte_bitsize) & UINT8_MAX),
        static_cast<uint8_t>(v >> (3 * byte_bitsize) & UINT8_MAX),
    };
}
[[nodiscard]] constexpr std::array<uint8_t, 4> to_big_endian_bytes(uint32_t v) {
    constexpr int byte_bitsize = 8;
    return {
        static_cast<uint8_t>(v >> (3 * byte_bitsize) & UINT8_MAX),
        static_cast<uint8_t>(v >> (2 * byte_bitsize) & UINT8_MAX),
        static_cast<uint8_t>(v >> (1 * byte_bitsize) & UINT8_MAX),
        static_cast<uint8_t>(v >> (0 * byte_bitsize) & UINT8_MAX),
    };
}
template <class Key, class Val>
class SparseSet {
    std::unordered_map<Key, size_t> sparse_; //maybe make a sparse array for this for better cache locality
    std::vector<Val> dense_;
public:
    constexpr Val& emplace_back(Key key, Val&& val) {
        Val& v = dense_.emplace_back(std::forward<Val&&>(val));
        const size_t index = dense_.size() - 1;
        sparse_.insert(std::make_pair(key, index));
        return v;
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
    constexpr void shrink_to_fit() {
        dense_.shrink_to_fit();
    }
    [[nodiscard]] constexpr Val& at(Key key) {
        return dense_.at(sparse_.at(key));
    }
    [[nodiscard]] constexpr Val& operator[](Key key) {
        return dense_[sparse_[key]];
    }
    [[nodiscard]] constexpr Val* at_if(Key key) {
        auto found = sparse_.find(key);
        if (found == sparse_.end()) return nullptr;
        else return &dense_.at(found->second);
    }
    [[nodiscard]] constexpr const Val& at(const Key key) const {
        return dense_.at(sparse_.at(key));
    }
    constexpr void clear() {
        dense_.clear();
        sparse_.clear();
    }
    [[nodiscard]] std::span<Val> span() {
        return dense_;
    }
    [[nodiscard]] size_t count(Key k) const {
        return sparse_.count(k);
    }
};
template <typename Key, typename Val>
class FlatMap {
public:
    using Pair = std::pair<Key, Val>;
    using Container = std::vector<Pair>;
    using Iterator = typename Container::iterator;
    using ConstIterator = typename Container::const_iterator;
    using ReverseIterator = typename Container::reverse_iterator;
    using ConstReverseIterator = typename Container::const_reverse_iterator;
    Iterator begin() { return data_.begin(); }
    ConstIterator begin() const { return data_.begin(); }
    Iterator end() { return data_.end(); }
    ConstIterator end() const { return data_.end(); }
    Iterator insert(Pair pair) {
        auto it = std::lower_bound(data_.begin(), data_.end(), pair,
            [](const auto& lhs, const auto& rhs) {
                return lhs.first < rhs.first;
            });
        return data_.insert(it, std::move(pair));
    }
    [[nodiscard]] const std::optional<std::reference_wrapper<Val>> find(const Key& key) const {
        Iterator it = search_for(key);
        if (it != data_.end() && it->first == key) {
            return std::ref(it->second);
        }
        return std::nullopt;
    }
    [[nodiscard]] Val& at(const Key& key) {
        auto it = search_for(key);
        #ifndef NDEBUG
        if (it == data_.end()) {
            throw std::out_of_range{"out of range"};
        }
        #endif
        return it->second;
    }
    [[nodiscard]] const Val& at(const Key& key) const {
        auto it = search_for(key);
        #ifndef NDEBUG
        if (it == data_.end()) {
            throw std::out_of_range{"out of range"};
        }
        #endif
        return it->second;
    }
    [[nodiscard]] Val& operator[](const Key& key) {
        if (auto found = find(key)) {
            return found.value().get();
        }
        return insert(std::make_pair(key, Val{}))->second;
    }
    void erase(const Key& key) {
        data_.erase(search_for(key));
    }
private:
    [[nodiscard]] ConstIterator search_for(const Key& key) const {
        constexpr Pair dummy = std::make_pair(key, Val{});
        return std::lower_bound(data_.begin(), data_.end(), dummy,
            [](const auto& lhs, const auto& rhs) {return lhs.first < rhs.first;});
    }
    Container data_; 
};
template <class Ty>
class FlatStack {
public:
    using Container = std::vector<Ty>;
    using Iterator = typename Container::iterator;
    using ConstIterator = typename Container::const_iterator;
    using ReverseIterator = typename Container::reverse_iterator;
    using ConstReverseIterator = typename Container::const_reverse_iterator;
    Iterator begin() { return data_.begin(); }
    ConstIterator begin() const { return data_.begin(); }
    Iterator end() { return data_.end(); }
    ConstIterator end() const { return data_.end(); }
    ReverseIterator rbegin() { return data_.rbegin(); }
    ConstReverseIterator rbegin() const { return data_.rbegin(); }
    ReverseIterator rend() { return data_.rend(); }
    ConstReverseIterator rend() const { return data_.rend(); }
    Ty& at(size_t idx) {return data_.at(idx);}
    const Ty& at(size_t idx) const {return data_.at(idx);}
    Ty& operator[](size_t idx) {return data_[idx];}
    Ty& emplace(Ty&& v) {return data_.emplace_back(std::forward<Ty&&>(v));}
    void push(const Ty& v) {data_.push_back(v);}
    void pop() {data_.pop_back();}
    Ty& top() {return data_.back();}
    size_t size() const {data_.size();}
    void reserve(size_t amount) {data_.reserve(amount);}
    bool empty() const {return data_.empty();}
    size_t capacity() const {return data_.capacity();}
private:
    std::vector<Ty> data_;
};
// constexpr structs
constexpr int bits_in_byte_count = 8;
class DynamicBitset {
    std::size_t bitsize_;
    std::size_t capacity_;
    std::uint8_t* data_;
public:
    DynamicBitset(std::size_t bit_count = 0) noexcept;
    ~DynamicBitset() noexcept;
    DynamicBitset(const DynamicBitset& a);
    DynamicBitset& operator=(const DynamicBitset& a);
    DynamicBitset(DynamicBitset&& a) noexcept;
    DynamicBitset& operator=(DynamicBitset&& a) noexcept;
    void set(std::size_t i);
    void set();
    void reset();
    void reset(std::size_t i);
    void reserve(std::size_t bit_count);
    void append(bool bit = 0);
    void shrink_to_fit();
    void resize(std::size_t bit_count);
    [[nodiscard]] std::size_t count() const;
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::size_t capacity() const;
    [[nodiscard]] bool test(std::size_t i) const;
    [[nodiscard]] bool none() const;
    [[nodiscard]] bool all() const;
private:
    void reallocate(std::size_t new_capacity);
};
template <class Ty>
class LazyPool {
    std::vector<Ty> data_;
    DynamicBitset unused_;
public:
    inline static auto default_init = [](Ty& v) {v = Ty{};};
    constexpr LazyPool(int initial_size = 0): data_(), unused_(initial_size) {
        if (initial_size > 0) data_.reserve(initial_size);
        unused_.set();

    }
    constexpr int32_t allocate(const std::function<void(Ty&)>& ctor = default_init) {
        if (unused_.none()) {
            auto& v = data_.emplace_back(Ty{});
            unused_.append(0);
            ctor(v);
            return data_.size() - 1;
        }
        for (int i{}; i < unused_.size(); ++i) {
            if (unused_.test(i)) {
                ctor(data_.at(i));
                return i;
            }
        }
        assert(false);
        return -1;
    }
    constexpr void free(std::size_t address) {
        unused_.reset(address);
    }
    constexpr Ty& at(std::size_t address) {
        assert(not unused_.test(address));
        return data_.at(address);
    }
    constexpr const Ty& at(std::size_t address) const {
        assert(not unused_.test(address));
        return data_.at(address);
    }
};
template <class T>
concept Printable = requires(T&& obj, std::ostream& os) {
    {os << obj} -> std::same_as<std::ostream&>;
};
template <Printable ...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
template <Printable ...Ts>
void printerr(Ts&&...args) {
    std::cerr << "\033[31m";//red
    ((std::cerr << args << ' '), ...) << "\033[0m\n";
}
