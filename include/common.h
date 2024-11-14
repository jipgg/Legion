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
#include <array>
#include <blaze/Blaze.h>
using vec2f = blaze::StaticVector<float, 2, false, blaze::aligned, blaze::unpadded>;
using vec2d = blaze::StaticVector<double, 2, false, blaze::aligned, blaze::unpadded>;
using vec2i16 = blaze::StaticVector<int16_t, 2, false, blaze::aligned, blaze::unpadded>;
using vec3d = blaze::StaticVector<double, 3, false, blaze::aligned, blaze::unpadded>;
using vec3f = blaze::StaticVector<float, 3>;
using vec2i = blaze::StaticVector<int, 2, false, blaze::aligned, blaze::unpadded>;
using mat3x3 = blaze::StaticMatrix<double, 3, 3>;
using mat3f = blaze::StaticMatrix<float, 3, 3>;
struct deferred {
    using defer_function = std::function<void()>;
    defer_function block;
    ~deferred() {block();};
};
//concepts
template <class Ty>
concept comparable_to_nullptr = std::is_convertible_v<decltype(std::declval<Ty>() != nullptr), bool>;
template <class Ty>
concept an_enum = std::is_enum_v<Ty>;
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
class sparse_set {
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
class flat_map {
public:
    using pair_t = std::pair<Key, Val>;
    using container_t = std::vector<pair_t>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
    using reverse_iterator = typename container_t::reverse_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;
    iterator begin() { return data_.begin(); }
    const_iterator begin() const { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator end() const { return data_.end(); }
    iterator insert(pair_t pair) {
        auto it = std::lower_bound(data_.begin(), data_.end(), pair,
            [](const auto& lhs, const auto& rhs) {
                return lhs.first < rhs.first;
            });
        return data_.insert(it, std::move(pair));
    }
    [[nodiscard]] const std::optional<std::reference_wrapper<Val>> find(const Key& key) const {
        iterator it = search_for(key);
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
    [[nodiscard]] const_iterator search_for(const Key& key) const {
        constexpr pair_t dummy = std::make_pair(key, Val{});
        return std::lower_bound(data_.begin(), data_.end(), dummy,
            [](const auto& lhs, const auto& rhs) {return lhs.first < rhs.first;});
    }
    container_t data_; 
};
template <class Ty>
class flat_stack {
public:
    using container_t = std::vector<Ty>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
    using reverse_iterator = typename container_t::reverse_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;
    iterator begin() { return data_.begin(); }
    const_iterator begin() const { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator end() const { return data_.end(); }
    reverse_iterator rbegin() { return data_.rbegin(); }
    const_reverse_iterator rbegin() const { return data_.rbegin(); }
    reverse_iterator rend() { return data_.rend(); }
    const_reverse_iterator rend() const { return data_.rend(); }
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
struct recti64 {
    int64_t data;
    [[nodiscard]] constexpr recti64(int16_t x, int16_t y, int16_t width, int16_t height): data(0) {
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
struct coloru32 {
    uint32_t data;
    [[nodiscard]] constexpr coloru32(uint32_t color): data(color) {}
    [[nodiscard]] constexpr coloru32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
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
constexpr int bits_in_byte_count = 8;
class dynamic_bitset {
    std::size_t bitsize_;
    std::size_t capacity_;
    std::uint8_t* data_;
public:
    dynamic_bitset(std::size_t bit_count = 0) noexcept;
    ~dynamic_bitset() noexcept;
    dynamic_bitset(const dynamic_bitset& a);
    dynamic_bitset& operator=(const dynamic_bitset& a);
    dynamic_bitset(dynamic_bitset&& a) noexcept;
    dynamic_bitset& operator=(dynamic_bitset&& a) noexcept;
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
class lazy_pool {
    std::vector<Ty> data_;
    dynamic_bitset unused_;
public:
    inline static auto default_init = [](Ty& v) {v = Ty{};};
    constexpr lazy_pool(int initial_size = 0): data_(), unused_(initial_size) {
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
template <class ...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
template <class ...Ts>
void printerr(Ts&&...args) {
    std::cerr << "\033[31m";//red
    ((std::cerr << args << ' '), ...) << "\033[0m\n";
}
