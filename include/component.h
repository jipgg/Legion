#pragma once
#include "common.h"  
namespace component {
using component_id = std::size_t;
constexpr const component_id nullid = 0;
template <class Ty>
[[nodiscard]] sparse_set<component_id, Ty>& storage() {
    static sparse_set<component_id, Ty> comps{};
    return comps;
}
[[nodiscard]] inline component_id create_unique_id() {
    static component_id curr_id{0};
    return ++curr_id;
}
template <class Ty, class ...Init_arguments_t>
[[nodiscard]] component_id make(Init_arguments_t&&...init) {
    const auto id = create_unique_id();
    storage<Ty>().emplace_back(id, Ty{std::forward<Init_arguments_t>(init)...});
    return id;
}
template <class Ty>
[[nodiscard]] Ty& get(component_id id) {
    return storage<Ty>().at(id);
}
template <class Ty>
[[nodiscard]] Ty* get_if(component_id id) {
    return storage<Ty>().at_if(id);
}
template <class Ty>
bool destroy(component_id id) {
    if (storage<Ty>().at_if(id)) {
        storage<Ty>().erase(id);
        return true;
    }
    return false;
}
template <class Ty>
using storage_view = std::span<Ty>;
template <class Ty>
storage_view<Ty> view() {
    return storage<Ty>().span();
}
template <class Ty>
struct raii_wrapper {
    component_id id;
    raii_wrapper(const raii_wrapper&) = delete;
    raii_wrapper& operator=(const raii_wrapper&) = delete;
    raii_wrapper(raii_wrapper&& other) noexcept: id(other.id) {other.id = nullid;}
    raii_wrapper& operator=(raii_wrapper&& other) noexcept {
        id = other.id;
        other.id = nullid;
        return *this;
    }
    raii_wrapper(Ty&& c): id(make<Ty>(std::forward<Ty>(c))) {}
    ~raii_wrapper() {
        if (id != nullid) {
            destroy();
            //common::print("destroyed component", id, typeid(Ty).name());
        } 
    }
    void destroy() {component::destroy<Ty>(id);}
    Ty& get() {return component::get<Ty>(id);}
    Ty* get_if() {return component::get_if<Ty>(id);}
};
}
