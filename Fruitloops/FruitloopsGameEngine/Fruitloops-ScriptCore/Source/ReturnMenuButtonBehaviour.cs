/******************************************************************************
/*!
\file  ReturnMenuButtonBehaviour.cs
\Proj name  Shroomy Doomy
\author  Zhang Tong Yan
\date    Feb 03, 2024
\brief  This file implements the ReturnMenuButtonBehaviour class, a button behaviour
        component that enables returning to the main menu when clicked.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{

    /**
     * @class ReturnMenuButtonBehaviour
     * @brief Behaviour component for handling the return to main menu action.
     *
     * This component listens for mouse hover and click events on its associated button.
     * When clicked, it triggers a transition from the pause menu back to the main menu.
     */
    public class ReturnMenuButtonBehaviour : BehaviourComponent
    {
        private ButtonComponent _localButtonComponent;
        private bool _pressedWhileHovering = false;
        private Vector2D _originalScale; // Field to store the original scale
        private const float HoverScaleMultiplier = 1.1f; // Scale up by 10%
        private bool _audioPlayed = false;
        private static bool _hasNavigated = false;

        // Default constructor required by Mono
        public ReturnMenuButtonBehaviour() { }

        // Constructor for creating ReturnMenuButtonBehaviour with an entity
        /**
         * @brief Constructor for initializing the ReturnMenuButtonBehaviour with an entity.
         *
         * @param entity The entity this behaviour is attached to.
         */
        public ReturnMenuButtonBehaviour(Entity entity)
        {
            Debug.LogInfo("[NewButton] Creating ReturnMenuButtonBehaviour for entity {0}", entity.ID);

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

            // Register the ReturnMenuButtonBehaviour's lifecycle methods
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[NewButton] ReturnMenuButtonBehaviour registered with index {0}", GetBehaviourIndex());
        }

        /**
         * @brief Factory method to create a ReturnMenuButtonBehaviour.
         *
         * Attempts to create and register a new instance of ReturnMenuButtonBehaviour for the given entity.
         *
         * @param entity The entity to attach the behaviour to.
         * @return A new instance of ReturnMenuButtonBehaviour as a BehaviourComponent, or null if an exception occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for ReturnMenuButtonBehaviour on entity {0}", entity.ID);
                return new ReturnMenuButtonBehaviour(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the ReturnMenuButtonBehaviour for the specified entity.
         *
         * This method is called once when the behaviour is first created, and is used to perform any necessary setup.
         *
         * @param entity The entity to initialize the behaviour for.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[NewButton] Initializing ReturnMenuButtonBehaviour for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the ReturnMenuButtonBehaviour for the specified entity.
         *
         * This method is called once per frame to update the behaviour's state.
         *
         * @param entity The entity to update the behaviour for.
         * @param deltaTime The time elapsed since the last frame.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<ReturnMenuButtonBehaviour>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] ReturnMenuButtonBehaviour or its ButtonComponent is null for entity {0}.", entity.ID);
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

                // Track press start
                if (Input.IsMouseButtonDown(0) && isHovering && !instance._pressedWhileHovering)
                {
                    instance._pressedWhileHovering = true;
                }

                // Handle click on release
                if (instance._pressedWhileHovering && !Input.IsMouseButtonDown(0))
                {
                    if (isHovering)
                    {
                        if (!instance._audioPlayed)
                        {
                            InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.2f);
                            instance._audioPlayed = true;
                        }

                        Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                        instance._localButtonComponent.InvokeOnClick();
                        CurrButtonBehaviour(entity);
                    }

                    instance._pressedWhileHovering = false;
                }

                // Hide button if PauseMenuLayer is off
                if (!InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer"))
                {
                    Vector2D zeroScale = Vector2D.Zero;
                    InternalCalls.Transform_SetScale(entity.ID, ref zeroScale);
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Error during ReturnMenuButtonBehaviour update: {0}", ex.Message);
            }
        }


        /**
         * @brief Cleanup function for the ReturnMenuButtonBehaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<ReturnMenuButtonBehaviour>(entity);
            if (instance == null)
                return;

            if (instance._localButtonComponent != null)
            {
                instance._localButtonComponent.IsHovered = false;
                instance._localButtonComponent = null;
            }

            _hasNavigated = false; // Reset navigation flag

            Debug.LogInfo("[NewButton] ReturnMenuButtonBehaviour cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is currently over the specified entity.
         *
         * Uses the entity's translation and scale to determine if the cursor is within its bounds.
         *
         * @param entityID The unique identifier of the entity.
         * @return True if the mouse cursor is over the entity, false otherwise.
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
         * @brief Executes the custom action for the ReturnMenuButtonBehaviour.
         *
         * Triggers the transition from the pause menu back to the main menu by calling the engine's level switch function.
         */
		private static void CurrButtonBehaviour(Entity entity)
		{
			if (_hasNavigated) return;
			_hasNavigated = true;

			Debug.LogInfo("[NewButton] Executing custom action for ReturnMenuButtonBehaviour.");

			int currLevel = InternalCalls.LevelSwitch_GetCurrLevel();
			if (currLevel == 0)
			{
				// Clear tutorial arrows when leaving tutorial
				TutorialManager.ResetAllArrows();
			}

			TileManager.Instance.HideAllTileHighlights();

			var instance = GameLogicSystem.Instance.GetBehaviour<ReturnMenuButtonBehaviour>(entity);
			if (instance != null)
			{
				Vector2D zeroScale = Vector2D.Zero;
				InternalCalls.Transform_SetScale(instance.GetOwner().ID, ref zeroScale);
			}

			InternalCalls.LevelSwitch_PauseMenuToMainMenu();
		}

	}
}