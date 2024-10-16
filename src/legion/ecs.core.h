#pragma once
#include "common.h"
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <utility>
#include <cassert>
#include <bitset>
using Entity_t = uint32_t;
namespace ecs {
namespace core {
struct Registry_entry_interface {
    virtual ~Registry_entry_interface() = default;
    virtual void erase(Entity_t) = 0;
};
template <class Component_t>
using Component_storage = Sparse_set<Entity_t, Component_t>;
using Storage_registry = std::unordered_map<std::type_index, std::unique_ptr<Registry_entry_interface>>;
static Storage_registry component_storage_registry;
template <Enum Flag, size_t Count>
struct Entity_bundle {
    Entity_t entity;
    std::bitset<size_t(Count)> flags;
    [[nodiscard]] bool flagged_for(Flag flag) const {
        return flags.test(static_cast<size_t>(flag));
    }
    void flag(Flag flag) {
        flags.set(static_cast<size_t>(flag));
    }
    void unflag(Flag flag) {
        flags.reset(static_cast<size_t>(flag));
    }
};
template <class Component_t>
Component_storage<Component_t>& component_storage() {
    static Component_storage<Component_t> storage;
    return storage;
}
inline void clear_entity_components(Entity_t entity) {
    for (auto& [type, entry] : component_storage_registry) {
        entry->erase(entity);
    }
}
template <class Component_t>
struct Component_storage_entry: Registry_entry_interface {
    void erase(Entity_t entity) override {
        component_storage<Component_t>().erase(entity);
    };
    virtual ~Component_storage_entry() {
        component_storage<Component_t>().clear();
        component_storage<Component_t>().shrink_to_fit();
    }
};
template <class Component_t, typename ...Initializer_list_t>
 void create_component(Entity_t entity, Initializer_list_t&&...args) {
    if (component_storage_registry.find(typeid(Component_t)) == component_storage_registry.end()) {
        component_storage_registry.insert(std::make_pair(typeid(Component_t), std::make_unique<Component_storage_entry<Component_t>>()));
    }
    component_storage<Component_t>().emplace_back(entity, Component_t{std::forward<Initializer_list_t>(args)...});
}
template <class Component_t>
void destroy_component(Entity_t entity) {
    component_storage<Component_t>().erase(entity);
}
template <class Component_t>
Component_t& get(Entity_t entity) {
    return component_storage<Component_t>().at(entity);
};
template <class Component_t>
std::optional<std::reference_wrapper<Component_t>> get_if(Entity_t entity) {
    return component_storage<Component_t>().at_if(entity);
}
[[nodiscard]] inline Entity_t create_entity() noexcept {
    static Entity_t curr_id{};
    return curr_id++;
}
}
}
