/******************************************************************************
/*!
\file  HoverInventoryBehaviour.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    Feb 03, 2024
\brief  This file implements the HoverInventoryBehaviour class, a button behaviour
        component that enables level switching when clicked.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @class HoverInventoryBehaviour
     * @brief Handles button interactions for switching game levels.
     *
     * This behaviour component listens for mouse interactions (hover and click)
     * and triggers a level switch when the button is clicked.
     */
    public class HoverInventoryBehaviour : BehaviourComponent
    {
        private ButtonComponent _localButtonComponent;
        //private bool _clickProcessed = false;
        private Vector2D _originalScale; // Field to store the original scale
        private const float HoverScaleMultiplier = 1.1f; // Scale up by 10%

        // Default constructor required by Mono
        public HoverInventoryBehaviour() { }

        // Constructor for creating MainMenuLevelSwitch with an entity
        /**
         * @brief Constructor for initializing the HoverInventoryBehaviour with an entity.
         *
         * @param entity The entity this behaviour is attached to.
         */
        public HoverInventoryBehaviour(Entity entity)
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
         * @brief Factory method to create an instance of HoverInventoryBehaviour.
         *
         * @param entity The entity to attach the behaviour to.
         * @return A new instance of HoverInventoryBehaviour or null if an error occurs.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[NewButton] [CreateBehaviour] Starting for MainMenuLevelSwitch on entity {0}", entity.ID);
                return new HoverInventoryBehaviour(entity);
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
            var instance = GameLogicSystem.Instance.GetBehaviour<HoverInventoryBehaviour>(entity);
            if (instance == null || instance._localButtonComponent == null)
            {
                Debug.LogWarning("[NewButton] MainMenuLevelSwitch or its ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                // Check if the button is hovered
                if (Input_IsMouseOverEntity(entity.ID))
                {
                    
                    instance._localButtonComponent.IsHovered = true;
                    // Scale up: multiply the original scale by the hover multiplier.
                    Vector2D hoveredScale = new Vector2D(
                        instance._originalScale.X * HoverScaleMultiplier,
                        instance._originalScale.Y * HoverScaleMultiplier
                    );
                    InternalCalls.Transform_SetScale(entity.ID, ref hoveredScale);
                    InternalCalls.Audio_PlaySound("SFX_UI_Hover 3", 0.3f);
                    HoverBehaviour();

                }
                else
                {
                    UnHoverBehaviour();
                    instance._localButtonComponent.IsHovered = false;
                    InternalCalls.Transform_SetScale(entity.ID, ref instance._originalScale);
                }

                //// Check if the button is clicked
                //if (Input.IsMouseButtonDown(0) && instance._localButtonComponent.IsHovered && !instance._clickProcessed)
                //{
                //    InternalCalls.Audio_PlaySound("SFX_ButtonClick");
                //    Debug.LogInfo("[NewButton] Button on entity {0} clicked!", entity.ID);
                //    instance._localButtonComponent.InvokeOnClick();

                //    // Call the new functionality for this button
                //    CurrButtonBehaviour();

                //    instance._clickProcessed = true; // Mark the click as processed
                //}

                //// Reset the clickProcessed flag when the mouse button is released
                //if (!Input.IsMouseButtonDown(0))
                //{
                //    instance._clickProcessed = false;
                //}

                // CHeck for layer status 
                if (!InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer"))
                {
                    Vector2D hoveredScale = new Vector2D(0.0f,0.0f);
                    InternalCalls.Transform_SetScale(entity.ID, ref hoveredScale);
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
            var instance = GameLogicSystem.Instance.GetBehaviour<HoverInventoryBehaviour>(entity);
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

            //instance._clickProcessed = false;

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

        /**W
         * @brief Executes the level switch when the button is clicked.
         */
        private static void HoverBehaviour()
        {

            // find entity of SkillsWIndow
            ulong skillsWindow = InternalCalls.Entity_GetEntityByName("SkillsWindow");
            // set scale of skills window
            Vector2D scale = new Vector2D(300.0f, 200.0f);
            InternalCalls.Transform_SetScale(skillsWindow, ref scale);
            // set text
            ulong txtBox = InternalCalls.Entity_GetEntityByName("HeadButt");
            
            InternalCalls.Font_SetFont(txtBox, "This is the most basic attack of the player,\n dealing damage to an adjacent enemy", 1.0f);
        }

        private static void UnHoverBehaviour()
        {
            // find entity of SkillsWIndow
            ulong skillsWindow = InternalCalls.Entity_GetEntityByName("SkillsWindow");
            // set scale of skills window
            Vector2D scale = new Vector2D(0.0f, 0.0f);
            InternalCalls.Transform_SetScale(skillsWindow, ref scale);
            // set text
            ulong txtBox = InternalCalls.Entity_GetEntityByName("HeadButt");
            InternalCalls.Font_SetFont(txtBox, "", 0.0f);
        }
    }
}