/**
 * @file EntityManager.cpp
 * @brief Implementation of the EntityManager class responsible for managing entities in the ECS (Entity Component System) architecture.
 *
 * This file contains the implementation of the EntityManager class, which is part of the FruitLoops game engine.
 * It handles the creation and destruction of entities, as well as managing their signatures, which represent the components associated with each entity.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once
#include "pch.h"
#include "EntityManager.h"

namespace FruitLoops {

    // Initialize and push entity IDs to the available queue
    /**
     * @brief Constructs the EntityManager and initializes the available entity queue.
     *
     * This constructor initializes the EntityManager by pushing all possible entity IDs (from 0 to maxEntities - 1)
     * into the available queue. It ensures that the entity manager is ready to create entities.
     */
    EntityManager::EntityManager() {
        for (Entity entity = 1; entity < maxEntities; ++entity) {
            availableEntities.push(entity);
        }
    }

    /**
     * @brief Creates a new entity.
     *
     * This function generates a new entity by retrieving an ID from the available queue, increments the count of living entities,
     * and returns the newly created entity ID. It asserts if the maximum entity limit has been exceeded.
     *
     * @return The newly created entity ID.
     */
    Entity EntityManager::CreateEntity() {
        assert(livingEntityCount < maxEntities && "Too many entities in existence.");  // Ensure entity limit is not exceeded
        Entity id = availableEntities.front();  // Get the next available entity ID
        availableEntities.pop();  // Remove it from the available queue
        ++livingEntityCount;  // Increment living entity count
        return id;  // Return the new entity ID
    }

    /**
     * @brief Destroys an existing entity.
     *
     * This function destroys an entity by resetting its signature and pushing its ID back into the available queue.
     * It decrements the count of living entities and asserts if the entity is out of range.
     *
     * @param entity The entity to be destroyed.
     */
    void EntityManager::DestroyEntity(Entity entity) {
        assert(entity < maxEntities && "Entity out of range.");  // Ensure the entity is valid
        signatures[entity].reset();  // Reset its signature
        availableEntities.push(entity);  // Add it back to the available queue
        --livingEntityCount;  // Decrement living entity count
    }

    /**
     * @brief Sets the component signature for an entity.
     *
     * This function assigns a component signature to a specific entity, which represents the components associated with that entity.
     * It asserts if the entity is out of range.
     *
     * @param entity The entity whose signature is being set.
     * @param signature The signature representing the entity's components.
     */
    void EntityManager::SetSignature(Entity entity, Signature signature) {
        assert(entity < maxEntities && "Entity out of range.");  // Ensure the entity is valid
        signatures[entity] = signature;  // Set the signature
    }

    /**
     * @brief Retrieves the component signature of an entity.
     *
     * This function returns the signature associated with a specific entity. It asserts if the entity is out of range.
     *
     * @param entity The entity whose signature is being retrieved.
     * @return The component signature of the entity.
     */
    Signature EntityManager::GetSignature(Entity entity) const {
        assert(entity < maxEntities && "Entity out of range.");  // Ensure the entity is valid
        return signatures[entity];  // Return the signature
    }
}