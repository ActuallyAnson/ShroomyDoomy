/******************************************************************************
/*!
\file  TutorialManager.cs
\Proj name  Shroomy Doomy
\author  Benjamin Lau Rey Jun
\date    April 06, 2025
\brief  This file implements the TutorialManager class, a behaviour component that 
        orchestrates the interactive tutorial flow. It manages tutorial steps, 
        handles visual arrow/text guidance, enforces player restrictions, and 
        processes interaction-based tutorial progression.

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
     * @class TutorialManager
     * @brief Behaviour component for managing the interactive tutorial system.
     *
     * Handles tutorial flow through scenes, controls UI elements like arrows and text,
     * restricts player movement when necessary, and listens for player actions
     * (e.g., moving, hovering, attacking) to determine tutorial progression.
     */
	public class TutorialManager : BehaviourComponent
    {
        private ButtonComponent _buttonComponent;
        private bool _clickProcessed = false;

        private int _currentStep = 0;
        private int _stepBeforeEnergyLoss = -1;
        private bool _skipClickThisFrame = false;

        private Entity _holeEntity;
        private Entity _endgoalTile;

        private readonly List<ulong> _tutorialTexts = new List<ulong>();
        private readonly List<int> _completedInteractiveScenes = new List<int>();
        private readonly Dictionary<int, List<ulong>> _arrowsToShow = new Dictionary<int, List<ulong>>();
        private readonly Dictionary<int, List<ulong>> _arrowsToHide = new Dictionary<int, List<ulong>>();

        public static Vector2D tbsize = new Vector2D(
            500.0f,
            350.0f
        );

        public static Vector2D arrowscale = new Vector2D(
            150.0f,
            100.0f
        );
        public TutorialManager() { }

		/**
		* @brief Constructor to attach the TutorialManager to a given entity.
		* @param entity The entity this behaviour is attached to.
		*/
		public TutorialManager(Entity entity)
        {
            Debug.LogInfo("[ArrowManipulator] Creating for entity {0}", entity.ID);

            if (!entity.HasLocalComponent<ButtonComponent>())
            {
                Debug.LogInfo("[ArrowManipulator] Adding ButtonComponent to entity {0}", entity.ID);
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
         * @brief Factory method to create a TutorialManager behaviour.
         * @param entity The entity this behaviour is attached to.
         * @return The newly created TutorialManager behaviour.
         */
		public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[ArrowManipulator] [CreateBehaviour] Starting for entity {0}", entity.ID);
                return new TutorialManager(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[ArrowManipulator] [CreateBehaviour] Exception: {0}", ex.Message);
                return null;
            }
        }

		/**
		* @brief Initializes the tutorial system for the entity.
		* Sets up tutorial texts, step detection, and visual arrow configurations.
		*/
		public static void Init(Entity entity)
        {
            Debug.LogInfo("[ArrowManipulator] Initializing for entity {0}", entity.ID);

            var button = GameLogicSystem.Instance.GetBehaviour<TutorialManager>(entity);
            if (button == null)
                return;

            // Give player a cheese at the start of the tutorial
            InventorySystem.Instance.AddItem("cheese");

            button._holeEntity = new Entity(InternalCalls.Entity_GetEntityByName("hole"));

            Vector2D holePos = new Vector2D();
            InternalCalls.Transform_GetTranslation(button._holeEntity.ID, out holePos);
            button._endgoalTile = TileManager.Instance.GetTileAtPosition(holePos);

            for (int i = 1; i <= 20; i++)
            {
                button._tutorialTexts.Add(InternalCalls.Entity_GetEntityByName($"TutorialText{i}"));
            }

            Vector2D zeroScale = Vector2D.Zero;

            // Detect the current visible text
            for (int i = 0; i < button._tutorialTexts.Count; i++)
            {
                Vector2D scale;
                InternalCalls.Transform_GetScale(button._tutorialTexts[i], out scale);
                if (scale.X >= 1)
                {
                    button._currentStep = i;
                }
            }

            // Apply visibility for tutorial texts
            for (int i = 0; i < button._tutorialTexts.Count; i++)
            {
                if (i == button._currentStep)
                    InternalCalls.Transform_SetScale(button._tutorialTexts[i], ref tbsize);
                else
                    InternalCalls.Transform_SetScale(button._tutorialTexts[i], ref zeroScale);
            }

            // -------------------
            //       SCENE 1: Welcome to Tutorial!
            // -------------------
            // Display the first text box, no arrows

            // I forgot a scene here

            // -------------------
            //       SCENE 2: Explanation of HP
            // -------------------
            button._arrowsToShow[2] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowHP")
            };
            button._arrowsToHide[2] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowHP")
            };

            // -------------------
            //       SCENE 3: Explanation oof Energy
            // -------------------
            button._arrowsToShow[3] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnergy")
            };
            button._arrowsToHide[3] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnergy")
            };

            // -------------------
            //       SCENE 4: Explanation of Power and Range
            // -------------------
            button._arrowsToShow[4] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowRange"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowPower")
            };
            button._arrowsToHide[4] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowRange"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowPower")
            };

            // -------------------
            //       SCENE 5: See these glowing tiles?
            // -------------------

            // -------------------
            //       SCENE 6: Move to this tile
            // -------------------
            button._arrowsToShow[6] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowTile")
            };
            button._arrowsToHide[6] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowTile")
            };

            // -------------------
            //       SCENE 7: Oh no no more energy!
            // -------------------
            button._arrowsToShow[7] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnergy")
            };
            button._arrowsToHide[7] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnergy")
            };

            // -------------------
            //       SCENE 8: Press End turn!
            // -------------------
            button._arrowsToShow[8] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEndTurn")
            };
            button._arrowsToHide[8] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEndTurn")
            };

            // -------------------
            //       SCENE 9: Energy back to full
            // -------------------
            button._arrowsToShow[9] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnergy")
            };
            button._arrowsToHide[9] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnergy")
            };

            // -------------------
            //       SCENE 10: Hover Enemy stats
            // -------------------
            button._arrowsToShow[10] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemy"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyHP"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyEnergy"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyRange"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyPower")
            };

            // -------------------
            //       SCENE 11: Hover Enemy stats (Press anywhere)
            // -------------------
            button._arrowsToHide[11] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemy"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyHP"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyEnergy"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyRange"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemyPower")
            };

            // -------------------
            //       SCENE 12: Open chest
            // -------------------
            button._arrowsToShow[12] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowChest")
            };
            button._arrowsToHide[12] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowChest")
            };

            // -------------------
            //       SCENE 13: Hover backpack
            // -------------------
            button._arrowsToShow[13] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowInventory")
            };

            // -------------------
            //       SCENE 14: Hover backpack (Press anywhere)
            // -------------------
            button._arrowsToHide[14] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowInventory")
            };

            // -------------------
            //       SCENE 15: Cook dish
            // -------------------
            button._arrowsToShow[15] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowSlot1"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowSlot2"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowWok")
            };
            button._arrowsToHide[15] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowSlot1"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowSlot2"),
                InternalCalls.Entity_GetEntityByName("TutorialArrowWok")
            };

            // -------------------
            //       SCENE 16: Show enemy who's boss! (Kill the enemy)
            // -------------------
            button._arrowsToShow[16] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemy")
            };
            button._arrowsToHide[16] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowEnemy")
            };

            // -------------------
            //       SCENE 17: Boom Thats How Its Done!
            // -------------------


            // -------------------
            //       SCENE 18: Step on ladder
            // -------------------
            button._arrowsToShow[18] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowExit")
            };
            button._arrowsToHide[18] = new List<ulong> {
                InternalCalls.Entity_GetEntityByName("TutorialArrowExit")
            };

            // -------------------
            //       SCENE 19: You did great! Display Tutorial Clear after click again
            // -------------------
        }

		/**
		* @brief Updates the tutorial logic each frame.
		* Checks for scene-based triggers, interaction conditions, movement restrictions, and scene advancement.
		*/
		public static void Update(Entity entity, float deltaTime)
        {
            // If the pause/main/story menu is currently active, do NOT process any of the player's logic
            if (InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer"))
                return;

            var button = GameLogicSystem.Instance.GetBehaviour<TutorialManager>(entity);
            if (button == null || button._buttonComponent == null)
                return;

            button._buttonComponent.IsHovered = Input.IsMouseOverEntity(entity.ID);

            // -----------------------------
            // Interactive Scene Check (Scene 6 - Move to tile)
            // -----------------------------
            if (button._currentStep == 6 && !button._completedInteractiveScenes.Contains(6))
            {
                Player.DisableMovementRestriction();

                // ✅ Restrict movement during this scene
                if (!Player.IsMovementRestrictedByTutorial)
                {
                    Player.EnableMovementRestriction(new Vector2D(-220.0f, 0.0f));
                }

                ulong playerEntity = InternalCalls.Entity_GetEntityByName("Player");
                Vector2D playerPosition = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(playerEntity, ref playerPosition);

                Entity playerTile = TileManager.Instance.GetTileAtPosition(playerPosition);
                Entity targetTile = TileManager.Instance.GetTileAtPosition(new Vector2D(-220.0f, 0.0f));

                if (playerTile != null && targetTile != null && playerTile.ID == targetTile.ID)
                {
                    button.OnPlayerMovedToTile();
                }
                else
                {
                    int playerEnergy = InternalCalls.UnitComponent_GetEnergy(playerEntity);
                    if (playerEnergy <= 0)
                    {
                        Debug.LogInfo("[TutorialManager] Player has no energy in Scene 6. Replaying Scene 7 and 8.");

                        Vector2D zero = Vector2D.Zero;

                        // Hide Scene 6 content
                        InternalCalls.Transform_SetScale(button._tutorialTexts[6], ref zero);
                        if (button._arrowsToHide.TryGetValue(6, out var arrowsToHide6))
                        {
                            foreach (var arrow in arrowsToHide6)
                                InternalCalls.Transform_SetScale(arrow, ref zero);
                        }

                        // Save step to return to after Scene 9
                        button._stepBeforeEnergyLoss = 6;

                        // Show Scene 7
                        button._currentStep = 7;
                        InternalCalls.Transform_SetScale(button._tutorialTexts[7], ref tbsize);
                        if (button._arrowsToShow.TryGetValue(7, out var arrowsToShow7))
                        {
                            foreach (var arrow in arrowsToShow7)
                                InternalCalls.Transform_SetScale(arrow, ref arrowscale);
                        }

                        return;
                    }
                }
            }

            // -----------------------------
            // Global restriction before Scene 6
            // -----------------------------
            if (button._currentStep < 6 && !Player.IsMovementRestrictedByTutorial)
            {
                Player.EnableMovementRestriction(Vector2D.Zero);
            }

            // -----------------------------
            // Interactive Scene Check (Scene 8 - Press end turn)
            // -----------------------------
            if (button._currentStep == 8 && !button._completedInteractiveScenes.Contains(8))
            {
                if (UnitManager.Instance.CurrentTurn == TurnType.EnemyTurn)
                {
                    button.OnPlayerPressedEndTurn();

                    Vector2D zero = Vector2D.Zero;

                    // Hide Scene 8 content
                    InternalCalls.Transform_SetScale(button._tutorialTexts[8], ref zero);
                    if (button._arrowsToHide.TryGetValue(8, out var arrowsToHide8))
                    {
                        foreach (var arrow in arrowsToHide8)
                            InternalCalls.Transform_SetScale(arrow, ref zero);
                    }

                    // Show Scene 9 (Energy back full)
                    button._currentStep = 9;
                    InternalCalls.Transform_SetScale(button._tutorialTexts[9], ref tbsize);
                    if (button._arrowsToShow.TryGetValue(9, out var arrowsToShow9))
                    {
                        foreach (var arrow in arrowsToShow9)
                            InternalCalls.Transform_SetScale(arrow, ref arrowscale);
                    }

                    button._skipClickThisFrame = true;
                    return;
                }
            }

            // -----------------------------
            // Interactive Scene Check (Scene 10 - hover over enemy)
            // -----------------------------
            if (button._currentStep == 10 && !button._completedInteractiveScenes.Contains(10))
            {
                ulong enemyEntity = InternalCalls.Entity_GetEntityByName("Enemy");
                Vector2D enemyPos = new Vector2D();
                Vector2D enemyScale = new Vector2D();

                InternalCalls.Transform_GetTranslation(enemyEntity, out enemyPos);
                InternalCalls.Transform_GetScale(enemyEntity, out enemyScale);

                Vector2D cursorPos = Input.GetCursorPosition();

                float left = enemyPos.X - (enemyScale.X / 2);
                float right = enemyPos.X + (enemyScale.X / 2);
                float bottom = enemyPos.Y - (enemyScale.Y / 2);
                float top = enemyPos.Y + (enemyScale.Y / 2);

                bool isHovered = cursorPos.X >= left && cursorPos.X <= right &&
                                 cursorPos.Y >= bottom && cursorPos.Y <= top;

                if (isHovered)
                {
                    button._completedInteractiveScenes.Add(10);
                    Debug.LogInfo("[TutorialManager] Scene 10 interaction complete — Enemy hovered.");
                    button.AdvanceTutorialStep(); // 🚀 Immediately move to Scene 11
                    return;
                }
            }

            // -----------------------------
            // Interactive Scene Check (Scene 12 - Open chest)
            // -----------------------------
            if (button._currentStep == 12 && !button._completedInteractiveScenes.Contains(12))
            {
                if (ChesterChicken.IsOpenChest)
                {
                    button._completedInteractiveScenes.Add(12);
                    Debug.LogInfo("[TutorialManager] Scene 11 interaction complete — Chest opened.");
                }
            }

            // -----------------------------
            // Interactive Scene Check (Scene 13 - Hover backpack)
            // -----------------------------
            if (button._currentStep == 13 && !button._completedInteractiveScenes.Contains(13))
            {
                ulong backpackEntity = InternalCalls.Entity_GetEntityByName("bagIcon"); // Adjust if named differently
                Vector2D backpackPos = new Vector2D();
                Vector2D backpackScale = new Vector2D();

                InternalCalls.Transform_GetTranslation(backpackEntity, out backpackPos);
                InternalCalls.Transform_GetScale(backpackEntity, out backpackScale);

                Vector2D cursorPos = Input.GetCursorPosition();

                float left = backpackPos.X - (backpackScale.X / 2);
                float right = backpackPos.X + (backpackScale.X / 2);
                float bottom = backpackPos.Y - (backpackScale.Y / 2);
                float top = backpackPos.Y + (backpackScale.Y / 2);

                bool isHovered = cursorPos.X >= left && cursorPos.X <= right &&
                                 cursorPos.Y >= bottom && cursorPos.Y <= top;

                if (isHovered)
                {
                    button._completedInteractiveScenes.Add(13);
                    Debug.LogInfo("[TutorialManager] Scene 13 interaction complete — Backpack hovered.");
                    button.AdvanceTutorialStep(); // 🚀 Immediately move to Scene 14
                    return;
                }
            }

            // -----------------------------
            // Interactive Scene Check (Scene 16 - Kill the enemy)
            // -----------------------------
            if (button._currentStep == 16 && !button._completedInteractiveScenes.Contains(16))
            {
                ulong enemyEntity = InternalCalls.Entity_GetEntityByName("Enemy");
                int enemyHealth = InternalCalls.UnitComponent_GetHealth(enemyEntity);

                if (enemyHealth <= 0)
                {
                    button._completedInteractiveScenes.Add(16);
                    Debug.LogInfo("[TutorialManager] Scene 16 interaction complete — Enemy defeated.");
                    //button.AdvanceTutorialStep(); // Move to Scene 17
                    return;
                }
            }

            // -----------------------------
            // Interactive Scene Check (Scene 18 - Move to ladder tile)
            // -----------------------------
            if (button._currentStep == 18 && !button._completedInteractiveScenes.Contains(18))
            {
                ulong playerEntity = InternalCalls.Entity_GetEntityByName("Player");
                Vector2D playerPosition = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(playerEntity, ref playerPosition);
                Entity playerTile = TileManager.Instance.GetTileAtPosition(playerPosition);

                if (playerTile != null && button._endgoalTile != null && playerTile.ID == button._endgoalTile.ID)
                {
                    button._completedInteractiveScenes.Add(18);
                    Debug.LogInfo("[TutorialManager] Scene 18 interaction complete — Player reached ladder tile.");
                    button.AdvanceTutorialStep(); // Progress to Scene 19
                    return;
                }
            }

            // -----------------------------
            // Mouse Click (advance if allowed)
            // -----------------------------
            if (Input.IsMouseButtonDown(0) && button._buttonComponent.IsHovered && !button._clickProcessed)
        {
            if (button._skipClickThisFrame)
            {
                Debug.LogInfo("[TutorialManager] Skipping click this frame to prevent double advance.");
                return;
            }

            if (button.RequiresInteraction(button._currentStep) &&
                !button._completedInteractiveScenes.Contains(button._currentStep))
            {
                Debug.LogInfo($"[TutorialManager] Scene {button._currentStep} requires interaction before progressing.");
                return;
            }

            // If we’re in Scene 9 after energy refill and need to resume
            if (button._currentStep == 9 && button._stepBeforeEnergyLoss != -1)
            {
                int returnStep = button._stepBeforeEnergyLoss;
                button._stepBeforeEnergyLoss = -1;
                button._currentStep = returnStep - 1; // will be incremented in AdvanceTutorialStep
            }

            button.AdvanceTutorialStep();
            button._clickProcessed = true;
            }

            if (!Input.IsMouseButtonDown(0))
            {
                button._clickProcessed = false;
                button._skipClickThisFrame = false;
            }
        }

		/**
         * @brief Cleans up the tutorial system and resets button state.
         */
		public static void End(Entity entity)
        {
            var button = GameLogicSystem.Instance.GetBehaviour<TutorialManager>(entity);
            if (button == null) return;

            if (button._buttonComponent != null)
            {
                button._buttonComponent.IsHovered = false;
                button._buttonComponent = null;
            }

            button._clickProcessed = false;

            Debug.LogInfo("[ArrowManipulator] Cleanup complete for entity {0}.", entity.ID);
        }

		/**
         * @brief Moves the tutorial to the next step and handles showing/hiding of visuals.
         */
		private void AdvanceTutorialStep()
        {
            if (_currentStep >= _tutorialTexts.Count)
                return;

            // Hide ALL tutorial texts to avoid overlap (guarantees the first one gets hidden too)
            Vector2D zero = Vector2D.Zero;
            foreach (var text in _tutorialTexts)
            {
                InternalCalls.Transform_SetScale(text, ref zero);
            }

            // Hide arrows tied to current step (Scene X)
            if (_arrowsToHide.TryGetValue(_currentStep, out var arrowsToHideThisStep))
            {
                foreach (var arrow in arrowsToHideThisStep)
                {
                    InternalCalls.Transform_SetScale(arrow, ref zero);
                }
            }

            // Move to the next step
            _currentStep++;
            if (_currentStep >= _tutorialTexts.Count)
            {
                Debug.LogInfo("[TutorialManager] Tutorial complete — no further steps.");
                return; // Don't advance past the last scene
            }

            // Show next tutorial text
            ulong nextText = _tutorialTexts[_currentStep];
            InternalCalls.Transform_SetScale(nextText, ref tbsize);

            // Show arrows tied to new step (Scene X+1)
            if (_arrowsToShow.TryGetValue(_currentStep, out var arrowsToShowNextStep))
            {
                foreach (var arrow in arrowsToShowNextStep)
                {
                    InternalCalls.Transform_SetScale(arrow, ref arrowscale);
                }
            }

            // NEW: Restrict movement only in Scene 6
            if (_currentStep > 6 && Player.IsMovementRestrictedByTutorial)
            {
                Player.DisableMovementRestriction();
            }
        }


		/**
         * @brief Called when the player successfully moves to the required tile in Scene 6.
         */
		public void OnPlayerMovedToTile()
        {
            if (_currentStep == 6 && !_completedInteractiveScenes.Contains(6))
            {
                _completedInteractiveScenes.Add(6);
                Player.DisableMovementRestriction(); // ✅ Allow free movement now

                Debug.LogInfo("[TutorialManager] Scene 6 interaction complete — tile reached.");
                AdvanceTutorialStep();
            }
        }

		/**
		* @brief Called when the player successfully presses "End Turn" in Scene 8.
		*/
		public void OnPlayerPressedEndTurn()
        {
            if (_currentStep == 8 && !_completedInteractiveScenes.Contains(8))
            {
                _completedInteractiveScenes.Add(8);
                Debug.LogInfo("[TutorialManager] Scene 8 interaction complete — End Turn pressed.");
                // Do NOT advance here. Let the click handler do it.
            }
        }

		/**
         * @brief Called globally when the player successfully cooks a dish in Scene 15.
         */
		public static void OnPlayerCookedDish()
        {
            foreach (var behaviour in GameLogicSystem.Instance.Behaviours)
            {
                Entity owner = behaviour.Owner;
                var tutorial = GameLogicSystem.Instance.GetBehaviour<TutorialManager>(owner);
                if (tutorial != null)
                {
                    if (tutorial._currentStep == 15 && !tutorial._completedInteractiveScenes.Contains(15))
                    {
                        tutorial._completedInteractiveScenes.Add(15);
                        Debug.LogInfo("[TutorialManager] Scene 16 interaction complete — Dish cooked.");
                        tutorial.AdvanceTutorialStep();
                    }
                    return;
                }
            }
        }

		/**
		* @brief Helper method to determine if the enemy can be defeated based on the tutorial step.
		* @return True if enemy can die, false otherwise.
		*/
		public static bool CanEnemyDie()
        {
            foreach (var behaviour in GameLogicSystem.Instance.Behaviours)
            {
                var tutorial = GameLogicSystem.Instance.GetBehaviour<TutorialManager>(behaviour.Owner);
                if (tutorial != null)
                {
                    return tutorial._currentStep == 16;
                }
            }
            return true; // fallback to allow death if TutorialManager is not found
        }

		/**
		* @brief Returns the current tutorial step the player is on.
		* @return The current tutorial step index.
		*/
		public int GetCurrentStep()
        {
            return _currentStep;
        }

		/**
         * @brief Hides all tutorial arrows across all steps.
         * Called when transitioning out of tutorial.
         */
		public static void ResetAllArrows()
		{
			TutorialManager tutorialManager = null;

			// Find the active TutorialManager instance
			foreach (var behaviour in GameLogicSystem.Instance.Behaviours)
			{
				tutorialManager = GameLogicSystem.Instance.GetBehaviour<TutorialManager>(behaviour.Owner);
				if (tutorialManager != null)
					break;
			}

			// If no tutorial manager found, exit
			if (tutorialManager == null)
				return;

			Vector2D zero = Vector2D.Zero;

			foreach (var arrowList in tutorialManager._arrowsToShow.Values)
			{
				foreach (var arrow in arrowList)
				{
					InternalCalls.Transform_SetScale(arrow, ref zero);
				}
			}

			foreach (var arrowList in tutorialManager._arrowsToHide.Values)
			{
				foreach (var arrow in arrowList)
				{
					InternalCalls.Transform_SetScale(arrow, ref zero);
				}
			}
		}

		/**
         * @brief Determines whether a specific tutorial step requires interaction before advancing.
         * @param step The step index to check.
         * @return True if interaction is required, false otherwise.
         */
		private bool RequiresInteraction(int step)
        {
            return step == 6 || step == 8 || step == 10 || step == 12 || step == 13 || step == 15 || step == 16 || step == 18;
        }
    }
}

