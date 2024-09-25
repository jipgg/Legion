#pragma once
#include <blaze/Blaze.h>
#include <cassert>
#include <memory>
#define expects(precondition) assert(precondition and "expects")
#define ensures(postcondition) assert(postcondition and "ensures")
using V2 = blaze::StaticVector<float, 2>;
using M3x3 = blaze::StaticMatrix<float, 3, 2>;
struct Rect {V2 offset; float width, height;};
template <class T>
concept Comparable_to_nullptr = std::is_convertible_v<decltype(std::declval<T>() != nullptr), bool>;
template <Comparable_to_nullptr T>
class NotNull {
    T ptr_;
public:
    constexpr NotNull(std::nullptr_t) = delete;
    constexpr NotNull(T ptr) noexcept: ptr_(ptr) {
        expects(ptr != nullptr);
    };
    constexpr NotNull(const std::unique_ptr<T>& ptr) noexcept: NotNull(ptr.get()) {}
    constexpr T get() const {
        expects(ptr_);
        return ptr_;
    };
    constexpr operator T() const {
        return get();
    }
    constexpr T operator->() {
        return ptr_;
    }
};
