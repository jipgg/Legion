#pragma once
#include "common.h"  
namespace component {
using Id = std::size_t;
constexpr const Id nullid = 0;
template <class Component_t>
[[nodiscard]] common::Sparse_set<Id, Component_t>& storage() {
    static common::Sparse_set<Id, Component_t> comps{};
    return comps;
}
[[nodiscard]] inline Id create_unique_id() {
    static Id curr_id{0};
    return ++curr_id;
}
template <class Component_t, class ...Init_arguments_t>
[[nodiscard]] Id make(Init_arguments_t&&...init) {
    const auto id = create_unique_id();
    storage<Component_t>().emplace_back(id, Component_t{std::forward<Init_arguments_t>(init)...});
    return id;
}
template <class Component_t>
[[nodiscard]] Component_t& get(Id id) {
    return storage<Component_t>().at(id);
}
template <class Component_t>
[[nodiscard]] Component_t* get_if(Id id) {
    return storage<Component_t>().at_if(id);
}
template <class Component_t>
bool destroy(Id id) {
    if (storage<Component_t>().at_if(id)) {
        storage<Component_t>().erase(id);
        return true;
    }
    return false;
}
template <class Component_t>
using View = std::span<Component_t>;
template <class Component_t>
View<Component_t> view() {
    return storage<Component_t>().span();
}
template <class Component_t>
struct Wrapper {
    Id id;
    Wrapper(const Wrapper&) = delete;
    Wrapper& operator=(const Wrapper&) = delete;
    Wrapper(Wrapper&& other) noexcept: id(other.id) {other.id = nullid;}
    Wrapper& operator=(Wrapper&& other) noexcept {
        id = other.id;
        other.id = nullid;
        return *this;
    }
    Wrapper(Component_t&& c): id(make<Component_t>(std::forward<Component_t>(c))) {}
    ~Wrapper() {
        if (id != nullid) {
            destroy();
            common::print("destroyed component", id, typeid(Component_t).name());
        } 
    }
    void destroy() {component::destroy<Component_t>(id);}
    Component_t& get() {return component::get<Component_t>(id);}
    Component_t* get_if() {return component::get_if<Component_t>(id);}
};
}
