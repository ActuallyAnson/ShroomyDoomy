/**
 * @file EngineSystems.h
 * @brief Defines the base class for systems in the FruitLoops ECS framework.
 *
 * This header file declares the `EngineSystems` abstract class, which serves as a base class for
 * all systems in the Entity-Component-System architecture. It provides a common interface for
 * initializing, updating, and shutting down systems, allowing for a modular and extensible design.
 *
 * Key functionalities include:
 * - A virtual method for initializing systems.
 * - A virtual method for updating systems during each game frame.
 * - A virtual destructor for proper cleanup of derived classes.
 *
 * @note Derived classes should implement the `Initialize` and `Update` methods to define their
 *       specific behaviors.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once
#include "EntityManager.h"

using Entity = std::uint32_t;


namespace FruitLoops {

	class EngineSystems {
	public:
		std::set<Entity> entities; // Set of entities that this system is interested in

		/**
		* @brief Initialize the system.
		*
		* This pure virtual method must be implemented by derived classes to define the initialization
		* behavior of the system.
		*/
		virtual void Initialize() = 0;

		/**
		* @brief Update the system every game frame.
		*
		* This pure virtual method must be implemented by derived classes to define the update behavior
		* of the system during each frame.
		*
		* @param deltaTime The time elapsed since the last frame, used for frame-rate independent
		*                  updates.
		*/
		virtual void Update(float deltaTime) = 0;

		// Shut down the system and free resources 
		//not sure if needed if i have deconstructor n
		//virtual void Shutdown();

		 /**
		 * @brief Destructor for the EngineSystems class.
		 *
		 * This virtual destructor ensures that derived classes are properly cleaned up when deleted
		 * through a base class pointer.
		 */
		virtual ~EngineSystems() {}
	};
}