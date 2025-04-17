/******************************************************************************
/*!
\file  EndTurnButton.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    Feb 03, 2024
\brief  This file contains the implementation for the EndTurnButton class, which 
        handles the behavior for an end-turn button in the game. It manages mouse 
        hover and click events, scaling effects, and triggers the turn-switch functionality.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{

    /**
     * @brief Represents a behavior component for the EndTurnButton.
     *
     * This class extends BehaviourComponent and registers its lifecycle methods
     * (initialization, update, and cleanup) with the GameLogicSystem. It handles mouse
     * hover and click events to perform scaling effects and execute the end-turn action.
     */
    public class EndTurnButton : BehaviourComponent
    {
        private ButtonComponent _localButtonComponent;
        private bool _pressedWhileHovering = false;
        private Vector2D _originalScale; // Field to store the original scale
        private const float HoverScaleMultiplier = 1.1f; // Scale up by 10%

        /**
         * @brief Default constructor required by Mono.
         */
        public EndTurnButton() { }

        /**
         * @brief Constructs a new EndTurnButton behavior for a specified entity.
         *
         * This constructor creates or retrieves a local ButtonComponent for the entity,
         * stores the entity's original transform scale, and registers the behavior's lifecycle
         * methods with the GameLogicSystem.
         *
         * @param entity The entity associated with this EndTurnButton behavior.
         */
        public EndTurnButton(Entity entity)
        {
            Debug.LogInfo("[NewButton] Creating EndTurnButton for entity {0}", entity.ID);

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

            // Register the EndTurnButton's lifecycle methods
            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[NewButton] EndTurnButton registered with index {0}", GetBehaviourIndex());
        }

        /**
         * @brief Factory method to create an EndTurnButton behavior.
         *
         * This method attempts to create a new EndTurnButton instance for the specified entity.
         * If an exception occurs, it logs the error and returns null.
         *
         * @param entity The entity for which to create the behavior.
         * @return A new instance of EndTurnButton if successful; otherwise, null.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for EndTurnButton on entity {0}", entity.ID);
                return new EndTurnButton(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initialization function for the EndTurnButton behavior.
         *
         * Logs the initialization event for the behavior on the specified entity.
         *
         * @param entity The entity for which the behavior is being initialized.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[NewButton] Initializing EndTurnButton for entity {0}", entity.ID);
        }
        private bool _hoverSoundPlayed = false;
        /**
         * @brief Update function for the EndTurnButton behavior.
         *
         * This method checks for mouse hover and click events. When hovered, the button's scale
         * is increased by a predefined multiplier; when clicked, it triggers a click sound, invokes
         * the button's click event, and executes the turn-switch action.
         *
         * @param entity The entity associated with this behavior.
         * @param deltaTime The time elapsed since the last update.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            if (InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("StoryLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("ChestOverlayLayer"))
                return;

            var instance = GameLogicSystem.Instance.GetBehaviour<EndTurnButton>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] EndTurnButton or its ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                bool isHovering = Input_IsMouseOverEntity(entity.ID);

                // Hover effect
                if (isHovering)
                {
                    if (!instance._hoverSoundPlayed)
                    {
                        InternalCalls.Audio_PlaySound("SFX_UI_Hover 3", 0.07f);
                        instance._hoverSoundPlayed = true;
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
                    instance._hoverSoundPlayed = false;
                    InternalCalls.Transform_SetScale(entity.ID, ref instance._originalScale);
                }

                var playerEntity = UnitManager.Instance.GetPlayerEntity();
                var playerBehaviour = GameLogicSystem.Instance.GetBehaviour<Player>(playerEntity);
                bool playerMoving = playerBehaviour?.isMoving ?? false;

                // Register initial click when hovering
                if (Input.IsMouseButtonDown(0) &&
                    isHovering &&
                    !instance._pressedWhileHovering)
                {
                    instance._pressedWhileHovering = true;
                }

                // Trigger only if release happens on the same button
                if (instance._pressedWhileHovering &&
                    !Input.IsMouseButtonDown(0) &&
                    isHovering &&
                    UnitManager.Instance.CurrentTurn == TurnType.PlayerTurn &&
                    !playerMoving)
                {
                    InternalCalls.Audio_PlaySound("SFX_ButtonClick", 0.3f);
                    Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                    instance._localButtonComponent.InvokeOnClick();

                    CurrButtonBehaviour();
                }

                // Reset on release
                if (!Input.IsMouseButtonDown(0))
                {
                    instance._pressedWhileHovering = false;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[NewButton] Error during EndTurnButton update: {0} with entity number {1}", ex.Message, entity.ID);
            }
        }

        /**
         * @brief Cleanup function for the EndTurnButton behavior.
         *
         * Resets the state of the local ButtonComponent and clears the click processing flag,
         * then logs the cleanup completion for the specified entity.
         *
         * @param entity The entity for which the behavior is being cleaned up.
         */
        public static void End(Entity entity)
        {
            var instance = GameLogicSystem.Instance.GetBehaviour<EndTurnButton>(entity);
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


            Debug.LogInfo("[NewButton] EndTurnButton cleanup complete for entity {0}.", entity.ID);
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
         * @brief Executes the custom action associated with the EndTurnButton.
         *
         * Logs the action execution and calls the UnitManager to switch the turn.
         */
        private static void CurrButtonBehaviour()
        {
            // Place your custom action logic here.
            Debug.LogInfo("[NewButton] Executing custom action for EndTurnButton.");

            // Retrieve player entity
            var playerEntity = UnitManager.Instance.GetPlayerEntity();
            if (playerEntity != null)
            {
                // Reset player energy to max
                int maxEnergy = InternalCalls.UnitComponent_GetMaxEnergy(playerEntity.ID);
                InternalCalls.UnitComponent_SetEnergy(playerEntity.ID, maxEnergy);
            }

            // Update energy bar UI
            var playerBehaviour = GameLogicSystem.Instance.GetBehaviour<Player>(playerEntity);
            if (playerBehaviour != null)
            {
                playerBehaviour._energyBarComponent.UpdateEnergyBar(0);
            }

            UnitManager.Instance?.SwitchTurn();
        }
    }
}