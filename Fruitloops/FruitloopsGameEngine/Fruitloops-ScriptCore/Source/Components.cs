/******************************************************************************
/*!
\file  Components.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    Jan 27, 2025
\brief  This file defines the base Component class and various ECS components,
        including Transform, TileComponent, UnitComponent, and ButtonComponent.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;

namespace FruitLoops
{
    /**
     * @brief Base abstract Component class for the ECS.
     *
     * This class serves as the base for all components that can be attached to an entity.
     */
    public abstract class Component
	{
        /**
         * @brief Gets the entity to which this component is attached.
         */
        public Entity Entity { get; internal set; }
	}

    /**
     * @brief Attribute to mark a class as a MonoComponent.
     *
     * This attribute is used to associate an ECS name with a component class.
     */
    [AttributeUsage(AttributeTargets.Class)]
	public class MonoComponentAttribute : Attribute
	{
        /**
         * @brief The ECS name associated with the component.
         */
        public string ECSName { get; }

        /**
         * @brief Constructs a new MonoComponentAttribute with the specified ECS name.
         *
         * @param ecsName The ECS name for the component.
         */
        public MonoComponentAttribute(string ecsName)
		{
			ECSName = ecsName;
		}
	}

    /**
     * @brief Component representing an entity's transform properties.
     *
     * Contains translation, scale, and orientation data.
     */
    [MonoComponent("FruitLoops::Transform")]
	public class Transform : Component
	{
        /**
         * @brief Gets or sets the translation (position) of the entity.
         *
         * The translation is retrieved or updated via InternalCalls.
         *
         * @return The current translation vector.
         */
        public Vector2D Translation
		{
			get
			{
				InternalCalls.Transform_GetTranslation(Entity.ID, out Vector2D translation);
				return translation;
			}
			set
			{
				InternalCalls.Transform_SetTranslation(Entity.ID, ref value);
			}
		}

        /**
         * @brief Gets or sets the scale of the entity.
         *
         * The scale is retrieved or updated via InternalCalls.
         *
         * @return The current scale vector.
         */
        public Vector2D Scale
		{
			get
			{
				InternalCalls.Transform_GetScale(Entity.ID, out Vector2D scale);
				return scale;
			}
			set
			{
				InternalCalls.Transform_SetScale(Entity.ID, ref value);
			}
		}

        /**
         * @brief Gets or sets the orientation of the entity.
         *
         * The orientation is retrieved or updated via InternalCalls.
         *
         * @return The current orientation vector.
         */
        public Vector2D Orientation
		{
			get
			{
				InternalCalls.Transform_GetOrientation(Entity.ID, out Vector2D orientation);
				return orientation;
			}
			set
			{
				InternalCalls.Transform_SetOrientation(Entity.ID, ref value);
			}
		}
	}

    /**
     * @brief Component representing a tile in the game.
     *
     * Contains properties related to walkability, position, movement cost, occupant,
     * and neighbor relationships.
     */
    [MonoComponent("struct FruitLoops::TileComponent")]
	public class TileComponent : Component
	{
        /**
         * @brief Gets or sets whether the tile is walkable.
         *
         * @return True if the tile is walkable; otherwise, false.
         */
        public bool IsWalkable
		{
			get => InternalCalls.TileComponent_GetIsWalkable(Entity.ID);
			set => InternalCalls.TileComponent_SetIsWalkable(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the position of the tile.
         *
         * @return The tile's position vector.
         */
        public Vector2D Position
		{
			get
			{
				InternalCalls.TileComponent_GetPosition(Entity.ID, out Vector2D position);
				return position;
			}
			set
			{
				InternalCalls.TileComponent_SetPosition(Entity.ID, ref value);
			}
		}

        /**
         * @brief Gets or sets the movement cost associated with the tile.
         *
         * @return The movement cost as a float value.
         */
        public float MovementCost
		{
			get => InternalCalls.TileComponent_GetMovementCost(Entity.ID);
			set => InternalCalls.TileComponent_SetMovementCost(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the occupant of the tile.
         *
         * @return The occupant's unique identifier.
         */
        public ulong Occupant
		{
			get => InternalCalls.TileComponent_GetOccupant(Entity.ID);
			set => InternalCalls.TileComponent_SetOccupant(Entity.ID, value);
		}

        /**
         * @brief Clears all neighbor associations for this tile.
         */
        public void ClearNeighbors()
		{
			InternalCalls.TileComponent_ClearNeighbors(Entity.ID);
		}

        /**
         * @brief Adds a neighbor tile to this tile.
         *
         * @param neighbor The TileComponent representing the neighbor to add.
         */
        public void AddNeighbor(TileComponent neighbor)
		{
			InternalCalls.TileComponent_AddNeighbor(Entity.ID, neighbor.Entity.ID);
		}

        /**
         * @brief Retrieves a list of neighbor tiles.
         *
         * @return A list of TileComponent instances representing the neighbors.
         */
        public List<TileComponent> GetNeighbors()
		{
			var neighborIDs = InternalCalls.TileComponent_GetNeighbors(Entity.ID);
			var neighbors = new List<TileComponent>();
			foreach (var neighborID in neighborIDs)
			{
				neighbors.Add(new Entity(neighborID).GetECSComponent<TileComponent>());
			}
			return neighbors;
		}
	}

    /**
     * @brief Component representing a unit (e.g., character or object) in the game.
     *
     * Contains properties such as unit type, health, attack power, energy, and position,
     * as well as the tile the unit occupies.
     */
    [MonoComponent("struct UnitComponent")]
	public class UnitComponent : Component
	{
        /**
         * @brief Gets or sets the type of the unit.
         *
         * @return The unit type as a string.
         */
        public string UnitType
		{
			get => InternalCalls.UnitComponent_GetUnitType(Entity.ID);
			set => InternalCalls.UnitComponent_SetUnitType(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the current health of the unit.
         *
         * @return The current health as an integer.
         */
        public int Health
		{
			get => InternalCalls.UnitComponent_GetHealth(Entity.ID);
			set => InternalCalls.UnitComponent_SetHealth(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the maximum health of the unit.
         *
         * @return The maximum health as an integer.
         */
        public int MaxHealth
		{
			get => InternalCalls.UnitComponent_GetMaxHealth(Entity.ID);
			set => InternalCalls.UnitComponent_SetMaxHealth(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the attack power of the unit.
         *
         * @return The attack power as an integer.
         */
        public int AttackPower
		{
			get => InternalCalls.UnitComponent_GetAttackPower(Entity.ID);
			set => InternalCalls.UnitComponent_SetAttackPower(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the current energy of the unit.
         *
         * @return The current energy as an integer.
         */
        public int Energy
		{
			get => InternalCalls.UnitComponent_GetEnergy(Entity.ID);
			set => InternalCalls.UnitComponent_SetEnergy(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the maximum energy of the unit.
         *
         * @return The maximum energy as an integer.
         */
        public int MaxEnergy
		{
			get => InternalCalls.UnitComponent_GetMaxEnergy(Entity.ID);
			set => InternalCalls.UnitComponent_SetMaxEnergy(Entity.ID, value);
		}

        /**
         * @brief Gets or sets the position of the unit.
         *
         * @return The unit's position as a Vector2D.
         */
        public Vector2D Position
        {
            get
            {
                Vector2D position = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(Entity.ID, ref position);
                return position;
            }
            set
            {
                InternalCalls.UnitComponent_SetPosition(Entity.ID, ref value);
            }
        }

        /**
         * @brief Gets or sets the unique identifier of the tile occupied by the unit.
         *
         * @return The unique identifier of the occupied tile.
         */
        public ulong OccupiedTile
		{
			get => InternalCalls.UnitComponent_GetOccupiedTile(Entity.ID);
			set => InternalCalls.UnitComponent_SetOccupiedTile(Entity.ID, value);
		}

        /**
         * @brief Resets the occupied tile information for the unit.
         */
        public void ResetOccupiedTile()
		{
			InternalCalls.UnitComponent_ResetOccupiedTile(Entity.ID);
		}
	}

    /**
      * @brief Component representing a UI button.
      *
      * Manages button states such as hover, click, and enabled, and allows registration
      * of an on-click action.
      */
    public class ButtonComponent : Component
	{
        /**
         * @brief Gets or sets whether the button is currently hovered.
         */
        public bool IsHovered { get; set; }

        /**
         * @brief Gets or sets whether the button is currently clicked.
         */
        public bool IsClicked { get; set; }

        /**
         * @brief Gets or sets whether the button is enabled.
         */
        public bool IsEnabled { get; set; }

		private Action _onClick;

        /**
         * @brief Registers an action to be invoked when the button is clicked.
         *
         * @param onClick The action delegate to invoke on click.
         * @exception ArgumentNullException Thrown if the onClick action is null.
         */
        public void SetOnClick(Action onClick)
		{
			_onClick = onClick ?? throw new ArgumentNullException(nameof(onClick));
		}

        /**
         * @brief Invokes the registered on-click action if the button is enabled.
         */
        public void InvokeOnClick()
		{
			if (IsEnabled && _onClick != null)
			{
				_onClick.Invoke();
			}
		}
	}
}
