#pragma once
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <util.h>
#include "common.h"
#include <type_traits>


template <typename T>
concept Component = requires {
std::is_aggregate_v<T>;
std::is_trivially_constructible_v<T>;
std::is_standard_layout_v<T>;
};

// components
struct Physical {
    M3x3 blueprint;
    M3x3 transform;
    V2 velocity;
    V2 acceleration;
    Rect collision_boundaries;
    float mass;
    float elasticity;
    float friction;
};
// entity
using EntityID = uint32_t;
namespace details {
template <Component T>
using ComponentStorage = util::SparseSet<EntityID, T>;
template <Component T>
ComponentStorage<T>& get_component_storage() {
    static ComponentStorage<T> storage;
    return storage;
}
using ComponentRemovers = std::unordered_map<std::type_index, std::function<void(EntityID)>>;
inline static ComponentRemovers component_removal_funcs;
template <Component T>
void register_remover_for() {
    component_removal_funcs[typeid(T)] = [](EntityID entity) {
        get_component_storage<T>().erase(entity);
    };
}
[[nodiscard]] inline EntityID create_unique_entity_id() noexcept {
    static uint32_t curr_id{};
    return curr_id++;
}
}/*namespace details*/
class Entity {
    EntityID id_;
public:
    Entity() noexcept: id_(details::create_unique_entity_id()) {}
    Entity(EntityID id): id_(id) {}
    ~Entity() {
        for(auto& [type, remover] : details::component_removal_funcs) {
            remover(id_);
        }
    }
    EntityID id() const {
        return id_;
    }
    template <Component T>
    void remove() {
        details::component_removal_funcs[typeid(T)](id_);
    }
    template <Component T, class ...InitArgs>
    constexpr void create(InitArgs&&...args) {
        if (details::component_removal_funcs.count(typeid(T)) == 0) {
            details::register_remover_for<T>();
        }
        details::get_component_storage<T>().emplace_back(id_, T{std::forward<InitArgs>(args)...});
    }
    template <Component T>
    bool has() const {
        return details::get_component_storage<T>().count(id_) > 0;
    }
    template <Component T>
    T& get() {
        return details::get_component_storage<T>()[id_];
    }
    template <Component T>
    const T& get() const {
        return details::get_component_storage<T>(id_);
    }
    template <Component T>
    T* get_if() {
        return details::get_component_storage<T>().find(id_);
    }
};
//systems
namespace details {
template <Component T, Component ...Ts>
bool validate(Entity& entity, std::tuple<Ts*...>& cache) {
    T* found = entity.get_if<T>();
    std::get<T*>(cache) = found;
    return found != nullptr;
}
}/*details*/
template <Component ...Ts>
std::optional<std::tuple<std::reference_wrapper<Ts>...>> get_components(Entity& entity) {
    std::tuple <Ts*...> cache{nullptr};
    if (bool has_all = (details::validate<Ts>(entity, cache) and ...)) {
        return std::make_tuple(std::ref(*std::get<Ts*>(cache)...));
    }
    return std::nullopt;
}
