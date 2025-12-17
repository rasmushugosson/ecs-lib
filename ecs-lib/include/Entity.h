#pragma once

#include "Log.h"

#include <expected>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ae
{
typedef uint32_t Entity;

Entity NewEntity();

struct EmptyComponent
{
    Entity entity;
};

template <typename T> class ComponentPool
{
  public:
    ComponentPool() : m_Components() {}

    ~ComponentPool() = default;

    inline bool Has(Entity entity) const noexcept
    {
        return m_Sparse.contains(entity);
    }

    inline T &Get(Entity entity)
    {
        return m_Components[m_Sparse[entity]];
    }

    inline const T &Get(Entity entity) const
    {
        return m_Components[m_Sparse.at(entity)];
    }

    inline std::vector<T> &GetAll()
    {
        return m_Components;
    }

    inline const std::vector<T> &GetAll() const
    {
        return m_Components;
    }

    inline void Add(Entity entity, const T &component)
    {
#ifdef AE_DEBUG
        if (Has(entity))
        {
            AE_THROW_RUNTIME_ERROR("Failed to add component to entity {}, but it already has a component of the same "
                                   "type. Each entity can only contain one instance of each component type.",
                                   entity);
        }
#endif // AE_DEBUG

        uint32_t index = static_cast<uint32_t>(m_Components.size());
        m_Components.push_back(component);
        m_Entities.push_back(entity);
        m_Sparse.insert(std::make_pair(entity, index));
    }

    void Remove(Entity entity)
    {
        auto it = m_Sparse.find(entity);

        if (it == m_Sparse.end())
        {
            return;
        }

        uint32_t index = it->second;
        uint32_t lastIndex = static_cast<uint32_t>(m_Components.size() - 1);

        m_Components[index] = std::move(m_Components[lastIndex]);
        m_Entities[index] = m_Entities[lastIndex];
        m_Sparse[m_Entities[lastIndex]] = index;

        m_Components.pop_back();
        m_Entities.pop_back();
        m_Sparse.erase(it);
    }

  private:
    std::vector<T> m_Components;
    std::vector<Entity> m_Entities;
    std::unordered_map<Entity, uint32_t> m_Sparse;
};

struct ComponentPoolWrapper
{
    virtual ~ComponentPoolWrapper() = default;
    virtual bool Has(Entity entity) = 0;
    virtual void RemoveEntity(Entity entity) = 0;
};

template <typename T> struct ComponentPoolWrapperImpl final : ComponentPoolWrapper
{
    ComponentPool<T> pool;
    bool Has(Entity entity) override
    {
        return pool.Has(entity);
    }
    void RemoveEntity(Entity entity) override
    {
        pool.Remove(entity);
    }
};

class ComponentSystem;

class ComponentCollection
{
  public:
    ComponentCollection();
    ComponentCollection(const ComponentCollection &) = delete;
    ComponentCollection &operator=(const ComponentCollection &) = delete;
    ~ComponentCollection();

    template <typename T> inline bool Has(Entity entity) const
    {
        auto *pool = TryGetPool<T>();

        if (!pool)
        {
            return false;
        }

        return pool->Has(entity);
    }

    template <typename T> inline T *Get(Entity entity)
    {
        auto *pool = TryGetPool<T>();

        if (!pool)
        {
            return nullptr;
        }

        if (!pool->Has(entity))
        {
            return nullptr;
        }

        return &pool->Get(entity);
    }

    template <typename T> inline const T *Get(Entity entity) const
    {
        auto *pool = TryGetPool<T>();

        if (!pool)
        {
            return nullptr;
        }

        if (!pool->Has(entity))
        {
            return nullptr;
        }

        return &pool->Get(entity);
    }

    template <typename T> inline ComponentPool<T> &GetPool()
    {
        std::type_index type = std::type_index(typeid(T));

        auto it = m_Pools.find(type);

        if (it == m_Pools.end())
        {
            m_Pools.insert(std::make_pair(type, std::make_unique<ComponentPoolWrapperImpl<T>>()));

            it = m_Pools.find(type);
        }

        return static_cast<ComponentPoolWrapperImpl<T> *>(it->second.get())->pool;
    }

    template <typename T> inline ComponentPool<T> *TryGetPool()
    {
        std::type_index type = std::type_index(typeid(T));
        auto it = m_Pools.find(type);

        if (it == m_Pools.end())
        {
            return nullptr;
        }

        return &static_cast<ComponentPoolWrapperImpl<T> *>(it->second.get())->pool;
    }

    template <typename T> inline const ComponentPool<T> *TryGetPool() const
    {
        std::type_index type = std::type_index(typeid(T));
        auto it = m_Pools.find(type);
        if (it == m_Pools.end())
            return nullptr;
        return &static_cast<const ComponentPoolWrapperImpl<T> *>(it->second.get())->pool;
    }

    template <typename T> inline void Add(Entity entity, const T &component)
    {
        GetPool<T>().Add(entity, component);

        m_Entities.insert(entity);

        ValidateEntity(entity);
    }

    template <typename T> inline void Remove(Entity entity)
    {
        GetPool<T>().Remove(entity);
        ValidateEntity(entity);

        bool found = false;

        for (auto &[_, p] : m_Pools)
        {
            if (p->Has(entity))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            m_Entities.erase(entity);
        }
    }

    void RemoveAll(Entity entity);

    inline const std::unordered_set<Entity> &GetEntities() const
    {
        return m_Entities;
    }

  private:
    void ValidateEntity(Entity entity) const;

    void AddSystem(ComponentSystem *pSystem);
    void RemoveSystem(ComponentSystem *pSystem);

  private:
    std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolWrapper>> m_Pools;
    std::vector<ComponentSystem *> m_pSystems;
    std::unordered_map<ComponentSystem *, uint32_t> m_SystemsLookup;
    std::unordered_set<Entity> m_Entities;

    friend class ComponentSystem;
};

class ComponentSystem
{
  public:
    ComponentSystem(const std::string &name, std::shared_ptr<ComponentCollection> pCollection);
    ComponentSystem(const ComponentSystem &) = delete;
    ComponentSystem &operator=(const ComponentSystem &) = delete;
    virtual ~ComponentSystem();

    void Run();
    void Validate();

  protected:
    [[nodiscard]] inline const ComponentCollection &GetCollection() const
    {
        return *m_pCollection;
    }

  protected:
    [[nodiscard]] virtual bool ValidImpl(Entity entity) const = 0;
    virtual void RunImpl() = 0;

  private:
    void Validate(Entity entity);

  private:
    std::string m_Name;
    std::shared_ptr<ComponentCollection> m_pCollection;
#ifdef AE_DEBUG
    std::unordered_set<Entity> m_InvalidEntities;
#endif // AE_DEBUG
    bool m_Valid;
    bool m_Validated;

    friend class ComponentCollection;
};
} // namespace ae
