/******************************************************************************
/*!
\file  ReloadButtonBehaviour.cs
\Proj name  Shroomy Doomy
\author  Zhang Tong Yan
\date    Feb 03, 2024
\brief  This file implements the ReloadButtonBehaviour class, a behaviour component that 
       handles the reload action when its associated button is clicked. It manages mouse 
       hover effects and click events to trigger the reload of the current level.

All content � 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @class ReloadButtonBehaviour
     * @brief Behaviour component for handling reload actions via a button.
     *
     * This component listens for mouse hover and click events. When the button is clicked,
     * it triggers the level reload functionality.
     */
    public class ReloadButtonBehaviour : BehaviourComponent
    {
        private ButtonComponent _localButtonComponent;
        private bool _pressedWhileHovering = false;
        private Vector2D _originalScale; // Field to store the original scale
        private const float HoverScaleMultiplier = 1.1f; // Scale up by 10%

        // Default constructor required by Mono
        public ReloadButtonBehaviour() { }

        // Constructor for creating ReloadButton with an entity
        /**
         * @brief Constructor for initializing the ReloadButton with an entity.
         *
         * @param entity The entity this behaviour is attached to.
         */
        public ReloadButtonBehaviour(Entity entity)
        {
            Debug.LogInfo("[NewButton] Creating ReloadButton for entity {0}", entity.ID);

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

            // Register the ReloadButton's lifecycle methods
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[NewButton] ReloadButton registered with index {0}", GetBehaviourIndex());
        }

        /**
         * @brief Factory method to create a ReloadButton behaviour.
         *
         * @param entity The entity to which this behaviour is attached.
         * @return The newly created ReloadButton behaviour.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for ReloadButton on entity {0}", entity.ID);
                return new ReloadButtonBehaviour(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initialization method for the ReloadButton.
         *
         * This method is called when the ReloadButton is initialized.
         *
         * @param entity The entity to which this behaviour is attached.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[NewButton] Initializing ReloadButton for entity {0}", entity.ID);
        }

        /**
         * @brief Update method for the ReloadButton.
         *
         * This method is called every frame to update the ReloadButton's state.
         *
         * @param entity The entity to which this behaviour is attached.
         * @param deltaTime The time elapsed since the last frame.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<ReloadButtonBehaviour>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] ReloadButton or its ButtonComponent is null for entity {0}.", entity.ID);
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
                        InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.3f);
                        Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                        instance._localButtonComponent.InvokeOnClick();
                        CurrButtonBehaviour();
                    }

                    instance._pressedWhileHovering = false; // Reset
                }

                // Hide button if PauseMenuLayer is off
                if (!InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer"))
                {
                    Vector2D zeroScale = Vector2D.Zero;
                    InternalCalls.Transform_SetScale(entity.ID, ref zeroScale);
                    return;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Error during ReloadButton update: {0}", ex.Message);
            }
        }



        /**
         * @brief Cleanup function for the ReloadButton behaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<ReloadButtonBehaviour>(entity);
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

            Debug.LogInfo("[NewButton] ReloadButton cleanup complete for entity {0}.", entity.ID);
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
         * @brief Executes the custom action for the reload button.
         *
         * Triggers the reload current level functionality by calling the engine's reload function.
         */
        private static void CurrButtonBehaviour()
        {
            // Place your custom action logic here.
            Debug.LogInfo("[NewButton] Executing custom action for ReloadButton.");

            // First close the pause menu
            InternalCalls.PauseMenu_Toggle();

			int current = InternalCalls.LevelSwitch_GetCurrLevel();
			if (current == 2)
			{
				InternalCalls.LevelSwitch_Level2ToLevel1();
                InventorySystem.Instance.ResetInventorySystem();
				InternalCalls.LevelSwitch_ReloadCurrLevel();
			}
			else
			{
				InternalCalls.LevelSwitch_ReloadCurrLevel();
                InventorySystem.Instance.ResetInventorySystem();
			}
			return;
		}
    }
}