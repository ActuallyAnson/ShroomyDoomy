/******************************************************************************
/*!
\file  PlayerBehaviour.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  This file implements the Player class, handling movement, attacks,
        energy management, and interactions with the game environment.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using Fruitloops;
using System;
using System.Collections.Generic;

namespace FruitLoops
{
/**
 * @class Player
 * @brief Behaviour component for managing the player's actions.
 *
 * This component handles player movement, energy usage, turn-based interactions,
 * attacking enemies (melee and ranged), and special actions like AOE attacks.
 */
    public class Player : BehaviourComponent, IGameObserver
    {
        // -----------------------[ EXISTING FIELDS ]------------------------
        private readonly Unit _unit;
        private static bool mouseClickedLastFrame = false;
        private static bool Idle = false;
        private HealthBarComponent _healthBarComponent;
        public EnergyBarComponent _energyBarComponent;
        
        private int _estimatedEnergyCost = 0;
        private Entity _hoveredTile = null;

        public static bool rangedunlocked = false;
        // -----------------------[ NEW FIELDS ]-----------------------------
        
        // A list of tiles (Entity) along the path the player needs to walk
        private List<Entity> movementPath = new List<Entity>();

        private List<Entity> _previewPath = new List<Entity>();

        // Index in movementPath to indicate which tile we are currently heading toward
        private int currentPathIndex = -1;

        // Flag to indicate if we're currently moving incrementally
        public bool isMoving = false;
        
        // Movement speed (world units per second)
        private float moveSpeed = 230.0f;

        // How long to pause (in seconds) at each tile before moving on
        public float timeBetweenSteps = 0.3f;

        // A countdown timer that tracks how much longer we should wait
        public float waitTimer = 0.3f;

        // Stores the last logged turn number
        private static int lastLoggedTurn = -1;

        // Track the last turn a ranged attack was used per player
        private static Dictionary<int, int> lastRangedAttackTurn = new Dictionary<int, int>();

        // -----------------------[ TUTORIAL FIELDS ]-----------------------------
        public static bool IsMovementRestrictedByTutorial = false;
        public static Vector2D TutorialTargetTilePosition = Vector2D.Zero;
        public static Vector2D allowedMovementTile = Vector2D.Zero;

		// -----------------------[ GAMEOVER BUTTON FIELDS ]-----------------------------
		private static bool _pressedWhileHoveringYes = false;
		private static bool _pressedWhileHoveringNo = false;
		private static bool _buttonClickedOnce = false;

		public static Player Instance { get; private set; }


        private static bool hasCheckedEntities = false;

        // ----------------------[ ATTACK SOUND ]----------------------------
        private static int attackSoundIndex = 0;
        private static readonly string[] attackSoundNames = new string[]
        {
            "SFX_Enemy2_Attack_01", "SFX_Enemy2_Attack_02", "SFX_Enemy2_Attack_03",
            "SFX_Enemy2_Attack_04", "SFX_Enemy2_Attack_05", "SFX_Enemy2_Attack_06",
            "SFX_Enemy2_Attack_07", "SFX_Enemy2_Attack_08", "SFX_Enemy2_Attack_09",
            "SFX_Enemy2_Attack_10"
        };

        // -----------------------[ Death Sound ]---------------------------
        //private static int deathsoundIndex = 0;
        private static readonly string[] deathsoundName = new string[]
        {
            "SFX_Enemy1_Death_01", "SFX_Enemy1_Death_02", "SFX_Enemy1_Death_03",
            "SFX_Enemy1_Death_04", "SFX_Enemy1_Death_05", "SFX_Enemy1_Death_06",
            "SFX_Enemy1_Death_07", "SFX_Enemy1_Death_08", "SFX_Enemy1_Death_09",
            "SFX_Enemy1_Death_10"
        };

        // -----------------------[ CONSTRUCTORS ]---------------------------
        public Player() { }

        /**
        * @brief Constructs a Player instance with specified attributes.
        *
        * This constructor initializes the player's position, stats, and UI components
        * such as health and energy bars. It also registers the player's behavior in the game logic system.
        *
        * @param entity The entity representing the player.
        * @param position The initial position of the player in the world.
        * @param health The current health of the player.
        * @param maxHealth The maximum health of the player.
        * @param attackPower The attack power of the player.
        * @param energy The current energy level of the player.
        * @param maxEnergy The maximum energy level of the player.
        */
        private Player(Entity entity, Vector2D position, int health, int maxHealth, int attackPower, 
            int energy, int maxEnergy, bool isAlive, bool isMoving, int speed)
        {
            Debug.LogInfo("[PlayerBehaviour] Player constructor called!");
            _unit = new Unit(position, health, maxHealth, attackPower, energy, maxEnergy, isAlive, isMoving, speed);

            var behaviour = new BehaviourFCT(entity, Start, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            // Initialize HealthBarComponent
            Entity healthBarFiller = new Entity(InternalCalls.Entity_GetEntityByName("healthbar(filler)"));
            Entity healthBarEmpty = new Entity(InternalCalls.Entity_GetEntityByName("healthbar(empty)"));
            Debug.LogInfo("Setting healthbar filler component with entity {0} and empty component {0}", healthBarFiller.ID, healthBarEmpty.ID);
            _healthBarComponent = new HealthBarComponent(entity, healthBarFiller, healthBarEmpty);

            Entity energyBarFiller = new Entity(InternalCalls.Entity_GetEntityByName("energybar(filler)"));
            Entity energyBarEmpty = new Entity(InternalCalls.Entity_GetEntityByName("energybar(empty)"));
            Debug.LogInfo("Setting healthbar component with entity {0}", entity.ID);
            Debug.LogInfo("Setting energybar filler component with entity {0} and empty component {0}", energyBarFiller.ID, energyBarEmpty.ID);
            _energyBarComponent = new EnergyBarComponent(entity, energyBarFiller, energyBarEmpty);

            // Register with message system
            RegisterForMessages();
        }

        /**
         * @brief Factory method for creating a Player behaviour component.
         *
         * This method extracts player attributes from the entity and initializes a Player instance.
         * It logs the player's starting attributes for debugging purposes.
         *
         * @param entity The entity that will be associated with the Player component.
         * @return A new Player instance linked to the given entity.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            Debug.LogInfo("[PlayerBehaviour][CreateBehaviour] Starting for behaviour PlayerBehaviour on entity {0}", entity.ID);

            Vector2D position = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(entity.ID, ref position);

            int health = InternalCalls.UnitComponent_GetHealth(entity.ID);
            int maxHealth = InternalCalls.UnitComponent_GetMaxHealth(entity.ID);
            int attackPower = InternalCalls.UnitComponent_GetAttackPower(entity.ID);
            int energy = InternalCalls.UnitComponent_GetEnergy(entity.ID);
            int maxEnergy = InternalCalls.UnitComponent_GetMaxEnergy(entity.ID);
            bool isAlive = InternalCalls.UnitComponent_GetIsAlive(entity.ID);
            bool isMoving = InternalCalls.UnitComponent_GetIsMoving(entity.ID);
            int speed = InternalCalls.UnitComponent_GetSpeed(entity.ID);

            Debug.LogInfo("[Player] Player stats: Position={0}, Health={1}/{2}, AttackPower={3}, Energy={4}/{5}",
                position, health, maxHealth, attackPower, energy, maxEnergy);

            return new Player(entity, position, health, maxHealth, attackPower, energy, maxEnergy, isAlive, isMoving, speed);
        }


        // -----------------------[ OBSERVER PATTERN METHODS ]----------------------

        /**
         * @brief Registers this player with the message system.
         * 
         * Subscribes to specific game message types that affect player behavior.
         */
        private void RegisterForMessages()
        {
            // Register for specific message types
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EnemyAttacked, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EnemyMoved, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EnemyDied, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.TurnChanged, this);

            Debug.LogInfo("[Player] Player {0} registered with message hub", GetOwner().ID);
        }

        /**
         * @brief Unregisters this player from the message system.
         * 
         * Should be called during cleanup to prevent memory leaks.
         */
        private void UnregisterFromMessages()
        {
            GameMessageHub.Instance.RemoveObserver(this);
            Debug.LogInfo("[Player] Player {0} unregistered from message hub", GetOwner().ID);
        }

        /**
         * @brief Implementation of IGameObserver.OnNotify to handle incoming messages.
         * 
         * This method is called when the player receives a message from the message hub.
         * It routes each message type to the appropriate handler.
         * 
         * @param message The message object containing event details.
         */
        public void OnNotify(GameMessage message)
        {
            if (message == null)
                return;

            switch (message.MessageType)
            {
                case GameMessageType.EnemyAttacked:
                    HandleEnemyAttack(message);
                    break;

                case GameMessageType.EnemyDied:
                    HandleEnemyDeath(message);
                    break;

                case GameMessageType.TurnChanged:
                    HandleTurnChange(message);
                    break;
            }
        }

        private void HandleEnemyAttack(GameMessage message)
        {
            // Check if this player is the target of the attack
            if (message.Target != null && message.Target.ID == GetOwner().ID)
            {
                Debug.LogInfo("[Player] Player {0} was attacked by enemy {1} for {2} damage",
                    GetOwner().ID, message.Source?.ID ?? 0, message.Value);

                // Update UI
                _healthBarComponent.UpdateHealthBar();

                // Could play hit reaction animation
                // InternalCalls.AnimationComponent_PlayAnimation(GetOwner().ID, "Hit");
            }
        }
       

        /**
         * @brief Handles incoming enemy death messages.
         * 
         * Can trigger special effects, award bonuses, or update UI when enemies die.
         * 
         * @param message The death message to process.
         */
        private void HandleEnemyDeath(GameMessage message)
        {
            Debug.LogInfo("[Player] Enemy {0} has died", message.Target?.ID ?? 0);

            // If player killed this enemy, could trigger specific rewards
            if (message.Source != null && message.Source.ID == GetOwner().ID)
            {
                Debug.LogInfo("[Player] Player {0} defeated enemy {1}",
                    GetOwner().ID, message.Target?.ID ?? 0);
            }
        }


        /**
         * @brief Handles turn change messages.
         * 
         * Updates player state when turns change, especially when it becomes the player's turn.
         * 
         * @param message The turn change message to process.
         */
        private void HandleTurnChange(GameMessage message)
        {
            if (message.Data != null && message.Data is TurnType turnType)
            {
                if (turnType == TurnType.PlayerTurn)
                {
                    // Reset for new turn
                    hasCheckedEntities = false;
                    Debug.LogInfo("[Player] Player turn started");
                }
            }
        }


        private static bool Input_IsMouseOverEntity(Vector2D pos, Vector2D scale)
        {
            // This method remains the same as in your original ButtonBehaviour
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
        // -----------------------[ LIFECYCLE METHODS ]----------------------


        /**
         * @brief Initializes the Player entity when the game starts.
         *
         * This method resets entity checks before movement begins and logs the initialization.
         *
         * @param entity The player entity being initialized.
         */
        private static void Start(Entity entity)
        {
            hasCheckedEntities = false; // Reset before movement starts
            IsMovementRestrictedByTutorial = false;
            Debug.LogInfo("[PlayerBehaviour.cs] Start called for Player {0}", entity.ID);

            // Send game start message if this is the first player to initialize
            if (UnitManager.Instance.GetPlayerTurnCount() == 0)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameStart,
                    entity,
                    null,
                    0,
                    null,
                    null
                );
            }

			// Force idle animation to play once on scene load
			if (!InternalCalls.AnimationComponent_CheckEntityPlaying(entity.ID))
			{
				Debug.LogInfo("[Player] No animation playing on load. Forcing idle animation.");
				InternalCalls.AnimationComponent_PlayAnimation(entity.ID, "PapaShroom Idle");
				Idle = true;
			}

            if(InternalCalls.LevelSwitch_GetCurrLevel() == 2)
            {
                PlayerStats.ApplyToPlayer(entity); // Restore after the level loads
            }

            if (InternalCalls.BoolSwitch_Level2ToLevel1())
            {
                PlayerStats.ResettoDefault(entity);
            }
        }

        /**
         * @brief Updates the Player entity's state every frame.
         *
         * This method:
         * - Ensures updates only happen on the player's turn.
         * - Logs the player's turn number.
         * - Checks for entities around the player once per turn.
         * - Handles movement and incremental movement logic.
         * - Allows the player to perform an AOE attack when 'E' is pressed.
         * - Handles movement and attacks based on mouse clicks.
         * - Updates the player's health and energy bars.
         *
         * @param entity The player entity being updated.
         * @param deltaTime The time elapsed since the last frame.
         */
        private static void Update(Entity entity, float deltaTime)
        {
            // If the pause/main/story menu is currently active, do NOT process any of the player's logic
            if (InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("StoryLayer"))
                return;

            var player = GameLogicSystem.Instance.GetBehaviour<Player>(entity);
			if (InternalCalls.UnitComponent_GetHealth(entity.ID) <= 0)
			{
				_buttonClickedOnce = false;
				var unitManager = UnitManager.Instance;
				unitManager.ShowGameOverOverlay();

				Vector2D buttonScale = new Vector2D(380.0f, 130.0f);
				Vector2D button1Pos = new Vector2D(50, -100);
				Vector2D button2Pos = new Vector2D(50, -230);

				if (Input_IsMouseOverEntity(button1Pos, buttonScale))
				{
					Vector2D hoveredScale = buttonScale * 1.1f;
					InternalCalls.Transform_SetScale(unitManager._gameOverYesButtonEntity.ID, ref hoveredScale);

					if (Input.IsMouseButtonDown(0) && !_pressedWhileHoveringYes)
						_pressedWhileHoveringYes = true;

					if (_pressedWhileHoveringYes && !Input.IsMouseButtonDown(0) && !_buttonClickedOnce)
					{
						_buttonClickedOnce = true;
						_pressedWhileHoveringYes = false;

						GameMessageHub.Instance.SendMessage(
							GameMessageType.UIInteraction,
							null,
							unitManager._gameOverYesButtonEntity,
							1,
							button1Pos,
							"ButtonClickedReplay"
						);

						unitManager.HideGameOverOverlay();
						int current = InternalCalls.LevelSwitch_GetCurrLevel();
						if (current == 2)
						{
							InternalCalls.LevelSwitch_Level2ToLevel1();
							InternalCalls.LevelSwitch_ReloadCurrLevel();
						}
						else
						{
							InternalCalls.LevelSwitch_ReloadCurrLevel();
						}
						return;
					}
				}
				else
				{
					InternalCalls.Transform_SetScale(unitManager._gameOverYesButtonEntity.ID, ref buttonScale);
				}

				if (Input_IsMouseOverEntity(button2Pos, buttonScale))
				{
					Vector2D hoveredScale = buttonScale * 1.1f;
					InternalCalls.Transform_SetScale(unitManager._gameOverNoButtonEntity.ID, ref hoveredScale);

					if (Input.IsMouseButtonDown(0) && !_pressedWhileHoveringNo)
						_pressedWhileHoveringNo = true;

					if (_pressedWhileHoveringNo && !Input.IsMouseButtonDown(0) && !_buttonClickedOnce)
					{
						_buttonClickedOnce = true;
						_pressedWhileHoveringNo = false;

						GameMessageHub.Instance.SendMessage(
							GameMessageType.UIInteraction,
							null,
							unitManager._gameOverNoButtonEntity,
							2,
							button2Pos,
							"ButtonClickedMenu"
						);

						unitManager.HideGameOverOverlay();
						InternalCalls.LevelSwitch_ReturnMainMenu();
						return;
					}
				}
				else
				{
					InternalCalls.Transform_SetScale(unitManager._gameOverNoButtonEntity.ID, ref buttonScale);
				}

				// Send player died message (only once)
				if (InternalCalls.UnitComponent_GetIsAlive(entity.ID))
				{
					InternalCalls.UnitComponent_SetIsAlive(entity.ID, false);
					GameMessageHub.Instance.SendMessage(
						GameMessageType.PlayerDied,
						entity,
						null,
						0,
						null,
						null
					);
				}
				return;
			}


			if (UnitManager.Instance.CurrentTurn != TurnType.PlayerTurn)
                return;

            int turnNumber = UnitManager.Instance.GetPlayerTurnCount();

            // Log only if the turn count has changed
            if (turnNumber != lastLoggedTurn)
            {
                Debug.LogInfo("[PlayerBehaviour] Current Player Turn: " + turnNumber);
                lastLoggedTurn = turnNumber; // Update the last logged turn

                // Send player turn started message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PlayerTurnStarted,
                    entity,
                    null,
                    turnNumber,
                    null,
                    null
                );
            }

            // Only check once per turn or action
            if (!hasCheckedEntities)
            {
                CheckEntitiesAroundPlayer(entity);
                hasCheckedEntities = true; // Prevents multiple checks
            }

            // 1. Check if we are currently in the middle of an incremental move
            
            if (player != null && player.isMoving)
            {
                // If we are moving, incrementally move a bit toward the next tile this frame
                DoIncrementalMove(entity, deltaTime, player);
                return;
            }

            // ----------------------[ AOE ATTACK WHEN 'E' KEY IS PRESSED ]----------------------
            if (Input.IsKeyPressed(69)) // ASCII Keycode for 'E'
            {
                int currentEnergy = InternalCalls.UnitComponent_GetEnergy(entity.ID);

                if (currentEnergy < 3)
                {
                    Debug.LogWarning("[Player] Not enough energy to perform AOE attack. Requires 3 energy.");
                }
                else
                {
                    Vector2D playerPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(entity.ID, ref playerPosition);

                    Entity playerTile = TileManager.Instance.GetTileAtPosition(playerPosition);
                    if (playerTile != null)
                    {
                        List<Entity> nearbyTiles = GetSurroundingTiles(playerTile);
                        bool aoeAttackPerformed = false;

                        foreach (var tile in nearbyTiles)
                        {
                            Entity occupant = TileManager.Instance.GetOccupant(TileManager.Instance.GetTileCenter(tile));
                            if (occupant != null && UnitManager.Instance.IsEnemy(occupant))
                            {
                                // Perform AOE attack
                                Debug.LogInfo($"[Player] AOE Attack on Enemy {occupant.ID} in tile {tile.ID}");

                                // Reduce energy (costs 3 energy for AOE attack)
                                currentEnergy -= 3;
                                InternalCalls.UnitComponent_SetEnergy(entity.ID, currentEnergy);

                                // Send energy change message
                                GameMessageHub.Instance.SendMessage(
                                    GameMessageType.PlayerEnergyChanged,
                                    entity,
                                    null,
                                    currentEnergy,
                                    playerPosition,
                                    null
                                );

                                // Apply increased damage (1.5x attack power)
                                int attackPower = InternalCalls.UnitComponent_GetAttackPower(entity.ID);
                                int increasedDamage = (int)(attackPower * 1.5f); // 1.5x multiplier
                                int enemyHealth = InternalCalls.UnitComponent_GetHealth(occupant.ID);
                                enemyHealth -= increasedDamage;
                                InternalCalls.UnitComponent_SetHealth(occupant.ID, enemyHealth);

                                // Send attack message
                                GameMessageHub.Instance.SendMessage(
                                    GameMessageType.PlayerAttacked,
                                    entity,
                                    occupant,
                                    increasedDamage,
                                    playerPosition,
                                    "AOE"  // Mark this as an AOE attack in the data field
                                );

                                // Play AOE animation & sound
                                InternalCalls.AnimationComponent_PlayAnimation(entity.ID, "AOE_Attack");

                                Debug.LogInfo($"[Player] Enemy {occupant.ID} took {increasedDamage} damage. Remaining health: {enemyHealth}");

                                if (enemyHealth <= 0)
                                {
                                   

                                    // Send enemy death message
                                    GameMessageHub.Instance.SendMessage(
                                        GameMessageType.EnemyDied,
                                        entity,
                                        occupant,
                                        0,
                                        null,
                                        null
                                    );

                                    Debug.LogInfo($"[Player] Enemy {occupant.ID} has been defeated.");
                                }
                                aoeAttackPerformed = true;

                                // Only deduct energy once even if multiple enemies are hit
                                break;
                            }
                        }

                        if (!aoeAttackPerformed)
                        {
                            Debug.LogWarning("[Player] No enemies in range for AOE attack.");
                        }
                    }
                }
            }
            // ----------------------[ END AOE ATTACK CODE ]----------------------

            // 2. If not currently moving, handle mouse click to start a new path
            bool isMouseClicked = InternalCalls.Input_IsMouseButtonDown(0); // Left mouse button
            if (isMouseClicked && !mouseClickedLastFrame)
            {
                // Mouse was clicked
                InternalCalls.Input_GetCursorPosition(out Vector2D mousePosition);
                var clickedTile = FindTileUnderMouse(mousePosition);

                if (clickedTile == null)
                {
                    Debug.LogWarning("[Player] Invalid tile clicked.");
                    mouseClickedLastFrame = true;
                    return;
                }

                // Start tile
                Vector2D startPosition = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(entity.ID, ref startPosition);
                var startTile = TileManager.Instance.GetTileAtPosition(startPosition);

                if (startTile == clickedTile)
                {
                    Debug.LogInfo("[Player] Player clicked on the same tile they are currently on.");
                    mouseClickedLastFrame = true;
                    return;
                }

                var occupant = TileManager.Instance.GetOccupant(TileManager.Instance.GetTileCenter(clickedTile));
                if (occupant != null)
                {
                    Debug.LogInfo("[Player] Tile {0} is occupied by entity {1}.", clickedTile.ID, occupant.ID);

                    if (UnitManager.Instance.IsEnemy(occupant))
                    {
                        bool attacked = false;

                        // Attack if adjacent
                        if (IsAdjacentToTile(startTile, clickedTile))
                        {
                            Debug.LogInfo("[Player] Enemy is adjacent. Attempting to attack.");
                            InternalCalls.AnimationComponent_Stop("PapaShroom_Jump");
                            AttackEnemy(entity, occupant);
                            attacked = true;

                            // Send attack message
                            Vector2D attackPosition = TileManager.Instance.GetTileCenter(clickedTile);
                            int attackPower = InternalCalls.UnitComponent_GetAttackPower(entity.ID);
                            GameMessageHub.Instance.SendMessage(
                                GameMessageType.PlayerAttacked,
                                entity,
                                occupant,
                                attackPower,
                                attackPosition,
                                "Melee"
                            );
                        }
                        if (rangedunlocked == true)
                        {
                            if (RangedAttack(startTile, clickedTile))
                            {
                                Debug.LogInfo("[Player] Enemy is within ranged attack distance (2-3 tiles away). Attempting ranged attack.");
                                InternalCalls.AnimationComponent_Stop("PapaShroom_Jump");
                                AttackEnemy(entity, occupant);
                                attacked = true;

                                // Send ranged attack message
                                Vector2D attackPosition = TileManager.Instance.GetTileCenter(clickedTile);
                                int attackPower = InternalCalls.UnitComponent_GetAttackPower(entity.ID);
                                GameMessageHub.Instance.SendMessage(
                                    GameMessageType.PlayerAttacked,
                                    entity,
                                    occupant,
                                    attackPower,
                                    attackPosition,
                                    "Ranged"
                                );
                            }
                        }
                        if (!attacked)
                        {
                            Debug.LogWarning("[Player] No valid attack was performed (either out of range or not in turn for ranged attack).");
                        }
                    }
                    else
                    {
                        Debug.LogWarning("[Player] Tile is occupied by another unit. Unable to move.");
                    }
                    mouseClickedLastFrame = true;
                    return;
                }

                if (!TileManager.Instance.IsTileWalkable(clickedTile))
                {
                    Debug.LogWarning("[Player] Tile is not walkable.");
                    mouseClickedLastFrame = true;
                    return;
                }

                // We have a valid pathable tile: Attempt to set up path & move
                MovePlayerToTile(entity, startTile, clickedTile);
                mouseClickedLastFrame = true;
            }
            else if (!isMouseClicked)
            {
                // Reset state when the mouse button is released
                mouseClickedLastFrame = false;
            }


            if (player != null && !player.isMoving)
            {
                // Get current mouse position
                InternalCalls.Input_GetCursorPosition(out Vector2D mousePosition);
                var hoveredTile = FindTileUnderMouse(mousePosition);

                // Reset estimated cost when not hovering any tile
                if (hoveredTile == null || !InternalCalls.TileComponent_GetIsHighlighted(hoveredTile.ID))
                {
                    if (player._estimatedEnergyCost != 0)
                    {
                        player._estimatedEnergyCost = 0;
                        player._energyBarComponent.UpdateEnergyBar(player._estimatedEnergyCost);
                        player._previewPath.Clear();
                    }
                    player._hoveredTile = null;
                }
                else if (hoveredTile != player._hoveredTile && hoveredTile.ID != 0)
                {
                    player._hoveredTile = hoveredTile;

                    // Get player's current position and tile
                    Vector2D startPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(entity.ID, ref startPosition);
                    var startTile = TileManager.Instance.GetTileAtPosition(startPosition);

                    // Don't show preview for the tile we're already on
                    if (startTile != hoveredTile)
                    {
                        // Check if the tile is walkable and not occupied by another entity
                        bool canMoveTo = TileManager.Instance.IsTileWalkable(hoveredTile);

                        // Check if tile is occupied by a non-player entity
                        var occupant = TileManager.Instance.GetOccupant(TileManager.Instance.GetTileCenter(hoveredTile));
                        if (occupant != null && occupant.ID != entity.ID)
                        {
                            canMoveTo = false;
                        }

                        if (canMoveTo)
                        {
                            // Calculate path and energy cost
                            var path = TileManager.Instance.ExecuteAStarPathfinding(startTile, hoveredTile);
                            player._previewPath = path;

                            if (path.Count > 0)
                            {
                                int energyCost = path.Count - 1;
                                player._estimatedEnergyCost = energyCost;

                                // Update energy bar to show preview (pass the cost as a preview value)
                                player._energyBarComponent.UpdateEnergyBar(energyCost);

                                // Optionally send a message for UI feedback
                                GameMessageHub.Instance.SendMessage(
                                    GameMessageType.PathCheck,
                                    entity,
                                    hoveredTile,
                                    energyCost,
                                    mousePosition,
                                    "MovePreview"
                                );
                            }
                        }
                        else
                        {
                            player._estimatedEnergyCost = 0;
                            player._previewPath.Clear();
                            player._energyBarComponent.UpdateEnergyBar(0);
                        }
                    }
                    else
                    {
                        player._estimatedEnergyCost = 0;
                        player._previewPath.Clear();
                        player._energyBarComponent.UpdateEnergyBar(0);
                    }
                }
            }


            player._healthBarComponent.UpdateHealthBar();
            if (!player.isMoving && player._estimatedEnergyCost == 0)
            {
                // Only update with no preview if we're not showing a preview already
                player._energyBarComponent.UpdateEnergyBar();
            }

            // Idle animation
            if (!InternalCalls.AnimationComponent_CheckEntityPlaying(entity.ID) && Idle == false)
            {
                Idle = true;
                InternalCalls.AnimationComponent_PlayAnimation(entity.ID, "PapaShroom Idle");
            }
            else if (InternalCalls.AnimationComponent_CheckEntityPlaying(entity.ID) && Idle == true)
            {
                Idle = false;
                InternalCalls.AnimationComponent_Stop("PapaShroom Idle");
            }
        }

        /**
         * @brief Cleans up the Player entity when the game ends.
         *
         * This method resets the player's stats and movement state, ensuring a clean exit.
         * It also unregisters the player from the message system.
         *
         * @param entity The player entity being cleaned up.
         */
        private static void End(Entity entity)
        {
            var playerBehaviour = GameLogicSystem.Instance.GetBehaviour<Player>(entity);
            if (playerBehaviour == null)
            {
                Debug.LogWarning("[PlayerBehaviour] PlayerBehaviour is null for entity {0}. Skipping End.", entity.ID);
                return;
            }

            //// Send player turn ended message
            //GameMessageHub.Instance.SendMessage(
            //    GameMessageType.PlayerTurnEnded,
            //    entity,
            //    null,
            //    UnitManager.Instance.GetPlayerTurnCount(),
            //    null,
            //    null
            //);

            // Clean up resources
            if (playerBehaviour._unit != null)
            {
                playerBehaviour._unit.ResetStats();
                Debug.LogInfo("[PlayerBehaviour] Reset Unit stats for entity {0}.", entity.ID);
            }

            // Unregister from message system to prevent memory leaks
            playerBehaviour.UnregisterFromMessages();
            Instance = null;

            mouseClickedLastFrame = false;



            Debug.LogInfo("[PlayerBehaviour] PlayerBehaviour cleanup complete for entity {0}.", entity.ID);
        }

        // -----------------------[ MOVEMENT LOGIC ]-------------------------

        // -----------------------[ MOVEMENT LOGIC ]-------------------------

        /**
         * @brief Initiates movement along a path using A* pathfinding.
         *
         * This method determines if the player has enough energy to move to the selected tile,
         * sets up the path, and starts the movement process.
         *
         * @param playerEntity The player entity initiating movement.
         * @param startTile The tile where the player is currently located.
         * @param clickedTile The target tile the player wants to move to.
         */
        private static void MovePlayerToTile(Entity playerEntity, Entity startTile, Entity clickedTile)
        {
            // [TUTORIAL RESTRICTION] If tutorial restricts movement, only allow moving to the designated tile
            if (IsMovementRestrictedByTutorial)
            {
                Vector2D clickedTilePos = TileManager.Instance.GetTileCenter(clickedTile);
                if (TutorialTargetTilePosition == Vector2D.Zero || !clickedTilePos.Equals(TutorialTargetTilePosition))
                {
                    Debug.LogWarning("[Player] You cannot move to this tile during the tutorial step.");
                    return;
                }
            }

            var path = TileManager.Instance.ExecuteAStarPathfinding(startTile, clickedTile);
            
            hasCheckedEntities = false; // Reset before movement starts

            if (path.Count == 0)
            {
                Debug.LogWarning("[Player] No valid path found to tile {0}.", clickedTile.ID);
                return;
            }

            int currentEnergy = InternalCalls.UnitComponent_GetEnergy(playerEntity.ID);
            int neededEnergy = path.Count - 1; // # of moves from start to end
            if (neededEnergy > currentEnergy)
            {
                Debug.LogWarning("[Player] Not enough energy to move to tile {0}. Required: {1}, Available: {2}",
                                 clickedTile.ID, neededEnergy, currentEnergy);
                return;
            }

            // We have enough energy: store the path in the player's behaviour
            var player = GameLogicSystem.Instance.GetBehaviour<Player>(playerEntity);
            if (player == null)
            {
                Debug.LogError("[Player] Player behaviour not found for entity {0}.", playerEntity.ID);
                return;
            }

            player._energyBarComponent.UpdateEnergyBar();


            // 1. Make the starting tile walkable & occupant = 0 (we're leaving it)
            InternalCalls.TileComponent_SetOccupant(startTile.ID, 0);
            InternalCalls.TileComponent_SetIsWalkable(startTile.ID, true);
            InternalCalls.TileComponent_SetOccupant(clickedTile.ID, playerEntity.ID);
            InternalCalls.TileComponent_SetIsWalkable(clickedTile.ID, false);

            // 2. Play animation for player movement
            InternalCalls.AnimationComponent_PlayAnimation(playerEntity.ID, "PapaShroom_Jump");

            // 3. Store path to traverse, begin moving from path[0] to path[1]
            player.movementPath = path;
            player.currentPathIndex = 1; // next tile to move toward
            player.isMoving = true;

            // Get the target position for the movement message
            Vector2D targetPosition = TileManager.Instance.GetTileCenter(clickedTile);

            // Broadcast movement start message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.PlayerMoved,
                playerEntity,
                null,
                neededEnergy, // Include the energy cost
                targetPosition,
                path.Count    // Include the path length in the data field
            );

            Debug.LogInfo("[Player] Movement setup from tile {0} to {1} steps, total {2} tiles.",
                          startTile.ID, clickedTile.ID, path.Count);
        }

        /**
         * @brief Handles incremental movement along the pre-determined path.
         *
         * Moves the player toward the next tile in the movement path, ensuring smooth animation.
         * If the player reaches the target tile, the energy is decremented.
         *
         * @param playerEntity The player entity being moved.
         * @param dt The delta time since the last frame.
         * @param player The player's behavior instance.
         */
        private static void DoIncrementalMove(Entity playerEntity, float dt, Player player)
        {
            // 1. If we still need to wait (i.e., we just arrived at a tile),
            //    decrement the timer and skip movement until it's done
            if (player.waitTimer > 0.0f)
            {
                player.waitTimer -= dt;
                return; // No movement this frame
            }

            // If we've used up the path, stop moving
            if (player.currentPathIndex >= player.movementPath.Count)
            {
                player.isMoving = false;
                return;
            }

            // Identify the tile we are heading toward
            var targetTile = player.movementPath[player.currentPathIndex];
            if (targetTile == null || targetTile.ID == 0)
            {
                player.isMoving = false;
                return;
            }

            // 1. Get the target tile center
            Vector2D targetPos = TileManager.Instance.GetTileCenter(targetTile);

            // 2. Get current position
            Vector2D currentPos = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref currentPos);

            // 3. Compute direction & distance
            Vector2D direction = targetPos - currentPos;
            float distance = direction.Length;
            if (distance < 0.001f)
            {
                // Already close to this tile
                ArriveAtTile(playerEntity, targetTile, player);
                return;
            }
            direction = direction.Normalized;

            // 4. Move a partial step this frame
            float step = player.moveSpeed * dt;
            if (step < distance)
            {
                // Move partway
                Vector2D newPos = currentPos + direction * step;
                InternalCalls.UnitComponent_SetPosition(playerEntity.ID, ref newPos);
                InternalCalls.Transform_SetTranslation(playerEntity.ID, ref newPos);
            }
            else
            {
                // We'll overshoot, so just clamp directly to target
                InternalCalls.UnitComponent_SetPosition(playerEntity.ID, ref targetPos);
                InternalCalls.Transform_SetTranslation(playerEntity.ID, ref targetPos);

                // Mark that we arrived
                ArriveAtTile(playerEntity, targetTile, player);
                player.waitTimer = player.timeBetweenSteps;
            }
        }

        /**
         * @brief Handles logic when the player arrives at a tile.
         *
         * This method marks the tile as occupied, decrements the player's energy,
         * and progresses the player to the next tile in the movement path.
         *
         * @param playerEntity The player entity that arrived at the tile.
         * @param tileEntity The tile the player arrived at.
         * @param player The player's behavior instance.
         */
        private static void ArriveAtTile(Entity playerEntity, Entity tileEntity, Player player)
        {
            // Get current position for the message
            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);

            // Decrement energy
            int currentEnergy = InternalCalls.UnitComponent_GetEnergy(playerEntity.ID);
            int previousEnergy = currentEnergy;
            currentEnergy--;
            InternalCalls.UnitComponent_SetEnergy(playerEntity.ID, currentEnergy);

            // Update energy bar
            player._energyBarComponent.UpdateEnergyBar();

            // Send energy change message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.PlayerEnergyChanged,
                playerEntity,
                null,
                currentEnergy,
                playerPosition,
                previousEnergy  // Store old energy value for reference if needed
            );

            // Advance to next tile in the path
            player.currentPathIndex++;

            // If we've arrived at the final tile in the path, stop
            if (player.currentPathIndex >= player.movementPath.Count)
            {
                Debug.LogInfo("[Player] Finished moving along path.");
                player.isMoving = false;
                InternalCalls.AnimationComponent_Stop("PapaShroom_Jump");

                // Send final movement completed message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PlayerMoved,
                    playerEntity,
                    tileEntity,  // Include final tile as target
                    0,           // No more energy cost
                    playerPosition,
                    "MovementComplete"  // Signal this is the end of movement
                );
            }
            else
            {
                // Send intermediate movement update message
                int remainingTiles = player.movementPath.Count - player.currentPathIndex;
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PlayerMoved,
                    playerEntity,
                    tileEntity,  // Current tile as target
                    remainingTiles,  // Remaining steps
                    playerPosition,
                    "MovementProgress"  // Signal this is an intermediate step
                );
            }
        }

        // -----------------------[ HELPER METHODS ]-------------------------

        // -----------------------[ HELPER METHODS ]-------------------------

        /**
         * @brief Finds the tile entity under the given mouse position.
         *
         * Iterates through all tiles and determines if the mouse position is within the tile's boundaries.
         * Also sends messages about tile interactions through the observer pattern.
         *
         * @param mousePosition The current position of the mouse cursor.
         * @return The tile entity under the mouse, or null if no tile is found.
         */
        private static Entity FindTileUnderMouse(Vector2D mousePosition)
        {
            Entity foundTile = null;

            foreach (var tileEntity in TileManager.Instance.GetAllTiles())
            {
                if (tileEntity == null || tileEntity.ID == 0)
                {
                    Debug.LogWarning("[FindTileUnderMouse] Skipping invalid tile entity.");
                    continue;
                }

                Vector2D tileCenter = TileManager.Instance.GetTileCenter(tileEntity);
                float tileSize = 110.0f; // Adjust if your tile size is different

                if (mousePosition.X >= tileCenter.X - tileSize / 2 &&
                    mousePosition.X <= tileCenter.X + tileSize / 2 &&
                    mousePosition.Y >= tileCenter.Y - tileSize / 2 &&
                    mousePosition.Y <= tileCenter.Y + tileSize / 2)
                {
                    //Debug.LogInfo("[FindTileUnderMouse] Found tile {0} under mouse position.", tileEntity.ID);
                    foundTile = tileEntity;

                    // Send tile interaction message
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.TileInteraction,
                        null, // No specific source entity for mouse interactions
                        foundTile,
                        0,
                        mousePosition,
                        "MouseHover"
                    );

                    break;
                }   
            }

            if (foundTile == null)
            {
                //Debug.LogWarning("[FindTileUnderMouse] No tile found under mouse position.");

                //Send message for mouse interaction with no tile

               GameMessageHub.Instance.SendMessage(
                   GameMessageType.TileInteraction,
                   null,
                   null,
                   0,
                   mousePosition,
                   "NoTileFound"
               );
            }

            return foundTile;
        }

        // -----------------------[ ATTACK LOGIC ]-------------------------
        /**
         * @brief Performs a melee attack on an enemy.
         *
         * Reduces the enemy's health and plays attack animations. If the enemy's health reaches zero,
         * a defeat animation is played. Sends appropriate messages through the observer pattern.
         *      
         * @param playerEntity The player entity performing the attack.
         * @param enemyEntity The enemy entity being attacked.
         */
        private static void AttackEnemy(Entity playerEntity, Entity enemyEntity)
        {
            bool enemyAlive = InternalCalls.UnitComponent_GetIsAlive(enemyEntity.ID);

            if (enemyAlive == true)
            {
                Debug.LogInfo("[Player] Player {0} is attacking Enemy {1}.", playerEntity.ID, enemyEntity.ID);

                int currentEnergy = InternalCalls.UnitComponent_GetEnergy(playerEntity.ID);
                if (currentEnergy > 0)
                {
                    // Store the previous energy for message
                    int previousEnergy = currentEnergy;

                    // Reduce energy
                    currentEnergy--;
                    InternalCalls.UnitComponent_SetEnergy(playerEntity.ID, currentEnergy);

                    // Send energy change message
                    Vector2D playerPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.PlayerEnergyChanged,
                        playerEntity,
                        null,
                        currentEnergy,
                        playerPosition,
                        previousEnergy
                    );

                    // Calculate damage
                    int attackPower = InternalCalls.UnitComponent_GetAttackPower(playerEntity.ID);
                    int enemyHealth = InternalCalls.UnitComponent_GetHealth(enemyEntity.ID);
                    int previousHealth = enemyHealth;
                    enemyHealth -= attackPower;
                    InternalCalls.UnitComponent_SetHealth(enemyEntity.ID, enemyHealth);

                    // Send attack message
                    Vector2D enemyPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(enemyEntity.ID, ref enemyPosition);

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.PlayerAttacked,
                        playerEntity,
                        enemyEntity,
                        attackPower,
                        enemyPosition,
                        "MeleeAttack"
                    );

                    // Also send enemy health changed message
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.EnemyHealthChanged,
                        playerEntity,
                        enemyEntity,
                        enemyHealth,
                        enemyPosition,
                        previousHealth
                    );

                    // Play enemy attack animation
                    InternalCalls.AnimationComponent_PlayAnimation(playerEntity.ID, "Player Attack 2");
                    InternalCalls.AnimationComponent_PlayAnimation(0, "Player Attack");
                    Debug.LogInfo("[Player] Enemy {0} took {1} damage. Remaining health: {2}", enemyEntity.ID, attackPower, enemyHealth);

                    // Play a different attack sound each time by cycling through the array.
                    InternalCalls.Audio_PlaySound(attackSoundNames[attackSoundIndex], 0.2f);
                    attackSoundIndex = (attackSoundIndex + 1) % attackSoundNames.Length;

                    if (enemyHealth <= 0)
                    {

                        if (!TutorialManager.CanEnemyDie())
                        {
                            Debug.LogInfo("[Player] Enemy cannot be killed outside of Scene 14. Clamping HP to 1.");
                            enemyHealth = 1;
                            InternalCalls.UnitComponent_SetHealth(enemyEntity.ID, enemyHealth);

                            // Also send enemy health changed message for feedback
                            GameMessageHub.Instance.SendMessage(
                                GameMessageType.EnemyHealthChanged,
                                playerEntity,
                                enemyEntity,
                                enemyHealth,
                                enemyPosition,
                                previousHealth
                            );

                            return; // Stop here, don't kill
                        }

                        // Send enemy death message
                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.EnemyDied,
                            playerEntity,
                            enemyEntity,
                            0,
                            enemyPosition,
                            "KilledByPlayer"
                        );

                        // Get the tile the enemy was on to mark it as available
                        Entity enemyTile = TileManager.Instance.GetTileAtPosition(enemyPosition);
                        if (enemyTile != null)
                        {
                            // Send tile state change message
                            GameMessageHub.Instance.SendMessage(
                                GameMessageType.TileStateChanged,
                                null,
                                enemyTile,
                                0,
                                enemyPosition,
                                "Cleared"
                            );
                        }
                    }
                }
                else
                {
                    Debug.LogWarning("[Player] Not enough energy to attack.");

                    // Send insufficient energy message
                    Vector2D playerPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.PlayerActionFailed,
                        playerEntity,
                        enemyEntity,
                        0,
                        playerPosition,
                        "InsufficientEnergy"
                    );
                }
            }
        }

        // -----------------------[ PHYSICAL ATTACK  ]-------------------------
        /**
         * @brief Checks if two tiles are adjacent to each other.
         *
         * This method determines if two tiles are adjacent based on the grid size (110x110).
         * Can also send an optional message about the adjacency check.
         *
         * @param tileA The first tile entity to check.
         * @param tileB The second tile entity to check.
         * @param sendMessage Whether to send a message about the adjacency check.
         * @return True if the tiles are adjacent, false otherwise.
         */
        public static bool IsAdjacentToTile(Entity tileA, Entity tileB, bool sendMessage = false)
        {
            if (tileA == null || tileB == null)
                return false;

            Vector2D positionA = TileManager.Instance.GetTileCenter(tileA);
            Vector2D positionB = TileManager.Instance.GetTileCenter(tileB);

            bool isAdjacent = (Math.Abs(positionA.X - positionB.X) == 110 && positionA.Y == positionB.Y) ||
                              (Math.Abs(positionA.Y - positionB.Y) == 110 && positionA.X == positionB.X);

            // If requested, send a message about this adjacency check
            if (sendMessage)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.TileInteraction,
                    null,
                    tileB,  // Target tile
                    isAdjacent ? 1 : 0,  // 1 for adjacent, 0 for not
                    positionB,
                    isAdjacent ? "TilesAdjacent" : "TilesNotAdjacent"
                );
            }

            return isAdjacent;
        }


        // -----------------------[ RANGED ATTACK LOGIC ]-------------------------

        /**
         * @brief Determines if an enemy is within the player's ranged attack distance.
         *
         * Checks if the enemy is exactly 2 or 3 tiles away from the player.
         * Can also send a message about the range check via the messaging system.
         *
         * @param playerTile The tile the player is currently on.
         * @param enemyTile The tile the enemy is on.
         * @param sendMessage Whether to send a message about the range check.
         * @return True if the enemy is in range, false otherwise.
         */
        public static bool IsInRangedDistance(Entity playerTile, Entity enemyTile, bool sendMessage = false)
        {
            if (playerTile == null || enemyTile == null)
                return false;

            Vector2D positionA = TileManager.Instance.GetTileCenter(playerTile);
            Vector2D positionB = TileManager.Instance.GetTileCenter(enemyTile);

            float tileSize = 110.0f;
            float xDist = Math.Abs(positionA.X - positionB.X);
            float yDist = Math.Abs(positionA.Y - positionB.Y);

            // A small tolerance to account for floating-point rounding
            float epsilon = 1.0f;

            bool twoTilesAway = (Math.Abs(xDist - 2 * tileSize) <= epsilon && yDist <= epsilon)
                             || (Math.Abs(yDist - 2 * tileSize) <= epsilon && xDist <= epsilon);

            bool threeTilesAway = (Math.Abs(xDist - 3 * tileSize) <= epsilon && yDist <= epsilon)
                               || (Math.Abs(yDist - 3 * tileSize) <= epsilon && xDist <= epsilon);

            bool inRange = twoTilesAway || threeTilesAway;

            // Send range check message if requested
            if (sendMessage)
            {
                string rangeStatus = inRange ?
                    (twoTilesAway ? "TwoTilesAway" : "ThreeTilesAway") :
                    "OutOfRange";

                GameMessageHub.Instance.SendMessage(
                    GameMessageType.RangeCheck,
                    null,
                    enemyTile,
                    inRange ? 1 : 0,
                    positionB,
                    rangeStatus
                );
            }

            return inRange;
        }

        /**
         * @brief Checks if a ranged attack can be executed by verifying path clearance.
         *
         * Ensures that no obstacles are present between the player and the target.
         * Can also send messages about path checking status.
         *
         * @param startTile The tile the player is attacking from.
         * @param endTile The tile the enemy is located on.
         * @param sendMessage Whether to send messages about the path check.
         * @return True if the attack path is clear, false otherwise.
         */
        public static bool IsPathClearForOverlay(Entity startTile, Entity endTile, bool sendMessage = false)
        {
            if (startTile == null || endTile == null)
                return false;

            // Convert both tiles to their center positions
            Vector2D startPos = TileManager.Instance.GetTileCenter(startTile);
            Vector2D endPos = TileManager.Instance.GetTileCenter(endTile);

            float tileSize = 110.0f;
            // The step direction indicates one "tile" movement each iteration
            Vector2D direction = new Vector2D(
                Math.Sign(endPos.X - startPos.X) * tileSize,
                Math.Sign(endPos.Y - startPos.Y) * tileSize
            );

            // Begin checking from the first step away from startPos
            Vector2D currentPosition = startPos + direction;

            // Continue until we've reached or passed the end tile
            while (!IsSameTile(currentPosition, endPos))
            {
                Entity tile = TileManager.Instance.GetTileAtPosition(currentPosition);
                if (tile == null || !TileManager.Instance.IsTileWalkable(tile))
                {
                    Debug.LogWarning($"[IsPathClearForOverlay] Blocked at ({currentPosition.X}, {currentPosition.Y}).");

                    if (sendMessage)
                    {
                        // Send message about path obstruction
                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.PathCheck,
                            startTile,
                            endTile,
                            0,
                            currentPosition,
                            "PathBlocked"
                        );
                    }

                    return false;
                }
                currentPosition += direction;
            }

            if (sendMessage)
            {
                // Send message that path is clear
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PathCheck,
                    startTile,
                    endTile,
                    1,
                    endPos,
                    "PathClear"
                );
            }

            return true;
        }

        /**
         * @brief Attempts to perform a ranged attack from player to target.
         *
         * Checks range, cooldowns, and path clearance before allowing a ranged attack.
         * Sends appropriate messages through the observer pattern about attack status.
         *
         * @param player The player entity performing the attack.
         * @param target The target entity being attacked.
         * @return True if the attack can be performed, false otherwise.
         */
        private static bool RangedAttack(Entity player, Entity target)
        {
            if (player == null || target == null)
                return false;

            int currentTurn = UnitManager.Instance.GetPlayerTurnCount();
            int playerID = (int)player.ID;
            Vector2D playerPos = TileManager.Instance.GetTileCenter(player);
            Vector2D targetPos = TileManager.Instance.GetTileCenter(target);

            // Check if attack is on cooldown
            int turnsUntilAvailable = 0;
            if (lastRangedAttackTurn.TryGetValue(playerID, out int lastTurnUsed))
            {
                turnsUntilAvailable = 3 - (currentTurn - lastTurnUsed);
                if (turnsUntilAvailable > 0)
                {
                    Debug.LogWarning($"[RangedAttack] Ranged attack on cooldown. Available in {turnsUntilAvailable} turns.");

                    // Send cooldown message
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.PlayerActionFailed,
                        player,
                        target,
                        turnsUntilAvailable,
                        playerPos,
                        "RangedAttackOnCooldown"
                    );

                    return false;
                }
            }

            Debug.LogInfo($"[RangedAttack] Checking range: Player({playerPos.X},{playerPos.Y}) Enemy({targetPos.X},{targetPos.Y})");

            // Check if target is in range (2-3 tiles away)
            bool inRange = IsInRangedDistance(player, target, true);

            if (!inRange)
            {
                // Send out of range message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PlayerActionFailed,
                    player,
                    target,
                    0,
                    targetPos,
                    "TargetOutOfRange"
                );

                return false;
            }

            // Check if path is clear
            bool pathClear = IsPathWalkable(playerPos, targetPos);

            if (!pathClear)
            {
                // Send blocked path message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PlayerActionFailed,
                    player,
                    target,
                    0,
                    targetPos,
                    "PathBlocked"
                );

                return false;
            }

            // Attack is valid - update cooldown and return success
            lastRangedAttackTurn[playerID] = currentTurn;

            Debug.LogInfo("[RangedAttack] Attack successful! Path is clear.");

            // Send ranged attack started message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.RangedAttackStarted,
                player,
                target,
                3, // Cooldown duration
                targetPos,
                currentTurn // Current turn number for reference
            );

            return true;
        }

        /**
         * @brief Checks if a path between two positions is walkable.
         *
         * Iterates along a straight line between the positions to verify no obstacles exist.
         * Can send messages about path status.
         *
         * @param start The starting position.
         * @param end The ending position.
         * @param sendMessage Whether to send messages about the path check.
         * @return True if the path is walkable, false otherwise.
         */
        private static bool IsPathWalkable(Vector2D start, Vector2D end, bool sendMessage = false)
        {
            float tileSize = 110.0f;

            Vector2D direction = new Vector2D(
                Math.Sign(end.X - start.X) * tileSize,
                Math.Sign(end.Y - start.Y) * tileSize
            );

            Vector2D currentPosition = start + direction; // Start checking from the first step

            while (!IsSameTile(currentPosition, end))
            {
                Entity tile = TileManager.Instance.GetTileAtPosition(currentPosition);

                if (tile == null || !TileManager.Instance.IsTileWalkable(tile))
                {
                    Debug.LogWarning($"[IsPathWalkable] Blocked at ({currentPosition.X}, {currentPosition.Y}).");

                    if (sendMessage)
                    {
                        // Send message about path obstruction
                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.PathCheck,
                            null,
                            tile,  // The blocking tile
                            0,
                            currentPosition,
                            "PathBlocked"
                        );
                    }

                    return false;
                }

                currentPosition += direction; // Move to the next tile in the direction
            }

            if (sendMessage)
            {
                // Send message that path is clear
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PathCheck,
                    null,
                    null,
                    1,
                    end,
                    "PathClear"
                );
            }

            return true;
        }

        /**
         * @brief Determines if two positions correspond to the same tile.
         *
         * Used for comparing positions to see if they're referring to the same game tile.
         *
         * @param posA The first position.
         * @param posB The second position.
         * @return True if the positions are on the same tile, false otherwise.
         */
        private static bool IsSameTile(Vector2D posA, Vector2D posB)
        {
            float tileSize = 110.0f;
            return Math.Abs(posA.X - posB.X) < tileSize / 2 && Math.Abs(posA.Y - posB.Y) < tileSize / 2;
        }

        // -----------------------[ AOE ATTACK LOGIC ]-------------------------
        /**
         * @brief Gets all surrounding tiles around a center tile.
         * 
         * Returns a list of tiles that surround the given center tile,
         * including diagonal tiles. Also sends messages about the discovered tiles.
         * 
         * @param centerTile The center tile to find surroundings for.
         * @param sendMessage Whether to send messages about the surrounding tiles.
         * @return A list of surrounding tiles.
         */
        private static List<Entity> GetSurroundingTiles(Entity centerTile, bool sendMessage = false)
        {
            List<Entity> surroundingTiles = new List<Entity>();

            if (centerTile == null || centerTile.ID == 0)
            {
                Debug.LogWarning("[GetSurroundingTiles] Invalid center tile.");
                return surroundingTiles;
            }

            Vector2D centerPosition = TileManager.Instance.GetTileCenter(centerTile);
            float tileSize = 110.0f; // Adjust based on actual game grid size

            // Offsets for surrounding tiles (left, right, up, down, and diagonals)
            (Vector2D offset, string direction)[] offsets = new (Vector2D, string)[]
            {
        (new Vector2D(-tileSize, -tileSize), "TopLeft"),     // Top-left
        (new Vector2D(0, -tileSize),         "Top"),         // Top-center
        (new Vector2D(tileSize, -tileSize),  "TopRight"),    // Top-right
        (new Vector2D(-tileSize, 0),         "Left"),        // Left
        (new Vector2D(tileSize, 0),          "Right"),       // Right
        (new Vector2D(-tileSize, tileSize),  "BottomLeft"),  // Bottom-left
        (new Vector2D(0, tileSize),          "Bottom"),      // Bottom-center
        (new Vector2D(tileSize, tileSize),   "BottomRight")  // Bottom-right
            };

            foreach (var (offset, direction) in offsets)
            {
                Vector2D checkPosition = centerPosition + offset;
                Entity tile = TileManager.Instance.GetTileAtPosition(checkPosition);

                if (tile != null && tile.ID != 0)
                {
                    surroundingTiles.Add(tile);

                    if (sendMessage)
                    {
                        // Check if the tile has an occupant
                        Entity occupant = TileManager.Instance.GetOccupant(TileManager.Instance.GetTileCenter(tile));
                        bool isOccupied = (occupant != null && occupant.ID != 0);
                        bool isEnemy = (isOccupied && UnitManager.Instance.IsEnemy(occupant));

                        // Send message about this surrounding tile
                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.TileScanned,
                            centerTile,
                            tile,
                            isOccupied ? 1 : 0,  // Value indicates if occupied
                            checkPosition,
                            new TileScanData  // Custom data class to hold scan info
                            {
                                Direction = direction,
                                IsOccupied = isOccupied,
                                IsEnemy = isEnemy,
                                Occupant = occupant
                            }
                        );
                    }
                }
            }

            Debug.LogInfo($"[GetSurroundingTiles] Found {surroundingTiles.Count} surrounding tiles.");
            return surroundingTiles;
        }

        /**
         * @brief Data container for tile scan information.
         * 
         * Holds detailed information about a scanned tile, used in messages.
         */
        private class TileScanData
        {
            public string Direction { get; set; }
            public bool IsOccupied { get; set; }
            public bool IsEnemy { get; set; }
            public Entity Occupant { get; set; }
        }

        // -----------------------[ ENTITY CHECKING LOGIC ]-------------------------
        /**
         * @brief Scans surrounding tiles for entities and logs their presence.
         *
         * This function checks adjacent tiles for enemies, obstacles, or empty spaces.
         * Also sends messages about discovered entities and tiles.
         *
         * @param playerEntity The player entity whose surroundings are being checked.
         * @param sendMessages Whether to send messages about the findings.
         */
        private static void CheckEntitiesAroundPlayer(Entity playerEntity, bool sendMessages = true)
        {
            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);

            Entity playerTile = TileManager.Instance.GetTileAtPosition(playerPosition);
            if (playerTile == null)
            {
                Debug.LogWarning("[CheckEntitiesAroundPlayer] Player is not on a valid tile.");
                return;
            }

            Vector2D centerPosition = TileManager.Instance.GetTileCenter(playerTile);
            float tileSize = 110.0f; // Adjust based on your grid size

            // Offsets and direction labels
            (Vector2D offset, string direction)[] offsets = new (Vector2D, string)[]
            {
        (new Vector2D(0, -tileSize), "Up"),     // Up
        (new Vector2D(-tileSize, 0), "Left"),   // Left
        (new Vector2D(tileSize, 0), "Right"),   // Right
        (new Vector2D(0, tileSize), "Down")     // Down
            };

            Debug.LogInfo("[CheckEntitiesAroundPlayer] Checking for entities in adjacent tiles...");

            // Send scan started message
            if (sendMessages)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnvironmentScanStarted,
                    playerEntity,
                    playerTile,
                    0,
                    playerPosition,
                    null
                );
            }

            // Track what we find for the summary message
            int entitiesFound = 0;
            int enemiesFound = 0;
            List<Entity> foundEntities = new List<Entity>();

            foreach (var (offset, direction) in offsets)
            {
                Vector2D checkPosition = centerPosition + offset;
                Entity tile = TileManager.Instance.GetTileAtPosition(checkPosition);

                if (tile != null && tile.ID != 0)
                {
                    Entity occupant = TileManager.Instance.GetOccupant(TileManager.Instance.GetTileCenter(tile));
                    if (occupant != null && occupant.ID != 0)
                    {
                        foundEntities.Add(occupant);
                        entitiesFound++;

                        // Check if this entity is an enemy
                        bool isEnemy = UnitManager.Instance.IsEnemy(occupant);
                        if (isEnemy)
                        {
                            enemiesFound++;
                        }

                        Debug.LogInfo($"[Nearby Entity] Entity {occupant.ID} detected {direction} on tile {tile.ID} at position ({checkPosition.X}, {checkPosition.Y}).");

                        // Send entity detected message
                        if (sendMessages)
                        {
                            GameMessageHub.Instance.SendMessage(
                                GameMessageType.EntityDetected,
                                playerEntity,
                                occupant,
                                isEnemy ? 1 : 0,  // Value 1 indicates enemy, 0 indicates non-enemy
                                checkPosition,
                                new EntityDetectionData
                                {
                                    Direction = direction,
                                    IsEnemy = isEnemy,
                                    Distance = Vector2D.Distance(playerPosition, checkPosition),
                                    Tile = tile
                                }
                            );
                        }
                    }
                    else
                    {
                        Debug.LogInfo($"[Nearby Tile] No entity detected {direction}. Tile {tile.ID} at ({checkPosition.X}, {checkPosition.Y}) is empty.");

                        // Send empty tile detected message
                        if (sendMessages)
                        {
                            GameMessageHub.Instance.SendMessage(
                                GameMessageType.EmptyTileDetected,
                                playerEntity,
                                tile,
                                0,
                                checkPosition,
                                direction
                            );
                        }
                    }
                }
                else
                {
                    Debug.LogWarning($"[Nearby Tile] No tile found {direction} at ({checkPosition.X}, {checkPosition.Y}).");

                    // Send missing tile message
                    if (sendMessages)
                    {
                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.NoTileDetected,
                            playerEntity,
                            null,
                            0,
                            checkPosition,
                            direction
                        );
                    }
                }
            }

            // Send scan completed message with summary
            if (sendMessages)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnvironmentScanCompleted,
                    playerEntity,
                    null,
                    entitiesFound,
                    playerPosition,
                    new ScanSummaryData
                    {
                        EntitiesFound = entitiesFound,
                        EnemiesFound = enemiesFound,
                        TilesScanned = offsets.Length,
                        FoundEntities = foundEntities.ToArray()
                    }
                );
            }
        }

        // -----------------------[ TUTORIAL SPECIFIC LOGIC ]-------------------------
        public static void EnableMovementRestriction(Vector2D targetTilePos)
        {
            IsMovementRestrictedByTutorial = true;
            TutorialTargetTilePosition = targetTilePos;
            Debug.LogInfo($"[Player] Movement restricted to target tile at ({targetTilePos.X}, {targetTilePos.Y})");
        }

        public static void DisableMovementRestriction()
        {
            IsMovementRestrictedByTutorial = false;
            TutorialTargetTilePosition = Vector2D.Zero;
            Debug.LogInfo("[Player] Tutorial movement restriction disabled.");
        }

        /**
         * @brief Data container for entity detection information.
         * 
         * Holds detailed information about detected entities.
         */
        private class EntityDetectionData
        {
            public string Direction { get; set; }
            public bool IsEnemy { get; set; }
            public float Distance { get; set; }
            public Entity Tile { get; set; }
        }

        /**
         * @brief Data container for environment scan summary.
         * 
         * Provides summary statistics about a completed environment scan.
         */
        private class ScanSummaryData
        {
            public int EntitiesFound { get; set; }
            public int EnemiesFound { get; set; }
            public int TilesScanned { get; set; }
            public Entity[] FoundEntities { get; set; }
        }

    }
}