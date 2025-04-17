/******************************************************************************
/*!
\file  CookButton.cs
\Proj name  Shroomy Doomy
\author  Ming Sheng
\date    March 14, 2025
\brief  Implements the CookButton class, a behaviour component that handles the cooking action,
        increasing player attack power and unlocking ranged abilities when clicked.

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
    /**
     * @brief Manages the cook button for crafting food items and enhancing player abilities.
     * 
     * @note Handles button hover state with texture changes, processes clicks to increase player
     * attack power by 470, unlocks ranged abilities, plays sounds and animations, and consumes
     * crafted items. Inherits from BehaviourComponent.
     */
    public class CookButton : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _pressedWhileHovering = false;

        /**
         * @brief Default constructor for CookButton.
         */
        public CookButton() { }

        /**
         * @brief Constructs a CookButton instance for a specific entity.
         * 
         * @param entity The Entity to associate with this cook button behavior.
         * @note Adds or retrieves a ButtonComponent, registers as a behavior with GameLogicSystem.
         */
        public CookButton(Entity entity)
        {
            Debug.LogInfo("[CookButton] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[CookButton] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Creates a new CookButton behavior instance for an entity.
         * 
         * @param entity The Entity to associate with the cook button behavior.
         * @return A new CookButton instance, or null if an exception occurs.
         * @note Wraps construction in a try-catch for error handling.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[CookButton] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new CookButton(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[CookButton] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the CookButton behavior for an entity.
         * 
         * @param entity The Entity associated with the cook button behavior.
         * @note Logs initialization; no additional setup required.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[CookButton] Initializing for entity {0}", entity.ID);
            //CraftingSystem.Instance.RemoveCraftingIngredients();
        }

        /**
         * @brief Updates the cook button behavior state.
         * 
         * @param entity The Entity associated with the cook button behavior.
         * @param deltaTime The time elapsed since the last update in seconds.
         * @note Manages hover state with texture updates, processes clicks to enhance player attack
         * power, unlock ranged abilities, play sounds and animations, and consume crafted items.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            //if (InternalCalls.LevelSwitch_GetCurrLevel() == 2)

            //if (InternalCalls.BoolSwitch_ReloadCurrLevel() == true) {
            //    InventorySystem.Instance.ResetInventorySystem();
            //}

            //if (InternalCalls.BoolSwitch_Level1ToLevel2() == true)
            //{
            //    InventorySystem.Instance.UpdateItemTexture(1);
            //    InventorySystem.Instance.UpdateItemTexture(2);
            //}

            if (InternalCalls.BoolSwitch_MainMenuToLevel1() == true)
            {
                InventorySystem.Instance.ResetInventorySystem();
            }

            if (InternalCalls.BoolSwitch_TutorialToLevel1() == true)
            {
                InventorySystem.Instance.ResetInventorySystem();
            }

            var button = GameLogicSystem.Instance.GetBehaviour<CookButton>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[CookButton] ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                if (CraftingSystem.Instance.CanCraft())
                {
                    InventorySystem.Instance.UpdateItemTexture(1);
                    InventorySystem.Instance.UpdateItemTexture(2);
                    InventorySystem.Instance.UpdateCraftingOutput();

                    ulong outputid = InternalCalls.Entity_GetEntityByName("InventoryOutput");
                    string outputfood = InternalCalls.MaterialComponent_GetTextureType(outputid);

                    CraftingSystem.Instance.HideFoodAbilities();
                    CraftingSystem.Instance.UpdateFoodAbilities(outputfood);
                    CraftingSystem.Instance.UpdateCookButton();
                }
                else
                {
                    CraftingSystem.Instance.HideFoodAbilities();
                    CraftingSystem.Instance.HideCookButton();
                }

                bool isHovering = Input_IsMouseOverEntity(entity.ID) && CraftingSystem.Instance.CanCraft();

                // Hover logic + texture change
                if (isHovering)
                {
                    button._buttonComponent.IsHovered = true;
                    //InternalCalls.SetEntityTextureCook("cookPressed");
                }
                else
                {
                    button._buttonComponent.IsHovered = false;
                    //InternalCalls.SetEntityTextureCook("cookStill");
                }

                // Track if click started on this button
                if (Input.IsMouseButtonDown(0) && isHovering && !button._pressedWhileHovering)
                {
                    button._pressedWhileHovering = true;
                }

                // Trigger only if click was initiated on hover and mouse is released while still hovering
                if (button._pressedWhileHovering && !Input.IsMouseButtonDown(0))
                {
                    if (isHovering)
                    {
                        Entity playerEntity = UnitManager.Instance.GetPlayerEntity();
                        Debug.LogInfo("[CookButton] Button clicked on entity {0}", entity.ID);

                        InternalCalls.Audio_PlaySound("SFX_FoodReady", 0.4f);
                        InternalCalls.AnimationComponent_PlayAnimation(playerEntity.ID, "Player Power Up");

                        if (playerEntity != null)
                        {
                            //int currentAttack = InternalCalls.UnitComponent_GetAttackPower(playerEntity.ID);
                            //int newAttack = currentAttack + 470;
                            //InternalCalls.UnitComponent_SetAttackPower(playerEntity.ID, newAttack);

                            CraftingSystem.Instance.ConsumeCraftedItem(playerEntity);
                            //Player.rangedunlocked = true;
                        }
                        else
                        {
                            Debug.LogWarning("[CookButton] No valid player found to increase attack power.");
                        }
                    }

                    button._pressedWhileHovering = false;
                }

            }
            catch (Exception ex)
            {
                Debug.LogError("[CookButton] Error during update: {0}", ex.Message);
            }
        }


        /**
         * @brief Cleans up the CookButton behavior.
         * 
         * @param entity The Entity associated with the cook button behavior.
         * @note Resets hover state, clears button reference, resets click state, and logs cleanup.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<CookButton>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            Debug.LogInfo("[CookButton] Cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Checks if the mouse cursor is over the entity.
         * 
         * @param entityID The ID of the entity to check.
         * @return True if the mouse is over the entity, false otherwise.
         * @note Uses entity's position and scale to determine bounds; handles transform retrieval errors.
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
                Debug.LogError("[CookButton] Entity {0} missing Transform: {1}", entityID, ex.Message);
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