/******************************************************************************
/*!
\file  SlotTwoDown.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    Feb 03, 2024
\brief  This file implements the SlotOneDown class, a behaviour component that handles
       cycling through inventory items for slot one (selecting the previous item).
       
All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using FruitLoops;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fruitloops
{
    public class SlotTwoDown : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _clickProcessed = false;

        public SlotTwoDown() { }

        /**
         * @brief Constructs a new SlotTwoDown behaviour for the specified entity.
         *
         * This constructor creates or retrieves the ButtonComponent associated with the entity,
         * registers the lifecycle methods with the GameLogicSystem, and sets the behaviour index and owner.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public SlotTwoDown(Entity entity)
        {
            Debug.LogInfo("[SlotTwoDown] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[SlotTwoDown] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Factory method to create a SlotTwoDown behaviour.
         *
         * Attempts to create and return a new instance of SlotTwoDown for the given entity.
         *
         * @param entity The entity to which the SlotTwoDown behaviour will be attached.
         * @return A new SlotTwoDown behaviour component, or null if an exception occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[SlotTwoDown] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new SlotTwoDown(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[SlotTwoDown] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the SlotTwoDown behaviour for the specified entity.
         *
         * This method logs a message indicating that the behaviour is being initialized.
         *
         * @param entity The entity for which the behaviour is being initialized.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[SlotTwoDown] Initializing for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the SlotTwoDown behaviour for the specified entity.
         *
         * This method is called once per frame to update the SlotTwoDown behaviour.
         *
         * @param entity The entity to update the behaviour for.
         * @param deltaTime The time since the last frame, in seconds.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<SlotTwoDown>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[SlotTwoDown] ButtonComponent is null for entity {0}.", entity.ID);
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
                    Debug.LogInfo("[SlotTwoDown] Button clicked on entity {0}", entity.ID);
                    InventorySystem.Instance?.PreviousItem(2);

                    button._clickProcessed = true;
                }

                if (!Input.IsMouseButtonDown(0))
                {
                    button._clickProcessed = false;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[SlotTwoDown] Error during update: {0}", ex.Message);
            }
        }

        /**
         * @brief Cleanup function for the SlotTwoDown behaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<SlotTwoDown>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            button._clickProcessed = false;

            Debug.LogInfo("[SlotTwoDown] Cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is over the specified entity.
         *
         * This method checks if the mouse cursor is over the specified entity by comparing the cursor
         * position to the entity's position and scale.
         *
         * @param entityID The ID of the entity to check.
         * @return True if the cursor is over the entity, false otherwise.
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
                Debug.LogError("[SlotTwoDown] Entity {0} missing Transform: {1}", entityID, ex.Message);
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
