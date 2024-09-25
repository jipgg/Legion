//all the ecs base logic is implemented here. Sparse set ECS structured
#pragma once
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
#include <util.h>
#include "common.h"
#include <type_traits>
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

namespace ecs {
template <typename T>
concept Component = requires {
std::is_aggregate_v<T>;
std::is_trivially_constructible_v<T>;
std::is_standard_layout_v<T>;
};
// entity
using Entity = uint32_t;
struct IRegistryEntry {
    virtual ~IRegistryEntry() = default;
    virtual void remove(Entity) = 0;
};
template <Component T>
using ComponentStorage = util::SparseSet<Entity, T>;
using ComponentRegistry = std::unordered_map<std::type_index, std::unique_ptr<IRegistryEntry>>;
inline static ComponentRegistry component_registry{};
template <Component T>
ComponentStorage<T>& get_entity_component_storage() {
    static ComponentStorage<T> storage;
    return storage;
}
template <Component T>
struct RegistryEntry: public IRegistryEntry {
    void remove(Entity entity) override {
        get_entity_component_storage<T>().erase(entity);
    };
};
template <Component T, typename ...InitArgs>
 void create(Entity entity, InitArgs&&...args) {
    if (component_registry.find(typeid(T)) == component_registry.end()) {
        component_registry.insert(std::make_pair(typeid(T), std::make_unique<RegistryEntry<T>>()));
    }
    get_entity_component_storage<T>().emplace_back(entity, T{std::forward<InitArgs>(args)...});
}
template <Component T>
void erase(Entity entity) {
    get_entity_component_storage<T>().erase(entity);
}
template <Component T>
T& get(Entity entity) {
    get_entity_component_storage<T>().at(entity);
};
template <Component T>
T* get_if(Entity entity) {
    return get_entity_component_storage<T>().find(entity);
}
[[nodiscard]] inline Entity create_entity() noexcept {
    static uint32_t curr_id{};
    return curr_id++;
}
/*
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
*/
//systems
namespace {
template <Component T, Component ...Ts>
bool validate(Entity entity, std::tuple<Ts*...>& cache) {
    T* found = ecs::get_if<T>(entity);
    std::get<T*>(cache) = found;
    return found != nullptr;
}
}
template <Component ...Ts>
std::optional<std::tuple<Ts*>...> collect(Entity entity) {
    std::tuple <Ts*...> cache{nullptr};
    if (bool has_all = (validate<Ts>(entity, cache) and ...)) {
        return cache;
    }
    return std::nullopt;
}
}
