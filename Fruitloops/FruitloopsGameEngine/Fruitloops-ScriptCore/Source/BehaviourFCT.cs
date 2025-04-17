/******************************************************************************/
/*!
\file  BehaviorFCT.cs
\Proj name  Shroomy Doomy
\author  Benjamin 
\date   02 Feb, 2025
\brief  This file contains the implementations for the PhysicsObject class
        and related functions, responsible for handling physics calculations
        in the game, such as velocity, acceleration, and object movement.

All content @ 2025 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @brief Delegate for initializing a behavior on an entity.
     *
     * @param entity The entity for which the behavior is being initialized.
     */
    public delegate void InitBehaviour(Entity entity);

    /**
     * @brief Delegate for updating a behavior on an entity.
     *
     * @param entity The entity whose behavior is updated.
     * @param deltaTime The time elapsed since the last update.
     */
    public delegate void UpdateBehaviour(Entity entity, float deltaTime);

    /**
     * @brief Delegate for ending a behavior on an entity.
     *
     * @param entity The entity for which the behavior is being ended.
     */
    public delegate void EndBehaviour(Entity entity);

    /**
     * @brief Provides a unified interface for initializing, updating, and ending a behavior associated with an entity.
     *
     * Automatically registers the behavior with the GameLogicSystem upon creation.
     */
    public class BehaviourFCT
	{
		private readonly InitBehaviour _initBehaviour;
		private readonly UpdateBehaviour _updateBehaviour;
		private readonly EndBehaviour _endBehaviour;

        /**
         * @brief Gets the entity that owns this behavior.
         */
        public Entity Owner { get; private set; } // The entity that owns this behavior


        /**
         * @brief Constructs a new instance of the BehaviourFCT class.
         *
         * This constructor sets the owner entity and the associated behavior functions.
         * It also automatically registers this behavior with the GameLogicSystem.
         *
         * @param owner The entity that owns this behavior.
         * @param init The delegate to initialize the behavior.
         * @param update The delegate to update the behavior.
         * @param end The delegate to end the behavior.
         * @exception ArgumentNullException Thrown if any of the arguments are null.
         */
        public BehaviourFCT(Entity owner, InitBehaviour init, UpdateBehaviour update, EndBehaviour end)
		{
			Owner = owner ?? throw new ArgumentNullException(nameof(owner));
			_initBehaviour = init ?? throw new ArgumentNullException(nameof(init));
			_updateBehaviour = update ?? throw new ArgumentNullException(nameof(update));
			_endBehaviour = end ?? throw new ArgumentNullException(nameof(end));

			// Automatically add this behavior to the GameLogicSystem
			GameLogicSystem.Instance.AddBehaviour(this);
		}

        /**
         * @brief Invokes the initialization behavior for the owner entity.
         */
        public void Init()
		{
			_initBehaviour?.Invoke(Owner);
		}

        /**
         * @brief Invokes the update behavior for the owner entity.
         *
         * @param deltaTime The time elapsed since the last update.
         */
        public void Update(float deltaTime)
		{
			_updateBehaviour?.Invoke(Owner, deltaTime);
		}

        /**
         * @brief Invokes the end behavior for the owner entity.
         */
        public void End()
		{
			_endBehaviour?.Invoke(Owner);
		}
	}
}
