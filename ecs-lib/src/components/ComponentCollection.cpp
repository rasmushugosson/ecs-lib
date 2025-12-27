#include "general/pch.h"

#include "Entity.h"

ae::ComponentCollection::ComponentCollection() = default;

ae::ComponentCollection::~ComponentCollection() = default;

void ae::ComponentCollection::RemoveAll(Entity entity)
{
    for (auto &[_, p] : m_Pools)
    {
        p->RemoveEntity(entity);
    }

    m_Entities.erase(entity);
    ValidateEntity(entity);
}

void ae::ComponentCollection::ValidateEntity(Entity entity) const
{
    for (ComponentSystem *pSystem : m_pSystems)
    {
        pSystem->Validate(entity);
    }
}

void ae::ComponentCollection::AddSystem(ComponentSystem *pSystem)
{
    uint32_t index = static_cast<uint32_t>(m_pSystems.size());

    m_pSystems.push_back(pSystem);
    m_SystemsLookup.insert(std::make_pair(pSystem, index));
}

void ae::ComponentCollection::RemoveSystem(ComponentSystem *pSystem)
{
    auto it = m_SystemsLookup.find(pSystem);

    if (it == m_SystemsLookup.end())
    {
        AE_LOG_WARNING("Tried to remove system from component collection but failed to find the system. This should "
                       "never happen, please report as a bug.");
        return;
    }

    uint32_t index = it->second;
    uint32_t lastIndex = static_cast<uint32_t>(m_pSystems.size() - 1);

    m_pSystems[index] = m_pSystems[lastIndex];
    m_SystemsLookup[m_pSystems[lastIndex]] = index;

    m_pSystems.pop_back();
    m_SystemsLookup.erase(it);
}
