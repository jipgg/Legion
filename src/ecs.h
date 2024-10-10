#pragma once
#include "common.h"
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <utility>
#include <cassert>
namespace ecs {
struct Component{};
template <typename T>
concept Tagged_component = std::is_base_of_v<Component, T>;
template <typename T>
concept Enum = std::is_enum_v<T>;
using Entity = uint32_t;
constexpr uint32_t nullentity = std::numeric_limits<uint32_t>::max();
struct Registry_entry_interface {
    virtual ~Registry_entry_interface() = default;
    virtual void erase(Entity) = 0;
};
template <Tagged_component T>
using Component_storage = Sparse_set<Entity, T>;
using Storage_registry = std::unordered_map<std::type_index, std::unique_ptr<Registry_entry_interface>>;
static Storage_registry component_storage_registry;
template <Tagged_component T>
Component_storage<T>& component_storage() {
    static Component_storage<T> storage;
    return storage;
}
inline void clear_entity_components(Entity entity) {
    for (auto& [type, entry] : component_storage_registry) {
        entry->erase(entity);
    }
}
template <Tagged_component T>
struct Component_storage_entry: Registry_entry_interface {
    void erase(Entity entity) override {
        component_storage<T>().erase(entity);
    };
};
template <Tagged_component T, typename ...InitArgs>
 void create_component(Entity entity, InitArgs&&...args) {
    if (component_storage_registry.find(typeid(T)) == component_storage_registry.end()) {
        component_storage_registry.insert(std::make_pair(typeid(T), std::make_unique<Component_storage_entry<T>>()));
    }
    component_storage<T>().emplace_back(entity, T{std::forward<InitArgs>(args)...});
}
template <Tagged_component T>
void destroy_component(Entity entity) {
    component_storage<T>().erase(entity);
}
template <Tagged_component T>
T& component(Entity entity) {
    component_storage<T>().at(entity);
};
template <Tagged_component T>
std::optional<std::reference_wrapper<T>> component_if(Entity entity) {
    return component_storage<T>().at_if(entity);
}
[[nodiscard]] inline Entity create_entity() noexcept {
    static Entity curr_id{};
    Entity new_entity = ++curr_id;
    return new_entity;
}
}
