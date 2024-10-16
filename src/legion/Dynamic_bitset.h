#pragma once
#include <cstdint>
#include <cstddef>
constexpr int bits_in_byte_count = 8;
class Dynamic_bitset {
    std::size_t bitsize_;
    std::size_t capacity_;
    std::uint8_t* data_;
public:
    Dynamic_bitset(std::size_t bit_count = 0) noexcept;
    ~Dynamic_bitset() noexcept;
    Dynamic_bitset(const Dynamic_bitset& a);
    Dynamic_bitset& operator=(const Dynamic_bitset& a);
    Dynamic_bitset(Dynamic_bitset&& a) noexcept;
    Dynamic_bitset& operator=(Dynamic_bitset&& a) noexcept;
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
