/******************************************************************************
/*!
\file  QuitButton.cs
\Proj name  Shroomy Doomy
\author  Zhang Tong Yan
\date    Feb 03, 2024
\brief  This file implements the QuitButton class, a behaviour component that handles
       the quit game action when the associated button is clicked. It manages mouse hover
       and click events for the button.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @class QuitButton
     * @brief Behaviour component for handling the quit game action.
     *
     * This class listens for mouse input over its button and, when clicked, triggers the engine's
     * quit game function.
     */
    public class QuitButton : BehaviourComponent
    {
        private ButtonComponent _localButtonComponent;
        private bool _pressedWhileHovering = false;
        private Vector2D _originalScale; // Field to store the original scale
        private const float HoverScaleMultiplier = 1.1f; // Scale up by 10%


        // Default constructor required by Mono
        public QuitButton() { }

        // Constructor for creating QuitButton with an entity
        /**
         * @brief Constructor for initializing the QuitButton with an entity.
         *
         * @param entity The entity this behaviour is attached to.
         */
        public QuitButton(Entity entity)
        {
            Debug.LogInfo("[NewButton] Creating QuitButton for entity {0}", entity.ID);

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

            // Register the QuitButton's lifecycle methods
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[NewButton] QuitButton registered with index {0}", GetBehaviourIndex());
        }

        /**
         * @brief Factory method to create a QuitButton behaviour.
         *
         * @param entity The entity to which this behaviour is attached.
         * @return The created QuitButton behaviour.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for QuitButton on entity {0}", entity.ID);
                return new QuitButton(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the QuitButton behaviour.
         *
         * This method is called when the QuitButton behaviour is initialized.
         *
         * @param entity The entity this behaviour is attached to.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[NewButton] Initializing QuitButton for entity {0}", entity.ID);
        }

        /**
         * @brief Updates the QuitButton behaviour.
         *
         * This method is called every frame to update the QuitButton behaviour.
         *
         * @param entity The entity this behaviour is attached to.
         * @param deltaTime The time elapsed since the last frame.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<QuitButton>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] QuitButton or its ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                // Hover detection
                bool isHovering = Input_IsMouseOverEntity(entity.ID);

                if (isHovering)
                {
                    if (!instance._localButtonComponent.IsHovered)
                    {
                        InternalCalls.Audio_PlaySound("SFX_TileHover", 0.05f);
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

                // Only trigger action on release if click began while hovering
                if (instance._pressedWhileHovering && !Input.IsMouseButtonDown(0))
                {
                    if (isHovering)
                    {
                        InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.2f);
                        Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                        instance._localButtonComponent.InvokeOnClick();

                        CurrButtonBehaviour();
                    }

                    instance._pressedWhileHovering = false; // Reset after release
                }


                // Hide if main menu layer is off
                if (!InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer"))
                {
                    Vector2D zeroScale = Vector2D.Zero;
                    InternalCalls.Transform_SetScale(entity.ID, ref zeroScale);
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Error during QuitButton update: {0}", ex.Message);
            }
        }


        /**
         * @brief Cleanup function for the QuitButton behaviour.
         *
         * Resets the button state and clears the reference to the ButtonComponent.
         *
         * @param entity The entity associated with the behaviour.
         */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<QuitButton>(entity);
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


            Debug.LogInfo("[NewButton] QuitButton cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is currently over the specified entity.
         *
         * Uses the entity's translation and scale to determine if the cursor is within its bounds.
         *
         * @param entityID The unique identifier of the entity.
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
         * @brief Executes the custom action for the QuitButton.
         *
         * Triggers the quit game action by calling the engine's quit function.
         */
        private static void CurrButtonBehaviour()
        {
            // Place your custom action logic here.
            Debug.LogInfo("[NewButton] Executing custom action for QuitButton.");
            InternalCalls.Engine_QuitGame();
        }
    }
}