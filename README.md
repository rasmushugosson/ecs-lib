# Entity Component System Library

## General

This library provides a simple Entity Component System (ECS) for `C++` projects. The library is built using [Premake5](https://premake.github.io/) for `C++23`.

This library depends on [log-lib](https://github.com/rasmushugosson/log-lib), which is included as a git submodule.

## Getting Started

1. **Clone the repository** with submodules and open a terminal in the project root.
   ```bash
   git clone --recursive https://github.com/rasmushugosson/ecs-lib.git
   ```

The next steps depend on your preferred build system below.

### Visual Studio

2. Run `premake5 vs20XX` to generate a Visual Studio solution file (`.sln`).
3. Open the solution file in Visual Studio and build using MSVC.

### Gmake (force G++)

2. Run the pre-defined action `premake5 gmake-gcc` to generate makefiles specifically for GCC.
3. Navigate into `/build/[os]-gcc` where the `Makefile` is created.
4. Run `make config=[build type]` where the possible options are `debug`, `release` or `dist`.
5. Navigate into `/bin/Sandbox/[build type]` and run the `Sandbox` executable.

### Gmake (force Clang++)

2. Run the pre-defined action `premake5 gmake-clang` to generate makefiles specifically for Clang.
3. Navigate into `/build/[os]-clang` where the `Makefile` is created.
4. Run `make config=[build type]` where the possible options are `debug`, `release` or `dist`.
5. Navigate into `/bin/Sandbox/[build type]` and run the `Sandbox` executable.

### Formatting and Linting

There are additional actions for formatting with `clang-format` and linting through `clang-tidy`. These are run through:

```bash
# Run clang-format
premake5 format

# Run clang-tidy
premake5 lint

# Run clang-tidy and apply fixes
premake5 lint-fix
```

These commands assume `clang-format` and `clang-tidy` are installed on your system.

### Additional Dependencies

- **Premake5:** This library uses [Premake5](https://premake.github.io/) as its build configuration tool.
  Ensure that `premake5` is installed on your system or copied into the root folder.
  You can download it [here](https://premake.github.io/download/).

## Using as a Submodule

This library can be used as a git submodule in other Premake5 projects. Add it as a submodule (with recursive init to get log-lib) and include the ECS project definition in your `premake5.lua`:

```lua
include("path/to/ecs-lib/ecs-project.lua")

project("YourProject")
    -- ...
    includedirs({
        "path/to/ecs-lib/dep/log-lib/log-lib/include",
        "path/to/ecs-lib/ecs-lib/include"
    })
    links({ "Log", "ECS" })
```

The `ecs-project.lua` file defines the ECS project and automatically includes the Log project dependency. The `premake5.lua` is used for standalone builds including the Sandbox example.

## Usage

To use the library, include the `Entity.h` header file in your project. The ECS consists of five main pillars: entities, components, collections, pools and systems. The general design of the library is SoA, where collections contain a pool for each component type as a contiguous array. However, the pools themselves are stored as AoS. These components can then be processed through systems. Below is a brief explanation of each of the mentioned pillars.

### Entities

Entities are simply IDs, or 32 bit `unsigned int`s. They are merely used to organize components and have no separate functionality. An entity is acquired through:

```c++
ae::Entity entity = ae::NewEntity(); // Returns a unique entity ID
```

### Components

Component types are defined as data `struct`s. This means they *should not* contain methods, and this is because any functionality would be implemented through [Systems](#systems) instead. Below is an example of a component `struct` that we will use in later examples.

```c++
struct ExampleComponent // Data struct for an example component type
{
    float a;
    int b;
};

ExampleComponent component = { 1.0f, 123 }; // Instantiates a component of the defined type
```

### Collections

Collections are where components are stored, and the link between the entities, the components and the systems processing them. The collection class contains functionality for adding components, querying components, checking which entities have specific components and so on. It is also possible to add the same component to multiple collections. A collection can be created and populated with components like this:

```c++
std::shared_ptr<ae::ComponentCollection> pCollection = std::make_shared<ae::ComponentCollection>();

pCollection->Add(entity, component); // Adds the component to the entity
```

### Pools

Pools are never created by the user manually, but are rather an internal piece of collections. Each collection contains one component pool for each component type, and these pools can be accessed through the collection. Here is how to access component pools:

```c++
// GetPool returns a reference (creates the pool if it doesn't exist)
ae::ComponentPool<ExampleComponent>& pool = pCollection->GetPool<ExampleComponent>();

// TryGetPool returns a pointer (nullptr if pool doesn't exist)
const ae::ComponentPool<ExampleComponent>* pool = pCollection->TryGetPool<ExampleComponent>();
```

### Systems

As mentioned, systems provide a way to access and process components. Systems are implemented as derived classes from the `ae::ComponentSystem` base class. It contains two pure virtual functions to be implemented, `ValidImpl` and `RunImpl`. The `ValidImpl` function is automatically called for all entities that have a component in the system's specified collection, and can be used to define constraints for how components are combined. A return value of true means the entity provided as an argument is valid and false means it is not valid. The second function, `RunImpl`, is where components are processed. `ae::ComponentSystem::Run` cannot be called unless all entities satisfy the constraints specified in `ValidImpl`. Below is an example of a system that has no constraints for component combinations, and that simply prints all component values when run:

```c++
class ExampleSystem : public ae::ComponentSystem
{
public:
    ExampleSystem(std::shared_ptr<ae::ComponentCollection> pCollection)
        : ae::ComponentSystem("ExampleSystem", pCollection)
    {
        Validate(); // Validate all entities in the collection
    }

    bool ValidImpl(ae::Entity entity) const override
    {
        return true; // All entities are always valid
    }

    void RunImpl() override
    {
        const auto& collection = GetCollection();
        const auto* pool = collection.TryGetPool<ExampleComponent>();

        if (!pool) return; // No components of this type

        for (const ExampleComponent& component : pool->GetAll())
        {
            std::cout << component.a << ", " << component.b << std::endl;
        }
    }
};

auto pSystem = std::make_unique<ExampleSystem>(pCollection);
pSystem->Run(); // Validates (if not already) and runs the system
```

### Build Configurations

The build configuration determines which logging macros from log-lib are active:

| Configuration | Define        | Logging                                      |
|---------------|---------------|----------------------------------------------|
| `debug`       | `AE_DEBUG`    | `AE_LOG()` and `AE_LOG_BOTH()` are active    |
| `release`     | `AE_RELEASE`  | `AE_LOG_RELEASE()` and `AE_LOG_BOTH()` are active |
| `dist`        | `AE_DIST`     | All logging disabled                         |

### Code Example

A complete example project demonstrating the mentioned features and more can be found in `sandbox/src/Sandbox.cpp`.

## License

This library is licensed under the **Apache License 2.0**.
See the [LICENSE](LICENSE) file in this repository for details.
