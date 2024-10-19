#pragma once
#include <source_location>
#include <string>
#include <type_traits>
#include <utility>
#include <unordered_map>
template <class T>
concept Enum = std::is_enum_v<T>;
struct Enum_info {
    std::string type;
    std::string value;
    std::string raw;
    int index;
};
template <Enum type, type value>
constexpr Enum_info enum_info() {
    const std::string str{std::source_location::current().function_name()};
    const std::string to_find{"value = "};
    auto found = str.find(to_find);
    std::string e = str.substr(found + to_find.size());
    e.pop_back();
    const std::string e_t = e.substr(0, e.find("::"));
    const std::string e_i = e.substr(e.find("::") + 2);
    return {
        .type = e_t,
        .value = e_i,
        .raw = e,
        .index = int(value),
    };
}
template<std::size_t N>
struct comptime { static const constexpr auto v = N; };
template <class F, std::size_t... Is>
constexpr void comptime_for(F func, std::index_sequence<Is...>) {
  (func(comptime<Is>{}), ...);
}
template <std::size_t N, typename Fn>
constexpr void comptime_for(Fn func) {
  comptime_for(func, std::make_index_sequence<N>());
}
template <Enum Enum_t>
std::unordered_map<Enum_t, Enum_info> unordered_map() {
    std::unordered_map<Enum_t, Enum_info> map{};
    comptime_for<size_t(Enum_t::_count)>([](auto i){
        map.insert({Enum_t(i), enum_info<Enum_t>()});
    });
    return map;
}
