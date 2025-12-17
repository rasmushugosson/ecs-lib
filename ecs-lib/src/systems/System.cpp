#include "Entity.h"
#include "general/pch.h"

#include <utility>

ae::ComponentSystem::ComponentSystem(const std::string &name, std::shared_ptr<ComponentCollection> pCollection)
    : m_Name(name), m_pCollection(std::move(pCollection)), m_Valid(false), m_Validated(false)
{
#ifdef AE_DEBUG
    m_InvalidEntities = std::unordered_set<Entity>();
#endif // AE_DEBUG

    m_pCollection->AddSystem(this);
}

ae::ComponentSystem::~ComponentSystem()
{
    m_pCollection->RemoveSystem(this);
}

void ae::ComponentSystem::Run()
{
#ifdef AE_DEBUG
    if (!m_Valid)
    {
        AE_LOG_ERROR("Failed to run component system '{}' as all entities are not valid.", m_Name);

        std::ostringstream oss;

        oss << "ValidImpl(Entity) returned false for the following entities:\n";

        for (Entity entity : m_InvalidEntities)
        {
            oss << " - ID: " << entity << "\n";
        }

        AE_LOG_ERROR("{}", oss.str());
        return;
    }
#endif // AE_DEBUG

    RunImpl();
}

void ae::ComponentSystem::Validate()
{
#ifdef AE_DEBUG
    if (m_Validated)
    {
        AE_LOG_WARNING("Tried to validate component system '{}' but this has already been done and should only be done "
                       "once. Consider calling Validate() in the constructior of the derived class.",
                       m_Name);
        return;
    }
#endif // AE_DEBUG

    bool found = false;

    for (Entity entity : m_pCollection->GetEntities())
    {
        if (!ValidImpl(entity))
        {
#ifdef AE_DEBUG
            m_InvalidEntities.insert(entity);
#endif // AE_DEBUG
            found = true;
        }
    }

    m_Valid = !found;
    m_Validated = true;
}

void ae::ComponentSystem::Validate([[maybe_unused]] Entity entity)
{
#ifdef AE_DEBUG
    if (!m_Validated)
    {
        AE_LOG_WARNING(
            "Tried to validate entity for component system '{}' but it has not been fully validated. No components "
            "should be added or removed from the collection before all attached systems have been fully validated. "
            "Consider calling Validate() in the constructior of the derived class of the component system.",
            m_Name);
        return;
    }

    bool valid = ValidImpl(entity);
    bool inSet = m_InvalidEntities.contains(entity);

    if (!valid && !inSet)
    {
        m_InvalidEntities.insert(entity);

        m_Valid = false;
    }

    else if (valid && inSet)
    {
        m_InvalidEntities.erase(entity);

        if (m_InvalidEntities.empty())
        {
            m_Valid = true;
        }
    }
#endif // AE_DEBUG
}
