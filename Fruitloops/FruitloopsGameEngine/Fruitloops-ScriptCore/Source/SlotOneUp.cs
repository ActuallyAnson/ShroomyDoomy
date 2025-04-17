/******************************************************************************
/*!
\file  SlotOneUp.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    Feb 03, 2024
\brief  This file implements the SlotOneDown class, a behaviour component that handles
       cycling through inventory items for slot one (selecting the previous item).
       
All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FruitLoops
{
    public class SlotOneUp : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _clickProcessed = false;

        public SlotOneUp() { }

        /**
         * @brief Constructs a new SlotOneUp behaviour for the specified entity.
         *
         * This constructor creates or retrieves the ButtonComponent associated with the entity,
         * registers the lifecycle methods with the GameLogicSystem, and sets the behaviour index and owner.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public SlotOneUp(Entity entity)
        {
            Debug.LogInfo("[SlotOneUp] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[SlotOneUp] Adding ButtonComponent to entity {0}", entity.ID);
                _buttonComponent = entity.AddLocalComponent(new ButtonComponent());
            }
            else
            {
                _buttonComponent = entity.GetLocalComponent<ButtonComponent>();
            }

            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);
        }

        /**
         * @brief Factory method to create a SlotOneUp behaviour.
         *
         * Attempts to create and return a new instance of SlotOneUp for the given entity.
         *
         * @param entity The entity for which the behaviour is to be created.
         * @return A new instance of SlotOneUp as a BehaviourComponent, or null if an exception occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[SlotOneUp] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new SlotOneUp(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[SlotOneUp] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the SlotOneUp behaviour for the specified entity.
         *
         * This method initializes the SlotOneUp behaviour for the specified entity.
         *
         * @param entity The entity to which the behaviour is attached.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[SlotOneUp] Initializing for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the SlotOneUp behaviour for the specified entity.
         *
         * This method updates the SlotOneUp behaviour for the specified entity.
         *
         * @param entity The entity to update the behaviour for.
         * @param deltaTime The time since the last frame.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<SlotOneUp>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[SlotOneUp] ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                if (Input_IsMouseOverEntity(entity.ID))
                {
                    button._buttonComponent.IsHovered = true;
                }
                else
                {
                    button._buttonComponent.IsHovered = false;
                }

                if (Input.IsMouseButtonDown(0) && button._buttonComponent.IsHovered && !button._clickProcessed)
                {
                    Debug.LogInfo("[SlotOneUp] Button clicked on entity {0}", entity.ID);
                    InventorySystem.Instance?.NextItem(1);

                    button._clickProcessed = true;
                }

                if (!Input.IsMouseButtonDown(0))
                {
                    button._clickProcessed = false;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[SlotOneUp] Error during update: {0}", ex.Message);
            }
        }

        /**
         * @brief Cleanup function for the SlotOneUp behaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<SlotOneUp>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            button._clickProcessed = false;

            Debug.LogInfo("[SlotOneUp] Cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is over the specified entity.
         *
         * This method checks if the mouse cursor is over the specified entity.
         *
         * @param entityID The ID of the entity to check.
         * @return True if the mouse cursor is over the entity, false otherwise.
         */
        private static bool Input_IsMouseOverEntity(ulong entityID)
        {
            Vector2D entityPos = new Vector2D();
            Vector2D entityScale = new Vector2D();

            try
            {
                InternalCalls.Transform_GetTranslation(entityID, out entityPos);
                InternalCalls.Transform_GetScale(entityID, out entityScale);
            }
            catch (Exception ex)
            {
                Debug.LogError("[SlotOneUp] Entity {0} missing Transform: {1}", entityID, ex.Message);
                return false;
            }

            Vector2D cursorPos = Input.GetCursorPosition();

            float left = entityPos.X - (entityScale.X / 2);
            float right = entityPos.X + (entityScale.X / 2);
            float bottom = entityPos.Y - (entityScale.Y / 2);
            float top = entityPos.Y + (entityScale.Y / 2);

            return (cursorPos.X >= left && cursorPos.X <= right &&
                    cursorPos.Y >= bottom && cursorPos.Y <= top);
        }
    }
}