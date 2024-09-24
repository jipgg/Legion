#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <filesystem>
#include <string_view>
#include <optional>
#include <unordered_map>
#include <vector>
#include <utility>
#include <span>
namespace util {
template <class Key, class Val>
class SparseSet {
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
    constexpr Val& at(Key key) {
        return dense_.at(sparse_.at(key));
    }
    constexpr Val& operator[](Key key) {
        return dense_[sparse_[key]];
    }
    constexpr Val* find(Key key) {
        auto found = sparse_.find(key);
        return found == sparse_.end() ? nullptr : &(dense_.at(found->second));
    }
    constexpr const Val& at(const Key key) const {
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
[[nodiscard]] std::optional<std::string> read_file(const std::filesystem::path& path);
[[nodiscard]] std::string compile_source(std::string_view string);
#ifdef _WIN32
void attach_console();
#endif
}
#endif
