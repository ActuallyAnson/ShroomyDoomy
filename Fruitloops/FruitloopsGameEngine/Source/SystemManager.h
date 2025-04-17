/**
 * @file SystemManager.h
 * @brief Manages the registration and organization of systems in the FruitLoops ECS framework.
 *
 * This header file defines the `SystemManager` class, which is responsible for handling the various
 * systems in the Entity-Component-System architecture. It allows for the registration of systems,
 * managing their signatures, and handling entity destruction and signature changes.
 *
 * Key functionalities include:
 * - Registering new systems of various types.
 * - Associating signatures with each system to define the entities they operate on.
 * - Handling entity destruction by removing entities from relevant systems.
 * - Updating entity memberships in systems based on changes to entity signatures.
 *
 * @note The `SystemManager` relies on templates for system registration, providing flexibility for
 *       the addition of new systems in the framework.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once
#include "EntityManager.h"
#include "ECSDefinitions.h"
#include "System.h"

namespace FruitLoops {

    class SystemManager {
    public:
        /**
        * @brief Register a new system of type T.
        *
        * This method creates an instance of the specified system type and stores it in the system manager.
        *
        * @tparam T The type of the system to register.
        * @return std::shared_ptr<T> A shared pointer to the registered system instance.
        */
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            const char* typeName = typeid(T).name();  // Get the type name of the system
            assert(systems.find(typeName) == systems.end() && "System registered multiple times.");  // Ensure the system type is not already registered

            auto system = std::make_shared<T>();  // Create a new instance of the system
            systems[typeName] = system;  // Store the system instance in the map
            return system;  // Return the system instance
        }

        /**
         * @brief Set the signature for a specific system.
         *
         * This method associates a given signature with the specified system type, defining which entities
         * the system should operate on based on their components.
         *
         * @tparam T The type of the system for which the signature is being set.
         * @param signature The signature that defines the components required by the system.
         */
        template<typename T>
        void SetSignature(Signature signature) {
            const char* typeName = typeid(T).name();  // Get the type name of the system
            assert(systems.find(typeName) != systems.end() && "System used before registration.");  // Ensure the system is registered
            signatures[typeName] = signature;  // Associate the signature with the system
        }

        /**
         * @brief Handle entity destruction.
         *
         * This method is called when an entity is destroyed, removing it from all systems that may be
         * interested in it.
         *
         * @param entity The ID of the entity that has been destroyed.
         */
        void EntityDestroyed(Entity entity) {
            // Remove the entity from all systems
            for (const auto& pair : systems) {
                pair.second->entities.erase(entity);  // Erase the entity from the system's entity set
            }
        }

        /**
        * @brief Handle changes to entity signatures.
        *
        * This method updates entity memberships in systems based on changes to an entity's component
        * signature, adding or removing the entity from systems as necessary.
        *
        * @param entity The ID of the entity whose signature has changed.
        * @param entitySignature The new signature for the entity.
        */
        void EntitySignatureChanged(Entity entity, Signature entitySignature) {
            // Update entity memberships in systems based on signature changes
            for (const auto& pair : systems) {
                const auto& type = pair.first;  // Get the system type
                const auto& system = pair.second;  // Get the system instance
                const auto& systemSignature = signatures[type];  // Get the system's expected signature

                if ((entitySignature & systemSignature) == systemSignature) {
                    system->entities.insert(entity);  // Entity matches system's signature, add it
                }
                else {
                    system->entities.erase(entity);  // Entity no longer matches, remove it
                }
            }
        }

    private:
        std::unordered_map<const char*, Signature> signatures;  // Maps system type to its signature
        std::unordered_map<const char*, std::shared_ptr<EngineSystems>> systems;  // Maps system type to system instance
    };
}
