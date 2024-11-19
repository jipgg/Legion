#include "common.h"
#include <cstring>
#include <cassert>
#include <bit>
static constexpr size_t calc_total_bytes_needed(size_t bit_count) {
    return (bit_count + (bits_in_byte_count - 1)) / bits_in_byte_count;
}
DynamicBitset::DynamicBitset(size_t bitsize) noexcept:
    bitsize_(bitsize),
    capacity_(calc_total_bytes_needed(bitsize)),
    data_(new uint8_t[capacity_]) {
}

DynamicBitset::~DynamicBitset() noexcept {
    delete[] data_;
}
DynamicBitset::DynamicBitset(const DynamicBitset& a):
    bitsize_(a.bitsize_),
    capacity_(a.capacity_) {
    delete[] data_;
    data_ = new uint8_t[capacity_];
    std::memcpy(data_, a.data_, a.capacity_);
}
DynamicBitset& DynamicBitset::operator=(const DynamicBitset& a) {
    delete[] data_;
    bitsize_ = a.bitsize_;
    capacity_ = a.capacity_;
    data_ = new uint8_t[capacity_];
    std::memcpy(data_, a.data_, a.capacity_);
    return *this;
}
DynamicBitset::DynamicBitset(DynamicBitset&& a) noexcept:
    bitsize_(a.bitsize_),
    capacity_(a.capacity_) {
    delete[] data_;
    data_ = a.data_;
    a.data_ = nullptr;
    a.bitsize_ = 0;
    a.capacity_ = 0;
}
DynamicBitset& DynamicBitset::operator=(DynamicBitset&& a) noexcept {
    delete[] data_;
    data_ = a.data_;
    a.data_ = nullptr;
    bitsize_ = a.bitsize_;
    a.bitsize_ = 0;
    capacity_ = a.capacity_;
    a.capacity_ = 0;
    return *this;
}

void DynamicBitset::set(size_t i) {
    assert(i < bitsize_);
    static constexpr int byte_size = 8;
    data_[i / byte_size] |= 1 << (i % byte_size);
}
bool DynamicBitset::test(size_t i) const {
    const uint8_t mask = 1 << (i % bits_in_byte_count);
    return (data_[i / bits_in_byte_count] & mask) != 0;
}
bool DynamicBitset::none() const {
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
bool DynamicBitset::all() const {
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
size_t DynamicBitset::size() const {
    return bitsize_;
}
void DynamicBitset::set() {
    std::memset(data_, 0xff, capacity_);
}
void DynamicBitset::reset() {
    std::memset(data_, 0x00, capacity_);
}
void DynamicBitset::reset(size_t i) {
    assert(i < bitsize_);
    data_[i / bits_in_byte_count] &= ~(0b1 << (i % bits_in_byte_count));
}
void DynamicBitset::reserve(size_t bitcount) {
    const size_t new_capacity = calc_total_bytes_needed(bitsize_) + calc_total_bytes_needed(bitcount); 
    if (new_capacity > capacity_) reallocate(new_capacity);
}
void DynamicBitset::append(bool bit) {
    const size_t new_size = calc_total_bytes_needed(bitsize_ + 1);
    if (new_size > capacity_) reallocate(new_size);
    return bit ? set(bitsize_++) : reset(bitsize_++);
}
void DynamicBitset::shrink_to_fit() {
    const size_t size = calc_total_bytes_needed(bitsize_);
    if (size < capacity_) reallocate(size);
}
void DynamicBitset::resize(size_t bitcount) {
    const size_t size = calc_total_bytes_needed(bitcount);
    if (size > capacity_) reallocate(size);
    bitsize_ = bitcount;
}
size_t DynamicBitset::capacity() const {
    return capacity_;
}
void DynamicBitset::reallocate(size_t new_capacity) {
    uint8_t* temp = new uint8_t[new_capacity];
    std::memcpy(temp, data_, capacity_);
    delete[] data_;
    data_ = temp;
    capacity_ = new_capacity;
}
size_t DynamicBitset::count() const {
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
