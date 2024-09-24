#pragma once
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <util.h>
using EntityID = uint32_t;
namespace details {
template <class Component>
using ComponentStorage = util::SparseSet<EntityID, Component>;
template <class Component>
ComponentStorage<Component>& get_component_storage() {
    static ComponentStorage<Component> storage;
    return storage;
}
using ComponentRemovers = std::unordered_map<std::type_index, std::function<void(EntityID)>>;
inline static ComponentRemovers component_removal_funcs;
template <class Component>
void register_remover_for() {
    component_removal_funcs[typeid(Component)] = [](EntityID entity) {
        get_component_storage<Component>().erase(entity);
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
    template <class Component>
    void remove() {
        details::component_removal_funcs[typeid(Component)](id_);
    }
    template <class Component, class ...InitArgs>
    void create(InitArgs&&...args) {
        if (details::component_removal_funcs.count(typeid(Component)) == 0) {
            details::register_remover_for<Component>();
        }
        details::get_component_storage<Component>().emplace_back(id_, Component{std::forward<InitArgs>(args)...});
    }
    template <class Component>
    bool exists() const {
        return details::get_component_storage<Component>().count(id_) > 0;
    }
    template <class Component>
    Component& get() {
        return details::get_component_storage<Component>()[id_];
    }
    template <class Component>
    const Component& get() const {
        return details::get_component_storage<Component>(id_);
    }
    template <class Component>
    Component* get_if() {
        return exists<Component>() ? &get<Component>() : nullptr;
    }
};
