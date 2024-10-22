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
#include <array>
#include <blaze/Blaze.h>
namespace legion {
#define EXPECTS(precondition) assert(precondition and "unexpected.")
#define ENSURES(postcondition) assert(postcondition and "was not ensured.")
using Vec2f = blaze::StaticVector<float, 2>;
using Vec2d = blaze::StaticVector<double, 2>;
using Vec2i16 = blaze::StaticVector<int16_t, 2>;
using Vec2i = blaze::StaticVector<int, 2>;
struct defer {
    std::function<void()> f;
    ~defer() {f();};
};
//concepts
template <class T>
concept Comparable_to_nullptr = std::is_convertible_v<decltype(std::declval<T>() != nullptr), bool>;
template <class T>
concept Enum = std::is_enum_v<T>;
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

//classes
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
    constexpr void shrink_to_fit() {
        dense_.shrink_to_fit();
    }
    [[nodiscard]] constexpr Val& at(Key key) {
        return dense_.at(sparse_.at(key));
    }
    [[nodiscard]] constexpr Val& operator[](Key key) {
        return dense_[sparse_[key]];
    }
    [[nodiscard]] constexpr std::optional<std::reference_wrapper<Val>> at_if(Key key) {
        auto found = sparse_.find(key);
        if (found == sparse_.end()) return std::nullopt;
        else return dense_.at(found->second);
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
template <typename Key_t, typename Val_t>
class Flat_map {
public:
    using Pair_t = std::pair<Key_t, Val_t>;
    using Container_t = std::vector<Pair_t>;
    using Iterator = typename Container_t::iterator;
    using Const_iterator = typename Container_t::const_iterator;
    using Reverse_iterator = typename Container_t::reverse_iterator;
    using Const_reverse_iterator = typename Container_t::const_reverse_iterator;
    Iterator begin() { return data_.begin(); }
    Const_iterator begin() const { return data_.begin(); }
    Iterator end() { return data_.end(); }
    Const_iterator end() const { return data_.end(); }
    Iterator insert(Pair_t pair) {
        auto it = std::lower_bound(data_.begin(), data_.end(), pair,
            [](const auto& lhs, const auto& rhs) {
                return lhs.first < rhs.first;
            });
        return data_.insert(it, std::move(pair));
    }
    [[nodiscard]] const std::optional<std::reference_wrapper<Val_t>> find(const Key_t& key) const {
        Iterator it = search_for(key);
        if (it != data_.end() && it->first == key) {
            return std::ref(it->second);
        }
        return std::nullopt;
    }
    [[nodiscard]] Val_t& at(const Key_t& key) {
        auto it = search_for(key);
        #ifndef NDEBUG
        if (it == data_.end()) {
            throw std::out_of_range{"out of range"};
        }
        #endif
        return it->second;
    }
    [[nodiscard]] const Val_t& at(const Key_t& key) const {
        auto it = search_for(key);
        #ifndef NDEBUG
        if (it == data_.end()) {
            throw std::out_of_range{"out of range"};
        }
        #endif
        return it->second;
    }
    [[nodiscard]] Val_t& operator[](const Key_t& key) {
        if (auto found = find(key)) {
            return found.value().get();
        }
        return insert(std::make_pair(key, Val_t{}))->second;
    }
    void erase(const Key_t& key) {
        data_.erase(search_for(key));
    }
private:
    [[nodiscard]] Const_iterator search_for(const Key_t& key) const {
        constexpr Pair_t dummy = std::make_pair(key, Val_t{});
        return std::lower_bound(data_.begin(), data_.end(), dummy,
            [](const auto& lhs, const auto& rhs) {return lhs.first < rhs.first;});
    }
    Container_t data_; 
};
template <class T>
class Flat_stack {
public:
    using Container_t = std::vector<T>;
    using Iterator = typename Container_t::iterator;
    using Const_iterator = typename Container_t::const_iterator;
    using Reverse_iterator = typename Container_t::reverse_iterator;
    using Const_reverse_iterator = typename Container_t::const_reverse_iterator;
    Iterator begin() { return data_.begin(); }
    Const_iterator begin() const { return data_.begin(); }
    Iterator end() { return data_.end(); }
    Const_iterator end() const { return data_.end(); }
    Reverse_iterator rbegin() { return data_.rbegin(); }
    Const_reverse_iterator rbegin() const { return data_.rbegin(); }
    Reverse_iterator rend() { return data_.rend(); }
    Const_reverse_iterator rend() const { return data_.rend(); }
    T& at(size_t idx) {return data_.at(idx);}
    const T& at(size_t idx) const {return data_.at(idx);}
    T& operator[](size_t idx) {return data_[idx];}
    T& emplace(T&& v) {return data_.emplace_back(std::forward<T&&>(v));}
    void push(const T& v) {data_.push_back(v);}
    void pop() {data_.pop_back();}
    T& top() {return data_.back();}
    size_t size() const {data_.size();}
    void reserve(size_t amount) {data_.reserve(amount);}
    bool empty() const {return data_.empty();}
    size_t capacity() const {return data_.capacity();}
private:
    std::vector<T> data_;
};
// constexpr structs
struct Recti64 {
    int64_t data;
    [[nodiscard]] constexpr Recti64(int16_t x, int16_t y, int16_t width, int16_t height): data(0) {
        data |= static_cast<int64_t>(x) << x_s
            | static_cast<int64_t>(y) << y_s
            | static_cast<int64_t>(width) << width_s
            | static_cast<int64_t>(height) << height_s;
    }
    [[nodiscard]] constexpr int16_t x() const {return data >> x_s & INT16_MAX;}
    [[nodiscard]] constexpr int16_t y() const {return data >> y_s & INT16_MAX;}
    [[nodiscard]] constexpr int16_t width() const {return data >> width_s & INT16_MAX;}
    [[nodiscard]] constexpr int16_t height() const {return data >> height_s & INT16_MAX;}
private:
    static constexpr int bitsize = 16;
    static constexpr int x_s = 0 * bitsize;
    static constexpr int y_s = 1 * bitsize;
    static constexpr int width_s = 2 * bitsize;
    static constexpr int height_s = 3 * bitsize;
};
struct Coloru32 {
    uint32_t data;
    [[nodiscard]] constexpr Coloru32(uint32_t color): data(color) {}
    [[nodiscard]] constexpr Coloru32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        uint32_t color{0};
        color |= r << red_s;
        color |= g << green_s;
        color |= b << blue_s;
        color |= a << alpha_s;
        data = color;
    }
    [[nodiscard]] constexpr uint8_t red() const {return data >> red_s & UINT8_MAX;}
    [[nodiscard]] constexpr uint8_t green() const {return data >> green_s & UINT8_MAX;}
    [[nodiscard]] constexpr uint8_t blue() const {return data >> blue_s & UINT8_MAX;}
    [[nodiscard]] constexpr uint8_t alpha() const {return data >> alpha_s & UINT8_MAX;}
private:
    static constexpr int bitsize = 8;
    static constexpr int red_s = 3 * bitsize;
    static constexpr int green_s = 2 * bitsize;
    static constexpr int blue_s = 1 * bitsize;
    static constexpr int alpha_s = 0 * bitsize;
};
struct Sizei32 {
    int32_t data;
    [[nodiscard]] constexpr Sizei32(int32_t size): data(size) {}
    [[nodiscard]] constexpr Sizei32(int16_t width, int16_t height): data(width | height << shift_increment) {}
    [[nodiscard]] constexpr int16_t width() const {return static_cast<int16_t>(data);}
    [[nodiscard]] constexpr int16_t height() const {return data >> shift_increment & 0xffffi16;}
private:
    static constexpr int shift_increment = 16;
};
template <class ...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
template <class ...Ts>
void printerr(Ts&&...args) {
    std::cerr << "\033[31m";//red
    ((std::cerr << args << ' '), ...) << "\033[0m\n";
}
}
