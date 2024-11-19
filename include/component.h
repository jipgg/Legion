#pragma once
#include "common.h"  
namespace component {
using ComponentId = std::size_t;
constexpr const ComponentId nullid = 0;
template <class Ty>
[[nodiscard]] SparseSet<ComponentId, Ty>& storage() {
    static SparseSet<ComponentId, Ty> comps{};
    return comps;
}
[[nodiscard]] inline ComponentId create_unique_id() {
    static ComponentId curr_id{0};
    return ++curr_id;
}
template <class Ty, class ...Init_arguments_t>
[[nodiscard]] ComponentId make(Init_arguments_t&&...init) {
    const auto id = create_unique_id();
    storage<Ty>().emplace_back(id, Ty{std::forward<Init_arguments_t>(init)...});
    return id;
}
template <class Ty>
[[nodiscard]] Ty& get(ComponentId id) {
    return storage<Ty>().at(id);
}
template <class Ty>
[[nodiscard]] Ty* get_if(ComponentId id) {
    return storage<Ty>().at_if(id);
}
template <class Ty>
bool destroy(ComponentId id) {
    if (storage<Ty>().at_if(id)) {
        storage<Ty>().erase(id);
        return true;
    }
    return false;
}
template <class Ty>
using StorageView = std::span<Ty>;
template <class Ty>
StorageView<Ty> view() {
    return storage<Ty>().span();
}
template <class Ty>
struct RaiiWrapper {
    ComponentId id;
    RaiiWrapper(const RaiiWrapper&) = delete;
    RaiiWrapper& operator=(const RaiiWrapper&) = delete;
    RaiiWrapper(RaiiWrapper&& other) noexcept: id(other.id) {other.id = nullid;}
    RaiiWrapper& operator=(RaiiWrapper&& other) noexcept {
        id = other.id;
        other.id = nullid;
        return *this;
    }
    RaiiWrapper(Ty&& c): id(make<Ty>(std::forward<Ty>(c))) {}
    ~RaiiWrapper() {
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
