/******************************************************************************/
/*!
\file  BehaviorComponent.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng 
\date   06 Feb, 2025
\brief  This file contains the implementation for the BehaviourComponent class,
        which handles game logic behaviors associated with an entity in the game.
        It manages the behavior index and the owner of the behavior.

All content @ 2025 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
/******************************************************************************/
namespace FruitLoops
{
    /// <summary>
    /// The BehaviourComponent class manages the behavior logic for game entities.
    /// It allows setting and retrieving a behavior index, as well as setting and
    /// retrieving the owner entity for the behavior.
    /// </summary>
    public class BehaviourComponent : Component
	{
		// Protected: holds the behavior index in the GameLogicSystem's behavior container
		protected int _behaviourIndex;

        /// <summary>
        /// Sets the behavior index for this component.
        /// </summary>
        public void SetBehaviourIndex(int behaviourIndex)
		{
			_behaviourIndex = behaviourIndex;
		}

        /// <summary>
        /// Gets the current behavior index.
        /// </summary>
        public int GetBehaviourIndex()
		{
			return _behaviourIndex;
		}

		// Owner of the behavior
		private Entity _owner;

        /// <summary>
        /// Sets the owner entity for this behavior component.
        /// </summary>
        public void SetOwner(Entity owner)
		{
			_owner = owner;
		}

        /// <summary>
        /// Gets the owner entity for this behavior component.
        /// </summary>
        public Entity GetOwner()
		{
			return _owner;
		}
	}
}
