/**
 * @file EntityManager.h
 * @brief Declares the EntityManager class for managing entities and their signatures in the ECS architecture.
 *
 * This header file defines the EntityManager class, which is used to create, destroy, and manage the signatures of entities
 * in the FruitLoops game engine. Each entity is associated with a signature that determines the components it possesses.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once
#include "ECSDefinitions.h"

namespace FruitLoops {
    class EntityManager {
    public:
        EntityManager();  // Constructor
        Entity CreateEntity();  // Creates a new entity
        void DestroyEntity(Entity entity);  // Destroys an existing entity
        void SetSignature(Entity entity, Signature signature);  // Sets the component signature for an entity
        Signature GetSignature(Entity entity) const;  // Gets the component signature for an entity
    private:
        std::queue<Entity> availableEntities;  // Queue of available entity IDs
        Signature signatures[maxEntities];  // Array to hold signatures for each entity
        std::uint32_t livingEntityCount = 0;  // Count of currently living entities
    };
}