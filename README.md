# Entity Component System Library

## General

This library provides a simple logging system, and some additional utility functionality for `C++` projects. The library is built using [Premake5](https://premake.github.io/) for `C++20`.

## Getting started

1. **Clone the repository** and open a terminal in the project root.
2. Navigate into the `dev` folder and **Run the provided `Premake5` script**.
   - If you’re on **Windows** with **Visual Studio 2022**, use the included `.bat` file to generate a `.sln` solution.
   - Otherwise, run `premake5` with the appropriate arguments to generate project files for your platform and IDE of choice.
3. **Open the generated solution/project** in your IDE and build the `Sandbox` project. The resulting binaries will appear in the `bin` folder.
4. **Run the `Sandbox` project** to verify that the library and dependencies are set up correctly.

### Additional Dependencies

- **Premake5:** This library uses [Premake5](https://premake.github.io/) as its build configuration tool.  
  Ensure that `premake5` is installed on your system or copied into the `dev` folder.  
  You can download it [here](https://premake.github.io/download/).

## Usage

To use the library, include the `Entity.h` header file in your project. The ECS consists of five main pillars; entities, components, collections, pools and systems. The general design of the library is SoA, where collections contain a pool for each component type as a continuous array. However, the pools themselves are stored as AoS. These components can then be processed through systems. Below is a breif explanation of each of the mentioned pillars.

### Entities

Entities are simply IDs, or 32 bit `unsigned int`s. They are merely used to organize components and have no separate functionality. An entity is aquired through:

```c++
ae::Entity entity = NewEntity(); // Returns a unique entity ID
```

### Components

Component types are defined as data `struct`s. This means they *should not* contain methods, and this is because any functionality would be implemented through [Systems](#systems) instead. Below is an example of a component `struct` that we will use in later examples.

```c++
struct ExampleComponent // Data struct for an example component type
{
  float a;
  int b;
}

ExampleComponent component = { 1.0f, 123 }; // Instanciates a component of the defined type
```

### Collections

Collections are where components are stored, and the link between the entities, the components and the systems processing them. The collection class contains functionality for adding components, querying components, check which entities have specific components and so on. It is also possible to add the same component to multiple collections. A collection can be created and populated with components like this:

```c++
std::shared_ptr<ae::ComponentCollection> pCollection = std::make_shared<ae::ComponentCollection>(); // Creates an empty component collection

pCollection->AddComponent(entity, component); // Adds the component to the entity from before
```

### Pools

Pools are never created by the user manually, but are rather an internal piece of collections. Each collection contains one component pool for each component type, and these pools can be accessed through the collection. Here is how to access component pools:

```c++
const ae::ComponentPool<ExampleComponent>& pool = pCollection->GetPool<ExampleComponent>(); // Gets the pool containing components of the template argument type
```

### Systems

As mentioned, systems provide a way to access and process components. Systems are implemented as derived classes from the `ae::ComponentSystem` base class. It contains two pure virtual functions to be implemented, `ValidImpl` and `RunImpl`. The `ValidImpl` function is automatically called for all entities that have a component in the system's specified collection, and can be used to define constraints for how components are combined. A return value of true means the entity provided as an argument is valid and false means it is not valid. The second function, `RunImpl`, is where components are processed. `ae::ComponentSystem::Run` cannot be called unless all entities satisfies the constraints specified in `ValidImpl`. Below is an example of a system that has no contraints for component combinations, and that simply prints all component values when run:

```c++
class ExampleSystem : public ae::ComponentSystem
{
public:
  ExampleSystem();
  virtual ~ExampleSystem();

  bool ValidImpl(ae::Entity entity) const override
  {
    return true; // All entities are always valid
  }

  void RunImpl() override
  {
    // We can retrieve the collection and its pools
    const auto& collection = GetCollection();
    const auto& pool = collection.GetPool<ExampleComponent>();

    // Then loop through each individual component
    for (const ExampleComponent& component : pool.GetAll())
    {
      // Print the data, but this could of course be any form of processing
      std::cout << component.a << ", " << component.b << std::endl;
    }
  }
}

std::unique_ptr<ExampleSystem> pSystem = std::make_unique<ExampleSystem>(pCollection); // Creates a system that will act on the previously created collection

pCollection->Run(); // Checks if the system is valid, and then calls the RunImpl function if that is the case
```

### Code example

A complete example project demonstrating the mentioned features and more can be found in `sandbox/src/Sandbox.cpp`.

## License

This library is licensed under the **Apache License 2.0**.  
See the [LICENSE](LICENSE) file in this repository for details.
