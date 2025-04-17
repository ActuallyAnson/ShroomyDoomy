/******************************************************************************
/*!
\file  ChesterCheese.cs
\Proj name  Shroomy Doomy
\author  
\date    March 14, 2025
\brief  Implements the ChesterCheese class, a behavior component for managing an interactive
        cheese treasure chest entity that adds "cheese" to the inventory when clicked.

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
     * @brief Manages a cheese treasure chest entity with button-like behavior.
     * 
     * @note Handles chest interaction, adding "cheese" to the inventory when clicked
     * by the player when adjacent, updating the chest's texture, and playing a sound.
     * Inherits from BehaviourComponent.
     */
    public class ChesterCheese : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _clickProcessed = false;

        private static bool isopenchest = false;

        /**
         * @brief Default constructor for ChesterCheese.
         */
        public ChesterCheese() { }

        /**
         * @brief Constructs a ChesterCheese instance for a specific entity.
         * 
         * @param entity The Entity to associate with this chest behavior.
         * @note Adds or retrieves a ButtonComponent, registers as a behavior with GameLogicSystem.
         */
        public ChesterCheese(Entity entity)
        {
            Debug.LogInfo("[ChesterCheese] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[ChesterCheese] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Creates a new ChesterCheese behavior instance for an entity.
         * 
         * @param entity The Entity to associate with the chest behavior.
         * @return A new ChesterCheese instance, or null if an exception occurs.
         * @note Wraps construction in a try-catch for error handling.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[ChesterCheese] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new ChesterCheese(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[ChesterCheese] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the ChesterCheese behavior for an entity.
         * 
         * @param entity The Entity associated with the chest behavior.
         * @note Logs initialization; no additional setup required.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[ChesterCheese] Initializing for entity {0}", entity.ID);
            isopenchest = false;
        }

        /**
         * @brief Updates the chest behavior state.
         * 
         * @param entity The Entity associated with the chest behavior.
         * @param deltaTime The time elapsed since the last update in seconds.
         * @note Handles hover detection, click processing to open the chest, adds "cheese" to
         * inventory, updates texture, and plays a sound when adjacent to the player.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<ChesterCheese>(entity);
            if (button == null || button._buttonComponent == null)
            {
                Debug.LogWarning("[ChesterCheese] ButtonComponent is null for entity {0}.", entity.ID);
                return;
            }

            try
            {
                if (Input_IsMouseOverEntity(entity.ID))
                {
                    button._buttonComponent.IsHovered = true;
                }
                else
                {
                    button._buttonComponent.IsHovered = false;
                }

                ulong playerID = InternalCalls.Entity_GetEntityByName("Player");
                ulong chestID = InternalCalls.Entity_GetEntityByName("ChesterCheese");

                float tileSize = 110.0f; // Your game's tile size

                if (Entity_IsAdjacent(playerID, chestID, tileSize))
                {
                    if (Input.IsMouseButtonDown(0) && button._buttonComponent.IsHovered && !button._clickProcessed)
                    {
                        if (isopenchest == false)
                        {
                            Debug.LogInfo("[ChesterCheese] Button clicked on entity {0}", entity.ID);
                            InventorySystem.Instance?.AddItem("cheese"); // Call the AddItem function

                            // Remove 1 Energy (commented out in original code)
                            Entity playerEntity = UnitManager.Instance.GetPlayerEntity();
                            //int currentEnergy = InternalCalls.UnitComponent_GetEnergy(playerEntity.ID);
                            //currentEnergy--;
                            //InternalCalls.UnitComponent_SetEnergy(playerEntity.ID, currentEnergy);

                            InternalCalls.SetEntityTextureByID(entity.ID, "cheese_treasure_chest");

                            // Play sound only when the chest is opened
                            InternalCalls.Audio_PlaySound("SFX_ChestOpen", 0.3f);

                            isopenchest = true;

                            InternalCalls.SetChestOverlayTrigger("ChesterCheeseTime");
                            button._clickProcessed = true;
                        }
                    }
                }

                if (!Input.IsMouseButtonDown(0))
                {
                    button._clickProcessed = false;
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[ChesterCheese] Error during update: {0}", ex.Message);
            }
        }

        /**
         * @brief Cleans up the ChesterCheese behavior.
         * 
         * @param entity The Entity associated with the chest behavior.
         * @note Resets hover state, clears button reference, and logs cleanup.
         */
        public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<ChesterCheese>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            button._clickProcessed = false;

            Debug.LogInfo("[ChesterCheese] Cleanup complete for entity {0}.", entity.ID);
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
                Debug.LogError("[ChesterCheese] Entity {0} missing Transform: {1}", entityID, ex.Message);
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
         * @brief Determines if two entities are adjacent based on tile size.
         * 
         * @param sourceEntityID The ID of the source entity (e.g., player).
         * @param targetEntityID The ID of the target entity (e.g., chest).
         * @param tileSize The size of a tile in the game world.
         * @return True if entities are exactly one tile apart horizontally or vertically, false otherwise.
         * @note Handles invalid IDs and transform retrieval errors.
         */
        private static bool Entity_IsAdjacent(ulong sourceEntityID, ulong targetEntityID, float tileSize)
        {
            // Ensure both entities are valid
            if (sourceEntityID == 0 || targetEntityID == 0)
            {
                Debug.LogWarning("[Entity_IsAdjacent] Invalid entity ID(s) provided.");
                return false;
            }

            Vector2D sourcePos = new Vector2D();
            Vector2D targetPos = new Vector2D();

            try
            {
                InternalCalls.Transform_GetTranslation(sourceEntityID, out sourcePos);
                InternalCalls.Transform_GetTranslation(targetEntityID, out targetPos);
            }
            catch (Exception ex)
            {
                Debug.LogError($"[Entity_IsAdjacent] Error fetching positions: {ex.Message}");
                return false;
            }

            float dx = Math.Abs(targetPos.X - sourcePos.X);
            float dy = Math.Abs(targetPos.Y - sourcePos.Y);

            // Check if the target is exactly one tile away (horizontal or vertical)
            bool isAdjacent = (dx == tileSize && dy == 0) || (dy == tileSize && dx == 0);

            return isAdjacent;
        }
    }
}