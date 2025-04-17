/******************************************************************************
/*!
\file  TutorialButtonBehaviour.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    Feb 03, 2024
\brief  This file implements the TutorialButtonBehaviour class, a button behaviour
        component that transitions to tutorial level when clicked.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
    * @class TutorialButtonBehaviour
    * @brief Behaviour component for handling option menu button interactions.
    *
    * This component manages mouse hover effects, click detection, and executes
    * custom actions when the button is clicked.
    */
    public class TutorialButtonBehaviour : BehaviourComponent
    {
        private ButtonComponent _localButtonComponent;
        private bool _pressedWhileHovering = false;
        private Vector2D _originalScale; // Field to store the original scale
        private const float HoverScaleMultiplier = 1.1f; // Scale up by 10%

        // Default constructor required by Mono
        public TutorialButtonBehaviour() { }

        // Constructor for creating TutorialButtonBehaviour with an entity
        /**
         * @brief Initializes an instance of TutorialButtonBehaviour.
         *
         * Checks for an existing ButtonComponent on the associated entity and assigns it,
         * or creates a new one if none exists.
         *
         * @param entity The entity to which this button behaviour is attached.
         */
        public TutorialButtonBehaviour(Entity entity)
        {
            Debug.LogInfo("[NewButton] Creating TutorialButtonBehaviour for entity {0}", entity.ID);

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

            // Register the TutorialButtonBehaviour's lifecycle methods
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[NewButton] TutorialButtonBehaviour registered with index {0}", GetBehaviourIndex());
        }

    /**
     * @brief Creates an instance of TutorialButtonBehaviour for a given entity.
     *
     * @param entity The entity to which the button behaviour is assigned.
     * @return A new instance of TutorialButtonBehaviour.
     */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for TutorialButtonBehaviour on entity {0}", entity.ID);
                return new TutorialButtonBehaviour(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

     /**
     * @brief Initializes the TutorialButtonBehaviour component.
     *
     * Logs initialization information for debugging purposes.
     *
     * @param entity The entity that owns this behaviour.
     */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[NewButton] Initializing TutorialButtonBehaviour for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the button behaviour every frame.
         *
         * Handles mouse hover scaling, click detection, and triggers the appropriate action
         * when the button is clicked.
         *
         * @param entity The entity to update.
         * @param deltaTime The time elapsed since the last frame.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<TutorialButtonBehaviour>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] TutorialButtonBehaviour or its ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                bool isHovering = Input_IsMouseOverEntity(entity.ID);

                // Hover effect
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

                // Track if click began while hovering
                if (Input.IsMouseButtonDown(0) && isHovering && !instance._pressedWhileHovering)
                {
                    instance._pressedWhileHovering = true;
                }

                // On mouse release: only fire if click started on button and ended still hovering
                if (instance._pressedWhileHovering && !Input.IsMouseButtonDown(0))
                {
                    if (isHovering)
                    {
                        InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.2f);
                        Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                        instance._localButtonComponent.InvokeOnClick();
                        CurrButtonBehaviour();
                    }

                    instance._pressedWhileHovering = false;
                }


                // Hide button if MainMenuLayer is off
                if (!InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer"))
                {
                    Vector2D hiddenScale = new Vector2D(0.0f, 0.0f);
                    InternalCalls.Transform_SetScale(entity.ID, ref hiddenScale);
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Error during TutorialButtonBehaviour update: {0}", ex.Message);
            }
        }


        /**
         * @brief Cleans up the TutorialButtonBehaviour when the entity is destroyed or disabled.
         *
         * Resets the button's state and removes references to avoid memory leaks.
         *
         * @param entity The entity being cleaned up.
         */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<TutorialButtonBehaviour>(entity);
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

            Debug.LogInfo("[NewButton] TutorialButtonBehaviour cleanup complete for entity {0}.", entity.ID);
        }

    /**
     * @brief Checks if the mouse cursor is currently over the entity.
     *
     * Uses the entity's transform position and scale to determine if the cursor is within bounds.
     *
     * @param entityID The ID of the entity to check.
     * @return True if the cursor is over the entity, false otherwise.
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
     * @brief Executes the custom action associated with the button.
     *
     * This function is intended to be overridden or modified to define the specific
     * functionality when the button is clicked.
     */
        private static void CurrButtonBehaviour()
        {
            // Place your custom action logic here.
            Debug.LogInfo("[NewButton] Executing custom action for TutorialButtonBehaviour.");
            // Do nothing for now
            InternalCalls.LevelSwitch_MainMenuToTutorial();
			InternalCalls.LevelSwitch_ReloadCurrLevel(); // Reinitialize Enemy UI Health and Energy Bar
		}
    }
}