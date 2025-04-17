/**
 * @file ECSHub.h
 * @brief Central hub for managing the Entity-Component-System architecture.
 *
 * This header file defines the `ECSHub` class, which encapsulates the
 * management of entities, components, and systems within the FruitLoops
 * ECS framework. It provides methods for creating and destroying entities,
 * registering components and systems, and managing entity signatures.
 *
 * Key functionalities include:
 * - Initializing and managing component, entity, and system managers.
 * - Creating and destroying entities while ensuring proper cleanup of
 *   components and systems.
 * - Adding and removing components from entities, updating their signatures
 *   accordingly.
 * - Retrieving entities that have specific components and checking for the
 *   presence of components on entities.
 *
 * @note The `ECSHub` class utilizes templates for flexibility and type safety
 *       when registering and managing components and systems.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once
#include "DebugMacros.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "EntityManager.h"
#include "TileComponent.h"
#include "ScriptComponent.h"


namespace FruitLoops {
    class ECSHub {
    public:
        /**
        * @brief Initialize the ECSHub.
        *
        * This method creates instances of the component, entity, and system managers
        * to manage their respective functionalities within the ECS architecture.
        */
        void Init() {
            componentManager = std::make_unique<ComponentManager>();
            entityManager = std::make_unique<EntityManager>();
            systemManager = std::make_unique<SystemManager>();

            FL_DEBUG_INFO("ECSHub initialized.");
            //componentManager->LogRegisteredComponents();  // Log all registered components
        }

        /**
        * @brief Create a new entity.
        *
        * This method creates a new entity and returns its ID.
        *
        * @return Entity The ID of the newly created entity.
        */
        Entity CreateEntity()
        {
            return entityManager->CreateEntity();
        }

        /**
         * @brief Destroy an existing entity.
         *
         * This method removes an entity from the EntityManager and notifies the
         * ComponentManager and SystemManager to handle its destruction.
         *
         * @param entity The ID of the entity to destroy.
         */
        void DestroyEntity(Entity entity)
        {
            entityManager->DestroyEntity(entity);  // Remove the entity from the EntityManager
            componentManager->EntityDestroyed(entity);  // Notify the ComponentManager
            systemManager->EntityDestroyed(entity);  // Notify the SystemManager
        }

        /**
         * @brief Registers a new component type.
         *
         * This method delegates the registration of a new component type to the ComponentManager.
         *
         * @tparam T The type of the component to register.
         */
        template<typename T>
        void RegisterComponent()
        {
            componentManager->Register<T>();  // Delegate registration to the ComponentManager
        }

        /**
         * @brief Adds a component to an entity.
         *
         * This method adds the specified component to the entity and updates the entity's signature to reflect
         * the new component.
         *
         * @tparam T The type of the component to add.
         * @param entity The ID of the entity to which the component will be added.
         * @param component The component instance to add.
         */
        template<typename T>
        void AddComponent(Entity entity, T component)
        {
            componentManager->AddComponent<T>(entity, component);  // Add the component to the entity

            // Update the entity's signature to reflect the new component
            auto signature = entityManager->GetSignature(entity);
            signature.set(componentManager->GetComponentType<T>(), true);  // Set the component type bit to true
            entityManager->SetSignature(entity, signature);  // Update the entity's signature

            systemManager->EntitySignatureChanged(entity, signature);  // Notify systems of the signature change
        }

        /**
        * @brief Removes a component from an entity.
        *
        * This method removes the specified component from the entity and updates the entity's signature to reflect
        * the removal.
        *
        * @tparam T The type of the component to remove.
        * @param entity The ID of the entity from which the component will be removed.
        */
        template<typename T>
        void RemoveComponent(Entity entity)
        {
            componentManager->RemoveComponent<T>(entity);  // Remove the component from the entity

            // Update the entity's signature to reflect the removed component
            auto signature = entityManager->GetSignature(entity);
            signature.set(componentManager->GetComponentType<T>(), false);  // Set the component type bit to false
            entityManager->SetSignature(entity, signature);  // Update the entity's signature

            systemManager->EntitySignatureChanged(entity, signature);  // Notify systems of the signature change
        }

        /**
          * @brief Retrieves a component from an entity.
          *
          * This method returns a reference to the specified component of the entity.
          *
          * @tparam T The type of the component to retrieve.
          * @param entity The ID of the entity from which to retrieve the component.
          * @return T& A reference to the requested component.
          */
        template<typename T>
        T& GetComponent(Entity entity)
        {
            return componentManager->GetComponent<T>(entity);  // Retrieve the component from the ComponentManager
        }

        /**
          * @brief Gets the component type ID for a specified component type.
          *
          * @tparam T The type of the component for which to retrieve the type ID.
          * @return ComponentType The type ID of the specified component.
          */
        template<typename T>
        ComponentType GetComponentType()
        {
            return componentManager->GetComponentType<T>();  // Return the component type ID from the ComponentManager
        }

        // System methods
        /**
         * @brief Registers a new system of type T.
         *
         * This method delegates the system registration to the SystemManager and returns a shared pointer to the
         * registered system instance.
         *
         * @tparam T The type of the system to register.
         * @return std::shared_ptr<T> A shared pointer to the registered system instance.
         */
        template<typename T>
        std::shared_ptr<T> RegisterSystem()
        {
            return systemManager->RegisterSystem<T>();  // Delegate system registration to the SystemManager
        }

        /**
         * @brief Sets the signature for a specific system.
         *
         * This method associates the specified signature with the given system type in the SystemManager.
         *
         * @tparam T The type of the system for which the signature is being set.
         * @param signature The signature to associate with the system.
         */
        template<typename T>
        void SetSystemSignature(Signature signature)
        {
            systemManager->SetSignature<T>(signature);  // Associate the signature with the system
        }

        /**
        * @brief Retrieves entities that have specific components.
        *
        * This method checks all entities against the required component signature and returns those that match.
        *
        * @tparam ComponentTypes The types of components to check for.
        * @return std::vector<Entity> A vector of entity IDs that have the specified components.
        */
        template<typename... ComponentTypes>
        std::vector<Entity> GetEntitiesWithComponents() {
            // Create a signature for the required components
            Signature requiredSignature = CreateSignature<ComponentTypes...>();

            // Vector to store matching entities
            std::vector<Entity> matchingEntities;

            // Iterate over all entities and check if they match the required signature
            for (Entity entity = 0; entity < maxEntities; ++entity) {
                Signature entitySignature = entityManager->GetSignature(entity);

                // Check if the entity's signature matches the required signature
                if ((entitySignature & requiredSignature) == requiredSignature) {
                    matchingEntities.push_back(entity);  // Entity matches, add to the list
                }
            }

            return matchingEntities;  // Return the list of matching entities
        }

        /**
        * @brief Checks if an entity has a specific component.
        *
        * @tparam T The type of the component to check for.
        * @param entity The ID of the entity to check.
        * @return bool True if the entity has the specified component, false otherwise.
        */
        template<typename T>
        bool HasComponent(Entity entity)
        {
            // Get the component type ID
            ComponentType componentType = componentManager->GetComponentType<T>();

            // Get the entity's signature
            Signature entitySignature = entityManager->GetSignature(entity);

            // Check if the entity has the component by checking the component bit in the signature
            return entitySignature.test(componentType);
        }

        // Expose the ComponentManager for script glue
        ComponentManager& GetComponentManager()
        {
            return *componentManager;
        }

        // Check if an entity has a component dynamically by type name
        bool HasComponentByName(Entity entity, const std::string& typeName)
        {
            auto typeID = componentManager->GetComponentTypeByName(typeName);
            if (typeID == INVALID_COMPONENT_TYPE)
                return false; // Component type is not registered

            auto signature = entityManager->GetSignature(entity);
            return signature.test(typeID);
        }

        // Fetch component type dynamically by name
        ComponentType GetComponentTypeByName(const std::string& typeName)
        {
            return componentManager->GetComponentTypeByName(typeName);
        }

        // Fetch entities with a specific component dynamically by name (optional)
        std::vector<Entity> GetEntitiesWithComponentName(const std::string& typeName)
        {
            
            auto typeID = componentManager->GetComponentTypeByName(typeName);
            if (typeID == INVALID_COMPONENT_TYPE)
                return {}; // Return empty if type is not registered

            std::vector<Entity> matchingEntities;

            for (Entity entity = 0; entity < maxEntities; ++entity)
            {
                auto signature = entityManager->GetSignature(entity);
                if (signature.test(typeID)) {
                    if (HasComponent<ScriptComponent>(entity)) {
                        auto scriptComponent = GetComponent<ScriptComponent>(entity);
                        if (scriptComponent.isActive) {
                            matchingEntities.push_back(entity);
                        }
                    }
                    else if (HasComponent<FruitLoops::TileComponent>(entity)) {
                        auto tileComponent = GetComponent<TileComponent>(entity);
                        if (tileComponent.isActive) {
                            matchingEntities.push_back(entity);
                        }
                    }
                    else {
                        matchingEntities.push_back(entity);
                    }
                    
                    
                }
                    
            }
           
            return matchingEntities;
        }

        template <typename T>
        Entity GetEntityByComponent(const T* componentInstance) {
            auto allEntities = GetEntitiesWithComponents<T>();
            for (const auto& entity : allEntities) {
                const auto& component = GetComponent<T>(entity);
                if (&component == componentInstance) {
                    return entity; // Found the matching entity
                }
            }
            return INVALID_ENTITY; // Return an invalid entity if not found
        }

        MonoObject* GetEntityMonoObject(Entity entity) {
            auto it = m_EntityMonoObjects.find(entity);
            if (it != m_EntityMonoObjects.end()) {
                FL_DEBUG_INFO("Retrieved MonoObject for entity {}.", entity);
                return it->second;
            }

            FL_DEBUG_WARN("Entity {} does not have a corresponding MonoObject. Check if it was properly registered.", entity);
            return nullptr;
        }

        void RegisterEntityMonoObject(Entity entity, MonoObject* monoObject) {
            if (!monoObject) {
                FL_DEBUG_WARN("Attempted to register a null MonoObject for entity {}.", entity);
                return;
            }

            if (m_EntityMonoObjects.find(entity) != m_EntityMonoObjects.end()) {
                FL_DEBUG_INFO("Entity {} already has a registered MonoObject. Overwriting.", entity);
            }

            m_EntityMonoObjects[entity] = monoObject;
            FL_DEBUG_INFO("Registered MonoObject for entity {}.", entity);
        }

    private: 
        /**
          * @brief Sets the component bit in the signature.
          *
          * This helper function sets the bit for the specified component type in the given signature.
          *
          * @tparam ComponentType The type of the component to set in the signature.
          * @param signature The signature to modify.
          */
        template<typename ComponentType>
        void SetComponentBit(Signature& signature) {
            signature.set(componentManager->GetComponentType<ComponentType>());
        }

        /**
          * @brief Creates a signature for a set of component types.
          *
          * This helper function sets the appropriate bits in a signature for each component type.
          *
          * @tparam ComponentTypes The types of components to include in the signature.
          * @return Signature A bitset representing the combined signature of the specified components.
          */
        template<typename... ComponentTypes>
        Signature CreateSignature() {
            Signature signature;

            // Set the appropriate bits for each component type
            (SetComponentBit<ComponentTypes>(signature), ...);  // Fold expression for multiple components

            return signature;
        }

    private:
        std::unique_ptr<ComponentManager> componentManager;  // Pointer to the ComponentManager
        std::unique_ptr<EntityManager> entityManager;  // Pointer to the EntityManager
        std::unique_ptr<SystemManager> systemManager;  // Pointer to the SystemManager
        std::map<Entity, MonoObject*> m_EntityMonoObjects;
    };

    extern ECSHub* ecsHub; ///< Pointer to the ECSHub instance.
}
