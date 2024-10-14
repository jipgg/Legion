#include "common.h"
#include <cstring>
#include <cassert>
#include <bit>
Dynamic_bitset::Dynamic_bitset(size_t bitsize) noexcept:
    bitsize_(bitsize),
    capacity_(calc_total_bytes_needed(bitsize)),
    data_(new uint8_t[capacity_]) {
}

Dynamic_bitset::~Dynamic_bitset() noexcept {
    delete[] data_;
}
Dynamic_bitset::Dynamic_bitset(const Dynamic_bitset& a):
    bitsize_(a.bitsize_),
    capacity_(a.capacity_) {
    delete[] data_;
    data_ = new uint8_t[capacity_];
    std::memcpy(data_, a.data_, a.capacity_);
}
Dynamic_bitset& Dynamic_bitset::operator=(const Dynamic_bitset& a) {
    delete[] data_;
    bitsize_ = a.bitsize_;
    capacity_ = a.capacity_;
    data_ = new uint8_t[capacity_];
    std::memcpy(data_, a.data_, a.capacity_);
    return *this;
}
Dynamic_bitset::Dynamic_bitset(Dynamic_bitset&& a) noexcept:
    bitsize_(a.bitsize_),
    capacity_(a.capacity_) {
    delete[] data_;
    data_ = a.data_;
    a.data_ = nullptr;
    a.bitsize_ = 0;
    a.capacity_ = 0;
}
Dynamic_bitset& Dynamic_bitset::operator=(Dynamic_bitset&& a) noexcept {
    delete[] data_;
    data_ = a.data_;
    a.data_ = nullptr;
    bitsize_ = a.bitsize_;
    a.bitsize_ = 0;
    capacity_ = a.capacity_;
    a.capacity_ = 0;
    return *this;
}

void Dynamic_bitset::set(size_t i) {
    assert(i < bitsize_);
    static constexpr int byte_size = 8;
    data_[i / byte_size] |= 1 << (i % byte_size);
}
bool Dynamic_bitset::test(size_t i) const {
    const uint8_t mask = 1 << (i % bits_in_byte_count);
    return (data_[i / bits_in_byte_count] & mask) != 0;
}
bool Dynamic_bitset::none() const {
    const size_t total_bytes = calc_total_bytes_needed(bitsize_);
    constexpr int chunk_size = sizeof(uint64_t);
    const size_t bulk_count = total_bytes / chunk_size;
    size_t remaining_bytes = total_bytes % chunk_size;
    const uint64_t bulk_reset_mask = 0;
    for (size_t i{}; i < bulk_count; ++i) {
        if (std::memcmp(&data_[i * chunk_size], &bulk_reset_mask, chunk_size) != 0) {
            return false;
        }
    }
    if (remaining_bytes > 0) {
        const size_t bytes_start = bulk_count * chunk_size; 
        const size_t maybe_last_byte_idx = total_bytes - 1;
        for (size_t i = bytes_start; i < maybe_last_byte_idx; ++i) {
            if (data_[i] != 0) {
                return false;
            }
        }
        const int remaining_bits = bitsize_ % 8;//8 bits
        if (remaining_bits > 0) {
            uint8_t last_byte = data_[maybe_last_byte_idx];
            uint8_t bits_set_mask = (1 << remaining_bits) - 1;
            if ((last_byte & bits_set_mask) != 0) {
                return false;
            }
        }
    }
    return true;
}
bool Dynamic_bitset::all() const {
    const size_t total_bytes = calc_total_bytes_needed(bitsize_);
    constexpr int chunk_size = sizeof(uint64_t);
    const size_t bulk_count = total_bytes / chunk_size;
    size_t remaining_bytes = total_bytes % chunk_size;
    const uint64_t bulk_set_mask = UINT64_MAX;
    for (size_t i{}; i < bulk_count; ++i) {
        if (std::memcmp(&data_[i * chunk_size], &bulk_set_mask, chunk_size) != 0) {
            return false;
        }
    }
    if (remaining_bytes > 0) {
        const size_t bytes_start = bulk_count * chunk_size; 
        const size_t maybe_last_byte_idx = total_bytes - 1;
        for (size_t i = bytes_start; i < maybe_last_byte_idx; ++i) {
            if (data_[i] != UINT8_MAX) {
                return false;
            }
        }
        const int remaining_bits = bitsize_ % 8;//8 bits
        if (remaining_bits > 0) {
            uint8_t last_byte = data_[maybe_last_byte_idx];
            uint8_t bits_set_mask = (1 << remaining_bits) - 1;
            if ((last_byte & bits_set_mask) != bits_set_mask) {
                return false;
            }
        }
    }
    return true;
}
size_t Dynamic_bitset::size() const {
    return bitsize_;
}
void Dynamic_bitset::set() {
    std::memset(data_, 0xff, capacity_);
}
void Dynamic_bitset::reset() {
    std::memset(data_, 0x00, capacity_);
}
void Dynamic_bitset::reset(size_t i) {
    assert(i < bitsize_);
    data_[i / bits_in_byte_count] &= ~(0b1 << (i % bits_in_byte_count));
}
void Dynamic_bitset::reserve(size_t bitcount) {
    const size_t new_capacity = calc_total_bytes_needed(bitsize_) + calc_total_bytes_needed(bitcount); 
    if (new_capacity > capacity_) reallocate(new_capacity);
}
void Dynamic_bitset::append(bool bit) {
    const size_t new_size = calc_total_bytes_needed(bitsize_ + 1);
    if (new_size > capacity_) reallocate(new_size);
    return bit ? set(bitsize_++) : reset(bitsize_++);
}
void Dynamic_bitset::shrink_to_fit() {
    const size_t size = calc_total_bytes_needed(bitsize_);
    if (size < capacity_) reallocate(size);
}
void Dynamic_bitset::resize(size_t bitcount) {
    const size_t size = calc_total_bytes_needed(bitcount);
    if (size > capacity_) reallocate(size);
    bitsize_ = bitcount;
}
size_t Dynamic_bitset::capacity() const {
    return capacity_;
}
void Dynamic_bitset::reallocate(size_t new_capacity) {
    uint8_t* temp = new uint8_t[new_capacity];
    std::memcpy(temp, data_, capacity_);
    delete[] data_;
    data_ = temp;
    capacity_ = new_capacity;
}
size_t Dynamic_bitset::count() const {
    const size_t total_bytes = calc_total_bytes_needed(bitsize_);
    const size_t remaining_bits = bitsize_ % 8;
    size_t count = 0;
    for (size_t i{}; i < total_bytes - 1; ++i) {
        count += std::popcount(data_[i]);
    }
    if (remaining_bits > 0) {
        for (size_t i{bitsize_ - remaining_bits}; i < bitsize_; ++i) {
            count += test(i);
        }
        return count;
    }
    count += std::popcount(data_[total_bytes - 1]);
    return count;
}
