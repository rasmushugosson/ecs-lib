#include "Log.h"

#include <print>

// Author: Rasmus Hugosson
// Date: 2025-12-17

// Description: This is a simple example of how to use this library

// Header to include for the ECS library
#include "Entity.h"

// Simple utility struct for the example
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
        : ComponentSystem("Render", std::move(pCollection))
    {
        Validate();
    }

    // This pure virtual function defines validation rules for entities
    bool ValidImpl(ae::Entity entity) const override
    {
        AE_LOG(AE_TRACE, "Validating entity {} for render system", entity);

        const auto &collection = GetCollection();

        // Entities with a MeshComponent must also have a TransformComponent
        if (collection.Has<MeshComponent>(entity) && !collection.Has<TransformComponent>(entity))
        {
            AE_LOG(AE_WARNING, "Validation failed: entity {} has MeshComponent but no TransformComponent", entity);
            return false;
        }

        return true;
    }

    // This function processes the components
    void RunImpl() override
    {
        AE_LOG(AE_INFO, "Running render system...");

        const auto &collection = GetCollection();
        const auto *pool = collection.TryGetPool<MeshComponent>();

        // Loop through all mesh components
        for (const MeshComponent &mesh : pool->GetAll())
        {
            // Get the associated transform component
            const TransformComponent *transform = collection.Get<TransformComponent>(mesh.entity);

            // Display the component data
            AE_LOG(AE_TRACE, "Entity {}: position=[{}, {}, {}], rotation=[{}, {}, {}], mesh={}",
                   mesh.entity,
                   transform->position.x, transform->position.y, transform->position.z,
                   transform->rotation.x, transform->rotation.y, transform->rotation.z,
                   mesh.mesh);
        }

        AE_LOG(AE_INFO, "Render system done");
    }
};

void Demo()
{
    // Add sinks where logs will show up
    ae::Logger::Get().AddConsoleSink("Console", ae::LogSinkConsoleKind::STDOUT, AE_TRACE);

    AE_LOG(AE_INFO, "ECS demo started");
    AE_LOG_NEWLINE();

    // Entities are created with unique IDs
    ae::Entity entity = ae::NewEntity();
    AE_LOG(AE_TRACE, "Created entity with ID {}", entity);

    // Create some example components
    TransformComponent transform{
        .position = Vec3{ .x = 1.0f, .y = 2.0f, .z = 3.0f },
        .rotation = Vec3{ .x = 4.0f, .y = 5.0f, .z = 6.0f }
    };

    MeshComponent mesh{
        .entity = entity,
        .mesh = 7
    };

    // Component collections store the components
    auto pCollection = std::make_shared<ae::ComponentCollection>();

    // Add components to the collection first
    pCollection->Add(entity, transform);
    pCollection->Add(entity, mesh);
    AE_LOG(AE_TRACE, "Added TransformComponent and MeshComponent to entity {}", entity);

    // Then create systems that act on the collection
    auto pSystem = std::make_unique<RenderSystem>(pCollection);
    AE_LOG_NEWLINE();

    // Run the system (validates first, then calls RunImpl)
    pSystem->Run();
    AE_LOG_NEWLINE();

    // Remove the transform component
    AE_LOG(AE_TRACE, "Removing TransformComponent from entity {}...", entity);
    pCollection->Remove<TransformComponent>(entity);
    AE_LOG_NEWLINE();

    // Running the system again will fail validation
    // Entity has MeshComponent but no TransformComponent
    try
    {
        pSystem->Run();
    }
    catch (const std::exception &e)
    {
        AE_LOG(AE_ERROR, "{}", e.what());
    }

    // Clean up
    AE_LOG(AE_TRACE, "Cleaning up entity {}...", entity);
    pCollection->RemoveAll(entity);
    pSystem.reset();
    pCollection.reset();
    AE_LOG_NEWLINE();

    AE_LOG(AE_INFO, "ECS demo finished");
}

int main()
{
    try
    {
        Demo();
        return EXIT_SUCCESS;
    }

    catch (...)
    {
        std::fputs("Fatal error: unknown exception\n", stderr);
        return EXIT_FAILURE;
    }
}
