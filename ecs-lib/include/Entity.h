#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <typeindex>

#include "Log.h"

namespace ae
{
	class InvalidEntityError : public std::runtime_error
	{
	public:
		explicit InvalidEntityError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Invalid entity", file, line, message))
		{
		}
	};

#define AE_THROW_INVALID_ENTITY_ERROR(m) throw ae::InvalidEntityError(__FILE__, __LINE__, std::ostringstream() << m)

	class ComponentNotFoundError : public std::runtime_error
	{
	public:
		explicit ComponentNotFoundError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Component not found", file, line, message))
		{
		}
	};

#define AE_THROW_COMPONENT_NOT_FOUND_ERROR(m) throw ae::ComponentNotFoundError(__FILE__, __LINE__, std::ostringstream() << m)

	class DuplicateComponentError : public std::runtime_error
	{
	public:
		explicit DuplicateComponentError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Duplicate component", file, line, message))
		{
		}
	};

#define AE_THROW_DUPLICATE_COMPONENT_ERROR(m) throw ae::DuplicateComponentError(__FILE__, __LINE__, std::ostringstream() << m)

	class PoolNotFoundError : public std::runtime_error
	{
	public:
		explicit PoolNotFoundError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Pool not found", file, line, message))
		{
		}
	};

#define AE_THROW_POOL_NOT_FOUND_ERROR(m) throw ae::PoolNotFoundError(__FILE__, __LINE__, std::ostringstream() << m)

	class InvalidSystemError : public std::runtime_error
	{
	public:
		explicit InvalidSystemError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Invalid system", file, line, message))
		{
		}
	};

#define AE_THROW_INVALID_SYSTEM_ERROR(m) throw ae::InvalidSystemError(__FILE__, __LINE__, std::ostringstream() << m)

	typedef uint32_t Entity;

	Entity NewEntity();

	struct EmptyComponent
	{
		Entity entity;
	};

	template <typename T>
	class ComponentPool
	{
	public:
		ComponentPool()
			: m_Components(), m_Entities(), m_Sparse()
		{
		}

		~ComponentPool()
		{
		}

		inline bool Has(Entity entity) const noexcept
		{
			return m_Sparse.count(entity) != 0;
		}

		inline T& Get(Entity entity)
		{
			return m_Components[m_Sparse[entity]];
		}

		inline const T& Get(Entity entity) const
		{
			return m_Components[m_Sparse.at(entity)];
		}

		inline std::vector<T>& GetAll()
		{
			return m_Components;
		}

		inline const std::vector<T>& GetAll() const
		{
			return m_Components;
		}

		inline void Add(Entity entity, const T& component)
		{
#ifdef AE_DEBUG
			if (Has(entity))
			{
				AE_THROW_DUPLICATE_COMPONENT_ERROR("Failed to add component to entity " << entity << ", but it already has a component of the same type. Each entity can only contain one instance of each component type.");
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

	template <typename T>
	struct ComponentPoolWrapperImpl final : ComponentPoolWrapper
	{
		ComponentPool<T> pool;
		bool Has(Entity entity) override { return pool.Has(entity); }
		void RemoveEntity(Entity entity) override { pool.Remove(entity); }
	};

	class ComponentSystem;

	class ComponentCollection
	{
	public:
		ComponentCollection();
		ComponentCollection(const ComponentCollection&) = delete;
		ComponentCollection& operator =(const ComponentCollection&) = delete;
		~ComponentCollection();

		template <typename T>
		inline bool Has(Entity entity) const
		{
			try
			{
				bool found = GetPool<T>().Has(entity);

				return found;
			}
			catch (PoolNotFoundError e)
			{
				return false;
			}
		}

		template <typename T>
		inline T& Get(Entity entity)
		{
			ComponentPool<T>& pool = GetPool<T>();

#ifdef AE_DEBUG
			if (!pool.Has(entity))
			{
				AE_THROW_COMPONENT_NOT_FOUND_ERROR("Failed to get entity from collection. The specified pool does not contain a component for entity " << entity);
			}
#endif // AE_DEBUG

			return pool.Get(entity);
		}

		template <typename T>
		inline const T& Get(Entity entity) const
		{
#ifdef AE_DEBUG
			try
			{
#endif // AE_DEBUG
				const ComponentPool<T>& pool = GetPool<T>();

#ifdef AE_DEBUG
				if (!pool.Has(entity))
				{
					AE_THROW_COMPONENT_NOT_FOUND_ERROR("Failed to get entity from collection. The specified pool does not contain a component for entity " << entity);
				}
#endif // AE_DEBUG

				return pool.Get(entity);
#ifdef AE_DEBUG
			}
			catch (PoolNotFoundError e)
			{
				AE_THROW_COMPONENT_NOT_FOUND_ERROR("Failed to get entity from collection. The specified pool does not contain a component for entity " << entity);
			}
#endif // AE_DEBUG
		}

		template <typename T>
		inline ComponentPool<T>& GetPool()
		{
			std::type_index type = std::type_index(typeid(T));

			auto it = m_Pools.find(type);

			if (it == m_Pools.end())
			{
				m_Pools.insert(std::make_pair(type, std::make_unique<ComponentPoolWrapperImpl<T>>()));

				it = m_Pools.find(type);
			}

			return static_cast<ComponentPoolWrapperImpl<T>*>(it->second.get())->pool;
		}

		template <typename T>
		inline const ComponentPool<T>& GetPool() const
		{
			std::type_index type = std::type_index(typeid(T));

			auto it = m_Pools.find(type);

			if (it == m_Pools.end())
			{
				AE_THROW_POOL_NOT_FOUND_ERROR("Failed to find requested component pool");
			}

			return static_cast<ComponentPoolWrapperImpl<T>*>(it->second.get())->pool;
		}

		template <typename T>
		inline void Add(Entity entity, const T& component)
		{
			GetPool<T>().Add(entity, component);

			m_Entities.insert(entity);

			ValidateEntity(entity);
		}

		template <typename T>
		inline void Remove(Entity entity)
		{
			GetPool<T>().Remove(entity);
			ValidateEntity(entity);

			bool found = false;

			for (auto& [_, p] : m_Pools)
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

		inline const std::unordered_set<Entity>& GetEntities() const
		{
			return m_Entities;
		}
	private:
		void ValidateEntity(Entity entity) const;

		void AddSystem(ComponentSystem* pSystem);
		void RemoveSystem(ComponentSystem* pSystem);
	private:
		std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolWrapper>> m_Pools;
		std::vector<ComponentSystem*> m_pSystems;
		std::unordered_map<ComponentSystem*, uint32_t> m_SystemsLookup;
		std::unordered_set<Entity> m_Entities;

		friend class ComponentSystem;
	};

	class ComponentSystem
	{
	public:
		ComponentSystem(const std::string& name, std::shared_ptr<ComponentCollection> pCollection);
		ComponentSystem(const ComponentSystem&) = delete;
		ComponentSystem& operator =(const ComponentSystem&) = delete;
		virtual ~ComponentSystem();

		void Run();
		void Validate();
	protected:
		inline const ComponentCollection& GetCollection() const
		{
			return *m_pCollection;
		}
	protected:
		virtual bool ValidImpl(Entity entity) const = 0;
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
}
