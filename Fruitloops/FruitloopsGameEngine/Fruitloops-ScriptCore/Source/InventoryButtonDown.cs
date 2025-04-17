/******************************************************************************
/*!
\file  InventoryButtonDown.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    Feb 03, 2024
\brief  This file implements the InventoryButtonDown class, a behaviour component
       that listens for mouse input over its associated button. When clicked,
       it calls the InventoryManager to select the previous inventory item.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FruitLoops
{
    /**
     * @brief Behaviour component for navigating inventory downward.
     *
     * The InventoryButtonDown component listens for mouse hover and click events on its associated
     * entity's button. When clicked, it calls the InventoryManager to select the previous item in the inventory.
     */
    public class InventoryButtonDown : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _pressedWhileHovering = false;

        public InventoryButtonDown() { }

        /**
         * @brief Constructs a new InventoryButtonDown behaviour for the specified entity.
         *
         * This constructor creates or retrieves the ButtonComponent associated with the entity,
         * registers the lifecycle methods with the GameLogicSystem, and sets the behaviour index and owner.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public InventoryButtonDown(Entity entity)
        {
            Debug.LogInfo("[InventoryButtonDown] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[InventoryButtonDown] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Factory method to create an InventoryButtonDown behaviour.
         *
         * Wraps the constructor call in a try-catch block to handle exceptions.
         *
         * @param entity The entity for which the behaviour is to be created.
         * @return A new InventoryButtonDown instance as a BehaviourComponent, or null if an exception occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[InventoryButtonDown] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new InventoryButtonDown(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[InventoryButtonDown] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initialization function for the InventoryButtonDown behaviour.
         *
         * Logs the initialization event for the behaviour.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[InventoryButtonDown] Initializing for entity {0}", entity.ID);
        }

        /**
         * @brief Update function called every frame for the InventoryButtonDown behaviour.
         *
         * Checks whether the mouse is hovering over the entity's button and whether the left mouse button
         * is pressed. If the button is hovered and clicked (and the click hasn't been processed yet),
         * it calls the InventoryManager to select the previous item.
         *
         * @param entity The entity associated with the behaviour.
         * @param deltaTime The time elapsed since the last update.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<InventoryButtonDown>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[InventoryButtonDown] ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                bool isHovering = Input_IsMouseOverEntity(entity.ID);

                // Handle hover state
                button._buttonComponent.IsHovered = isHovering;

                // If mouse is pressed while hovering and not already marked
                if (Input.IsMouseButtonDown(0) && isHovering && !button._pressedWhileHovering)
                {
                    button._pressedWhileHovering = true;
                }

                // If mouse is released and the pointer is still over the button
                if (button._pressedWhileHovering && !Input.IsMouseButtonDown(0))
                {
                    if (isHovering)
                    {
                        Debug.LogInfo("[InventoryButtonDown] Button clicked on entity {0}", entity.ID);
                        InventorySystem.Instance?.PreviousItem(1);
                    }

                    // Always reset after release
                    button._pressedWhileHovering = false;
                }

                // Reset click processed state if not holding the button

            }
            catch (Exception ex)
            {
                Debug.LogError("[InventoryButtonDown] Error during update: {0}", ex.Message);
            }
        }


        /**
         * @brief Cleanup function for the InventoryButtonDown behaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<InventoryButtonDown>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            Debug.LogInfo("[InventoryButtonDown] Cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Determines whether the mouse cursor is over the specified entity.
         *
         * Retrieves the entity's position and scale, then compares these with the current cursor position.
         *
         * @param entityID The unique identifier of the entity.
         * @return True if the cursor is over the entity; otherwise, false.
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
                Debug.LogError("[InventoryButtonDown] Entity {0} missing Transform: {1}", entityID, ex.Message);
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

