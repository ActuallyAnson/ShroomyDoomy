/******************************************************************************
/*!
\file  EnemyBehaviour.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    Oct 03, 2024
\brief  This file implements the Enemy class, a behaviour component that controls
       enemy units in the game. The Enemy class handles movement, pathfinding,
       attacking, and UI updates for enemy health and energy bars.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;

namespace FruitLoops
{

    enum Animation
    {
        Idle,
        Move,
        Attack,
        Death,
        IdleFlip,
        MoveFlip,
        AttackFlip,
        DeathFlip
    }
    /**
     * @brief Represents an enemy unit in the game.
     *
     * The Enemy class extends BehaviourComponent and manages the enemy's unit stats,
     * path movement, attack logic, and UI components (health and energy bars).
     */
    public class Enemy : BehaviourComponent, IGameObserver
    {
        // ----------------------[ EXISTING FIELDS ]-------------------------
        private readonly Unit _unit;
        private HealthBarComponent _enemyHealthBarComponent;
        private EnergyBarComponent _enemyEnergyBarComponent;
        private Entity _attackPowerEntity;
        private Entity _rangeEntity;

        private long lastJumpedTileID = -1; // Each enemy tracks its own last jumped tile
        private bool hasPlayedJumpSound;
        private List<string> animationNames = new List<string>();
        private Vector2D _lastReportedPosition;
        private bool _lastReportedIsMoving;
        private bool _lastReportedIsAlive;
        private int _lastReportedEnergy;
        private int _lastReportedHealth;

        private Vector2D _lastReportedPlayerPosition;
        private Vector2D _lastReportedEnemyPosition;
        private bool _lastReportedFacingDirection;

        private bool isFacingLeft = false; // can change to left if needed  
        private bool prevFacing = false;
        private bool InventoryAdded = false;
        private static List<Entity> _hoveredEntities = new List<Entity>();

        // magic
        public static Vector2D Deadsize = new Vector2D(
            270.0f,
            130.0f
        );
        public static Vector2D DeadsizeBloopie = new Vector2D(
            330.0f,
            220.0f
        );

        public static Vector2D Defaultsize = new Vector2D(
            110.0f,
            110.0f
        );

        public static Vector2D ZeroSize = new Vector2D(
            0.0f,
            0.0f
        );



        // ----------------------[ NEW FIELDS ]------------------------------
        // A list of tiles (Entity) along the path we need to walk
        private List<Entity> movementPath = new List<Entity>();

        // Index into movementPath representing which tile we are currently walking toward
        private int currentPathIndex = -1;

        // Movement speed in "world units per second"
        private float moveSpeed = 250.0f;

        // How long to pause (in seconds) at each tile before moving on
        public float timeBetweenSteps = 0.2f;

        // A countdown timer that tracks how much longer we should wait
        public float waitTimer = 0.2f;

        // A countdown timer that tracks how much longer we should wait
        private bool setTexture = false;

        private bool _waitingForSpriteRestore = false;
        private float _spriteRestoreTimer = 0f;
        private static readonly float ATTACK_ANIMATION_DURATION = 0.5f; 


        private bool idle = false;

        private static string lastPlayedDamageSound = "";
        // ----------------------[ ATTACK SOUND ]----------------------------
        private static int attackSoundIndex = 0;
        private static readonly string[] attackSoundNames = new string[]
        {
            "SFX_Enemy1_Attack_01", "SFX_Enemy1_Attack_02", "SFX_Enemy1_Attack_03",
            "SFX_Enemy1_Attack_04", "SFX_Enemy1_Attack_05", "SFX_Enemy1_Attack_06",
            "SFX_Enemy1_Attack_07", "SFX_Enemy1_Attack_08", "SFX_Enemy1_Attack_09",
            "SFX_Enemy1_Attack_10"
        };

        // -----------------------[ Death Sound ]---------------------------
        private static int deathsoundIndex = 0;
        private static readonly string[] deathsoundName = new string[]
        {
            "SFX_Enemy1_Death_01", "SFX_Enemy1_Death_02", "SFX_Enemy1_Death_03",
            "SFX_Enemy1_Death_04", "SFX_Enemy1_Death_05", "SFX_Enemy1_Death_06",
            "SFX_Enemy1_Death_07", "SFX_Enemy1_Death_08", "SFX_Enemy1_Death_09",
            "SFX_Enemy1_Death_10"
        };

        // -----------------------[ Jumping Sound ]---------------------------
        private static int jumpsoundIndex = 0;
        private static readonly string[] jumpsoundName = new string[]
        {
            "SFX_EnemyMove_1", "SFX_EnemyMove_2", "SFX_EnemyMove_3"
        };

        // ----------------------[ CONSTRUCTORS ]----------------------------
        public Enemy() { }

        /**
         * @brief Constructs a new Enemy with the specified unit stats.
         *
         * @param entity The entity associated with this enemy.
         * @param position The starting position of the enemy.
         * @param health The enemy's current health.
         * @param maxHealth The enemy's maximum health.
         * @param attackPower The enemy's attack power.
         * @param energy The enemy's current energy.
         * @param maxEnergy The enemy's maximum energy.
         */
        private Enemy(Entity entity, Vector2D position,
                      int health, int maxHealth,
                      int attackPower, int energy, int maxEnergy, bool isAlive, bool isMoving, int speed, string name)
        {
            _unit = new Unit(position, health, maxHealth, attackPower, energy, maxEnergy, isAlive, isMoving, speed);

            Debug.LogInfo("[Enemy] Creating Enemy for entity {0}", entity.ID);

            // Register lifecycle methods
            var behaviour = new BehaviourFCT(entity, Start, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);
            int size = InternalCalls.AnimationComponent_GetAnimationNameSize(entity.ID);
            for (int i = 0; i < size; i++)
            {
                animationNames.Add(InternalCalls.AnimationComponent_GetAnimationName(i, entity.ID));
            }
            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            // Register with message system
            RegisterForMessages();

            Debug.LogInfo("[Enemy] Enemy registered with index {0}", GetBehaviourIndex());

            // Send initialization message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyInitialized,
                entity,
                null,
                health,
                position,
                new Dictionary<string, object>
                {
                    { "MaxHealth", maxHealth },
                    { "AttackPower", attackPower },
                    { "Energy", energy },
                    { "MaxEnergy", maxEnergy },
                    { "Speed", speed }
                }
            );
        }


        // -----------------------[ OBSERVER PATTERN METHODS ]----------------------

        /**
         * @brief Registers this enemy with the game message hub.
         * 
         * Sets up message listening for events related to enemy behavior.
         */
        private void RegisterForMessages()
        {
            GameMessageHub.Instance.RegisterObserver(GameMessageType.TurnChanged, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EnemyTurnStarted, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.PlayerAttacked, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.PlayerMoved, this);
            GameMessageHub.Instance.RegisterObserver(GameMessageType.EnemyDied, this);  // Add this line


            Debug.LogInfo("[Enemy] Enemy {0} registered for messages", GetOwner().ID);
        }

        /**
         * @brief Unregisters from the message system.
         */
        private void UnregisterFromMessages()
        {
            if (GameMessageHub.Instance != null)
            {
                GameMessageHub.Instance.RemoveObserver(this);
                Debug.LogInfo("[Enemy] Enemy {0} unregistered from messages", GetOwner().ID);
            }
        }

        /**
         * @brief Handles incoming game messages.
         * 
         * @param message The message to process.
         */
        public void OnNotify(GameMessage message)
        {
            if (message == null)
                return;

            // For death messages, we want to process them even if the enemy is already marked as not alive
            if (message.MessageType == GameMessageType.EnemyDied && message.Target != null &&
                message.Target.ID == GetOwner().ID)
            {
                HandleEnemyDeath(message);
                return;
            }

            if (message.MessageType == GameMessageType.AnimationStateChanged &&
            message.Data != null && message.Data.ToString() == "EnemyAttackAnimationStarted" &&
            message.Source != null && message.Source.ID == GetOwner().ID)
            {
                // Start the timer to restore sprite
                _waitingForSpriteRestore = true;
                _spriteRestoreTimer = ATTACK_ANIMATION_DURATION;

                Debug.LogInfo("[Enemy] Started sprite restore timer for Enemy {0}", GetOwner().ID);
                return;
            }

            // Only process other messages if alive
            if (_unit != null && !_unit.IsAlive)
                return;

            switch (message.MessageType)
            {
                case GameMessageType.TurnChanged:
                    HandleTurnChanged(message);
                    break;

                case GameMessageType.EnemyTurnStarted:
                    HandleEnemyTurnStarted(message);
                    break;

                case GameMessageType.PlayerAttacked:
                    HandlePlayerAttacked(message);
                    break;

                case GameMessageType.PlayerMoved:
                    HandlePlayerMoved(message);
                    break;
                case GameMessageType.EnemyDied:
                    HandleEnemyDeath(message);
                    break;
            }
        }

        private void HandleEnemyDeath(GameMessage message)
        {
            // Only handle if this message is about this enemy
            if (message.Target != null && message.Target.ID == GetOwner().ID && !setTexture)
            {
                var enemyInstance = GameLogicSystem.Instance.GetBehaviour<Enemy>(GetOwner());
                InternalCalls.UnitComponent_SetIsAlive(GetOwner().ID, false);
                InternalCalls.AnimationComponent_PlayAnimation(GetOwner().ID, enemyInstance.animationNames[(int)Animation.Death]);

                InternalCalls.Audio_PlaySound(deathsoundName[deathsoundIndex], 0.5f);
                deathsoundIndex = (deathsoundIndex + 1) % deathsoundName.Length;

                Debug.LogInfo("[Enemy] Enemy {0} is processing its own death", GetOwner().ID);

                // Handle textures
                // Check the current texture name
                string currentTexture = InternalCalls.MaterialComponent_GetTextureType(GetOwner().ID);
                if (currentTexture == "Enemy")
                {
                    InternalCalls.Transform_SetScale(GetOwner().ID, ref Deadsize);
                    InternalCalls.SetEntityTextureEnemy(GetOwner().ID, "Test1");
                }
                else if (currentTexture == "Enemy_2")
                {
                    InternalCalls.Transform_SetScale(GetOwner().ID, ref DeadsizeBloopie);
                    InternalCalls.SetEntityTextureEnemy(GetOwner().ID, "Bloopie_Death_Set"); 
                }
                // Handle animations based on facing direction

                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(GetOwner().ID, enemyInstance.animationNames[(int)Animation.Move]);
                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(GetOwner().ID, enemyInstance.animationNames[(int)Animation.Attack]);
                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(GetOwner().ID, enemyInstance.animationNames[(int)Animation.Idle]);

                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(GetOwner().ID, enemyInstance.animationNames[(int)Animation.MoveFlip]);
                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(GetOwner().ID, enemyInstance.animationNames[(int)Animation.AttackFlip]);
                InternalCalls.AnimationComponent_RemoveEntityFromAnimation(GetOwner().ID, enemyInstance.animationNames[(int)Animation.IdleFlip]);



                // Handle unit components
                InternalCalls.UnitComponent_SetEnergy(GetOwner().ID, 0);
                InternalCalls.UnitComponent_SetMaxEnergy(GetOwner().ID, 0);
                InternalCalls.UnitComponent_SetIsMoving(GetOwner().ID, false);

                // Handle tile component
                Vector2D belowPosition = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(GetOwner().ID, ref belowPosition);
                Entity tileBelowEnemy = TileManager.Instance.GetTileAtPosition(belowPosition);

                if (tileBelowEnemy != null)
                {
                    InternalCalls.TileComponent_SetOccupant(tileBelowEnemy.ID, 0);
                    InternalCalls.TileComponent_SetIsWalkable(tileBelowEnemy.ID, true);
                }

                // Handle cooking drops
                if (InventoryAdded == false)
                {
                    int uniquetype = InternalCalls.UnitComponent_GetMaxHealth(GetOwner().ID);
                    if (uniquetype == 450)
                    {
                        InventorySystem.Instance.AddItem("bread");
                        InventoryAdded = true;
                    }
                    else if(uniquetype == 120){
                        InventorySystem.Instance.AddItem("cheese");
                        InventoryAdded = true;
                    }
                    else 
                    {
                        InventorySystem.Instance.AddItem("fish");
                        InventoryAdded = true;
                    }

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.InventoryUpdated,
                        GetOwner(),
                        null,
                        1,
                        null,
                        "FishAddedFromEnemyDeath"
                    );
                }

                // Mark as processed
                setTexture = true;

                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EntityStateChanged,
                    GetOwner(),
                    null,
                    0,
                    null,
                    "EnemyDeathProcessingCompleted"
                );
            }
        }

        private void HandleTurnChanged(GameMessage message)
        {
            // React to turn changes if needed
            TurnType newTurn = (TurnType)message.Data;
            Debug.LogInfo("[Enemy] Enemy {0} received turn change to {1}",
                GetOwner().ID, newTurn);

            if (newTurn == TurnType.EnemyTurn)
            {
                // Reset for new turn if needed
                waitTimer = timeBetweenSteps;
            }
        }

        private void HandleEnemyTurnStarted(GameMessage message)
        {
            // React to enemy turn starting
            Entity activeEnemy = message.Target;

            // Check if this is the active enemy
            if (activeEnemy != null && activeEnemy.ID == GetOwner().ID)
            {
                Debug.LogInfo("[Enemy] Enemy {0} is now the active enemy", GetOwner().ID);

                // Reset any turn-specific state
                currentPathIndex = -1;
                movementPath.Clear();
            }
        }

        private void HandlePlayerAttacked(GameMessage message)
        {
            // React to player attacking if needed
            // For example, the enemy might change behavior based on player aggression
            if (message.Target != null && message.Target.ID == GetOwner().ID)
            {
                Debug.LogInfo("[Enemy] Enemy {0} was attacked by player for {1} damage",
                    GetOwner().ID, message.Value);
            }
        }

        private void HandlePlayerMoved(GameMessage message)
        {
            // React to player movement
            // For example, might want to recalculate path if player moves during enemy decision phase
            Debug.LogInfo("[Enemy] Enemy {0} noticed player movement", GetOwner().ID);
        }

        private void UpdateSpriteRestoreTimer(float deltaTime)
        {
            if (_waitingForSpriteRestore)
            {
                _spriteRestoreTimer -= deltaTime;

                if (_spriteRestoreTimer <= 0)
                {
                    // Timer expired, restore sprite
                    _waitingForSpriteRestore = false;

                    // Restore sprite scale to default size
                    InternalCalls.Transform_SetScale(GetOwner().ID, ref Defaultsize);

                    Debug.LogInfo("[Enemy] Restored sprite for Enemy {0} after attack animation", GetOwner().ID);

                    // Send message that sprite is restored
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.AnimationStateChanged,
                        GetOwner(),
                        null,
                        0,
                        null,
                        "EnemySpriteRestored"
                    );
                }
            }
        }


        /**
         * @brief Factory method to create an Enemy behaviour.
         *
         * Initializes the enemy's stats from its UnitComponent and creates an Enemy instance.
         *
         * @param entity The entity for which to create the enemy behaviour.
         * @return A new instance of Enemy as a BehaviourComponent if successful; otherwise, null.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[Enemy] [CreateBehaviour] Starting for Enemy on entity {0}", entity.ID);

                // Send creation started message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EntityCreationStarted,
                    null,
                    entity,
                    0,
                    null,
                    "EnemyBehaviourCreation"
                );

                // Initialize enemy stats
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
                string name = InternalCalls.MaterialComponent_GetTextureType(entity.ID);
                var enemy = new Enemy(entity, position, health, maxHealth, attackPower, energy, maxEnergy, isAlive, isMoving, speed, name);

                // Send creation completed message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EntityCreationCompleted,
                    null,
                    entity,
                    0,
                    position,
                    "EnemyBehaviourCreated"
                );

                return enemy;
            }
            catch (Exception ex)
            {
                Debug.LogError("[Enemy] [CreateBehaviour] Exception occurred: {0}", ex.Message);

                // Send error message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameError,
                    null,
                    entity,
                    0,
                    null,
                    "Failed to create enemy behavior: " + ex.Message
                );

                return null;
            }
        }

        // ----------------------[ LIFECYCLE METHODS ]-----------------------
        /**
         * @brief Initialization method for the Enemy.
         *
         * Sets up the shared health and energy bar components if they have not been initialized.
         *
         * @param entity The enemy entity being initialized.
         */
        private static void Start(Entity entity)
        {
            Debug.LogInfo("[Enemy] Initializing Enemy for entity {0}", entity.ID);

            // Send initialization started message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EntityInitializationStarted,
                null,
                entity,
                0,
                null,
                "EnemyInitialization"
            );

            var enemyInstance = GameLogicSystem.Instance.GetBehaviour<Enemy>(entity);
            if (enemyInstance == null)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameError,
                    null,
                    entity,
                    0,
                    null,
                    "Enemy behavior not found during initialization"
                );
                return; // No Enemy behaviour for this entity, so exit.
            }

            // Assume you have enemy-specific health bar entities; adjust the names as needed.
            Entity enemyHealthBarFiller = new Entity(InternalCalls.Entity_GetEntityByName("enemyHealthbar(filler)"));
            Entity enemyHealthBarEmpty = new Entity(InternalCalls.Entity_GetEntityByName("enemyHealthbar(empty)"));
            enemyInstance._enemyHealthBarComponent = new HealthBarComponent(entity, enemyHealthBarFiller, enemyHealthBarEmpty);
            enemyInstance._enemyHealthBarComponent.SetTarget    (entity);

            // Use your enemy-specific energy bar entities; adjust the names as needed.
            Entity enemyEnergyBarFiller = new Entity(InternalCalls.Entity_GetEntityByName("enemyEnergybar(filler)"));
            Entity enemyEnergyBarEmpty = new Entity(InternalCalls.Entity_GetEntityByName("enemyEnergybar(empty)"));
            enemyInstance._enemyEnergyBarComponent = new EnergyBarComponent(entity, enemyEnergyBarFiller, enemyEnergyBarEmpty);
            enemyInstance._enemyEnergyBarComponent.SetTarget(entity);

            // Send initialization completed message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EntityInitializationCompleted,
                null,
                entity,
                0,
                null,
                "EnemyInitializationCompleted"
            );
            Debug.LogInfo("[Enemy] Initializing Enemy for entity {0} complete", entity.ID);
        }

        /**
         * @brief Shows the attack power icon for this enemy.
         * 
         * Retrieves the icon entity by name and sets its scale to make it visible.
         */
        public void ShowAttackPowerIcon()
        {
            _attackPowerEntity = new Entity(InternalCalls.Entity_GetEntityByName("EnemyAttackPowerIcon"));
            // Ensure the icon entity is valid
            if (_attackPowerEntity == null || _attackPowerEntity.ID == 0)
            {
                Debug.LogWarning("[Enemy] _attackPowerEntity is invalid or not set.");

                // Send error message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.UIError,
                    GetOwner(),
                    null,
                    0,
                    null,
                    "AttackPowerIconMissing"
                );

                return;
            }

            // Set a visible scale (adjust numbers as appropriate)
            Vector2D visibleScale = new Vector2D(70.0f, 70.0f);
            InternalCalls.Transform_SetScale(_attackPowerEntity.ID, ref visibleScale);

            // Get position for message
            Vector2D enemyPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(GetOwner().ID, ref enemyPosition);

            // Send UI event message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.UIElementsPositioned,
                GetOwner(),
                _attackPowerEntity,
                InternalCalls.UnitComponent_GetAttackPower(GetOwner().ID), // Pass attack value
                enemyPosition,
                "AttackPowerIconShown"
            );
        }


        /**
   * @brief Shows the range icon for this enemy.
   * 
   * Retrieves the icon entity by name and sets its scale to make it visible.
   * Sends appropriate UI messaging events when showing the icon.
   */
        public void ShowRangeIcon()
        {
            _rangeEntity = new Entity(InternalCalls.Entity_GetEntityByName("EnemyRangeIcon"));
            // Ensure the icon entity is valid
            if (_rangeEntity == null || _rangeEntity.ID == 0)
            {
                Debug.LogWarning("[Enemy] _rangeEntity is invalid or not set.");

                // Send error message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.UIError,
                    GetOwner(),
                    null,
                    0,
                    null,
                    "RangeIconMissing"
                );

                return;
            }

            // Set a visible scale (adjust numbers as appropriate)
            Vector2D visibleScale = new Vector2D(70.0f, 70.0f);
            InternalCalls.Transform_SetScale(_rangeEntity.ID, ref visibleScale);

            // Get position for message
            Vector2D enemyPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(GetOwner().ID, ref enemyPosition);

            // Get enemy speed for the message value (which represents range capability)
            int enemySpeed = InternalCalls.UnitComponent_GetSpeed(GetOwner().ID);

            // Send UI event message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.UIElementsPositioned,
                GetOwner(),
                _rangeEntity,
                enemySpeed, // Pass speed/range value
                enemyPosition,
                "RangeIconShown"
            );
        }

        /**
         * @brief Hides the attack power icon for this enemy.
         * 
         * Retrieves the icon entity by name if needed, then sets its scale to zero.
         * Sends appropriate UI messaging events when hiding the icon.
         */
        private void HideAttackPowerIcon()
        {
            if (_attackPowerEntity == null)
            {
                _attackPowerEntity = new Entity(InternalCalls.Entity_GetEntityByName("EnemyAttackPowerIcon"));
                if (_attackPowerEntity == null)
                {
                    Debug.LogError("[UpdateEnemyAttackOverlays] Level1_Game_EnemyAttackPower entity not found.");

                    // Send error message
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.UIError,
                        GetOwner(),
                        null,
                        0,
                        null,
                        "AttackPowerIconNotFound"
                    );

                    return;
                }
            }

            // Hide by setting scale to zero
            Vector2D zeroScale = Vector2D.Zero;
            InternalCalls.Transform_SetScale(_attackPowerEntity.ID, ref zeroScale);
            //Debug.LogError("[UpdateEnemyAttackOverlays] Hiding Entity number {0}.", _attackPowerEntity.ID);
            // Send UI hidden message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.UIElementsPositioned,
                GetOwner(),
                _attackPowerEntity,
                0,
                null,
                "AttackPowerIconHidden"
            );
        }

        /**
         * @brief Hides the range icon for this enemy.
         * 
         * Retrieves the icon entity by name if needed, then sets its scale to zero.
         * Sends appropriate UI messaging events when hiding the icon.
         */
        private void HideRangeIcon()
        {
            if (_rangeEntity == null)
            {
                _rangeEntity = new Entity(InternalCalls.Entity_GetEntityByName("EnemyRangeIcon"));
                if (_rangeEntity == null)
                {
                    Debug.LogError("[Enemy] Level1_Game_EnemyRange entity not found.");

                    // Send error message
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.UIError,
                        GetOwner(),
                        null,
                        0,
                        null,
                        "RangeIconNotFound"
                    );

                    return;
                }
            }

            // Hide by setting scale to zero
            Vector2D zeroScale = Vector2D.Zero;
            InternalCalls.Transform_SetScale(_rangeEntity.ID, ref zeroScale);

            // Send UI hidden message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.UIElementsPositioned,
                GetOwner(),
                _rangeEntity,
                0,
                null,
                "RangeIconHidden"
            );
        }

        /**
         * @brief Update method called every frame.
         *
         * If it is the enemy's turn, the method either continues incremental movement
         * or computes a new path to the player and begins moving. It also updates or hides
         * the enemy's health and energy bars based on mouse hover status.
         *
         * @param entity The enemy entity being updated.
         * @param deltaTime The elapsed time since the last frame.
         */
        private static void Update(Entity entity, float deltaTime)
        {
            // If the pause/main/story menu is currently active, do NOT process any of the player's logic
            if (InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("StoryLayer"))
                return;
            
            CameraShake.Update(deltaTime);

            bool isMoving = InternalCalls.UnitComponent_GetIsMoving(entity.ID);
            bool isHovered = Input.IsMouseOverEntity(entity);
            bool isAlive = InternalCalls.UnitComponent_GetIsAlive(entity.ID);
            int energy = InternalCalls.UnitComponent_GetEnergy(entity.ID);
            int health = InternalCalls.UnitComponent_GetHealth(entity.ID);

            // Get current position
            Vector2D position = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(entity.ID, ref position);

            var enemyInstance = GameLogicSystem.Instance.GetBehaviour<Enemy>(entity);
            if (enemyInstance == null)
                return;

            enemyInstance.UpdateSpriteRestoreTimer(deltaTime);

            // Only send status update if something important changed
            bool statusChanged = isMoving != enemyInstance._lastReportedIsMoving ||
                                isAlive != enemyInstance._lastReportedIsAlive ||
                                energy != enemyInstance._lastReportedEnergy ||
                                health != enemyInstance._lastReportedHealth ||
                                Vector2D.Distance(position, enemyInstance._lastReportedPosition) > 0.1f;

            if (statusChanged)
            {
                // Send entity status update message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EntityStatusUpdate,
                    entity,
                    null,
                    isMoving ? 1 : 0,
                    position,
                    new Dictionary<string, object>
                    {
                { "IsMoving", isMoving },
                { "IsHovered", isHovered },
                { "IsAlive", isAlive },
                { "Energy", energy },
                { "Health", health }
                    }
                );

                // Update cached states
                enemyInstance._lastReportedIsMoving = isMoving;
                enemyInstance._lastReportedIsAlive = isAlive;
                enemyInstance._lastReportedEnergy = energy;
                enemyInstance._lastReportedHealth = health;
                enemyInstance._lastReportedPosition = position;
            }

            UpdateEnemyFacingDirection(entity, enemyInstance);

            // UI update logic - handles hover effects regardless of whose turn it is
            if (isHovered)
            {
                if (!_hoveredEntities.Contains(entity))
                {
                    _hoveredEntities.Add(entity);

                    // Send hover started message
                    Vector2D enemyPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(entity.ID, ref enemyPosition);

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.UIInteraction,
                        entity,
                        null,
                        0,
                        enemyPosition,
                        "EnemyHoverStarted"
                    );
                }

                enemyInstance.ShowAttackPowerIcon();
                enemyInstance.ShowRangeIcon();
                enemyInstance._enemyHealthBarComponent.UpdateHealthBar();
                enemyInstance._enemyEnergyBarComponent.UpdateEnergyBar();
            }
            else
            {
                if (_hoveredEntities.Contains(entity))
                {
                    _hoveredEntities.Remove(entity);

                    // Send hover ended message
                    Vector2D enemyPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(entity.ID, ref enemyPosition);

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.UIInteraction,
                        entity,
                        null,
                        0,
                        enemyPosition,
                        "EnemyHoverEnded"
                    );
                }
            }

            if (_hoveredEntities.Count == 0)
            {
                enemyInstance.HideAttackPowerIcon();
                enemyInstance.HideRangeIcon();
                enemyInstance._enemyHealthBarComponent.HideUI();
                enemyInstance._enemyEnergyBarComponent.HideUI();
            }

            // Only continue if the enemy is alive
            if (InternalCalls.UnitComponent_GetIsAlive(entity.ID) == true)
            {
                if (enemyInstance.isFacingLeft)
                {
                    // change in direction
                    if (enemyInstance.prevFacing != enemyInstance.isFacingLeft)
                    {

                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.MoveFlip]);
                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.IdleFlip]);

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.AnimationStateChanged,
                            entity,
                            null,
                            0,
                            null,
                            "EnemyChangedDirectionToLeft"
                        );

                        if (enemyInstance.idle)
                        {
                            InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Idle]);
                        }
                        else if (isMoving)
                        {
                            InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Move]);
                        }
                    }

                    // Idle animation
                    if (!isMoving && !enemyInstance.idle)
                    {
                        Debug.LogWarning("[Enemy] Setting idle bool of instace {0} to true.", entity.ID);
                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Move]);
                        InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Idle]);
                        enemyInstance.idle = true;

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.AnimationStateChanged,
                            entity,
                            null,
                            0,
                            null,
                            "EnemyIdleAnimationStarted"
                        );
                    }
                    else if (isMoving && enemyInstance.idle)
                    {
                        enemyInstance.idle = false;
                        Debug.LogWarning("[Enemy] Setting idle bool of instace {0} to false.", entity.ID);
                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Idle]);
                        InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Move]);

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.AnimationStateChanged,
                            entity,
                            null,
                            0,
                            null,
                            "EnemyJumpAnimationStarted"
                        );
                    }
                }
                else
                {
                    // change in direction
                    if (enemyInstance.prevFacing != enemyInstance.isFacingLeft)
                    {
                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Idle]);
                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.Move]);

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.AnimationStateChanged,
                            entity,
                            null,
                            0,
                            null,
                            "EnemyChangedDirectionToRight"
                        );

                        if (enemyInstance.idle)
                        {
                            InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.IdleFlip]);
                        }
                        else if (isMoving)
                        {
                            InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.MoveFlip]);
                        }
                    }

                    // Idle animation
                    if (!isMoving && !enemyInstance.idle)
                    {
                        Debug.LogWarning("[Enemy] Setting idle bool of instace {0} to true.", entity.ID);
                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.MoveFlip]);
                        InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.IdleFlip]);
                        enemyInstance.idle = true;

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.AnimationStateChanged,
                            entity,
                            null,
                            0,
                            null,
                            "EnemyIdleFlipAnimationStarted"
                        );
                    }
                    else if (isMoving && enemyInstance.idle)
                    {
                        enemyInstance.idle = false;
                        Debug.LogWarning("[Enemy] Setting idle bool of instace {0} to false.", entity.ID);
                        InternalCalls.AnimationComponent_PlayAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.MoveFlip]);
                        InternalCalls.AnimationComponent_RemoveEntityFromAnimation(entity.ID, enemyInstance.animationNames[(int)Animation.IdleFlip]);

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.AnimationStateChanged,
                            entity,
                            null,
                            0,
                            null,
                            "EnemyJumpFlipAnimationStarted"
                        );
                    }
                }

                // Only proceed with enemy turn logic if:
                // 1. It's the enemy's turn
                // 2. This enemy is the current active enemy in the turn manager
                if (UnitManager.Instance.CurrentTurn == TurnType.EnemyTurn)
                {
                    Entity currentActiveEnemy = UnitManager.Instance.GetCurrentActiveEnemy();

                    // Only process actions for the active enemy
                    if (currentActiveEnemy == null || currentActiveEnemy.ID != entity.ID)
                        return;

                    var enemy = GameLogicSystem.Instance.GetBehaviour<Enemy>(entity);
                    if (enemy == null)
                        return;

                    // If already moving, continue movement
                    if (isMoving)
                    {
                        Vector2D currentPos = new Vector2D();
                        InternalCalls.UnitComponent_GetPosition(entity.ID, ref currentPos);

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.EnemyMoved,
                            entity,
                            null,
                            enemy.currentPathIndex,
                            currentPos,
                            "ContinuingMovement"
                        );

                        DoIncrementalMove(entity, deltaTime, enemy);

                        // Check if enemy is adjacent to the player after moving
                        Entity PlayerEntity = UnitManager.Instance.GetPlayerEntity();
                        if (IsAdjacentToPlayer(entity, PlayerEntity) && enemy._unit.Energy > 0)
                        {
                            Vector2D belowPosition = new Vector2D();
                            InternalCalls.UnitComponent_GetPosition(entity.ID, ref belowPosition);
                            Entity tileBelowEnemy = TileManager.Instance.GetTileAtPosition(belowPosition);
                            InternalCalls.TileComponent_SetOccupant(tileBelowEnemy.ID, entity.ID);
                            InternalCalls.TileComponent_SetIsWalkable(tileBelowEnemy.ID, false);

                            GameMessageHub.Instance.SendMessage(
                                GameMessageType.EntityProximityChanged,
                                entity,
                                PlayerEntity,
                                1,
                                belowPosition,
                                "EnemyReachedAdjacentTile"
                            );
                        }
                        return;
                    }

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.EnemyActionStarted,
                        entity,
                        null,
                        0,
                        null,
                        "PlanningPath"
                    );

                    // Not moving yet - plan a path to the player
                    var playerEntity = UnitManager.Instance.GetPlayerEntity();
                    if (playerEntity == null)
                    {
                        Debug.LogWarning("[Enemy] No player entity found.");

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.GameError,
                            entity,
                            null,
                            0,
                            null,
                            "NoPlayerFoundForPathfinding"
                        );

                        UnitManager.Instance.NotifyEnemyFinished(entity); // Skip this enemy's turn
                        return;
                    }

                    // Retrieve the start (enemy) tile
                    Vector2D startPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(entity.ID, ref startPosition);
                    var startTile = TileManager.Instance.GetTileAtPosition(startPosition);

                    // Retrieve the player tile
                    Vector2D playerPosition = new Vector2D();
                    InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);
                    var playerTile = TileManager.Instance.GetTileAtPosition(playerPosition);

                    if (startTile == null || playerTile == null)
                    {
                        Debug.LogWarning("[Enemy] Invalid start or player tile.");

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.PathfindingError,
                            entity,
                            playerEntity,
                            0,
                            startPosition,
                            "InvalidStartOrTargetTile"
                        );

                        UnitManager.Instance.NotifyEnemyFinished(entity); // Skip this enemy's turn
                        return;
                    }

                    // Check if we're already adjacent to the player
                    if (IsAdjacentToPlayer(entity, playerEntity))
                    {
                        Debug.LogInfo("Enemy is adjacent to player, performing attack");

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.EnemyActionStarted,
                            entity,
                            playerEntity,
                            InternalCalls.UnitComponent_GetAttackPower(entity.ID),
                            startPosition,
                            "AttackingFromAdjacentTile"
                        );
                        // Hide enemy sprite by setting its scale to zero
                        InternalCalls.Transform_SetScale(entity.ID, ref ZeroSize);
                        AttackPlayer(entity, playerEntity);
                        // show enemy sprite by setting its scale to default
                        InternalCalls.Transform_SetScale(entity.ID, ref Defaultsize);
                        InternalCalls.UnitComponent_SetIsMoving(entity.ID, false);
                        return;
                    }

                    // Plan path to player
                    Debug.LogInfo("[Enemy] Calculating path to player...");

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.PathfindingStarted,
                        entity,
                        playerEntity,
                        0,
                        startPosition,
                        "CalculatingPathToPlayer"
                    );

                    var path = TileManager.Instance.ExecuteAStarPathfinding(
                        startTile, playerTile, allowNonWalkableEnd: false
                    );

                    if (path.Count == 0)
                    {
                        Debug.LogWarning("[Enemy] No valid path found to player for Enemy {0}.", entity.ID);

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.PathfindingFailed,
                            entity,
                            playerEntity,
                            0,
                            startPosition,
                            "NoValidPathFound"
                        );

                        UnitManager.Instance.NotifyEnemyFinished(entity); // Skip this enemy's turn
                        return;
                    }

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.PathfindingCompleted,
                        entity,
                        playerEntity,
                        path.Count,
                        startPosition,
                        new Dictionary<string, object> {
                            { "PathLength", path.Count },
                            { "StartTileID", startTile.ID },
                            { "TargetTileID", playerTile.ID }
                        }
                    );

                    // Check if we have enough energy for the path
                    int requiredSteps = path.Count - 1; // e.g., from path[0] to path[last]
                    int stepsWeCanTake = Math.Min(enemy._unit.Energy, requiredSteps);

                    // Log energy calculation
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.EnemyEnergyCalculation,
                        entity,
                        null,
                        stepsWeCanTake,
                        startPosition,
                        new Dictionary<string, object> {
                            { "RequiredSteps", requiredSteps },
                            { "AvailableEnergy", enemy._unit.Energy },
                            { "ActualSteps", stepsWeCanTake }
                        }
                    );

                    // This is the tile we'd end on if there's no early adjacency attack:
                    Entity finalTileIfNoAttack = path[stepsWeCanTake];
                    if (requiredSteps > enemy._unit.Energy)
                    {
                        Debug.LogWarning("[Enemy] Enemy {0} does not have enough energy for full path.", entity.ID);

                        GameMessageHub.Instance.SendMessage(
                            GameMessageType.EnemyEnergyInsufficient,
                            entity,
                            null,
                            enemy._unit.Energy,
                            startPosition,
                            new Dictionary<string, object> {
                                { "RequiredEnergy", requiredSteps },
                                { "AvailableEnergy", enemy._unit.Energy },
                                { "PathLength", path.Count }
                            }
                        );
                    }

                    // Mark the tile we're leaving as walkable again
                    InternalCalls.TileComponent_SetOccupant(startTile.ID, 0);
                    InternalCalls.TileComponent_SetIsWalkable(startTile.ID, true);

                    // Update occupancy for destination tile
                    InternalCalls.TileComponent_SetOccupant(finalTileIfNoAttack.ID, entity.ID);
                    InternalCalls.TileComponent_SetIsWalkable(finalTileIfNoAttack.ID, false);

                    // Send message about tile state changes
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.TileStateChanged,
                        entity,
                        startTile,
                        0,
                        startPosition,
                        "SourceTileFreed"
                    );

                    Vector2D finalPosition = TileManager.Instance.GetTileCenter(finalTileIfNoAttack);
                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.TileStateChanged,
                        entity,
                        finalTileIfNoAttack,
                        1,
                        finalPosition,
                        "DestinationTileOccupied"
                    );

                    // Store the path in the enemy's behaviour for incremental movement
                    enemy.movementPath = path;
                    enemy.currentPathIndex = 1; // start walking from path[0] to path[1]
                    InternalCalls.UnitComponent_SetIsMoving(entity.ID, true);

                    Debug.LogInfo("[Enemy] Path stored. Will move incrementally along {0} tiles.", path.Count);

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.EnemyMovementStarted,
                        entity,
                        null,
                        path.Count,
                        startPosition,
                        new Dictionary<string, object> {
                            { "PathLength", path.Count },
                            { "PlannedSteps", stepsWeCanTake },
                            { "StartTileID", startTile.ID },
                            { "FinalTileID", finalTileIfNoAttack.ID }
                        }
                    );
                }
            }
        }

        /**
   * @brief Cleanup method for the Enemy.
   *
   * Resets the enemy's unit stats and logs cleanup completion.
   *
   * @param entity The enemy entity to clean up.
   */
        private static void End(Entity entity)
        {
            _hoveredEntities.Clear();
            // Send entity cleanup started message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EntityCleanupStarted,
                entity,
                null,
                0,
                null,
                "EnemyCleanupStarted"
            );

            var enemyBehaviour = GameLogicSystem.Instance.GetBehaviour<Enemy>(entity);
            enemyBehaviour.ShowAttackPowerIcon();
            enemyBehaviour.ShowRangeIcon();
            enemyBehaviour._enemyHealthBarComponent.UpdateHealthBar();
            enemyBehaviour._enemyEnergyBarComponent.UpdateEnergyBar();
            if (enemyBehaviour == null)
            {
                Debug.LogWarning("[EnemyBehaviour] EnemyBehaviour is null for entity {0}.", entity.ID);

                // Send error message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameError,
                    entity,
                    null,
                    0,
                    null,
                    "EnemyBehaviourNotFoundDuringCleanup"
                );
                return;
            }

            // Unregister from message system
            enemyBehaviour.UnregisterFromMessages();

            if (enemyBehaviour._unit != null)
            {
                enemyBehaviour._unit.ResetStats();
                Debug.LogInfo("[EnemyBehaviour] Reset Unit stats for entity {0}.", entity.ID);

                //// Send unit reset message
                //GameMessageHub.Instance.SendMessage(
                //    GameMessageType.EntityStatsReset,
                //    entity,
                //    null,
                //    0,
                //    null,
                //    "EnemyStatsReset"
                //);
            }

            Debug.LogInfo("[EnemyBehaviour] EnemyBehaviour cleanup complete for entity {0}.", entity.ID);

            // Send entity cleanup completed message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EntityCleanupCompleted,
                entity,
                null,
                0,
                null,
                "EnemyCleanupCompleted"
            );
        }
        //private static long lastJumpedTileID = -1;
        // ----------------------[ INCREMENTAL MOVE LOGIC ]------------------
        /**
         * @brief Performs incremental movement of the enemy along its path.
         *
         * Moves the enemy towards the next tile incrementally. If the enemy arrives at the tile,
         * it triggers arrival logic.
         *
         * @param entity The enemy entity being moved.
         * @param dt The elapsed time since the last frame.
         * @param enemy The Enemy instance containing movement data.
         */
        private static void DoIncrementalMove(Entity entity, float dt, Enemy enemy)
        {
            if (enemy.waitTimer > 0.0f)
            {
                enemy.waitTimer -= dt;
                return; // No movement this frame
            }

            int currentEnergy = InternalCalls.UnitComponent_GetEnergy(entity.ID);

            // If we used up the path, we're done
            if (enemy.currentPathIndex >= enemy.movementPath.Count || currentEnergy <= 0)
            {
                Debug.LogInfo("[Enemy] Movement finished or out of energy. Stopping.");

                // Determine reason for movement end
                string reason = enemy.currentPathIndex >= enemy.movementPath.Count ?
                    "PathCompleted" : "EnergyDepleted";

                // Send movement finished message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyMovementFinished,
                    entity,
                    null,
                    currentEnergy,
                    null,
                    reason
                );

                InternalCalls.UnitComponent_SetIsMoving(entity.ID, false);
                enemy.hasPlayedJumpSound = false;
                UnitManager.Instance.NotifyEnemyFinished(entity);  // Use the encapsulated method
                UnitManager.Instance.CheckAndHandleEnemyExhaustion();
            }

            // Tile we're heading toward
            var nextTile = enemy.movementPath[enemy.currentPathIndex];
            if (nextTile == null || nextTile.ID == 0)
            {
                // Send invalid tile message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PathError,
                    entity,
                    null,
                    enemy.currentPathIndex,
                    null,
                    "InvalidTileInPath"
                );

                InternalCalls.UnitComponent_SetIsMoving(entity.ID, false);
                UnitManager.Instance.CheckAndHandleEnemyExhaustion();
                return;
            }

            // 1. Get target tile center
            Vector2D targetPos = TileManager.Instance.GetTileCenter(nextTile);

            // 2. Current position
            Vector2D currentPos = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(entity.ID, ref currentPos);

            // 3. Compute direction & distance
            Vector2D direction = targetPos - currentPos;
            float distance = direction.Length;

            // Play the jump sound only when moving to a **new tile** (not on every frame)
            if (!enemy.hasPlayedJumpSound && enemy.currentPathIndex < enemy.movementPath.Count - 1)
            {
                Debug.LogInfo("[Enemy] Playing jump sound: " + jumpsoundName[jumpsoundIndex]);
                InternalCalls.Audio_PlaySound(jumpsoundName[jumpsoundIndex], 0.5f);
                jumpsoundIndex = (jumpsoundIndex + 1) % jumpsoundName.Length;
                enemy.hasPlayedJumpSound = true; // Set flag so it doesn't play again for this jump
                //InternalCalls.Audio_PlaySound("SFX_EnemyMove_3", 0.5f);

                enemy.lastJumpedTileID = (long)nextTile.ID; // Update last tile jumped
            }

            // Send movement update message (once per frame)
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyMovementUpdate,
                entity,
                nextTile,
                enemy.currentPathIndex,
                currentPos,
                new Dictionary<string, object> {
            { "TargetPosition", targetPos },
            { "Distance", distance },
            { "RemainingEnergy", currentEnergy },
            { "PathProgress", $"{enemy.currentPathIndex}/{enemy.movementPath.Count}" }
                }
            );

            

            if (distance < 0.001f)
            {
                // We're basically at this tile
                ArriveAtTile(entity, nextTile, enemy);
                return;
            }

            direction = direction.Normalized;

            // 4. Move a partial step
            float step = enemy.moveSpeed * dt;
            if (step < distance)
            {
                //InternalCalls.Audio_PlaySound(jumpsoundName[jumpsoundIndex], 0.5f);
                //jumpsoundIndex = (jumpsoundIndex + 1) % jumpsoundName.Length;
                // Move partway
                Vector2D newPos = currentPos + direction * step;
                InternalCalls.UnitComponent_SetPosition(entity.ID, ref newPos);
                InternalCalls.Transform_SetTranslation(entity.ID, ref newPos);
            }
            else
            {
                // Play jump sound when reaching the new tile
                //InternalCalls.Audio_PlaySound(jumpsoundName[jumpsoundIndex], 0.5f);
                //jumpsoundIndex = (jumpsoundIndex + 1) % jumpsoundName.Length;

                // We'll overshoot, so clamp to final tile center
                InternalCalls.UnitComponent_SetPosition(entity.ID, ref targetPos);
                InternalCalls.Transform_SetTranslation(entity.ID, ref targetPos);

                // Send tile reached message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyReachedTile,
                    entity,
                    nextTile,
                    enemy.currentPathIndex,
                    targetPos,
                    "TileReached"
                );

                // Mark arrival
                ArriveAtTile(entity, nextTile, enemy);
                enemy.waitTimer = enemy.timeBetweenSteps;
                enemy.hasPlayedJumpSound = false;
            }
        }

        /**
  * @brief Called when the enemy arrives at a tile.
  *
  * Decrements energy, checks for adjacency to the player to possibly trigger an attack,
  * and advances the movement path.
  *
  * @param entity The enemy entity.
  * @param tileEntity The tile at which the enemy has arrived.
  * @param enemy The Enemy instance.
  */
        private static void ArriveAtTile(Entity entity, Entity tileEntity, Enemy enemy)
        {
            // Get tile position for logging
            Vector2D tilePosition = TileManager.Instance.GetTileCenter(tileEntity);

            // Get current enemy energy
            int currentEnergy = InternalCalls.UnitComponent_GetEnergy(entity.ID);
            int previousEnergy = currentEnergy;

            // Decrement energy
            currentEnergy--;
            InternalCalls.UnitComponent_SetEnergy(entity.ID, currentEnergy);

            // Send energy change message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyEnergyChanged,
                entity,
                null,
                currentEnergy,
                tilePosition,
                new Dictionary<string, object> {
            { "PreviousEnergy", previousEnergy },
            { "CurrentEnergy", currentEnergy },
            { "TileID", tileEntity.ID },
            { "Reason", "MovementStep" }
                }
            );

            // Check for player at this tile
            var occupant = TileManager.Instance.GetOccupant(tilePosition);
            var playerEntity = UnitManager.Instance.GetPlayerEntity();

            // If player is at this tile, attack
            if (occupant != null && playerEntity != null && occupant.ID == playerEntity.ID)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EntityProximityChanged,
                    entity,
                    playerEntity,
                    0,
                    tilePosition,
                    "EnemyArrivedAtPlayerTile"
                );

                // Hide enemy sprite by setting its scale to zero
                InternalCalls.Transform_SetScale(entity.ID, ref ZeroSize);
                AttackPlayer(entity, playerEntity);
                // show enemy sprite by setting its scale to default
                InternalCalls.Transform_SetScale(entity.ID, ref Defaultsize);
                InternalCalls.UnitComponent_SetIsMoving(entity.ID, false);
                UnitManager.Instance.CheckAndHandleEnemyExhaustion();
                return;
            }

            // Or check if player is adjacent
            if (IsAdjacentToPlayer(entity, playerEntity))
            {
                Debug.LogInfo("Enemy is adjacent to player, performing attack");

                // Get positions for detailed logging
                Vector2D enemyPosition = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(entity.ID, ref enemyPosition);
                Vector2D playerPosition = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);

                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EntityProximityChanged,
                    entity,
                    playerEntity,
                    1,
                    enemyPosition,
                    new Dictionary<string, object> {
                { "EnemyPosition", enemyPosition },
                { "PlayerPosition", playerPosition },
                { "Distance", Vector2D.Distance(enemyPosition, playerPosition) }
                    }
                );

                // Hide enemy sprite by setting its scale to zero
                InternalCalls.Transform_SetScale(entity.ID, ref ZeroSize);
                AttackPlayer(entity, playerEntity);
                // show enemy sprite by setting its scale to default
                InternalCalls.Transform_SetScale(entity.ID, ref Defaultsize); 
                InternalCalls.UnitComponent_SetIsMoving(entity.ID, false);
                UnitManager.Instance.CheckAndHandleEnemyExhaustion();
                return;
            }

            // Move on to the next tile in the path
            int previousPathIndex = enemy.currentPathIndex;
            enemy.currentPathIndex++;

            // Send path progression message
            GameMessageHub.Instance.SendMessage(
                GameMessageType.EnemyMovementUpdate,
                entity,
                null,
                enemy.currentPathIndex,
                tilePosition,
                new Dictionary<string, object> {
            { "PreviousPathIndex", previousPathIndex },
            { "CurrentPathIndex", enemy.currentPathIndex },
            { "PathLength", enemy.movementPath.Count },
            { "RemainingEnergy", currentEnergy }
                }
            );

            // If we've reached the end or have no energy, end movement
            if (enemy.currentPathIndex >= enemy.movementPath.Count || currentEnergy <= 0)
            {
                string reason = enemy.currentPathIndex >= enemy.movementPath.Count ?
                    "PathCompleted" : "EnergyDepleted";

                Debug.LogInfo("[Enemy] Movement finished or out of energy. Stopping. Reason: {0}", reason);

                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyMovementFinished,
                    entity,
                    null,
                    currentEnergy,
                    tilePosition,
                    reason
                );

                InternalCalls.UnitComponent_SetIsMoving(entity.ID, false);
                UnitManager.Instance.CheckAndHandleEnemyExhaustion();
            }
        }

        /**
         * @brief Triggers an attack on the player.
         *
         * Decreases enemy energy, reduces the player's health based on the enemy's attack power,
         * and plays the corresponding attack animations.
         *
         * @param enemyEntity The enemy entity performing the attack.
         * @param playerEntity The player entity being attacked.
         */
        private static void AttackPlayer(Entity enemyEntity, Entity playerEntity)
        {
            string currentTexture = InternalCalls.MaterialComponent_GetTextureType(enemyEntity.ID);
            Debug.LogInfo("[Enemy] Enemy {0} is attacking Player {1}.", enemyEntity.ID, playerEntity.ID);

            // Get positions for attack message
            Vector2D enemyPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(enemyEntity.ID, ref enemyPosition);

            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);


            // Get and update enemy energy
            int currentEnergy = InternalCalls.UnitComponent_GetEnergy(enemyEntity.ID);
            if (currentEnergy > 0)
            {
                int previousEnergy = currentEnergy;
                currentEnergy--;
                InternalCalls.UnitComponent_SetEnergy(enemyEntity.ID, currentEnergy);

                // Send energy change notification
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyEnergyChanged,
                    enemyEntity,
                    null,
                    currentEnergy,
                    enemyPosition,
                    new Dictionary<string, object> {
                { "PreviousEnergy", previousEnergy },
                { "CurrentEnergy", currentEnergy },
                { "Reason", "AttackAction" }
                    }
                );

                // Get attack info and update player health
                int attackPower = InternalCalls.UnitComponent_GetAttackPower(enemyEntity.ID);
                int previousPlayerHealth = InternalCalls.UnitComponent_GetHealth(playerEntity.ID);
                int playerHealth = previousPlayerHealth - attackPower;

                // Send attack started message before changing player health
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyAttacked,
                    enemyEntity,
                    playerEntity,
                    attackPower,
                    enemyPosition,
                    new Dictionary<string, object> {
                { "PlayerCurrentHealth", previousPlayerHealth },
                { "PlayerNewHealth", playerHealth },
                { "AttackPower", attackPower },
                { "Distance", Vector2D.Distance(enemyPosition, playerPosition) }
                    }
                );



                // Play enemy attack animation
                if (currentTexture == "Enemy")
                {
                    InternalCalls.AnimationComponent_PlayAnimation(enemyEntity.ID, "Pickle's Bite");
                }
                else if (currentTexture == "Enemy_2")
                {
                    InternalCalls.AnimationComponent_PlayAnimation(enemyEntity.ID, "Bloopie Bite");
                }
                CameraShake.Shake(0.5f, 100.0f);

                // **Check if enemy has died after the attack**
                if (InternalCalls.UnitComponent_GetHealth(enemyEntity.ID) <= 0)
                {
                    Debug.LogInfo("[Enemy] Enemy {0} has died! Playing death sound.", enemyEntity.ID);

                    // Stop any ongoing attack/damage sounds before playing death sound
                    if (!string.IsNullOrEmpty(lastPlayedDamageSound))
                    {
                        InternalCalls.Audio_StopSound(lastPlayedDamageSound);
                        lastPlayedDamageSound = ""; // Reset
                    }
                    InternalCalls.Audio_StopSound(attackSoundNames[attackSoundIndex]);

                    // Play death sound
                    InternalCalls.Audio_PlaySound(deathsoundName[deathsoundIndex], 0.6f);
                    deathsoundIndex = (deathsoundIndex + 1) % deathsoundName.Length;

                    // **Create a GameMessage to notify the system that the enemy has died**
                    GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyDied,  // Message Type: Enemy Death
                    enemyEntity,                // Target: The dying enemy
                    null,                       // No specific source
                    0,                          // No numerical value needed
                    enemyPosition,              // Position of the enemy
                    new Dictionary<string, object> {
                        { "EnemyID", enemyEntity.ID },
                        { "Reason", "HealthReachedZero" },
                        { "LastDamageTaken", attackPower }, // Include the last attack that killed it
                        { "CurrentHealth", InternalCalls.UnitComponent_GetHealth(enemyEntity.ID) }
                    }
                    );
                }
                else
                {
                    Debug.LogInfo("[Enemy] Enemy {0} took {1} damage, remaining health: {2}",
                                  enemyEntity.ID, attackPower, InternalCalls.UnitComponent_GetHealth(enemyEntity.ID));

                    // Play damage sound only if enemy is still alive
                    lastPlayedDamageSound = attackSoundNames[attackSoundIndex]; // Store last damage sound
                    InternalCalls.Audio_PlaySound(lastPlayedDamageSound, 0.2f);
                    attackSoundIndex = (attackSoundIndex + 1) % attackSoundNames.Length;
                }


                // Send animation/sound message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.AnimationStateChanged,
                    enemyEntity,
                    playerEntity,
                    attackSoundIndex,
                    enemyPosition,
                    "EnemyAttackAnimationStarted"
                );

                // Update player health after attack
                InternalCalls.UnitComponent_SetHealth(playerEntity.ID, playerHealth);

                // Send player health changed message
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PlayerHealthChanged,
                    enemyEntity,
                    playerEntity,
                    playerHealth,
                    playerPosition,
                    new Dictionary<string, object> {
                { "PreviousHealth", previousPlayerHealth },
                { "CurrentHealth", playerHealth },
                { "DamageTaken", attackPower },
                { "AttackerID", enemyEntity.ID }
                    }
                );

                Debug.LogInfo("[Enemy] Player {0} took {1} damage. Remaining health: {2}",
                              playerEntity.ID, attackPower, playerHealth);

                // Register for animation completion via a message in the future
                // Set a timeout to restore the enemy sprite after animation completes
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.TimedAction,
                    enemyEntity,
                    null,
                    1, // 1 second timeout (adjust as needed)
                    null,
                    "RestoreEnemySprite"
                );

                // Now you need to handle the timed action message in your update method
                // (we'll add the handler code next)

                // Check if enemy has no more energy
                if (currentEnergy == 0)
                {
                    Debug.LogWarning("[Enemy] Enemy {0} has no more energy after attack.", enemyEntity.ID);

                    GameMessageHub.Instance.SendMessage(
                        GameMessageType.EnemyActionFinished,
                        enemyEntity,
                        null,
                        0,
                        enemyPosition,
                        "EnemyOutOfEnergy"
                    );

                    UnitManager.Instance.NotifyEnemyFinished(enemyEntity);  // Use the encapsulated method
                    UnitManager.Instance.CheckAndHandleEnemyExhaustion();
                }
            }
            else
            {
                // Handle case where enemy tries to attack with no energy
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.GameWarning,
                    enemyEntity,
                    playerEntity,
                    currentEnergy,
                    enemyPosition,
                    "EnemyTriedToAttackWithNoEnergy"
                );
            }
        }

        /**
         * @brief Checks whether the enemy is adjacent to the player.
         *
         * Uses a simple 4-direction adjacency check based on a fixed spacing (e.g., 110 units).
         *
         * @param enemyEntity The enemy entity.
         * @param playerEntity The player entity.
         * @return True if the enemy is adjacent to the player; otherwise, false.
         */
        private static bool IsAdjacentToPlayer(Entity enemyEntity, Entity playerEntity)
        {
            if (playerEntity == null || playerEntity.ID == 0)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PathCheck,
                    enemyEntity,
                    null,
                    0,
                    null,
                    "AdjacentCheckFailed_InvalidPlayer"
                );
                return false;
            }

            Vector2D enemyPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(enemyEntity.ID, ref enemyPosition);
            var enemyTile = TileManager.Instance.GetTileAtPosition(enemyPosition);

            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);
            var playerTile = TileManager.Instance.GetTileAtPosition(playerPosition);

            if (enemyTile == null || playerTile == null)
            {
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.PathCheck,
                    enemyEntity,
                    playerEntity,
                    0,
                    null,
                    "AdjacentCheckFailed_InvalidTiles"
                );
                return false;
            }

            enemyPosition = TileManager.Instance.GetTileCenter(enemyTile);
            playerPosition = TileManager.Instance.GetTileCenter(playerTile);

            // Calculate tile differences
            float xDiff = Math.Abs(enemyPosition.X - playerPosition.X);
            float yDiff = Math.Abs(enemyPosition.Y - playerPosition.Y);

            // Simple 4-direction adjacency check for 110x110 spacing
            bool isAdjacent = (xDiff == 110 && enemyPosition.Y == playerPosition.Y) ||
                              (yDiff == 110 && enemyPosition.X == playerPosition.X);

            // Send adjacency check result for debugging and monitoring
            GameMessageHub.Instance.SendMessage(
                GameMessageType.PathCheck,
                enemyEntity,
                playerEntity,
                isAdjacent ? 1 : 0,
                null,
                new Dictionary<string, object> {
            { "EnemyPosition", enemyPosition },
            { "PlayerPosition", playerPosition },
            { "XDifference", xDiff },
            { "YDifference", yDiff },
            { "IsAdjacent", isAdjacent },
            { "EnemyTileID", enemyTile.ID },
            { "PlayerTileID", playerTile.ID }
                }
            );

            return isAdjacent;
        }

        // ----------------------[ ANIMATION + SPRITE ]----------------------

        /**
    * @brief Updates the enemy's facing direction based on player position.
    *
    * Compares the X positions of the enemy and player, then updates the enemy's
    * isFacingLeft flag and visual representation accordingly. Uses the message
    * system for logging and notifying other components about direction changes.
    *
    * @param enemyEntity The enemy entity to update.
    * @param enemyInstance The Enemy behavior instance.
    */

        private static void UpdateEnemyFacingDirection(Entity enemyEntity, Enemy enemyInstance)
        {
            // Only update facing for alive enemies
            if (!InternalCalls.UnitComponent_GetIsAlive(enemyEntity.ID))
                return;

            Entity playerEntity = UnitManager.Instance.GetPlayerEntity();
            if (playerEntity == null)
                return;

            // Get position data for player
            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);

            // Get current position of enemy
            Vector2D enemyPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(enemyEntity.ID, ref enemyPosition);

            // Check if the enemy should change facing direction
            bool shouldFaceLeft = playerPosition.X < enemyPosition.X;

            // Remember the previous facing state before updating
            enemyInstance.prevFacing = enemyInstance.isFacingLeft;

            // Only send a status message if positions have changed significantly OR this is the first time
            bool positionsChanged =
                enemyInstance._lastReportedPlayerPosition == null ||
                enemyInstance._lastReportedEnemyPosition == null ||
                !playerPosition.Equals(enemyInstance._lastReportedPlayerPosition) ||
                !enemyPosition.Equals(enemyInstance._lastReportedEnemyPosition) ||
                shouldFaceLeft != enemyInstance._lastReportedFacingDirection;

            if (positionsChanged)
            {
                // Send general position info message ONLY when something has changed
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.EnemyStatus,
                    enemyEntity,
                    playerEntity,
                    0,
                    null,
                    new Dictionary<string, object> {
                { "EnemyPosition", enemyPosition },
                { "PlayerPosition", playerPosition },
                { "CurrentFacing", enemyInstance.isFacingLeft ? "LEFT" : "RIGHT" }
                    }
                );

                // Update tracked positions
                enemyInstance._lastReportedPlayerPosition = playerPosition;
                enemyInstance._lastReportedEnemyPosition = enemyPosition;
                enemyInstance._lastReportedFacingDirection = shouldFaceLeft;
            }

            // Only update if there's an actual change in facing direction
            if (shouldFaceLeft != enemyInstance.isFacingLeft)
            {
                // Store previous facing direction for detailed logging
                bool previousFacing = enemyInstance.isFacingLeft;
                enemyInstance.isFacingLeft = shouldFaceLeft;

                // Send direction change message (this is fine as is - only happens on actual change)
                GameMessageHub.Instance.SendMessage(
                    GameMessageType.DirectionChange,
                    enemyEntity,
                    playerEntity,
                    shouldFaceLeft ? 1 : 0,
                    null,
                    new Dictionary<string, object> {
                { "PreviousFacing", previousFacing ? "LEFT" : "RIGHT" },
                { "NewFacing", shouldFaceLeft ? "LEFT" : "RIGHT" },
                { "EnemyPosition", enemyPosition },
                { "PlayerPosition", playerPosition }
                    }
                );
            }
        }

    }
}