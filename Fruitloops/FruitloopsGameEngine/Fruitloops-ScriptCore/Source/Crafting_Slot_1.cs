/******************************************************************************
/*!
\file  Crafting_Slot_1.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    March 14, 2025
\brief  Implements the Crafting_Slot_1 class, a behaviour component that handles
        cycling through inventory items for crafting slot one (selecting next or previous item).

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
    /**
     * @brief Manages the first crafting slot for cycling through inventory items.
     * 
     * @note Handles left-click to select the next item and right-click to select the previous
     * item in the inventory for crafting slot one, using a button-like interface.
     * Inherits from BehaviourComponent.
     */
    public class Crafting_Slot_1 : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        //private bool _clickProcessed = false;

        private static bool _previousLeftClick = false;
        private static bool _previousRightClick = false;

        /**
         * @brief Default constructor for Crafting_Slot_1.
         */
        public Crafting_Slot_1() { }

        /**
         * @brief Constructs a Crafting_Slot_1 instance for a specific entity.
         * 
         * @param entity The Entity to associate with this crafting slot behavior.
         * @note Adds or retrieves a ButtonComponent, registers as a behavior with GameLogicSystem.
         */
        public Crafting_Slot_1(Entity entity)
        {
            Debug.LogInfo("[Crafting_Slot_1] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[Crafting_Slot_1] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Creates a new Crafting_Slot_1 behavior instance for an entity.
         * 
         * @param entity The Entity to associate with the crafting slot behavior.
         * @return A new Crafting_Slot_1 instance, or null if an exception occurs.
         * @note Wraps construction in a try-catch for error handling.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[Crafting_Slot_1] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new Crafting_Slot_1(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[Crafting_Slot_1] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the Crafting_Slot_1 behavior for an entity.
         * 
         * @param entity The Entity associated with the crafting slot behavior.
         * @note Logs initialization; no additional setup required.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[Crafting_Slot_1] Initializing for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the crafting slot behavior state.
         * 
         * @param entity The Entity associated with the crafting slot behavior.
         * @param deltaTime The time elapsed since the last update in seconds.
         * @note Detects hover state and processes left-click (next item) and right-click
         * (previous item) to cycle through inventory items for slot one.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<Crafting_Slot_1>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[Crafting_Slot_1] ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                bool isHovered = Input_IsMouseOverEntity(entity.ID);
                button._buttonComponent.IsHovered = isHovered;

                bool leftClick = Input.IsMouseButtonDown(0);
                bool rightClick = Input.IsMouseButtonDown(1);

                // Process Left Click (Next Item)
                if (leftClick && !_previousLeftClick && isHovered)
                {
                    Debug.LogInfo("[Crafting_Slot_1] Left Button clicked on entity {0}", entity.ID);
                    InventorySystem.Instance?.NextItem(1);
                }

                // Process Right Click (Previous Item)
                if (rightClick && !_previousRightClick && isHovered)
                {
                    Debug.LogInfo("[Crafting_Slot_1] Right Button clicked on entity {0}", entity.ID);
                    InventorySystem.Instance?.PreviousItem(1);
                }

                // Update previous click state
                _previousLeftClick = leftClick;
                _previousRightClick = rightClick;
            }
            catch (Exception ex)
            {
                Debug.LogError("[Crafting_Slot_1] Error during update: {0}", ex.Message);
            }
        }

        /**
         * @brief Cleans up the Crafting_Slot_1 behavior.
         * 
         * @param entity The Entity associated with the crafting slot behavior.
         * @note Resets hover state, clears button reference, and logs cleanup.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<Crafting_Slot_1>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            //button._clickProcessed = false;

            Debug.LogInfo("[Crafting_Slot_1] Cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is over the entity.
         * 
         * @param entityID The ID of the entity to check.
         * @return True if the mouse is over the entity, false otherwise.
         * @note Uses entity's position and scale to determine bounds; handles transform retrieval errors.
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
                Debug.LogError("[Crafting_Slot_1] Entity {0} missing Transform: {1}", entityID, ex.Message);
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