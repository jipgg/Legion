//Legion
//single header-only base implementation of a sparse set based ECS structur
#pragma once
#define LEGION_ECS_CACHE_COMPONENT_SETS_ENABLED
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
#include <util.h>
#include <unordered_map>
#include <vector>
#include <utility>
#include <span>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <bitset>
namespace ECS {/////ECS/////
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
    [[nodiscard]] constexpr Val& at(Key key) {
        return dense_.at(sparse_.at(key));
    }
    [[nodiscard]] constexpr Val& operator[](Key key) {
        return dense_[sparse_[key]];
    }
    [[nodiscard]] constexpr Val* find(Key key) {
        auto found = sparse_.find(key);
        return found == sparse_.end() ? nullptr : &(dense_.at(found->second));
    }
    [[nodiscard]] constexpr const Val& at(const Key key) const {
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
struct Component{};
template <typename T>
concept component = std::is_base_of_v<Component, T>;
template <typename ...Ts>
concept component_set = (component<Ts> and ...);
// entity
using Entity = uint32_t;
constexpr uint32_t NoEntity = std::numeric_limits<uint32_t>::max();
enum class EntityFlags {Destroy, _count};
using EntityFlagBits = std::bitset<static_cast<size_t>(EntityFlags::_count)>;
namespace interfaces {
struct EntryManip {
    virtual ~EntryManip() = default;
    virtual void erase(Entity) = 0;
    virtual void clear(Entity) = 0;
};
}
template <component T>
using ComponentStorage = SparseSet<Entity, T>;
template <component_set ...Ts>
using ComponentSet = std::tuple<std::reference_wrapper<Ts>...>;
template <component_set ...Ts>
using SystemComponentSetCache = SparseSet<Entity, ComponentSet<Ts...>>;
using Registry = std::unordered_map<std::type_index, std::unique_ptr<interfaces::EntryManip>>;
namespace {/////ANONYMOUS/////
static Registry _component_storage_registry;
static Registry _system_cache_registry;
static std::vector<std::pair<Entity, EntityFlagBits>> _entities; 
template <component T>
ComponentStorage<T>& _get_component_storage() {
    static ComponentStorage<T> storage;
    return storage;
}
template <component_set ...Ts>
SystemComponentSetCache<Ts...> _get_system_cache() {
    static SystemComponentSetCache<Ts...> cache;
    return cache;
}
}/////ANONYMOUS/////END/////
template <component T>
struct ComponentStorageEntry: interfaces::EntryManip {
    void erase(Entity entity) override {
        _get_component_storage<T>().erase(entity);
    };
    void clear() {
        _get_component_storage<T>().clear();
    }
};
template <component_set ...Ts>
struct SystemCacheEntry: interfaces::EntryManip {
    void erase(Entity entity) override {
        _get_system_cache<Ts...>().erase(entity);
    }
    void clear() {
        _get_system_cache<Ts...>().clear();
    }
};
template <component T, typename ...InitArgs>
 void create(Entity entity, InitArgs&&...args) {
    if (_component_storage_registry.find(typeid(T)) == _component_storage_registry.end()) {
        _component_storage_registry.insert(std::make_pair(typeid(T), std::make_unique<ComponentStorageEntry<T>>()));
    }
    _get_component_storage<T>().emplace_back(entity, T{std::forward<InitArgs>(args)...});
}
template <component T>
void erase(Entity entity) {
    _get_component_storage<T>().erase(entity);
}
template <component T>
T& get(Entity entity) {
    _get_component_storage<T>().at(entity);
};
template <component T>
T* get_if(Entity entity) {
    return _get_component_storage<T>().find(entity);
}
[[nodiscard]] inline Entity create_entity() noexcept {
    static uint32_t curr_id{};
    return curr_id++;
}
inline void destroy_entity(Entity entity) {
    for (auto& [type, entry] : _system_cache_registry) {
        entry->erase(entity);
    }
    for (auto& [type, entry] : _component_storage_registry) {
        entry->erase(entity);
    }
}
namespace {/////ANONYMOUS/////
template <component T, component ...Ts>
bool _validate(Entity entity, std::tuple<Ts*...>& cache) {
    T* found = get_if<T>(entity);
    std::get<T*>(cache) = found;
    return found != nullptr;
}
}/////ANONYMOUS/////END/////
template <component ...Ts>
std::optional<std::tuple<std::reference_wrapper<Ts>...>> collect(Entity entity) {
    std::tuple <Ts*...> cache{nullptr};
    if (bool has_all = (_validate<Ts>(entity, cache) and ...)) {
        return std::make_tuple(*std::get<Ts*>(cache)...);
    }
    return std::nullopt;
}
template <component_set ...Ts>
struct System {
    using MySet = ComponentSet<Ts...>;
    System() {
        if (_system_cache_registry.find(typeid(MySet)) == _system_cache_registry.end()) {
            _system_cache_registry.insert(std::make_pair(typeid(MySet), SystemCacheEntry<Ts...>()));
        }
        _get_system_cache<Ts...>();
    }
    virtual ~System() = default;
    virtual void operator()() = 0;
    std::span<MySet> cache() {
        return _get_system_cache<Ts...>().span();
    }
};

}/////ECS/////END/////
