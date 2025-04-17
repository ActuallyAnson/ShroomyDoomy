/******************************************************************************
/*!
\file  Entity.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    Feb 03, 2024
\brief  This file defines the Entity class, which represents an entity in the ECS.
       It provides functionality to manage local and ECS-managed components, query
       entity properties (such as unit components), and perform various utility operations.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;

namespace FruitLoops
{
    /**
     * @brief Represents an entity within the ECS.
     *
     * Each Entity has a unique ID and can hold both local components (managed in C#)
     * and ECS-managed components (managed by the engine). It also provides helper methods
     * to retrieve and add components as well as query for specific entity properties.
     */
    public class Entity
	{
		// Internal constructor to initialize an entity with an ID
		public Entity(ulong id)
		{
			ID = id;
			if (id == 0)
			{
				//Console.WriteLine("[Entity Constructor] Warning: Constructing an entity with ID 0.");
			}
		}

		// Default constructor for special cases (e.g., null Entity)
		protected Entity()
		{
			ID = 0;
		}

		// Unique ID of the entity
		public readonly ulong ID;

		// Local components stored in a dictionary
		private readonly Dictionary<Type, Component> _localComponents = new Dictionary<Type, Component>();

        // --- Functions for C#-side components ---
        /**
         * @brief Determines if the entity has a local component of the specified type.
         *
         * @tparam T The type of the component.
         * @return True if the entity contains the component; otherwise, false.
         */
        public bool HasLocalComponent<T>() where T : Component
		{
			return _localComponents.ContainsKey(typeof(T));
		}

        /**
         * @brief Retrieves the local component of the specified type.
         *
         * @tparam T The type of the component.
         * @return The component of type T.
         * @exception InvalidOperationException Thrown if the component does not exist.
         */
        public T GetLocalComponent<T>() where T : Component
		{
			if (_localComponents.TryGetValue(typeof(T), out var component))
			{
				return (T)component;
			}

			throw new InvalidOperationException($"Entity does not have local component of type {typeof(T).Name}.");
		}

        /**
         * @brief Adds a new local component to the entity.
         *
         * @tparam T The type of the component.
         * @param component The component to add.
         * @return The added component.
         * @exception InvalidOperationException Thrown if a component of the specified type already exists.
         */
        public T AddLocalComponent<T>(T component) where T : Component
		{
			var type = typeof(T);

			if (_localComponents.ContainsKey(type))
			{
				throw new InvalidOperationException($"Entity already has a local component of type {type.Name}.");
			}

			component.Entity = this;
			_localComponents[type] = component;
			return component;
		}

        // --- Functions for ECS-managed components ---

        /**
         * @brief Determines if the entity has an ECS-managed component of the specified type.
         *
         * @tparam T The type of the ECS component.
         * @return True if the component exists; otherwise, false.
         */
        public bool HasECSComponent<T>() where T : Component
		{
			bool hasComponent = InternalCalls.Entity_HasECSComponent(ID, typeof(T));
			//Console.WriteLine($"Entity {ID} HasECSComponent<{typeof(T).Name}>: {hasComponent}");
			return hasComponent;
		}

        /**
         * @brief Retrieves the ECS-managed component of the specified type.
         *
         * @tparam T The type of the ECS component.
         * @return The ECS component of type T.
         * @exception InvalidOperationException Thrown if the component does not exist.
         */
        public T GetECSComponent<T>() where T : Component
		{
			if (!HasECSComponent<T>())
			{
				throw new InvalidOperationException($"Entity {ID} does not have ECS component of type {typeof(T).Name}.");
			}

			//Console.WriteLine($"Retrieving ECSComponent<{typeof(T).Name}> for entity {ID}");
			T ecsComponent = (T)InternalCalls.Entity_GetECSComponent(ID, typeof(T));
			ecsComponent.Entity = this;
			return ecsComponent;
		}

        /**
         * @brief Adds a new ECS-managed component to the entity.
         *
         * @tparam T The type of the ECS component.
         * @return The newly created component.
         * @exception InvalidOperationException Thrown if the component is a local-only component.
         */
        public T AddECSComponent<T>() where T : Component, new()
		{
			string typeName = typeof(T).Name;

			if (typeName == "ButtonComponent")
				throw new InvalidOperationException($"Component '{typeName}' is a local-only component and cannot be added to the ECS.");

			InternalCalls.Entity_AddECSComponent(ID, typeName);
			T component = new T { Entity = this };
			return component;
		}

        // --- Functions for UnitComponent ---

        /**
         * @brief Determines if the entity has a UnitComponent.
         *
         * @return True if the entity has a UnitComponent; otherwise, false.
         */
        public bool HasUnitComponent => InternalCalls.Entity_HasECSComponentByName(ID, "UnitComponent");

        /**
         * @brief Gets the unit type from the UnitComponent.
         *
         * @return The unit type as a string.
         * @exception InvalidOperationException Thrown if the entity does not have a UnitComponent.
         */
        public string UnitType
		{
			get
			{
				if (!HasUnitComponent)
					throw new InvalidOperationException($"Entity {ID} does not have a UnitComponent.");
				return InternalCalls.UnitComponent_GetUnitType(ID);
			}
		}

        /**
         * @brief Gets or sets the position of the entity via its UnitComponent.
         *
         * @return The current position as a Vector2D.
         * @exception InvalidOperationException Thrown if the entity does not have a UnitComponent.
         */
        public Vector2D Position
        {
            get
            {
                if (!HasUnitComponent)
                    throw new InvalidOperationException($"Entity {ID} does not have a UnitComponent.");

                Vector2D position = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(ID, ref position);
                return position;
            }
            set
            {
                if (!HasUnitComponent)
                    throw new InvalidOperationException($"Entity {ID} does not have a UnitComponent.");

                InternalCalls.UnitComponent_SetPosition(ID, ref value);
            }
        }

        /**
         * @brief Gets the ID of the tile occupied by the entity via its UnitComponent.
         *
         * @return The occupied tile's unique identifier.
         * @exception InvalidOperationException Thrown if the entity does not have a UnitComponent.
         */
        public ulong OccupiedTile
		{
			get
			{
				if (!HasUnitComponent)
					throw new InvalidOperationException($"Entity {ID} does not have a UnitComponent.");
				return InternalCalls.UnitComponent_GetOccupiedTile(ID);
			}
		}

        /**
         * @brief Resets the occupied tile information for the entity's UnitComponent.
         *
         * @exception InvalidOperationException Thrown if the entity does not have a UnitComponent.
         */
        public void ResetOccupiedTile()
		{
			if (!HasUnitComponent)
				throw new InvalidOperationException($"Entity {ID} does not have a UnitComponent.");
			InternalCalls.UnitComponent_ResetOccupiedTile(ID);
		}

        // --- Utility Functions ---

        /**
         * @brief Finds an entity by its name.
         *
         * @param name The name of the entity to find.
         * @return The Entity with the specified name, or null if not found.
         */
        public static Entity FindEntityByName(string name)
		{
			ulong entityID = InternalCalls.Entity_FindECSEntityByName(name);
			return entityID != 0 ? new Entity(entityID) : null;
		}

        /**
         * @brief Casts this entity to a specific type.
         *
         * @tparam T The type to cast the entity to.
         * @return The entity cast to type T.
         * @exception InvalidCastException Thrown if the cast fails.
         */
        public T As<T>() where T : Entity, new()
		{
			object instance = InternalCalls.GetScriptInstance(ID);
			if (instance is T scriptEntity)
			{
				return scriptEntity;
			}

			throw new InvalidCastException($"Failed to cast entity with ID {ID} to type {typeof(T).Name}.");
		}

        /**
         * @brief Retrieves all entities that have an ECS component with the specified name.
         *
         * @param componentTypeName The name of the ECS component.
         * @return An array of entities that contain the specified component.
         */
        public static Entity[] GetEntitiesWithECSComponentName(string componentTypeName)
		{
			var entityIDs = InternalCalls.Entity_GetEntitiesWithECSComponentName(componentTypeName);

			if (entityIDs == null)
			{
				//Console.WriteLine($"[GetEntitiesWithECSComponentName] No entities found for component '{componentTypeName}'.");
				return Array.Empty<Entity>();
			}

			//Console.WriteLine($"[GetEntitiesWithECSComponentName] Retrieved {entityIDs.Length} entity IDs.");
			var entities = new Entity[entityIDs.Length];
			for (int i = 0; i < entityIDs.Length; i++)
			{
				entities[i] = new Entity(entityIDs[i]);
			}

			return entities;
		}

        /**
         * @brief Retrieves an ECS component by its name from the entity.
         *
         * @param componentName The name of the ECS component.
         * @return The ECS component object.
         * @exception InvalidOperationException Thrown if the entity does not have the specified component.
         */
        public object GetECSComponentByName(string componentName)
		{
			//Console.WriteLine($"Checking if entity {ID} has ECSComponent '{componentName}'");
			if (!InternalCalls.Entity_HasECSComponentByName(ID, componentName))
			{
				throw new InvalidOperationException($"Entity {ID} does not have ECS component of type {componentName}.");
			}

			//Console.WriteLine($"Retrieving ECSComponent '{componentName}' for entity {ID}");
			return InternalCalls.Entity_GetECSComponentByName(ID, componentName);
		}
	}
}
