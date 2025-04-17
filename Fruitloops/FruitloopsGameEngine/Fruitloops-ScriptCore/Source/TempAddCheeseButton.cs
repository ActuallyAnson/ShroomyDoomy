/******************************************************************************
/*!
\file  TempAddCheeseButton.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    Feb 03, 2024
\brief  This file implements the TempAddCheeseButton class, a behaviour component that adds an item 
       (in this case, "fish") to the inventory when its associated button is clicked.
       
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
    public class TempAddCheeseButton : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _clickProcessed = false;

        public TempAddCheeseButton() { }

        /**
         * @brief Constructs a new TempAddCheeseButton behaviour for the specified entity.
         *
         * This constructor creates or retrieves the ButtonComponent associated with the entity,
         * registers the lifecycle methods with the GameLogicSystem, and sets the behaviour index and owner.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public TempAddCheeseButton(Entity entity)
        {
            Debug.LogInfo("[TempAddCheeseButton] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[TempAddCheeseButton] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Factory method to create a TempAddCheeseButton behaviour.
         *
         * Attempts to create and return a new instance of TempAddCheeseButton for the given entity.
         *
         * @param entity The entity to which this behaviour is attached.
         * @return The new TempAddCheeseButton behaviour, or null if an exception occurred.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[TempAddCheeseButton] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new TempAddCheeseButton(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[TempAddCheeseButton] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the TempAddCheeseButton behaviour for the specified entity.
         *
         * This method is called when the TempAddCheeseButton behaviour is initialized.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[TempAddCheeseButton] Initializing for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the TempAddCheeseButton behaviour for the specified entity.
         *
         * This method is called once per frame to update the TempAddCheeseButton behaviour.
         *
         * @param entity The entity to which this behaviour is attached.
         * @param deltaTime The time since the last frame, in seconds.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<TempAddCheeseButton>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[TempAddCheeseButton] ButtonComponent is null for entity {0}.", entity.ID);
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
                    Debug.LogInfo("[TempAddCheeseButton] Button clicked on entity {0}", entity.ID);
                    InventorySystem.Instance?.AddItem("fish"); // Call the AddItem function

                    button._clickProcessed = true;
                }

                if (!Input.IsMouseButtonDown(0))
                {
                    button._clickProcessed = false;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[TempAddCheeseButton] Error during update: {0}", ex.Message);
            }
        }

        /**
         * @brief Cleans up the TempAddCheeseButton behaviour for the specified entity.
         *
         * This method is called when the TempAddCheeseButton behaviour is removed from the entity.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<TempAddCheeseButton>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            button._clickProcessed = false;

            Debug.LogInfo("[TempAddCheeseButton] Cleanup complete for entity {0}.", entity.ID);
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
                Debug.LogError("[TempAddCheeseButton] Entity {0} missing Transform: {1}", entityID, ex.Message);
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

