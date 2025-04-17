/**
 * @file 
 .h
 * @brief Defines the interface for component storage in the FruitLoops ECS framework.
 *
 * This header file declares the `IComponentArray` interface and the `ComponentStorage` template class.
 * The `IComponentArray` interface is designed to ensure that all component storage classes implement the
 * required functionality for managing components associated with entities. The `ComponentStorage` class
 * provides concrete implementations for storing components of a specific type.
 *
 * Key functionalities include:
 * - Abstract methods for managing the lifecycle of components when an entity is destroyed.
 * - Concrete methods for adding, removing, and retrieving components for entities.
 *
 * @tparam T The type of the component being stored in the `ComponentStorage` class.
 *
 * @note This structure allows for a clean separation between interface and implementation, enhancing
 *       maintainability and extensibility in the ECS framework.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once
#include "ECSDefinitions.h"

namespace FruitLoops {

    /**
    * @class IComponentArray
    * @brief Interface for component storage.
    *
    * The IComponentArray class defines an interface for component storage, ensuring that any derived classes
    * implement the required functionality for managing components associated with entities.
    */
    class IComponentArray {
    public:

       /**
         * @class IComponentArray
         * @brief Interface for component storage.
         *
         * The IComponentArray class defines an interface for component storage, ensuring that any derived classes
         * implement the required functionality for managing components associated with entities.
         */
        virtual ~IComponentArray() = default;  // Virtual destructor for proper cleanup

        /**
         * @brief Abstract method for cleanup when an entity is destroyed.
         *
         * This method must be implemented to define how to handle the cleanup of components when an entity is destroyed.
         *
         * @param entity The entity that is being destroyed.
         */
        virtual void OnEntityDestroyed(Entity entity) = 0;  // Abstract method for cleanup when an entity is destroyed

        // Abstract method for dynamically adding a default component by name
        virtual void AddByName(Entity entity) = 0;

        // Check if an entity has the component
        virtual bool HasComponent(Entity entity) const = 0;

        virtual void* GetRawComponent(Entity entity) const = 0;
    };


    /**
     * @class ComponentStorage
     * @brief Stores components of a specific type for multiple entities.
     *
     * The ComponentStorage class is a template class that manages the storage of components of a specific type
     * for all entities. It provides methods to add, remove, and retrieve components associated with entities.
     *
     * @tparam T The type of the component being stored.
     */
    template<typename T>
    class ComponentStorage : public IComponentArray {
    public:
        /**
         * @brief Adds a component to an entity.
         *
         * This function associates a component of type T with a specified entity.
         *
         * @param entity The entity to which the component will be added.
         * @param component The component to add to the entity.
         */
        void Add(Entity entity, T component) {
            assert(entityToIndex.find(entity) == entityToIndex.end() && "Component added to same entity more than once.");  // Check for duplicate components
            size_t newIndex = mSize++;  // Get the new index and increment size
            entityToIndex[entity] = newIndex;  // Map entity to index
            indexToEntity[newIndex] = entity;  // Map index to entity
            components[newIndex] = std::move(component);  // Store the component
        }

        /**
        * @brief Removes a component from an entity.
        *
        * This function disassociates the component of type T from a specified entity.
        *
        * @param entity The entity from which the component will be removed.
        */
        void Remove(Entity entity) {
            assert(entityToIndex.find(entity) != entityToIndex.end() && "Removing non-existent component.");  // Ensure the component exists
            size_t index = entityToIndex[entity];  // Get the index of the component
            size_t lastIndex = --mSize;  // Get the last index and decrement size
            if (index != lastIndex) {
                // Move the last element into the current index
                components[index] = std::move(components[lastIndex]);  // Move the last component to the removed index
                Entity lastEntity = indexToEntity[lastIndex];  // Get the entity for the last index
                entityToIndex[lastEntity] = index;  // Update the mapping for the last entity
            }
            entityToIndex.erase(entity);  // Remove the entity from the index map
            indexToEntity.erase(lastIndex);  // Remove the last index from the entity map
        }

        /**
        * @brief Retrieves a component associated with an entity.
        *
        * This function retrieves a reference to the component of type T associated with a specified entity.
        *
        * @param entity The entity whose component is being retrieved.
        * @return A reference to the component of type T associated with the entity.
        */
        T& Get(Entity entity) {
            assert(entityToIndex.find(entity) != entityToIndex.end() && "Retrieving non-existent component.");  // Ensure the component exists
            return components[entityToIndex[entity]];  // Return the component
        }

         /**
         * @brief Cleans up the component when an entity is destroyed.
         *
         * This function removes the component associated with the destroyed entity.
         *
         * @param entity The entity that is being destroyed.
         */
  

        // Cleanup when an entity is destroyed
        void OnEntityDestroyed(Entity entity) override {
            if (HasComponent(entity)) {
                Remove(entity);
            }
        }

      //has component 
        bool HasComponent(Entity entity) const override {
            return entityToIndex.find(entity) != entityToIndex.end();
        }

        void AddByName(Entity entity) override {
            T component{};
            Add(entity, component);
        }

        void* GetRawComponent(Entity entity) const override {
            auto it = entityToIndex.find(entity);
            if (it != entityToIndex.end()) {
                return const_cast<void*>(static_cast<const void*>(&components[it->second]));
            }
            return nullptr;
        }

        /**
        * @brief Iterates through all components and applies a function.
        *
        * This function allows you to iterate through all components and apply a lambda function to each component.
        *
        * @param func The lambda function to apply to each component.
        */
        void IterateAll(std::function<void(T& component)> func) {
            for (size_t i = 0; i < mSize; i++) {
                func(components[i]);
            }
        }

    private:
        std::array<T, maxEntities> components;  // Array to store the components <T> for various entities.
        std::unordered_map<Entity, size_t> entityToIndex;  // Map to track entities to their indices
        std::unordered_map<size_t, Entity> indexToEntity;  // Map to track indices to their entities
        size_t mSize = 0;  // Current number of components
    };
}