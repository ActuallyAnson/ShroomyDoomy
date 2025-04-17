/******************************************************************************
/*!
\file  LevelSwitchButtonBehaviour.cs
\Proj name  Shroomy Doomy
\author  Zhang Tong Yan
\date    Feb 03, 2024
\brief  This file implements the LevelSwitchButtonBehaviour class, a button behaviour
        component that enables level switching when clicked.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @class LevelSwitchButtonBehaviour
     * @brief Handles button interactions for switching game levels.
     *
     * This behaviour component listens for mouse interactions (hover and click)
     * and triggers a level switch when the button is clicked.
     */
    public class LevelSwitchButtonBehaviour : BehaviourComponent
    {
        private ButtonComponent _localButtonComponent;
        private bool _pressedWhileHovering = false;
        private Vector2D _originalScale; // Field to store the original scale
        private const float HoverScaleMultiplier = 1.1f; // Scale up by 10%

        // Default constructor required by Mono
        public LevelSwitchButtonBehaviour() { }

        // Constructor for creating MainMenuLevelSwitch with an entity
        /**
         * @brief Constructor for initializing the LevelSwitchButtonBehaviour with an entity.
         *
         * @param entity The entity this behaviour is attached to.
         */
        public LevelSwitchButtonBehaviour(Entity entity)
        {
            Debug.LogInfo("[NewButton] Creating MainMenuLevelSwitch for entity {0}", entity.ID);

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

            // Register the MainMenuLevelSwitch's lifecycle methods
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[NewButton] MainMenuLevelSwitch registered with index {0}", GetBehaviourIndex());
        }

        /**
         * @brief Factory method to create an instance of LevelSwitchButtonBehaviour.
         *
         * @param entity The entity to attach the behaviour to.
         * @return A new instance of LevelSwitchButtonBehaviour or null if an error occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for MainMenuLevelSwitch on entity {0}", entity.ID);
                return new LevelSwitchButtonBehaviour(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the MainMenuLevelSwitch behaviour.
         *
         * @param entity The entity this behaviour is attached to.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[NewButton] Initializing MainMenuLevelSwitch for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the MainMenuLevelSwitch behaviour.
         *
         * @param entity The entity this behaviour is attached to.
         * @param deltaTime The time elapsed since the last frame.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<LevelSwitchButtonBehaviour>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] MainMenuLevelSwitch or its ButtonComponent is null for entity {0}.", entity.ID);
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

                // Track if mouse press began while hovering
                if (Input.IsMouseButtonDown(0) && isHovering && !instance._pressedWhileHovering)
                {
                    instance._pressedWhileHovering = true;
                }

                // Handle click release
                if (instance._pressedWhileHovering && !Input.IsMouseButtonDown(0))
                {
                    if (isHovering)
                    {
                        InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.3f);
                        Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                        instance._localButtonComponent.InvokeOnClick();
                        CurrButtonBehaviour();
                    }

                    instance._pressedWhileHovering = false; // Always reset
                }


                // Hide button if not on the right layer
                if (!InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer"))
                {
                    Vector2D zeroScale = new Vector2D(0.0f, 0.0f);
                    InternalCalls.Transform_SetScale(entity.ID, ref zeroScale);
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Error during MainMenuLevelSwitch update: {0}", ex.Message);
            }
        }


        /**
         * @brief Cleanup function for the MainMenuLevelSwitch behaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<LevelSwitchButtonBehaviour>(entity);
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


            Debug.LogInfo("[NewButton] MainMenuLevelSwitch cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is over the entity.
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
         * @brief Executes the level switch when the button is clicked.
         */
        private static void CurrButtonBehaviour()
        {
            // Place your custom action logic here.
            Debug.LogInfo("[NewButton] Executing custom action for MainMenuLevelSwitch.");
            InternalCalls.LevelSwitch_MainMenuToLevel1();
			InternalCalls.LevelSwitch_ReloadCurrLevel(); // Reinitialize Enemy UI Health and Energy Bar
		}
    }
}