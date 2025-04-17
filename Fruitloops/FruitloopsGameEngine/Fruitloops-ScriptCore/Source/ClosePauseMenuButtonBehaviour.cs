/******************************************************************************
/*!
\file  ClosePauseMenuButtonBehaviour.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    04 Feb, 2025
\brief  This file contains the implementation for the ClosePauseMenuButtonBehaviour class,
        which handles the behavior for closing the pause menu via a button in the game.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{

    /**
     * @brief A behavior component that manages the closing of the pause menu button.
     *
     * This class extends BehaviourComponent and registers its lifecycle methods
     * (initialization, update, and cleanup) with the GameLogicSystem. It handles
     * mouse hover and click events to perform scaling effects and trigger the
     * pause menu toggle.
     */
    public class ClosePauseMenuButtonBehaviour : BehaviourComponent
    {
        /**
         * @brief The local ButtonComponent associated with this behavior.
         */
        private ButtonComponent _localButtonComponent;

                /**
         * @brief The local ButtonComponent associated with this behavior.
         */
        private bool _escPreviouslyHeld = false;

        /**
         * @brief Flag to track whether the button click has been processed.
         */

        private bool _pressedWhileHovering = false;

        /**
         * @brief Stores the original scale of the entity's transform.
         */
        private Vector2D _originalScale;

        /**
         * @brief The scale multiplier applied when the button is hovered.
         */
        private const float HoverScaleMultiplier = 1.1f;

        // Default constructor required by Mono
        public ClosePauseMenuButtonBehaviour() { }

        /**
         * @brief Constructs a new ClosePauseMenuButtonBehaviour for a specified entity.
         *
         * Initializes the behavior by adding a local ButtonComponent if it does not exist,
         * storing the original transform scale, and registering the lifecycle methods with
         * the GameLogicSystem.
         *
         * @param entity The entity associated with this behavior.
         */
        public ClosePauseMenuButtonBehaviour(Entity entity)
        {
            Debug.LogInfo("[NewButton] Creating ClosePauseMenuButtonBehaviour for entity {0}", entity.ID);

            // Check if a local ButtonComponent exists, otherwise create one
            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[NewButton] Adding local ButtonComponent to entity {0}", entity.ID);
                _localButtonComponent = entity.AddLocalComponent(new ButtonComponent());
            }
            else
            {
                _localButtonComponent = entity.GetLocalComponent<ButtonComponent>();
            }

            // Store the original scale of the entity's transform.
            InternalCalls.Transform_GetScale(entity.ID, out _originalScale);

            // Register the ClosePauseMenuButtonBehaviour's lifecycle methods
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[NewButton] ClosePauseMenuButtonBehaviour registered with index {0}", GetBehaviourIndex());
        }

        /**
         * @brief Factory method for creating a ClosePauseMenuButtonBehaviour.
         *
         * Attempts to create a new instance of ClosePauseMenuButtonBehaviour for the specified entity.
         * If an exception occurs, it logs the error and returns null.
         *
         * @param entity The entity for which to create the behavior.
         * @return A new instance of ClosePauseMenuButtonBehaviour if successful; otherwise, null.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for ClosePauseMenuButtonBehaviour on entity {0}", entity.ID);
                return new ClosePauseMenuButtonBehaviour(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initialization function for the ClosePauseMenuButtonBehaviour.
         *
         * Logs the initialization event for the behavior on the specified entity.
         *
         * @param entity The entity for which the behavior is being initialized.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[NewButton] Initializing ClosePauseMenuButtonBehaviour for entity {0}", entity.ID);
        }

        /**
         * @brief Update function for the ClosePauseMenuButtonBehaviour.
         *
         * Checks for mouse hover and click events on the button. If hovered, the button is scaled
         * up by a predefined multiplier. On click, it plays a sound effect, invokes the button's click event,
         * and executes custom functionality to toggle the pause menu.
         *
         * @param entity The entity associated with this behavior.
         * @param deltaTime The time elapsed since the last update.
         */
        public static void Update(Entity entity, float deltaTime)
        {

            if (InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("StoryLayer"))
                return;

            var instance = GameLogicSystem.Instance.GetBehaviour<ClosePauseMenuButtonBehaviour>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] ClosePauseMenuButtonBehaviour or its ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            // ESC key toggles pause menu (open or close)
            bool escKeyDown = Input.IsKeyPressed(256);
            if (escKeyDown && !instance._escPreviouslyHeld)
            {
                CurrButtonBehaviour(); // Toggle pause menu
            }
            instance._escPreviouslyHeld = escKeyDown;

            // Only allow interactions if PauseMenuLayer is active
            if (!InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer"))
            {
                // Ensure the button is visually hidden
                Vector2D zeroScale = Vector2D.Zero;
                InternalCalls.Transform_SetScale(entity.ID, ref zeroScale);

                // Reset interaction state
                instance._pressedWhileHovering = false;
                instance._localButtonComponent.IsHovered = false;
                return;
            }

            try
            {
                // Hover logic
                bool isHovering = Input_IsMouseOverEntity(entity.ID);
                if (isHovering)
                {
                    if (!instance._localButtonComponent.IsHovered)
                    {
                        InternalCalls.Audio_PlaySound("SFX_TileHover", 0.005f);
                    }

                    instance._localButtonComponent.IsHovered = true;

                    Vector2D hoveredScale = new Vector2D(
                        instance._originalScale.X * HoverScaleMultiplier,
                        instance._originalScale.Y * HoverScaleMultiplier
                    );
                    InternalCalls.Transform_SetScale(entity.ID, ref hoveredScale);
                }
                else
                {
                    instance._localButtonComponent.IsHovered = false;
                    InternalCalls.Transform_SetScale(entity.ID, ref instance._originalScale);
                }

                // Click logic
                if (Input.IsMouseButtonDown(0) && isHovering && !instance._pressedWhileHovering)
                {
                    instance._pressedWhileHovering = true;
                }

                if (instance._pressedWhileHovering && !Input.IsMouseButtonDown(0))
                {
                    if (isHovering)
                    {
                        InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.3f);
                        Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                        instance._localButtonComponent.InvokeOnClick();
                        InternalCalls.PauseMenu_Toggle(); // Only CLOSE the pause menu
                    }

                    instance._pressedWhileHovering = false;
                }

            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Error during ClosePauseMenuButtonBehaviour update: {0}", ex.Message);
            }
        }


        /**
         * @brief Cleanup function for the ClosePauseMenuButtonBehaviour.
         *
         * Resets the state of the local ButtonComponent and clears the click processing flag,
         * then logs the cleanup completion for the specified entity.
         *
         * @param entity The entity for which the behavior is being cleaned up.
         */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<ClosePauseMenuButtonBehaviour>(entity);
            if (instance == null)
            {
                return;
            }

            // Reset the local ButtonComponent state
            if (instance._localButtonComponent != null)
            {
                instance._localButtonComponent.IsHovered = false;
                instance._localButtonComponent = null;
            }


            Debug.LogInfo("[NewButton] ClosePauseMenuButtonBehaviour cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Determines whether the mouse cursor is over the specified entity.
         *
         * Retrieves the entity's position and scale, then checks if the cursor is within the bounds
         * of the entity.
         *
         * @param entityID The unique identifier of the entity to check.
         * @return True if the mouse is over the entity; otherwise, false.
         */
        private static bool Input_IsMouseOverEntity(ulong entityID)
        {
            // This method remains the same as in your original ButtonBehaviour
            Vector2D entityPos = new Vector2D();
            Vector2D entityScale = new Vector2D();

            try
            {
                InternalCalls.Transform_GetTranslation(entityID, out entityPos);
                InternalCalls.Transform_GetScale(entityID, out entityScale);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Entity {0} is missing Transform components: {1}", entityID, ex.Message);
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

        /**
         * @brief Executes the custom action associated with the close pause menu button.
         *
         * Logs the execution and toggles the pause menu.
         */
        private static void CurrButtonBehaviour()
        {
            // Place your custom action logic here.
            Debug.LogInfo("[NewButton] Executing custom action for ClosePauseMenuButtonBehaviour.");
            InternalCalls.PauseMenu_Toggle();
        }
    }
}