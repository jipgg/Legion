#pragma once
#include "Dynamic_bitset.h"
#include <vector>
#include <cstddef>
#include <functional>
#include <cassert>

template <class T>
class Lazy_pool {
    std::vector<T> data_;
    Dynamic_bitset unused_;
public:
    inline static auto default_init = [](T& v) {v = T{};};
    constexpr Lazy_pool(int initial_size = 0): data_(), unused_(initial_size) {
        if (initial_size > 0) data_.reserve(initial_size);
        unused_.set();

    }
    constexpr int32_t allocate(const std::function<void(T&)>& ctor = default_init) {
        if (unused_.none()) {
            auto& v = data_.emplace_back(T{});
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
    constexpr T& at(std::size_t address) {
        assert(not unused_.test(address));
        return data_.at(address);
    }
    constexpr const T& at(std::size_t address) const {
        assert(not unused_.test(address));
        return data_.at(address);
    }
};
