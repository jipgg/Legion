#pragma once
#include <cassert>
#include <utility>
template <class Value_type, class Error_type>
class Expected {
    enum class Tag {Error, Valid};
    union Union{Value_type val; Error_type err;};
    const Tag tag_;
    Union value_;
public:
    [[nodiscard]] constexpr Expected(Value_type value): tag_(Tag::Valid) {
        new(&value_.val) Value_type(value);
    }
    constexpr Expected(Error_type error): tag_(Tag::Error) {
        new(&value_.err) Error_type(error);
    }
    ~Expected() {
        switch (tag_) {
            case Tag::Valid:
                value_.val.~T();
            break;
            case Tag::Error:
                value_.err.~E();
            break;
        }
    }
    constexpr bool valid() const {return tag_ == Tag::Valid;}
    constexpr operator bool() const {return valid();}
    constexpr Value_type& value() {
        assert(tag_ == Tag::Valid);
        return value_.val;
    }
    constexpr const Value_type& value() const {
        assert(tag_ == Tag::Valid);
        return value_.val;
    }
    constexpr Error_type& error() {
        assert(tag_ == Tag::Error);
        return value_.err;
    }
    constexpr const Error_type& error() const {
        assert(tag_ == Tag::Error);
        return value_.err;
    }
    constexpr Value_type* operator->() {
        assert(tag_ == Tag::Valid);
        return &value_.val;
    }
    constexpr Value_type& operator*() {
        return value();
    }
    constexpr const Value_type& operator*() const {
        return value();
    }
    constexpr const Value_type& value_or(const Value_type& default_value) const {
        return valid() ? value() : default_value;
    }
    constexpr operator Value_type() const {
        return value();
    }
};
template <class Error_type>
class Expected<void, Error_type> {
    enum class Tag { Error, Valid };
    const Tag tag_;
    Error_type err_;

public:
    constexpr Expected(Error_type err) : tag_(Tag::Error), err_(std::move(err)) {}
    //constexpr Expected(void) : tag_(Tag::Valid) {}
    constexpr Expected(std::nullptr_t) : tag_(Tag::Valid) {}
    constexpr bool valid() const { return tag_ == Tag::Valid; }
    constexpr operator bool() const { return valid(); }
    constexpr Error_type& error() {
        assert(tag_ == Tag::Error);
        return err_;
    }
    constexpr const Error_type& error() const {
        assert(tag_ == Tag::Error);
        return err_;
    }
};
template <class E>
constexpr E unexpected(E&& err) {
    return std::forward<E>(err);
}
