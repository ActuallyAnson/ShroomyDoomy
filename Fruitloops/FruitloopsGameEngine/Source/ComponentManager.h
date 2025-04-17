/**
 * @file ComponentManager.h
 * @brief Declares the ComponentManager class for managing component storage and associations with entities.
 *
 * This header file defines the ComponentManager class, which is responsible for registering component types,
 * adding, removing, and retrieving components associated with entities in the FruitLoops game engine.
 * The components are stored in arrays to provide efficient access and management.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once
#include "ECSDefinitions.h"
#include "IComponentArray.h"

namespace FruitLoops {
    class ComponentManager {
    public:
        /**
        * @brief Registers a new component type.
        *
        * This template function registers a new component type by mapping its type name to a unique component type ID
        * and creating a storage array for the components of that type.
        *
        * @tparam T The type of the component to register.
        */
        template<typename T>
        void Register() {
            const char* typeName = typeid(T).name();  // Get the type name
            assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once.");  // Ensure the component type is not already registered

            componentTypes[typeName] = nextComponentType++;  // Map type name to a unique component type ID
            componentArrays[typeName] = std::make_shared<ComponentStorage<T>>();  // Create and store a new component storage

            // Log the registration
            spdlog::info("Registered component: '{}' with ID {}", typeName, componentTypes[typeName]);
        }

        /**
        * @brief Adds a component to an entity.
        *
        * This template function adds a component of type T to a specified entity.
        *
        * @tparam T The type of the component to add.
        * @param entity The entity to which the component will be added.
        * @param component The component to add to the entity.
        */
        template<typename T>
        void AddComponent(Entity entity, T component) {
            const char* typeName = typeid(T).name();
            if (componentTypes.find(typeName) == componentTypes.end()) {
#ifdef DEBUG

                spdlog::error("Attempting to add unregistered component '{}' to entity {}", typeName, entity);
#endif // DEBUG
                throw std::runtime_error("Component type not registered before adding: " + std::string(typeName));
            }

            GetComponentStorage<T>()->Add(entity, std::move(component));  // Delegate to the component storage
            //spdlog::info("Added component '{}' to entity {}", typeName, entity);
        }

        /**
        * @brief Removes a component from an entity.
        *
        * This template function removes a component of type T from a specified entity.
        *
        * @tparam T The type of the component to remove.
        * @param entity The entity from which the component will be removed.
        */
        template<typename T>
        void RemoveComponent(Entity entity) {
            GetComponentStorage<T>()->Remove(entity);  // Delegate to the component storage
        }

        /**
         * @brief Retrieves a component from an entity.
         *
         * This template function retrieves a reference to a component of type T associated with a specified entity.
         *
         * @tparam T The type of the component to retrieve.
         * @param entity The entity from which to retrieve the component.
         * @return A reference to the component of type T associated with the entity.
         */
        template<typename T>
        T& GetComponent(Entity entity) {
            return GetComponentStorage<T>()->Get(entity);  // Delegate to the component storage
        }

        ComponentType GetComponentTypeByName(const std::string& typeName) const {
            auto it = componentTypes.find(typeName);
            if (it == componentTypes.end()) {
                spdlog::warn("Component type '{}' not found in registered components.", typeName);
                return INVALID_COMPONENT_TYPE;
            }

            //spdlog::info("Resolved component type '{}' to ID {}.", typeName, it->second);
            return it->second;
        }

        bool HasComponentType(const std::string& typeName) const {
            bool exists = componentTypes.find(typeName) != componentTypes.end();
            spdlog::info("Checking registration for component type '{}': {}", typeName, exists ? "Found" : "Not Found");
            return exists;
        }

        void AddComponentByName(Entity entity, const std::string& typeName) {
            auto typeID = GetComponentTypeByName(typeName);
            if (typeID == INVALID_COMPONENT_TYPE) {
#ifdef DEBUG

                spdlog::error("Attempted to add unregistered component '{}' to entity {}.", typeName, entity);
#endif // DEBUG
                throw std::runtime_error("Component type not registered: " + typeName);
            }

            auto it = componentArrays.find(typeName);
            if (it == componentArrays.end()) {
#ifdef DEBUG

                spdlog::error("Attempting to add unregistered component '{}' to entity {}", typeName, entity);
#endif // DEBUG
                throw std::runtime_error("Component type not registered: " + typeName);
            }

            it->second->AddByName(entity);
#ifdef DEBUG

            spdlog::info("Added component '{}' to entity {}", typeName, entity);
#endif // DEBUG
        }

        void RemoveComponentByName(Entity entity, const std::string& typeName) {
            auto it = componentArrays.find(typeName);
            if (it == componentArrays.end())
                throw std::runtime_error("Component type not registered: " + typeName);

            it->second->OnEntityDestroyed(entity);
        }

        void LogRegisteredComponents() const {
#ifdef DEBUG

            spdlog::info("Registered Components:");
            for (const auto& pair : componentTypes) {
                spdlog::info(" - '{}' with ID {}", pair.first, pair.second);
            }
#endif // DEBUG
        }

        void* GetComponentByName(Entity entity, const std::string& typeName) const {
            // Find the component array for the specified type name
            auto it = componentArrays.find(typeName);
            if (it == componentArrays.end()) {
#ifdef DEBUG

                spdlog::error("Component type '{}' not registered.", typeName);
#endif // DEBUG
                return nullptr;
            }

            // Retrieve the component array
            auto componentArray = it->second;
            if (!componentArray->HasComponent(entity)) {  // Updated to use HasComponent
#ifdef DEBUG

                spdlog::warn("Entity {} does not have a component of type '{}'.", entity, typeName);
#endif // DEBUG
                return nullptr;
            }

            // Retrieve and return the raw component
            void* component = componentArray->GetRawComponent(entity);
            if (!component) {
#ifdef DEBUG

                spdlog::error("Failed to retrieve raw component of type '{}' for entity {}.", typeName, entity);
#endif // DEBUG
            }

            return component;
        }

        bool HasComponentByName(Entity entity, const std::string& typeName) const {
            // Find the component array for the specified type name
            auto it = componentArrays.find(typeName);
            if (it == componentArrays.end()) {
#ifdef DEBUG

                spdlog::warn("Component type '{}' is not registered.", typeName);
#endif // DEBUG
                return false;
            }

            // Check if the component exists for the given entity
            return it->second->HasComponent(entity);
        }

        const std::unordered_map<std::string, ComponentType>& GetRegisteredComponents() const {
            return componentTypes;
        }

        std::shared_ptr<IComponentArray> GetComponentStorageByName(const std::string& typeName) const {
            auto it = componentArrays.find(typeName);
            return (it != componentArrays.end()) ? it->second : nullptr;
        }

        /**
         * @brief Gets the component type ID.
         *
         * This template function retrieves the component type ID associated with a specified component type.
         *
         * @tparam T The type of the component.
         * @return The component type ID for the specified type.
         */
        template<typename T>
        ComponentType GetComponentType() {
            const char* typeName = typeid(T).name();  // Get the type name
            assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");  // Ensure the component is registered
            return componentTypes[typeName];  // Return the component type ID
        }

        /**
        * @brief Cleans up component storage when an entity is destroyed.
        *
        * This function calls cleanup on all component storages associated with the entity being destroyed.
        *
        * @param entity The entity that is being destroyed.
        */
        void EntityDestroyed(Entity entity) {
            for (const auto& pair : componentArrays) {
                pair.second->OnEntityDestroyed(entity);  // Call cleanup on each component storage
            }
        }

    private:
        std::unordered_map<std::string, ComponentType> componentTypes;
        std::unordered_map<std::string, std::shared_ptr<IComponentArray>> componentArrays;
        ComponentType nextComponentType = {};  // Next available component type ID

        /**
         * @brief Gets the storage for a specific component type.
         *
         * This template function retrieves the component storage for a specific component type.
         *
         * @tparam T The type of the component.
         * @return A shared pointer to the component storage for the specified type.
         */
        template<typename T>
        std::shared_ptr<ComponentStorage<T>> GetComponentStorage() {
            const char* typeName = typeid(T).name();  // Get the type name
            assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");  // Ensure the component is registered
            return std::static_pointer_cast<ComponentStorage<T>>(componentArrays[typeName]);  // Return the component storage
        }
    };
}
