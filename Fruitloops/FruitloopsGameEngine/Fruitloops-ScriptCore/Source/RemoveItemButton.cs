/******************************************************************************
/*!
\file  RemoveItemButton.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    Feb 03, 2024
\brief  This file implements the RemoveItemButton class, a behaviour component that
       handles the removal of the last inventory item when its associated button is clicked.
       It manages click cooldowns and mouse interactions to ensure a smooth user experience.

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
    public class RemoveItemButton : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _clickProcessed = false;
        private float _clickCooldown = 0.3f; // Prevents double-clicking (300ms delay)
        private float _clickTimer = 0f;
        public RemoveItemButton() { }

        /**
         * @brief Constructs a new RemoveItemButton behaviour for the specified entity.
         *
         * This constructor initializes or retrieves the ButtonComponent for the entity,
         * registers the behaviour with the GameLogicSystem, and prepares the button for input handling.
         *
         * @param entity The entity to which the RemoveItemButton behaviour is attached.
         */
        public RemoveItemButton(Entity entity)
        {
            Debug.LogInfo("[RemoveItemButton] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[RemoveItemButton] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Factory method to create a RemoveItemButton behaviour.
         *
         * Attempts to create and register a new instance of RemoveItemButton for the given entity.
         *
         * @param entity The entity to attach the behaviour to.
         * @return A new instance of RemoveItemButton as a BehaviourComponent, or null if an exception occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[RemoveItemButton] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new RemoveItemButton(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[RemoveItemButton] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the RemoveItemButton behaviour for the specified entity.
         *
         * This method is called once when the behaviour is first created, and is used to perform any necessary setup.
         *
         * @param entity The entity to initialize the behaviour for.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[RemoveItemButton] Initializing for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the RemoveItemButton behaviour for the specified entity.
         *
         * This method is called every frame to update the behaviour's state and handle input events.
         *
         * @param entity The entity to update the behaviour for.
         * @param deltaTime The time elapsed since the last frame, in seconds.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<RemoveItemButton>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[RemoveItemButton] ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                // Update the cooldown timer
                if (button._clickProcessed)
                {
                    button._clickTimer -= deltaTime;
                    if (button._clickTimer <= 0f)
                    {
                        button._clickProcessed = false;
                    }
                }

                if (Input_IsMouseOverEntity(entity.ID))
                {
                    button._buttonComponent.IsHovered = true;
                }
                else
                {
                    button._buttonComponent.IsHovered = false;
                }

                // Process click if not already clicked recently
                if (Input.IsMouseButtonDown(0) && button._buttonComponent.IsHovered && !button._clickProcessed)
                {
                    Debug.LogInfo("[RemoveItemButton] Button clicked on entity {0}", entity.ID);

                    // Check if inventory exists
                    if (InventorySystem.Instance != null && InventorySystem.Instance.GetListCount() > 0)
                    {

                        string lastItem = InventorySystem.Instance.GetLastItem();

                        if (lastItem != null)
                        {
                            Debug.LogInfo($"[RemoveItemButton] Removing last item: {lastItem}");
                            InventorySystem.Instance.RemoveItem(lastItem);
                        }
                        else
                        {
                            Debug.LogWarning("[RemoveItemButton] GetLastItem() returned null.");
                        }
                    }
                    else
                    {
                        Debug.LogWarning("[RemoveItemButton] Inventory is empty. No item to remove.");
                    }

                    // Set click cooldown
                    button._clickProcessed = true;
                    button._clickTimer = button._clickCooldown;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[RemoveItemButton] Error during update: {0}", ex.Message);
            }
        }

        /**
         * @brief Cleanup function for the RemoveItemButton behaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<RemoveItemButton>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            button._clickProcessed = false;

            Debug.LogInfo("[RemoveItemButton] Cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is over the specified entity.
         *
         * This method uses the entity's Transform component to determine if the mouse cursor is within the entity's bounds.
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
                Debug.LogError("[RemoveItemButton] Entity {0} missing Transform: {1}", entityID, ex.Message);
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
