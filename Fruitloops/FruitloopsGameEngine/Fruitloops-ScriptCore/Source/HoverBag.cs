﻿/******************************************************************************
/*!
\file  HoverBag.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    Feb 03, 2024
\brief  This file implements the HoverBag class, a behaviour component that
       adds an item (in this case, "tomato") to the inventory when its associated button
       is clicked. It manages mouse hover and click events for the button.

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
     * @brief Behaviour component for adding an item to the inventory.
     *
     * The HoverBag listens for mouse input over its associated entity's button.
     * When clicked, it calls the InventoryManager's AddItem method to add an item ("tomato").
     */
    public class HoverBag : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        //private bool _clickProcessed = false;

        public HoverBag() { }

        /**
         * @brief Constructs a new HoverBag behaviour for the specified entity.
         *
         * This constructor checks for an existing ButtonComponent on the entity; if none is found,
         * it adds one. It then registers the lifecycle methods with the GameLogicSystem.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public HoverBag(Entity entity)
        {
            Debug.LogInfo("[HoverBag] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[HoverBag] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Factory method to create an HoverBag behaviour.
         *
         * This method wraps the constructor in a try-catch block to log any exceptions that occur
         * during the creation process.
         *
         * @param entity The entity for which the behaviour is to be created.
         * @return A new HoverBag instance as a BehaviourComponent, or null if an exception occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[HoverBag] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new HoverBag(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[HoverBag] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initialization function for the HoverBag behaviour.
         *
         * This method is called when the behaviour is first initialized.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[HoverBag] Initializing for entity {0}", entity.ID);
        }

        /**
         * @brief Update function called every frame for the HoverBag behaviour.
         *
         * This method checks whether the mouse is hovering over the entity and whether the left mouse
         * button is pressed. If so, and if the click has not been processed yet, it calls the InventoryManager
         * to add an item ("tomato").
         *
         * @param entity The entity associated with the behaviour.
         * @param deltaTime The time elapsed since the last update.
         */
        public static void Update(Entity entity, float deltaTime)
        {

            
            var button = GameLogicSystem.Instance.GetBehaviour<HoverBag>(entity);

            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[HoverBag] ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                if (Input_IsMouseOverEntity(entity.ID))
                {
                    button._buttonComponent.IsHovered = true;
                    InventorySystem.Instance.DisplayInventoryUI();
                }
                else
                {
                    button._buttonComponent.IsHovered = false;
                    InventorySystem.Instance.HideInventoryUI();
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[HoverBag] Error during update: {0}", ex.Message);
            }
        }

        /**
         * @brief Cleanup function for the HoverBag behaviour.
         *
         * This method resets the button state and clears the ButtonComponent reference.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<HoverBag>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            //button._clickProcessed = false;

            Debug.LogInfo("[HoverBag] Cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Determines if the mouse cursor is over the specified entity.
         *
         * Retrieves the entity's position and scale and compares them with the current cursor position
         * to determine if the cursor is within the bounds of the entity.
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
                Debug.LogError("[HoverBag] Entity {0} missing Transform: {1}", entityID, ex.Message);
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

