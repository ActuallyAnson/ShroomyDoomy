/******************************************************************************
/*!
\file  UnitManager.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief This file contains the implementation of the UnitManager class. 
       This class manages player and enemy units, their turns, and game state. 
        

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

using Fruitloops;
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;


public enum TurnType
{
    PlayerTurn,
    EnemyTurn
}


namespace FruitLoops
{
    public class UnitManager : BehaviourComponent, IGameObserver
    {
        private int playerTurnCount = 0;
		private int _framesSinceStart = 0;
		private bool _winTriggered = false;

		// --- CHEAT CODES ---
		private static bool invincibleModeEnabled = false;
		private static int originalAttackPower = -1;
		private static int originalHealth = -1;
        private static int originalEnergy = -1;
		private readonly Dictionary<int, bool> _cheatKeyHeld = new Dictionary<int, bool>();

		private readonly List<Entity> _players = new List<Entity>();
        private readonly List<Entity> _enemies = new List<Entity>();
        private readonly List<Entity> _rocks = new List<Entity>();

        private TurnType _currentTurn = TurnType.PlayerTurn;

        public static UnitManager Instance { get; private set; }

        private Entity _headbuttOverlayEntity;
        private Entity _specialAttackOverlayEntity;
        private Entity _stageClearOverlayEntity;
        private Entity _stageClearOverlayButtonEntity1;
        private Entity _stageClearOverlayButtonEntity2;
		private Entity _stage2ClearToBeContinuedEntity;
		private Entity _turnIndicatorEntity;

		public Entity _gameOverOverlayEntity;
		public Entity _gameOverYesButtonEntity;
		public Entity _gameOverNoButtonEntity;
		private bool _gameOverHandled = false;

		private bool _headbuttOverlayShown = false;
        private bool _specialOverlayShown = false;
        private Entity _lastOverlaidEnemy = null;

        //instance of enemyturnmanager
        public EnemyTurnManager _enemyTurnManager = new EnemyTurnManager();


        public TurnType CurrentTurn
        {
            get => _currentTurn;
            set => _currentTurn = value;
        }

        public UnitManager() { }

        private UnitManager(Entity entity)
        {
            if (Instance != null)
                throw new Exception("[UnitManager] Only one UnitManager instance is allowed.");

            Instance = this;

            Debug.LogInfo("[UnitManager] Creating UnitManager for entity {0}", entity.ID);

            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            // Register with messaging system
            RegisterForMessages();

            Debug.LogInfo("[UnitManager] UnitManager registered with index {0}", GetBehaviourIndex());
        }

        // -----------------------[ OBSERVER PATTERN METHODS ]----------------------

        /**
         * @brief Registers this manager with the message system.
         * 
         * Subscribes to specific game message types that affect game state.
         */
        private void RegisterForMessages()
        {
            // Register for relevant message types
            GameMessageHub.Instance.RegisterObserver(GameMessageType.PlayerMoved, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.PlayerAttacked, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EnemyCleanUp, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EnemyDied, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.TileStateChanged, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EntityStateChanged, this);


            Debug.LogInfo("[UnitManager] Registered with message hub");
        }

        /**
         * @brief Unregisters from the message system.
         * 
         * Should be called during cleanup to prevent memory leaks.
         */
        private void UnregisterFromMessages()
        {
            GameMessageHub.Instance.RemoveObserver(this);
            Debug.LogInfo("[UnitManager] Unregistered from message hub");
        }

        /**
         * @brief Implementation of IGameObserver.OnNotify to handle incoming messages.
         * 
         * @param message The message object containing event details.
         */
        public void OnNotify(GameMessage message)
        {
            if (message == null)
                return;

            switch (message.MessageType)
            {
                case GameMessageType.PlayerMoved:
                    HandlePlayerMoved(message);
                    break;

                case GameMessageType.PlayerAttacked:
                    HandlePlayerAttack(message);
                    break;

                case GameMessageType.PlayerDied:
                    HandlePlayerDeath(message);
                    break;

                case GameMessageType.EnemyCleanUp:
                    HandleEnemyDeath(message);
                    break;

                case GameMessageType.TileStateChanged:
                    HandleTileStateChanged(message);
                    break;

                case GameMessageType.EntityStateChanged:
                    HandleEntityStateChanged(message);
                    break;
            }
        }

        /**
  * @brief Handles player movement notifications and checks for win conditions.
  * 
  * @param message The GameMessage containing movement information.
  */
        private void HandlePlayerMoved(GameMessage message)
        {
            Debug.LogInfo("[UnitManager] Received player movement notification");
            // Check if player moved to end goal or other special tiles
            CheckWinCondition();
        }

        /**
         * @brief Processes player attack events and logs the attack details.
         * 
         * @param message The GameMessage containing attack information.
         * @note Logs the source ID, target ID, and damage value of the attack.
         * Uses null-conditional operators to handle cases where Source or Target might be null.
         */
        private void HandlePlayerAttack(GameMessage message)
        {
            Debug.LogInfo("[UnitManager] Player {0} attacked {1} for {2} damage",
                message.Source?.ID ?? 0,
                message.Target?.ID ?? 0,
                message.Value);
        }

        /**
         * @brief Handles player death notifications and logs the event.
         * 
         * @param message The GameMessage containing death information.
         * @note Could potentially trigger game over logic in the future.
         * Uses null-conditional operator for safe Source ID access.
         */
        private void HandlePlayerDeath(GameMessage message)
        {
            Debug.LogInfo("[UnitManager] Player {0} has died", message.Source?.ID ?? 0);
            // Game over logic could be triggered here
        }

        /**
         * @brief Processes entity state changes, specifically handling enemy death animation cleanup.
         * 
         * @param message The GameMessage containing state change information.
         * @note Specifically handles "EnemyDeathProcessingCompleted" messages by:
         * 1. Removing death animations
         * 2. Sending a follow-up message for final cleanup
         * Requires both Source and Target to be non-null for processing.
         */
        private void HandleEntityStateChanged(GameMessage message)
        {
            // Handle enemy death processing completion message
            if (message.Data != null && message.Data.ToString() == "EnemyDeathProcessingCompleted" &&
                message.Source != null && message.Target != null)
            {
                Debug.LogInfo("[UnitManager] Received death processing completion from Enemy {0}", message.Source.ID);

                // Do final animation cleanup - remove death animations
                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(message.Source.ID, "EnemyDie");
                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(message.Source.ID, "Enemy Death (Flip)");

                Debug.LogInfo("[UnitManager] Final animation cleanup for Enemy {0}", message.Source.ID);

                // Create a new message that will be processed by HandleEnemyDeath
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyCleanUp,      // This will be routed to HandleEnemyDeath
                    GetOwner(),                     // UnitManager as source
                    message.Source,                 // The enemy entity as target
                    0,                              // No specific value
                    null,                           // No position data needed
                    "AnimationCleanupCompleted"     // Special data to indicate this is a follow-up
                );
            }
        }

        /**
         * @brief Manages enemy death events including cleanup and win condition checking.
         * 
         * @param message The GameMessage containing enemy death information.
         * @note Handles two cases:
         * 1. Initial death notification: Removes enemy from managed list
         * 2. Animation cleanup completion: Performs final cleanup
         * Also checks if all enemies are defeated and triggers appropriate events.
         */
        private void HandleEnemyDeath(GameMessage message)
        {
            Entity enemyEntity = message.Target;
            if (enemyEntity != null)
            {
                // Check if this is a follow-up from animation cleanup
                bool isAnimationCleanupComplete =
                    message.Data != null && message.Data.ToString() == "AnimationCleanupCompleted";

                if (!isAnimationCleanupComplete)
                {
                    Debug.LogInfo("[UnitManager] Enemy {0} died, removing from managed list", enemyEntity.ID);

                    // Pass true to indicate we're already handling a death notification
                    RemoveEnemy(enemyEntity, true);
                }
                else
                {
                    Debug.LogInfo("[UnitManager] Final cleanup after animation removal for Enemy {0}", enemyEntity.ID);
                    // Any additional cleanup needed after all animations have been removed
                }

                // Check if all enemies are defeated - do this regardless of message type
                if (AreAllEnemiesDefeated())
                {
                    // Broadcast message that all enemies are defeated
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.AllEnemiesDefeated,
                        GetOwner(),
                        null,
                        _enemies.Count,
                        null,
                        null
                    );

                    // Check win condition
                    CheckWinCondition();
                }
            }
        }

        /**
         * @brief Handles notifications about tile state changes in the game.
         * 
         * @param message The GameMessage containing tile state change information.
         * @note Currently logs the change but could be expanded to handle game state updates
         * based on tile changes.
         */
        private void HandleTileStateChanged(GameMessage message)
        {
            Debug.LogInfo("[UnitManager] Tile state changed: {0}", message.Data);
            // Handle potential changes to game state based on tile changes
        }

        /**
         * @brief Checks if the win condition is met.
         * 
         * Win condition: Player on end goal tile and all enemies defeated.
         */
        private void CheckWinCondition()
        {
            Entity playerEntity = GetPlayerEntity();
            if (playerEntity == null) return;

            Vector2D playerPos = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPos);

            Entity playerTile = TileManager.Instance.GetTileAtPosition(playerPos);
            Entity endGoalTile = TileManager.Instance.GetEndGoalTile();

            if (playerTile != null && endGoalTile != null &&
                playerTile.ID == endGoalTile.ID &&
                AreAllEnemiesDefeated()
                )
            {
                if (!_winTriggered)
                {
                    InternalCalls.Audio_PlaySound("SFX_WinGame", 0.3f);
                    // Send level complete message
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.LevelComplete,
                        playerEntity,
                        endGoalTile,
                        playerTurnCount, // Number of turns it took to win
                        playerPos,       // Position where win occurred
                        "ReachedGoal"    // Win condition type
                    );
                    _winTriggered = true;
                }
                StageClearOverlay();
            }
        }

        /**
         * @brief Creates a UnitManager behavior component for the specified entity.
         * 
         * @param entity The Entity to associate with the UnitManager behavior.
         * @return A BehaviourComponent instance (UnitManager) or null if creation fails.
         * @note Attempts to create a UnitManager instance, sends an initialization message,
         * and handles exceptions by logging errors.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[UnitManager] [CreateBehaviour] Starting for behaviour UnitManager on entity {0}", entity.ID);
                var unitManager = new UnitManager(entity);
                Debug.LogInfo("[UnitManager] [CreateBehaviour] Successfully created behaviour UnitManager for entity {0}", entity.ID);

                // Send initialization message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameManagerInitialized,
                    entity,
                    null,
                    0,
                    null,
                    "UnitManager"
                );

                return unitManager;
            }
            catch (Exception ex)
            {
                Debug.LogError("[UnitManager] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the UnitManager for the given entity, setting up players, enemies, and rocks.
         * 
         * @param entity The Entity representing the UnitManager owner.
         * @note Initializes the turn indicator, scans for units, sets up entities based on type,
         * and sends initialization messages with unit counts.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[UnitManager] Initializing UnitManager for entity {0}", entity.ID);

			Instance._winTriggered = false;
			Instance._gameOverHandled = false;
			Instance._framesSinceStart = 0;

			Instance._turnIndicatorEntity = new Entity(InternalCalls.Entity_GetEntityByName("TurnIndicatorIcon"));

            // Send initialization started message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnvironmentScanStarted,
                entity,
                null,
                0,
                null,
                "UnitManagerInit"
            );

            // Find all entities with UnitComponent
            var allUnits = Entity.GetEntitiesWithECSComponentName("struct FruitLoops::UnitComponent");

            int playersFound = 0;
            int enemiesFound = 0;
            int rocksFound = 0;

            foreach (var unitEntity in allUnits)
            {
                string unitType = InternalCalls.UnitComponent_GetUnitType(unitEntity.ID);

                Vector2D position = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(unitEntity.ID, ref position);

                if (unitType == "Player")
                {
                    Instance.SetupPlayer(unitEntity, position);
                    playersFound++;
                }
                else if (unitType == "Enemy")
                {
                    Instance.SetupEnemy(unitEntity, position);
                    enemiesFound++;
                }
                else if (unitType == "Rock")
                {
                    Instance.SetupRock(unitEntity, position);
                    rocksFound++;
                }
            }

            // Send initialization completed message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnvironmentScanCompleted,
                entity,
                null,
                allUnits.Length, // Total units found
                null,
                new Dictionary<string, int> {
            { "Players", playersFound },
            { "Enemies", enemiesFound },
            { "Rocks", rocksFound }
                }
            );
        }

        /**
         * @brief Updates the UnitManager state based on the current turn and game conditions.
         * 
         * @param entity The Entity representing the UnitManager owner.
         * @param deltaTime The time elapsed since the last update in seconds.
         * @note Handles turn switching if all enemies are defeated during enemy turn,
         * updates enemy attack overlays during player turn, and checks win conditions.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            // Ensure the UnitManager instance is valid
            if (Instance == null)
                return;

			// If the pause/main/story menu is currently active, do NOT process any of the player's logic
			if (InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer") ||
				InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer") ||
				InternalCalls.LevelSwitch_GetLayerStatus("StoryLayer"))
				return;

			if (Instance._framesSinceStart < 5) // Delay for 5 frames
            {
                Instance._framesSinceStart++;
                return;
            }

            // If it's enemy turn but all enemies are defeated, switch to player turn
            if (Instance._currentTurn == TurnType.EnemyTurn && Instance.AreAllEnemiesDefeated())
            {
                Debug.LogInfo("[UnitManager] All enemies defeated during enemy turn, switching to Player Turn.");

                // Broadcast message before switching turn
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.AllEnemiesDefeated,
                    entity,
                    null,
                    Instance._enemies.Count,
                    null,
                    "DuringEnemyTurn"
                );

                Instance.SwitchTurn();
                return;
            }

            // Only update overlays if it's the player's turn
            if (Instance._currentTurn != TurnType.PlayerTurn)
                return;

            // Call the method that checks for hovered enemies and shows/hides the overlay
            Instance.UpdateEnemyAttackOverlays();

            // Check win condition
            Instance.CheckWinCondition();

			// --- CHEAT CODES START HERE ---
			if (Instance.IsNewKeyPress(81)) // Q key = Toggle Invincible Mode
			{
				invincibleModeEnabled = !invincibleModeEnabled;
				Debug.LogWarning($"[Cheat] Invincible Mode {(invincibleModeEnabled ? "ENABLED" : "DISABLED")}!");

				Entity player = Instance.GetPlayerEntity();
				if (player != null)
				{
					if (invincibleModeEnabled)
					{
						// Store original values only the first time
						if (originalAttackPower == -1)
							originalAttackPower = InternalCalls.UnitComponent_GetAttackPower(player.ID);
						if (originalHealth == -1)
							originalHealth = InternalCalls.UnitComponent_GetMaxHealth(player.ID);
						if (originalEnergy == -1)
							originalEnergy = InternalCalls.UnitComponent_GetMaxEnergy(player.ID);

						// Set super stats
						InternalCalls.UnitComponent_SetHealth(player.ID, 99999);
						InternalCalls.UnitComponent_SetMaxHealth(player.ID, 99999);
						InternalCalls.UnitComponent_SetAttackPower(player.ID, 9999);
						InternalCalls.UnitComponent_SetMaxEnergy(player.ID, 15);
						InternalCalls.UnitComponent_SetEnergy(player.ID, 15);
					}
					else
					{
						// Restore original values
						InternalCalls.UnitComponent_SetMaxHealth(player.ID, originalHealth);
						InternalCalls.UnitComponent_SetHealth(player.ID, originalHealth);
						InternalCalls.UnitComponent_SetAttackPower(player.ID, originalAttackPower);
						InternalCalls.UnitComponent_SetMaxEnergy(player.ID, originalEnergy);
						InternalCalls.UnitComponent_SetEnergy(player.ID, originalEnergy);

						// Reset cached values (optional)
						originalAttackPower = -1;
						originalHealth = -1;
						originalEnergy = -1;
					}
				}
			}

			if (Instance.IsNewKeyPress(87)) // W key = Next level
			{
				Debug.LogWarning("[Cheat] Skipping to next level!");
				int currentLevel = InternalCalls.LevelSwitch_GetCurrLevel();
				if (currentLevel == 0)
				{
					InternalCalls.LevelSwitch_TutorialToLevel1();
					InternalCalls.LevelSwitch_ReloadCurrLevel();
				}
				else if (currentLevel == 1)
				{
					InternalCalls.LevelSwitch_Level1ToLevel2();
					InternalCalls.LevelSwitch_ReloadCurrLevel();
				}
                else if (currentLevel == 2)
                {
					InternalCalls.LevelSwitch_ReturnMainMenu();
				}
			}

			if (Instance.IsNewKeyPress(69) && !Instance._winTriggered && !Instance._gameOverHandled) // E key = Win immediately
			{
				Debug.LogWarning("[Cheat] Triggering win condition: killing all enemies and teleporting to ladder!");

				// Kill all enemies
				foreach (var enemy in Instance.GetEnemies())
				{
					if (enemy != null && enemy.ID != 0 && InternalCalls.UnitComponent_GetIsAlive(enemy.ID))
					{
						InternalCalls.UnitComponent_SetHealth(enemy.ID, 0);
					}
				}

				// Teleport player to the ladder
				Entity player = Instance.GetPlayerEntity();
				Entity ladder = TileManager.Instance.GetEndGoalTile();
				if (player != null && ladder != null)
				{
					Vector2D ladderPos = TileManager.Instance.GetTileCenter(ladder);
					InternalCalls.UnitComponent_SetPosition(player.ID, ref ladderPos);
					InternalCalls.Transform_SetTranslation(player.ID, ref ladderPos);

					// Ensure the ladder tile becomes non-walkable
					InternalCalls.TileComponent_SetOccupant(ladder.ID, player.ID);
					InternalCalls.TileComponent_SetIsWalkable(ladder.ID, false);

					Debug.LogInfo("[Cheat] Player teleported to ladder at position ({0}, {1})", ladderPos.X, ladderPos.Y);
				}

				// Trigger win logic
				Instance.CheckWinCondition();
			}

			if (Instance.IsNewKeyPress(82) && !Instance._gameOverHandled && !Instance._winTriggered) // R key = Lose immediately
			{
				Debug.LogWarning("[Cheat] Triggering player death!");
				Entity player = Instance.GetPlayerEntity();
				if (player != null)
				{
					InternalCalls.UnitComponent_SetHealth(player.ID, 0);
				}
			}
			// --- CHEAT CODES END HERE ---
		}

		/**
         * @brief Cleans up the UnitManager instance and its managed entities.
         * 
         * @param entity The Entity representing the UnitManager owner.
         * @note Sends a cleanup message, unregisters from the messaging system,
         * clears all entity lists, and nullifies the instance.
         */
		public static void End(Entity entity)
        {
            if (Instance != null)
            {
                // Send cleanup message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameManagerCleanup,
                    entity,
                    null,
                    Instance._players.Count + Instance._enemies.Count + Instance._rocks.Count,
                    null,
                    "UnitManager"
                );

                // Unregister from messaging system
                Instance.UnregisterFromMessages();

                Instance._players.Clear();
                Instance._enemies.Clear();
                Instance._rocks.Clear();
                Instance._currentTurn = TurnType.PlayerTurn;

                Instance = null;
            }

            Debug.LogInfo("[UnitManager] All players and enemies instances cleared.");
            Debug.LogInfo("[UnitManager] Instance set to null.");
        }

        /**
         * @brief Sets up a player entity with its initial position.
         * 
         * @param entity The Entity representing the player.
         * @param position The initial Vector2D position of the player.
         * @note Snaps the player to a tile, adds it to the players list,
         * and sends a player initialized message.
         */
        private void SetupPlayer(Entity entity, Vector2D position)
        {
            Debug.LogInfo("[UnitManager] Setting up Player {0} at initial position {1}", entity.ID, position);
            SnapEntityToTile(entity);
            _players.Add(entity);

            // Send player initialized message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.PlayerInitialized,
                entity,
                null,
                0,
                position,
                null
            );
        }

        /**
         * @brief Sets up an enemy entity with its initial position.
         * 
         * @param entity The Entity representing the enemy.
         * @param position The initial Vector2D position of the enemy.
         * @note Snaps the enemy to a tile, adds it to the enemies list,
         * and sends an enemy initialized message.
         */
        private void SetupEnemy(Entity entity, Vector2D position)
        {
            Debug.LogInfo("[UnitManager] Setting up Enemy {0} at initial position {1}", entity.ID, position);
            SnapEntityToTile(entity);
            _enemies.Add(entity);

            // Send enemy initialized message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyInitialized,
                entity,
                null,
                0,
                position,
                null
            );

            Debug.LogInfo("[UnitManager] Added Enemy {0} to the enemies list.", entity.ID);
        }

        /**
         * @brief Sets up a rock entity with its initial position.
         * 
         * @param entity The Entity representing the rock.
         * @param position The initial Vector2D position of the rock.
         * @note Snaps the rock to a tile, adds it to the rocks list,
         * and sends an obstacle initialized message.
         */
        private void SetupRock(Entity entity, Vector2D position)
        {
            Debug.LogInfo("[UnitManager] Setting up Rock {0} at initial position {1}", entity.ID, position);
            SnapEntityToTile(entity);
            _rocks.Add(entity);

            // Send obstacle initialized message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.ObstacleInitialized,
                entity,
                null,
                0,
                position,
                "Rock"
            );
        }

        /**
         * @brief Snaps an entity to the nearest tile based on its position.
         * 
         * @param entity The Entity to snap to a tile.
         * @note Updates the entity's position to the tile center, sets the tile's occupant,
         * and sends appropriate messages for success or failure.
         */
        private void SnapEntityToTile(Entity entity)
        {
            if (!InternalCalls.Entity_HasECSComponentByName(entity.ID, "struct FruitLoops::UnitComponent"))
            {
                Debug.LogWarning("[UnitManager] Entity {0} does not have a UnitComponent.", entity.ID);
                return;
            }

            // Create a Vector2D instance for the position
            Vector2D entityPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(entity.ID, ref entityPosition);

            Entity tile = GetTileUnderPosition(entityPosition);

            if (tile == null)
            {
                Debug.LogWarning("[UnitManager] No tile found under position ({0}, {1}) for entity {2}",
                    entityPosition.X, entityPosition.Y, entity.ID);

                // Send message about failed tile snapping
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.TileInteraction,
                    entity,
                    null,
                    0,
                    entityPosition,
                    "SnapFailed"
                );

                return;
            }

            // Create a Vector2D instance for the tile center
            Vector2D tileCenter = new Vector2D();
            InternalCalls.TileComponent_GetPosition(tile.ID, out tileCenter);

            // Cache previous position for messaging
            Vector2D previousPosition = entityPosition;

            // Snap UnitComponent and Transform positions to the tile
            InternalCalls.UnitComponent_SetPosition(entity.ID, ref tileCenter);
            InternalCalls.Transform_SetTranslation(entity.ID, ref tileCenter);

            // Set tile's occupant
            InternalCalls.TileComponent_SetOccupant(tile.ID, entity.ID);

            Debug.LogInfo("[UnitManager] Snapped entity {0} to tile {1} at position ({2}, {3})",
                entity.ID, tile.ID, tileCenter.X, tileCenter.Y);

            // Send tile occupied message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.TileOccupied,
                entity,
                tile,
                0,
                tileCenter,
                previousPosition // Original position before snapping
            );
        }

        /**
         * @brief Retrieves the currently active enemy entity.
         * 
         * @return The Entity representing the current active enemy.
         * @note Delegates to the enemy turn manager to get the current enemy.
         */
        public Entity GetCurrentActiveEnemy()
        {
            return _enemyTurnManager.GetCurrentEnemy();
        }

        /**
         * @brief Finds the tile under a given position.
         * 
         * @param position The Vector2D position to check for a tile.
         * @return The Entity representing the tile under the position, or null if none found.
         * @note Iterates through walkable tiles and sends debug messages in DEBUG mode.
         */
        private Entity GetTileUnderPosition(Vector2D position)
        {
            foreach (var tileEntity in TileManager.Instance.GetWalkableTiles())
            {
                InternalCalls.TileComponent_GetPosition(tileEntity.ID, out Vector2D tilePosition);

                if (IsPositionWithinTile(position, tilePosition))
                {
#if DEBUG
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.TileInteraction,
                        null,  // No source entity
                        tileEntity,
                        0,
                        position,
                        "TileFound"
                    );
#endif
                    return tileEntity;
                }
            }

#if DEBUG
            GameMessageHub.Instance.SendMessage(
                GameMessageType.TileInteraction,
                null,
                null,
                0,
                position,
                "NoTileFound"
            );
#endif

            return null;
        }

        /**
 * @brief Checks if a position is within the bounds of a tile.
 * 
 * @param position The Vector2D position to check.
 * @param tilePosition The Vector2D center position of the tile.
 * @return True if the position is within the tile bounds, false otherwise.
 * @note Uses a fixed tile size of 110.0f to determine bounds.
 */
        private bool IsPositionWithinTile(Vector2D position, Vector2D tilePosition)
        {
            float tileSize = 110.0f;
            return position.X >= tilePosition.X - tileSize / 2 &&
                   position.X <= tilePosition.X + tileSize / 2 &&
                   position.Y >= tilePosition.Y - tileSize / 2 &&
                   position.Y <= tilePosition.Y + tileSize / 2;
        }

		/**
         * @brief Switches the current turn between player and enemy turns.
         * 
         * @note Handles turn transitions by ending the current turn, updating the turn state,
         * broadcasting turn change messages, and updating the turn indicator texture.
         */
		public void SwitchTurn()
		{
			// If all enemies are defeated, force it to be the player's turn
			if (AreAllEnemiesDefeated())
			{
				_currentTurn = TurnType.PlayerTurn;

				// Broadcast turn change message
				GameMessageHub.Instance.SendMessage(
					GameMessageType.TurnChanged,
					GetOwner(),
					null,
					playerTurnCount,
					null,
					TurnType.PlayerTurn
				);

				// Change icon to "PlayerTurn" texture
				InternalCalls.SetEntityTextureEnemy(_turnIndicatorEntity.ID, "PlayerTurn");

				Debug.LogInfo("[UnitManager] All enemies defeated — forcing Player Turn.");
				StartPlayerTurn();
				return;
			}

			if (_currentTurn == TurnType.PlayerTurn)
			{
				EndPlayerTurn();
				_currentTurn = TurnType.EnemyTurn;

				GameMessageHub.Instance.SendMessage(
					GameMessageType.TurnChanged,
					GetOwner(),
					null,
					playerTurnCount,
					null,
					TurnType.EnemyTurn
				);

				InternalCalls.SetEntityTextureEnemy(_turnIndicatorEntity.ID, "EnemyTurn");

				Debug.LogInfo("[UnitManager] Switched to Enemy Turn.");
				StartEnemyTurn();
			}
			else if (_currentTurn == TurnType.EnemyTurn)
			{
				EndEnemyTurn();
				_currentTurn = TurnType.PlayerTurn;

				GameMessageHub.Instance.SendMessage(
					GameMessageType.TurnChanged,
					GetOwner(),
					null,
					playerTurnCount,
					null,
					TurnType.PlayerTurn
				);

				InternalCalls.SetEntityTextureEnemy(_turnIndicatorEntity.ID, "PlayerTurn");

				Debug.LogInfo("[UnitManager] Switched to Player Turn.");
				StartPlayerTurn();
			}
		}


		/**
         * @brief Initiates the start of a player turn.
         * 
         * @note Increments the turn count, resets player energy, and broadcasts a player turn start message.
         */
		private void StartPlayerTurn()
        {
            playerTurnCount++;

            foreach (var player in _players)
                ResetEnergy(player);

            Debug.LogInfo("[UnitManager] Player turn started. Turn count: " + playerTurnCount);

            // Broadcast player turn start message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.PlayerTurnStarted,
                GetOwner(),
                null,
                playerTurnCount,
                null,
                null
            );
        }

        /**
         * @brief Retrieves the current player turn count.
         * 
         * @return The number of player turns that have occurred.
         */
        public int GetPlayerTurnCount()
        {
            return playerTurnCount;
        }

        /**
         * @brief Initiates the start of an enemy turn.
         * 
         * @note Checks if all enemies are defeated before proceeding; if so, switches back to player turn.
         * Otherwise, starts the enemy turn via the enemy turn manager and broadcasts a start message.
         */
        private void StartEnemyTurn()
        {
            // Check if all enemies are defeated before processing enemy turn
            if (AreAllEnemiesDefeated())
            {
                Debug.LogInfo("[UnitManager] All enemies defeated, switching back to Player Turn.");

                // Broadcast all enemies defeated message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.AllEnemiesDefeated,
                    GetOwner(),
                    null,
                    0,
                    null,
                    "BeforeEnemyTurn"
                );

                // Skip enemy turn entirely and switch back to player turn
                _currentTurn = TurnType.PlayerTurn;
                StartPlayerTurn();
                return;
            }

            _enemyTurnManager.StartEnemyTurn(_enemies);

            // Broadcast enemy turn start message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyTurnStarted,
                GetOwner(),
                _enemyTurnManager.GetCurrentEnemy(),  // Current active enemy
                _enemies.Count,                       // Number of enemies
                null,
                null
            );

            Debug.LogInfo("[UnitManager] Enemy turn started.");
        }

        /**
         * @brief Ends the current player turn.
         * 
         * @note Resets player energy and broadcasts a player turn end message.
         */
        public void EndPlayerTurn()
        {
            foreach (var player in _players)
                ResetEnergy(player);

            // Broadcast player turn end message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.PlayerTurnEnded,
                GetOwner(),
                null,
                playerTurnCount,
                null,
                null
            );

            Debug.LogInfo("[UnitManager] Player turn ended.");
        }

        /**
         * @brief Ends the current enemy turn.
         * 
         * @note Resets enemy energy and broadcasts an enemy turn end message.
         */
        public void EndEnemyTurn()
        {
            foreach (var enemy in _enemies)
                ResetEnergy(enemy);

            // Broadcast enemy turn end message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyTurnEnded,
                GetOwner(),
                null,
                _enemies.Count,
                null,
                null
            );

            Debug.LogInfo("[UnitManager] Enemy turn ended.");
        }

        /**
         * @brief Resets the energy of a given entity to its maximum value.
         * 
         * @param entity The Entity whose energy is to be reset.
         * @note Sends an energy change message specific to whether the entity is a player or enemy.
         */
        private void ResetEnergy(Entity entity)
        {
            // Store previous energy for the message
            int previousEnergy = InternalCalls.UnitComponent_GetEnergy(entity.ID);
            int maxEnergy = InternalCalls.UnitComponent_GetMaxEnergy(entity.ID);

            InternalCalls.UnitComponent_SetEnergy(entity.ID, maxEnergy);

            // Get position for the message
            Vector2D position = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(entity.ID, ref position);

            // Determine message type based on entity type
            bool isEnemy = IsEnemy(entity);
            GameMessageType messageType = isEnemy ?
                GameMessageType.EnemyEnergyChanged :
                GameMessageType.PlayerEnergyChanged;

            // Send energy change message
            GameMessageHub.Instance.SendMessage(
                messageType,
                entity,
                null,
                maxEnergy,     // New energy value
                position,
                previousEnergy // Previous energy value
            );
        }

        /**
         * @brief Checks and handles exhaustion of the current active enemy.
         * 
         * @note If the current enemy has no energy, marks it as finished and sends exhaustion messages.
         */
        public void CheckAndHandleEnemyExhaustion()
        {
            Entity currentActiveEnemy = _enemyTurnManager.GetCurrentEnemy();

            if (currentActiveEnemy != null && InternalCalls.UnitComponent_GetEnergy(currentActiveEnemy.ID) <= 0)
            {
                Debug.LogInfo("[UnitManager] Current enemy is exhausted. Moving to next enemy.");

                // Send energy exhausted message
                Vector2D enemyPos = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(currentActiveEnemy.ID, ref enemyPos);

                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyEnergyChanged,
                    currentActiveEnemy,
                    null,
                    0, // No energy left
                    enemyPos,
                    "Exhausted"
                );

                _enemyTurnManager.EnemyFinished(currentActiveEnemy);

                // Send enemy turn finished message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyActionFinished,
                    currentActiveEnemy,
                    null,
                    0,
                    enemyPos,
                    "EnergyExhausted"
                );
            }
        }

        /**
         * @brief Notifies that an enemy has finished its turn.
         * 
         * @param entity The Entity representing the enemy that finished its turn.
         * @note Sends an action finished message and informs the enemy turn manager.
         */
        public void NotifyEnemyFinished(Entity entity)
        {
            if (_enemyTurnManager != null)
            {
                Debug.LogInfo("[UnitManager] Enemy {0} notified turn finished", entity.ID);

                // Send message before finishing turn
                Vector2D enemyPos = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(entity.ID, ref enemyPos);

                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyActionFinished,
                    entity,
                    null,
                    InternalCalls.UnitComponent_GetEnergy(entity.ID), // Remaining energy
                    enemyPos,
                    "TurnFinished"
                );

                _enemyTurnManager.EnemyFinished(entity);
            }
        }

        /**
         * @brief Checks if all enemies are exhausted (out of energy).
         * 
         * @return True if all enemies have zero or less energy, false otherwise.
         * @note Sends an enemy turn end message if all enemies are exhausted.
         */
        public bool AreAllEnemiesExhausted()
        {
            foreach (var enemy in _enemies)
            {
                if (InternalCalls.UnitComponent_GetEnergy(enemy.ID) > 0)
                    return false;
            }

            // If we get here, all enemies are exhausted
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyTurnEnded,
                GetOwner(),
                null,
                _enemies.Count,
                null,
                "AllExhausted"
            );

            return true;
        }

        /**
         * @brief Checks if all enemies are defeated based on health and alive status.
         * 
         * @return True if all enemies are defeated (dead or no health), false otherwise.
         * @note Sends an all enemies defeated message if applicable and logs the result.
         */
        public bool AreAllEnemiesDefeated()
        {
            // If there are no enemies at all, consider them all defeated
            if (_enemies.Count == 0)
            {
                Debug.LogInfo("[UnitManager] [AreAllEnemiesDefeated] No enemies in list.");
                return true;
            }

            foreach (var enemy in _enemies)
            {
                // Check if the enemy is both alive and has health
                int enemyHealth = InternalCalls.UnitComponent_GetHealth(enemy.ID);
                bool isAlive = InternalCalls.UnitComponent_GetIsAlive(enemy.ID);

                if (enemyHealth > 0 && isAlive)
                {
                    return false; // At least one enemy is alive
                }
            }

            // If we get here, all enemies are defeated - send message once
            if (_enemies.Count > 0)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.AllEnemiesDefeated,
                    GetOwner(),
                    null,
                    _enemies.Count,
                    null,
                    null
                );
            }

            return true; // All enemies are dead or have no health
        }

        /**
         * @brief Retrieves the first player entity.
         * 
         * @return The Entity representing the player, or null if no players exist.
         * @note Assumes a single-player scenario and returns the first player in the list.
         */
        public Entity GetPlayerEntity()
        {
            if (_players.Count > 0)
                return _players[0]; // Assuming there is only one player
            return null; // No player found
        }

        /**
         * @brief Retrieves the list of enemy entities.
         * 
         * @return A List<Entity> containing all enemies.
         */
        public List<Entity> GetEnemies()
        {
            return _enemies;
        }

        /**
         * @brief Determines if an entity is an enemy.
         * 
         * @param entity The Entity to check.
         * @return True if the entity is in the enemies list, false otherwise.
         * @note Logs the result of the check for debugging purposes.
         */
        public bool IsEnemy(Entity entity)
        {
            Debug.LogInfo("[UnitManager] [IsEnemy] Checking if entity {0} is an enemy.", entity.ID);
            foreach (var enemy in _enemies)
            {
                if (enemy.ID == entity.ID)
                {
                    Debug.LogInfo("[UnitManager] [IsEnemy] Entity {0} is confirmed as an enemy.", entity.ID);
                    return true;
                }
            }
            Debug.LogInfo("[UnitManager] [IsEnemy] Entity {0} is NOT an enemy.", entity.ID);
            return false;
        }

        // ----------------------[ DEATH SOUND ]----------------------------
        //private static int deathsoundindex = 0;
        private static readonly string[] deathsounds = new string[]
        {
            "SFX_Enemy1_Death_01",
            "SFX_Enemy1_Death_02",
            "SFX_Enemy1_Death_03",
            "SFX_Enemy1_Death_04"
        };
        /**
  * @brief Removes an enemy entity from the managed list.
  * 
  * @param entity The Entity representing the enemy to remove.
  * @param alreadyNotified Optional flag indicating if death notification was already sent (default: false).
  * @note Removes the enemy by ID, logs the result, and sends appropriate messages unless already notified.
  */
        public void RemoveEnemy(Entity entity, bool alreadyNotified = false)
        {
            if (entity == null)
            {
                Debug.LogError("[UnitManager] Attempted to remove null enemy entity.");
                return;
            }

            // Get the entity's position before removal (for the message)
            Vector2D enemyPos = new Vector2D();
            if (entity.ID != 0)
            {
                InternalCalls.UnitComponent_GetPosition(entity.ID, ref enemyPos);
            }

            // Find the enemy by ID, not by reference
            Entity enemyToRemove = null;
            int indexToRemove = -1;

            for (int i = 0; i < _enemies.Count; i++)
            {
                if (_enemies[i] != null && _enemies[i].ID == entity.ID)
                {
                    enemyToRemove = _enemies[i];
                    indexToRemove = i;
                    break;
                }
            }

            // If found, remove it
            if (enemyToRemove != null)
            {
                _enemies.RemoveAt(indexToRemove);

                Debug.LogInfo("[UnitManager] Enemy {0} successfully removed from managed list.", entity.ID);

                // Only send the EnemyDied message if this isn't already in response to one
                if (!alreadyNotified)
                {
                    // Send enemy removed message
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.EnemyDied,
                        GetOwner(), // UnitManager as source
                        entity,     // Enemy as target
                        _enemies.Count, // Remaining enemies count
                        enemyPos,   // Position where enemy died
                        "Removed"   // Action type
                    );
                }
            }
            else
            {
                Debug.LogWarning("[UnitManager] Attempted to remove non-existent enemy {0}.", entity.ID);

                // Send error message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameError,
                    GetOwner(),
                    entity,
                    0,
                    enemyPos,
                    "EnemyRemovalFailed"
                );
            }
        }

        /**
         * @brief Updates attack overlays for enemies based on player proximity and mouse hover.
         * 
         * @note Manages headbutt and special attack overlays, showing them when the player is in range
         * of a hovered enemy, and hides them otherwise. Sends UI update messages on state changes.
         */
        private void UpdateEnemyAttackOverlays()
        {
            // 1. Make sure overlay entities are valid
            _headbuttOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("headbuttIcon"));
            _specialAttackOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("specialAttackIcon"));

            bool overlayShown = false;
            Entity hoveredEnemy = null;
            bool shouldShowHeadbuttOverlay = false;
            bool shouldShowSpecialOverlay = false;

            // 2. Retrieve player and playerTile
            Entity playerEntity = GetPlayerEntity();
            if (playerEntity == null)
            {
                // Only log this error once, not every frame
                if (_lastOverlaidEnemy != null)
                {
                    Debug.LogWarning("[UnitManager] No valid player entity found for overlay checks.");

                    // Send message about missing player only once
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.GameError,
                        GetOwner(),
                        null,
                        0,
                        null,
                        "NoPlayerForOverlay"
                    );

                    // Reset state since we can't show any overlays
                    _lastOverlaidEnemy = null;
                }

                // Make sure overlays are hidden
                if (_headbuttOverlayShown || _specialOverlayShown)
                {
                    HideHeadbuttOverlay();
                    HideSpecialOverlay();
                    _headbuttOverlayShown = false;
                    _specialOverlayShown = false;
                }

                return;
            }

            Vector2D playerPos = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPos);
            Entity playerTile = TileManager.Instance.GetTileAtPosition(playerPos);
            if (playerTile == null)
            {
                return;
            }

            // Only send UI update started message if we're going to make UI changes
            bool uiMessageSent = false;

            // 3. Iterate enemies to find if any hovered enemy is appropriate for overlay
            foreach (Entity enemyEntity in _enemies)
            {
                // a) Check hover first
                if (!InternalCalls.Input_IsMouseOverEntity(enemyEntity.ID))
                    continue;

                // b) Get enemy tile
                Vector2D enemyPos = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(enemyEntity.ID, ref enemyPos);
                Entity enemyTile = TileManager.Instance.GetTileAtPosition(enemyPos);
                if (enemyTile == null)
                    continue;

                // We found a hovered enemy
                hoveredEnemy = enemyEntity;

                // c) Check adjacency (1 tile away)
                if (Player.IsAdjacentToTile(playerTile, enemyTile))
                {
                    shouldShowHeadbuttOverlay = true;
                }
                // Otherwise check if 2-3 tiles away (unobstructed path)
                else if (Player.IsInRangedDistance(playerTile, enemyTile) && Player.IsPathClearForOverlay(playerTile, enemyTile))
                {
                    if (Player.rangedunlocked)
                    {
                        shouldShowSpecialOverlay = true;
                    }
                }
                break; // Only handle the first hovered enemy
            }

            // Check if state has changed
            bool stateChanged = hoveredEnemy != _lastOverlaidEnemy ||
                                shouldShowHeadbuttOverlay != _headbuttOverlayShown ||
                                shouldShowSpecialOverlay != _specialOverlayShown;

            if (stateChanged)
            {
                // State changed, send UI update started message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.UIUpdateStarted,
                    GetOwner(),
                    playerEntity,
                    0,
                    playerPos,
                    "AttackOverlays"
                );
                uiMessageSent = true;

                // Update headbutt overlay
                if (shouldShowHeadbuttOverlay && hoveredEnemy != null)
                {
                    EnemyAttackOverlay(hoveredEnemy, _headbuttOverlayEntity);
                    _headbuttOverlayShown = true;
                    overlayShown = true;

                    // Send melee attack available message (only once on change)
                    Vector2D enemyPos = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(hoveredEnemy.ID, ref enemyPos);
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.AttackRangeCheck,
                        playerEntity,
                        hoveredEnemy,
                        1,
                        enemyPos,
                        "MeleeRangeValid"
                    );
                }
                else if (_headbuttOverlayShown)
                {
                    HideHeadbuttOverlay();
                    _headbuttOverlayShown = false;
                }

                // Update special overlay
                if (shouldShowSpecialOverlay && hoveredEnemy != null)
                {
                    EnemyAttackOverlay(hoveredEnemy, _specialAttackOverlayEntity);
                    _specialOverlayShown = true;
                    overlayShown = true;

                    // Send ranged attack available message (only once on change)
                    Vector2D enemyPos = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(hoveredEnemy.ID, ref enemyPos);
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.AttackRangeCheck,
                        playerEntity,
                        hoveredEnemy,
                        2,
                        enemyPos,
                        "RangedAttackValid"
                    );
                }
                else if (_specialOverlayShown)
                {
                    HideSpecialOverlay();
                    _specialOverlayShown = false;
                }

                // Update the tracked enemy
                _lastOverlaidEnemy = hoveredEnemy;

                // Send UI update completed message only if we sent the started message
                if (uiMessageSent)
                {
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.UIUpdateCompleted,
                        GetOwner(),
                        null,
                        overlayShown ? 1 : 0,
                        null,
                        overlayShown ? "OverlayShown" : "NoOverlayShown"
                    );
                }
            }
        }

        /**
         * @brief Checks if the mouse cursor is over an entity based on its position and scale.
         * 
         * @param pos The Vector2D position of the entity.
         * @param scale The Vector2D scale of the entity.
         * @return True if the mouse cursor is over the entity, false otherwise.
         * @note Uses the entity's position and scale to define a bounding box for mouse detection.
         */
        private static bool Input_IsMouseOverEntity(Vector2D pos, Vector2D scale)
        {
            Vector2D entityPos = pos;
            Vector2D entityScale = scale;

            Vector2D cursorPos = Input.GetCursorPosition();

            float left = entityPos.X - (entityScale.X / 2);
            float right = entityPos.X + (entityScale.X / 2);
            float bottom = entityPos.Y - (entityScale.Y / 2);
            float top = entityPos.Y + (entityScale.Y / 2);

            return (cursorPos.X >= left && cursorPos.X <= right &&
                    cursorPos.Y >= bottom && cursorPos.Y <= top);
        }

		/**
         * @brief Displays the stage clear overlay with interactive buttons.
         * 
         * @note Shows a stage clear overlay with replay and next stage buttons, handles hover and click events,
         * and sends appropriate UI messages. Resets overlay on button click.
         */
		public void StageClearOverlay()
		{
			// Send stage clear start message
			GameMessageHub.Instance.SendMessage(
				GameMessageType.LevelComplete,
				GetOwner(),
				null,
				playerTurnCount,
				null,
				"StageClearOverlayShown"
			);
			Debug.LogError("[UnitManager] StageClearOverlay running.");

			int currLevel = InternalCalls.LevelSwitch_GetCurrLevel();

			if (currLevel == 0)
			{
				_stageClearOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("tutorialClear"));
				_stageClearOverlayButtonEntity1 = new Entity(InternalCalls.Entity_GetEntityByName("tutorialClearLevel1"));
				_stageClearOverlayButtonEntity2 = new Entity(InternalCalls.Entity_GetEntityByName("tutorialClearMainMenu"));
			}
			else if (currLevel == 1)
			{
				_stageClearOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("stageClear"));
				_stageClearOverlayButtonEntity1 = new Entity(InternalCalls.Entity_GetEntityByName("stageClearReplay"));
				_stageClearOverlayButtonEntity2 = new Entity(InternalCalls.Entity_GetEntityByName("stageClearNextStage"));
			}
			else if (currLevel == 2)
			{
				_stageClearOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("stage2Clear"));
				_stageClearOverlayButtonEntity1 = new Entity(InternalCalls.Entity_GetEntityByName("stage2ClearReplay"));
				_stageClearOverlayButtonEntity2 = new Entity(InternalCalls.Entity_GetEntityByName("stage2ClearMainMenu"));
				_stage2ClearToBeContinuedEntity = new Entity(InternalCalls.Entity_GetEntityByName("stage2ClearToBeContinued"));
			}

			if (_stageClearOverlayEntity == null || _stageClearOverlayEntity.ID == 0)
			{
				Debug.LogError("[UnitManager] StageClearOverlay entity not found or invalid.");

				// Send error message
				GameMessageHub.Instance.SendMessage(
					GameMessageType.GameError,
					GetOwner(),
					null,
					0,
					null,
					"StageClearOverlayMissing"
				);

				return;
			}

			Vector2D buttonScale = new Vector2D(380.0f, 130.0f);
			Vector2D zeroPos = new Vector2D(0, -50);
			Vector2D button1Pos = new Vector2D(50, -100);
			Vector2D button2Pos = new Vector2D(50, -230);

			// Set transform positions and scale
			InternalCalls.Transform_SetTranslation(_stageClearOverlayEntity.ID, ref zeroPos);
			InternalCalls.Transform_SetTranslation(_stageClearOverlayButtonEntity1.ID, ref button1Pos);
			InternalCalls.Transform_SetTranslation(_stageClearOverlayButtonEntity2.ID, ref button2Pos);

			Vector2D bigScale = new Vector2D(2000, 1000);
			InternalCalls.Transform_SetScale(_stageClearOverlayEntity.ID, ref bigScale);
			InternalCalls.Layer_ChangeOpacity(0.3f);

			// -------------------------------
			// LEVEL-SPECIFIC BUTTON LOGIC
			// -------------------------------
			if (currLevel == 0)
			{
				// Button 1: Go to Level 1
				if (Input_IsMouseOverEntity(button1Pos, buttonScale))
				{
					Vector2D hoveredScale = new Vector2D(380.0f * 1.1f, 130.0f * 1.1f);
					InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity1.ID, ref hoveredScale);
					if (Input.IsMouseButtonDown(0))
					{
						InternalCalls.LevelSwitch_TutorialToLevel1();
						InternalCalls.LevelSwitch_ReloadCurrLevel(); // Reinitialize Enemy UI Health and Energy Bar
                        InventorySystem.Instance.ResetInventorySystem();
                        HideStageClearOverlay();
						return;
					}
				}
				else InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity1.ID, ref buttonScale);

				// Button 2: Back to Main Menu
				if (Input_IsMouseOverEntity(button2Pos, buttonScale))
				{
					Vector2D hoveredScale = new Vector2D(380.0f * 1.1f, 130.0f * 1.1f);
					InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity2.ID, ref hoveredScale);
					if (Input.IsMouseButtonDown(0))
					{
						InternalCalls.LevelSwitch_ReturnMainMenu();
						HideStageClearOverlay();
						return;
					}
				}
				else InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity2.ID, ref buttonScale);

				return;
			}
			else if (currLevel == 1)
			{
				// Button 1: Replay Level 1
				if (Input_IsMouseOverEntity(button1Pos, buttonScale))
				{
					Debug.LogError("[UnitManager] Mouse over 1st button.");
					Vector2D hoveredScale = new Vector2D(380.0f * 1.1f, 130.0f * 1.1f);
					InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity1.ID, ref hoveredScale);

					GameMessageHub.Instance.SendMessage(
						GameMessageType.UIInteraction,
						null,
						_stageClearOverlayButtonEntity1,
						1,
						button1Pos,
						"MouseHoverButton1"
					);

					if (Input.IsMouseButtonDown(0))
					{
						GameMessageHub.Instance.SendMessage(
							GameMessageType.UIInteraction,
							null,
							_stageClearOverlayButtonEntity1,
							1,
							button1Pos,
							"ButtonClickedReplay"
						);

						HideStageClearOverlay();
						InternalCalls.LevelSwitch_ReloadCurrLevel();
                        InventorySystem.Instance.ResetInventorySystem();
                        
                        return;
					}
				}
				else InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity1.ID, ref buttonScale);

				// Button 2: Go to Level 2
				if (Input_IsMouseOverEntity(button2Pos, buttonScale))
				{
					Debug.LogError("[UnitManager] Mouse over 2nd button.");
					Vector2D hoveredScale = new Vector2D(380.0f * 1.1f, 130.0f * 1.1f);
					InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity2.ID, ref hoveredScale);

					GameMessageHub.Instance.SendMessage(
						GameMessageType.UIInteraction,
						null,
						_stageClearOverlayButtonEntity2,
						2,
						button2Pos,
						"MouseHoverButton2"
					);

					if (Input.IsMouseButtonDown(0))
					{
						GameMessageHub.Instance.SendMessage(
							GameMessageType.UIInteraction,
							null,
							_stageClearOverlayButtonEntity2,
							2,
							button2Pos,
							"ButtonClickedNextLevel"
						);

                        PlayerStats.SaveFromPlayer(InternalCalls.GetPlayerEntity());
                        HideStageClearOverlay();
						InternalCalls.LevelSwitch_Level1ToLevel2();
						InternalCalls.LevelSwitch_ReloadCurrLevel(); // Reinitialize Enemy UI Health and Energy Bar
						return;
					}
				}
				else InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity2.ID, ref buttonScale);

				return;
			}
			else if (currLevel == 2)
			{

				if (_stage2ClearToBeContinuedEntity != null)
				{
					Vector2D continuedScale = new Vector2D(1300.0f, 122.0f);
					InternalCalls.Transform_SetScale(_stage2ClearToBeContinuedEntity.ID, ref continuedScale);
				}

				// Button 1: Back to Level 1
				if (Input_IsMouseOverEntity(button1Pos, buttonScale))
				{
					Vector2D hoveredScale = new Vector2D(380.0f * 1.1f, 130.0f * 1.1f);
					InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity1.ID, ref hoveredScale);
					if (Input.IsMouseButtonDown(0))
					{
						InternalCalls.LevelSwitch_Level2ToLevel1();
						InternalCalls.LevelSwitch_ReloadCurrLevel(); // Reinitialize Enemy UI Health and Energy Bar
                        InventorySystem.Instance.ResetInventorySystem();
                        HideStageClearOverlay();
						return;
					}
				}
				else InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity1.ID, ref buttonScale);

				// Button 2: Back to Main Menu
				if (Input_IsMouseOverEntity(button2Pos, buttonScale))
				{
					Vector2D hoveredScale = new Vector2D(380.0f * 1.1f, 130.0f * 1.1f);
					InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity2.ID, ref hoveredScale);
					if (Input.IsMouseButtonDown(0))
					{
						InternalCalls.LevelSwitch_ReturnMainMenu();
						HideStageClearOverlay();
						return;
					}
				}
				else InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity2.ID, ref buttonScale);

				return;
			}
		}


		/**
         * @brief Hides stage clear overlay
         * 
         */
		private void HideStageClearOverlay()
        {
			Vector2D zeroScale = new Vector2D(0, 0);
			InternalCalls.Transform_SetScale(_stageClearOverlayEntity.ID, ref zeroScale);
			InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity1.ID, ref zeroScale);
			InternalCalls.Transform_SetScale(_stageClearOverlayButtonEntity2.ID, ref zeroScale);
			InternalCalls.Layer_ChangeOpacity(1.0f);

			// Hide the "To Be Continued" banner if it exists
			if (_stage2ClearToBeContinuedEntity != null && _stage2ClearToBeContinuedEntity.ID != 0)
			{
				InternalCalls.Transform_SetScale(_stage2ClearToBeContinuedEntity.ID, ref zeroScale);
			}
		}

		public void ShowGameOverOverlay()
		{
			if (_gameOverHandled)
				return;

			_gameOverHandled = true;

			_gameOverOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("GameOver"));
			_gameOverYesButtonEntity = new Entity(InternalCalls.Entity_GetEntityByName("YesButton"));
			_gameOverNoButtonEntity = new Entity(InternalCalls.Entity_GetEntityByName("NoButton"));

			Vector2D buttonScale = new Vector2D(380.0f, 130.0f);
			Vector2D zeroPos = new Vector2D(0, -50);
			Vector2D button1Pos = new Vector2D(50, -100);
			Vector2D button2Pos = new Vector2D(50, -230);
			Vector2D bigScale = new Vector2D(2000, 1000);

			InternalCalls.Transform_SetScale(_gameOverOverlayEntity.ID, ref bigScale);
			InternalCalls.Transform_SetScale(_gameOverYesButtonEntity.ID, ref buttonScale);
			InternalCalls.Transform_SetScale(_gameOverNoButtonEntity.ID, ref buttonScale);

			InternalCalls.Transform_SetTranslation(_gameOverOverlayEntity.ID, ref zeroPos);
			InternalCalls.Transform_SetTranslation(_gameOverYesButtonEntity.ID, ref button1Pos);
			InternalCalls.Transform_SetTranslation(_gameOverNoButtonEntity.ID, ref button2Pos);
			InternalCalls.Layer_ChangeOpacity(0.3f);
		}

		public void HideGameOverOverlay()
		{
			Vector2D zeroScale = new Vector2D(0, 0);

			if (_gameOverOverlayEntity != null) InternalCalls.Transform_SetScale(_gameOverOverlayEntity.ID, ref zeroScale);
			if (_gameOverYesButtonEntity != null) InternalCalls.Transform_SetScale(_gameOverYesButtonEntity.ID, ref zeroScale);
			if (_gameOverNoButtonEntity != null) InternalCalls.Transform_SetScale(_gameOverNoButtonEntity.ID, ref zeroScale);

			InternalCalls.Layer_ChangeOpacity(1.0f);

			_gameOverHandled = false; // Reset flag here too (optional)
		}

		/**
         * @brief Positions an attack overlay over a specified enemy entity.
         * 
         * @param enemyEntity The Entity representing the enemy to overlay.
         * @param overlayEntity The Entity representing the overlay to position.
         * @note Sets the overlay's position and scale to match the enemy if the enemy is alive.
         */
		public void EnemyAttackOverlay(Entity enemyEntity, Entity overlayEntity)
        {
            bool enemyAlive = InternalCalls.UnitComponent_GetIsAlive(enemyEntity.ID);

            if (enemyAlive == true)
            {
                // Acquire the enemy's position/scale
                InternalCalls.Transform_GetTranslation(enemyEntity.ID, out Vector2D enemyPosition);
                InternalCalls.Transform_GetScale(enemyEntity.ID, out Vector2D enemyScale);

                // Match the overlay to the enemy
                Vector2D upScale = new Vector2D(60, 60);
                InternalCalls.Transform_SetTranslation(overlayEntity.ID, ref enemyPosition);
                InternalCalls.Transform_SetScale(overlayEntity.ID, ref upScale);
            }
        }

        /**
         * @brief Hides the headbutt attack overlay.
         * 
         * @note Sets the headbutt overlay scale to zero and sends a hidden message if the entity is valid.
         */
        private void HideHeadbuttOverlay()
        {
            if (_headbuttOverlayEntity == null)
            {
                _headbuttOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("headbuttIcon"));
                if (_headbuttOverlayEntity == null)
                {
                    Debug.LogError("[UpdateEnemyAttackOverlays] Level1_Game_headbuttIcon entity not found.");
                    return;
                }
            }
            Vector2D zeroScale = Vector2D.Zero;
            InternalCalls.Transform_SetScale(_headbuttOverlayEntity.ID, ref zeroScale);
        }

        /**
         * @brief Hides the special attack overlay.
         * 
         * @note Sets the special attack overlay scale to zero and sends a hidden message if the entity is valid.
         */
        private void HideSpecialOverlay()
        {
            if (_specialAttackOverlayEntity == null)
            {
                _specialAttackOverlayEntity = new Entity(InternalCalls.Entity_GetEntityByName("specialAttackIcon"));
                if (_specialAttackOverlayEntity == null)
                {
                    Debug.LogError("[UpdateEnemyAttackOverlays] Level1_Game_specialAttackIcon entity not found.");
                    return;
                }
            }
            Vector2D zeroScale = Vector2D.Zero;
            InternalCalls.Transform_SetScale(_specialAttackOverlayEntity.ID, ref zeroScale);

            // Send overlay hidden message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.UIElementsPositioned,
                GetOwner(),
                _specialAttackOverlayEntity,
                0,
                null,
                "SpecialAttackOverlayHidden"
            );
        }

		private bool IsNewKeyPress(int keyCode)
		{
			if (!_cheatKeyHeld.ContainsKey(keyCode))
				_cheatKeyHeld[keyCode] = false;

			if (Input.IsKeyPressed(keyCode))
			{
				if (!_cheatKeyHeld[keyCode])
				{
					_cheatKeyHeld[keyCode] = true;
					return true; // Key was just pressed
				}
			}
			else
			{
				_cheatKeyHeld[keyCode] = false; // Key is released
			}

			return false;
		}
	}
}
