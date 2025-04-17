///******************************************************************************
///*!
//\file  SettingsButtonBehaviour.cs
//\Proj name  Shroomy Doomy
//\author  Zhang Tong Yan
//\date    Feb 03, 2024
//\brief  This file implements the SettingsButtonBehaviour class, a behaviour component that
//       toggles the settings menu when its associated button is clicked. It manages mouse hover
//       effects and click events to trigger the settings action.

//All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
//*/
///******************************************************************************/
//using System;

//namespace FruitLoops
//{
//    /**
//     * @class SettingsButtonBehaviour
//     * @brief Behaviour component for toggling the settings menu.
//     *
//     * This component listens for mouse hover and click events on its associated entity's button.
//     * When clicked, it triggers the settings action (e.g., toggling the pause menu).
//     */
//    public class SettingsButtonBehaviour : BehaviourComponent
//    {
//        private ButtonComponent _localButtonComponent;
//        private bool _escPreviouslyHeld = false;
//        private static bool _pauseMenuToggledThisFrame = false;  // Prevent toggle & click conflict

//        // Default constructor required by Mono
//        public SettingsButtonBehaviour() { }

//        // Constructor for creating SettingsButtonBehaviour with an entity
//        /**
//         * @brief Constructor for initializing the SettingsButtonBehaviour with an entity.
//         *
//         * @param entity The entity this behaviour is attached to.
//         */
//        public SettingsButtonBehaviour(Entity entity)
//        {
//            Debug.LogInfo("[NewButton] Creating SettingsButtonBehaviour for entity {0}", entity.ID);

//            // Check if a local ButtonComponent exists, otherwise create one
//            if (!entity.HasLocalComponent<ButtonComponent>())
//            {
//                Debug.LogInfo("[NewButton] Adding local ButtonComponent to entity {0}", entity.ID);
//                _localButtonComponent = entity.AddLocalComponent(new ButtonComponent());
//            }
//            else
//            {
//                _localButtonComponent = entity.GetLocalComponent<ButtonComponent>();
//            }

//            // Register the SettingsButtonBehaviour's lifecycle methods
//            var behaviour = new BehaviourFCT(entity, Init, Update, End);
//            GameLogicSystem.Instance.RegisterLogicComponent(this);

//            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
//            SetOwner(entity);

//            Debug.LogInfo("[NewButton] SettingsButtonBehaviour registered with index {0}", GetBehaviourIndex());
//        }

//        /**
//         * @brief Factory method to create a SettingsButtonBehaviour.
//         *
//         * Attempts to create and register a new instance of SettingsButtonBehaviour for the given entity.
//         *
//         * @param entity The entity to attach the behaviour to.
//         * @return A new instance of SettingsButtonBehaviour as a BehaviourComponent, or null if an exception occurs.
//         */
//        public static BehaviourComponent CreateBehaviour(Entity entity)
//        {
//            try
//            {
//                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for SettingsButtonBehaviour on entity {0}", entity.ID);
//                return new SettingsButtonBehaviour(entity);
//            }
//            catch (Exception ex)
//            {
//                Debug.LogError("[NewButton] [CreateBehaviour] Exception occurred: {0}", ex.Message);
//                return null;
//            }
//        }

//        /**
//         * @brief Initializes the SettingsButtonBehaviour for the specified entity.
//         *
//         * This method is called once when the behaviour is first created, and is used to perform any necessary setup.
//         *
//         * @param entity The entity to initialize the behaviour for.
//         */
//        public static void Init(Entity entity)
//        {
//            Debug.LogInfo("[NewButton] Initializing SettingsButtonBehaviour for entity {0}", entity.ID);
//        }

//        public static void Update(Entity entity, float deltaTime)
//        {
//            if (InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer") ||
//                InternalCalls.LevelSwitch_GetLayerStatus("StoryLayer"))
//                return;

//            var instance = GameLogicSystem.Instance.GetBehaviour<SettingsButtonBehaviour>(entity);
//            if (instance == null || instance._localButtonComponent == null)
//            {
//                Debug.LogWarning("[NewButton] SettingsButtonBehaviour or its ButtonComponent is null for entity {0}.", entity.ID);
//                return;
//            }

//            // -------------------------------------------------------------------------
//            // ESC handling (modified)
//            // -------------------------------------------------------------------------
//            // Reset toggle guard at start of frame
//            _pauseMenuToggledThisFrame = false;

//            // Detect ESC key down (not hold)
//            bool escKeyDown = Input.IsKeyPressed(256); // ESC
//            bool leftMouseHeld = Input.IsMouseButtonDown(0); // Mouse held

//            if (escKeyDown && !instance._escPreviouslyHeld && !leftMouseHeld)
//            {
//                // ESC key toggles pause only if mouse is not held
//                CurrButtonBehaviour();
//                _pauseMenuToggledThisFrame = true;
//            }

//            instance._escPreviouslyHeld = escKeyDown;
//        }

//        /**
//         * @brief Cleanup function for the SettingsButtonBehaviour.
//         *
//         * Resets the button state and clears the reference to the ButtonComponent.
//         *
//         * @param entity The entity associated with the behaviour.
//         */
//        public static void End(Entity entity)
//        {
//            var instance = GameLogicSystem.Instance.GetBehaviour<SettingsButtonBehaviour>(entity);
//            if (instance == null)
//            {
//                return;
//            }

//            // Reset the local ButtonComponent state
//            if (instance._localButtonComponent != null)
//            {
//                instance._localButtonComponent = null;
//            }

//            Debug.LogInfo("[NewButton] SettingsButtonBehaviour cleanup complete for entity {0}.", entity.ID);
//        }

//        /**
//         * @brief Executes the custom action for the settings button.
//         *
//         * Toggles the settings (or pause) menu by calling the engine's PauseMenu_Toggle function.
//         */
//        private static void CurrButtonBehaviour()
//        {
//            // Place your custom action logic here.
//            Debug.LogInfo("[NewButton] Executing custom action for SettingsButtonBehaviour.");
//            InternalCalls.PauseMenu_Toggle();

//        }
//    }
//}