#include "general/pch.h"

// Author: Rasmus Hugosson
// Date: 2025-05-17

// Description: This is a simple example program demonstrating how
// this library can be used to run a basic entity component system.

// Header to include for the library
#include "Entity.h"

// Simple utility struct, but not important for this example
struct Vec3
{
	float x, y, z;
};

// Components are represented as data structs
struct TransformComponent
{
	Vec3 position;
	Vec3 rotation;
};

// Any number of component types can be defined 
struct MeshComponent
{
	ae::Entity entity;
	uint32_t mesh;
};

// Systems are defined as derived classes from the provided base
class RenderSystem : public ae::ComponentSystem
{
public:
	RenderSystem(std::shared_ptr<ae::ComponentCollection> pCollection)
		: ComponentSystem("Render", pCollection)
	{
		// Systems must be validated, you can do it here
		Validate();
	}

	// This pure virtual function defines rules for entities
	bool ValidImpl(ae::Entity entity) const override
	{
		const auto& collection = GetCollection();

		// The ComponentCollection.Has<ComponentType>(Entity) can be used to check if an entity has a specific component
		if (collection.Has<MeshComponent>(entity) && !collection.Has<TransformComponent>(entity))
		{
			return false; // As an example, an entity can't have a Mesh with no Transform for this system
		}

		// Otherwise, the entity satisfies this system
		// Note that entities that are not affected by the system's Run function should always return true
		return true;
	}

	// This second function can process any component
	void RunImpl() override
	{
		AE_LOG_CONSOLE(AE_INFO, "Running system...");

		// If we want to process all copies of a specfic component type, we can retrieve its pool
		const auto& collection = GetCollection();
		const auto& pool = collection.GetPool<MeshComponent>(); // Returns all mesh components in the collection

		// We can then loop through the components
		for (const MeshComponent& mesh : pool.GetAll())
		{
			// Other components can also be retrieved
			// The entity must have a transform as well, and we know that because of the contition on lines 48-52
			const TransformComponent& transform = collection.Get<TransformComponent>(mesh.entity);

			// For now, we can just display the data
			AE_LOG_CONSOLE(AE_TRACE, "Entity ID: " << mesh.entity);
			AE_LOG_CONSOLE(AE_TRACE, "Transform position: [" << transform.position.x << 
				", " << transform.position.y << ", " << transform.position.z << "]");
			AE_LOG_CONSOLE(AE_TRACE, "Transform rotation: [" << transform.rotation.x << 
				", " << transform.rotation.y << ", " << transform.rotation.z << "]");
			AE_LOG_CONSOLE(AE_TRACE, "Mesh ID: " << mesh.mesh);
			AE_LOG_CONSOLE_NEWLINE();
		}

		AE_LOG_CONSOLE(AE_INFO, "System done");
	}
};

int main()
{
	AE_LOG_CONSOLE(AE_INFO, "Program started");

	// Wrap everything in a try-catch in case exceptions are thrown
	try
	{
		// Entities are created with unique IDs
		ae::Entity entity = ae::NewEntity();

		// Some example components to use in the system
		TransformComponent transform({
			Vec3{1.0f, 2.0f, 3.0f},
			Vec3{4.0f, 5.0f, 6.0f}
			});
		
		MeshComponent mesh({
			entity,
			7
			});
		
		// Component collections store the components
		std::shared_ptr<ae::ComponentCollection> pCollection = std::make_shared<ae::ComponentCollection>();

		// Then component systems can be created to act on this collection
		std::unique_ptr<RenderSystem> pSystem = std::make_unique<RenderSystem>(pCollection);
		
		// Each component must be added to the collection
		pCollection->Add(entity, mesh);
		pCollection->Add(entity, transform);

		// When running the system, it will first check that it is valid and then call the RunImpl function
		pSystem->Run();

		// Individual components can be removed
		pCollection->Remove<TransformComponent>(entity);

		// If we run the system again now, it will fail since entities with mesh components must also have a transform. See lines 48-52
		pSystem->Run();

		// All components for an entity can also be removed at once
		pCollection->RemoveAll(entity);
		
		pSystem.reset();
		pCollection.reset();
	}
	catch (std::exception e)
	{
		// Log any thrown errors to the console
		AE_LOG_CONSOLE(AE_FATAL, e.what());
	}

	AE_LOG_CONSOLE(AE_INFO, "Program finished");

	return 0;
}
