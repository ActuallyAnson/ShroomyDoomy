/******************************************************************************
/*!
\file  TileManager.cs
\project  Shroomy Doomy
\author  Benjamin
\date    Mar 14, 2025
\brief  
       This file implements the TileManager class, which is responsible for managing 
       the tile grid, handling tile interactions, and facilitating movement and pathfinding 
       within the game world.

       The TileManager:
       - Initializes and maintains a dictionary of tiles with their corresponding positions.
       - Handles pathfinding (A* Algorithm) to determine valid movement paths.
       - Highlights tiles for movement and enemy attack zones using BFS.
       - Manages tile-based events such as level completion and interaction with special tiles.
       - Keeps track of player and enemy movements on the grid.

       Additionally, the TileManager integrates with the game's entity system and manages
       tile-based highlights to indicate movement possibilities and enemy attack range.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;
using System.IO;

namespace FruitLoops
{
    /**
     * @brief Manages the tile grid and tile-based interactions in the game world.
     * 
     * @note Implements a singleton pattern to maintain a single instance, handling grid 
     * initialization, A* pathfinding, BFS-based tile highlighting, and level completion 
     * logic. Inherits from BehaviourComponent.
     */
    public class TileManager : BehaviourComponent
    {
        private readonly Dictionary<Vector2D, Entity> _gridTiles = new Dictionary<Vector2D, Entity>();

        /** @brief Gets the singleton instance of TileManager. */
        public static TileManager Instance { get; private set; }

        private List<Entity> _moveToTileEntities = new List<Entity>();
        private Entity _tileHighlightEntity;
        private Entity _holeEntity;
        private Entity _endgoalTile;
        private Entity _tutorialBackground;
		private Entity _tutorialChest;
		private bool _isLevelCompleted = false;

        /**
         * @brief Default constructor for TileManager.
         */
        public TileManager() { }

        /**
         * @brief Private constructor for TileManager to enforce singleton pattern.
         * 
         * @param entity The Entity to associate with this TileManager instance.
         * @note Registers the instance as a behavior with GameLogicSystem; throws an exception 
         * if another instance already exists.
         */
        private TileManager(Entity entity)
        {
            if (Instance != null)
                throw new Exception("[TileManager] Only one TileManager instance is allowed.");

            Instance = this;

            Debug.LogInfo("[TileManager] Creating TileManager for entity {0}", entity.ID);

            var behaviour = new BehaviourFCT(entity, Init, Update, End);
            GameLogicSystem.Instance.RegisterLogicComponent(this);

            SetBehaviourIndex(GameLogicSystem.Instance.Behaviours.Count - 1);
            SetOwner(entity);

            Debug.LogInfo("[TileManager] TileManager registered with index {0}", GetBehaviourIndex());
        }

        /**
         * @brief Creates a new TileManager behavior instance for an entity.
         * 
         * @param entity The Entity to associate with the TileManager behavior.
         * @return A new TileManager instance, or null if an exception occurs.
         * @note Wraps construction in a try-catch for error handling.
         */
        public static BehaviourComponent CreateBehaviour(Entity entity)
        {
            try
            {
                Debug.LogInfo("[TileManager] [CreateBehaviour] Starting for TileManager on entity {0}", entity.ID);
                return new TileManager(entity);
            }
            catch (Exception ex)
            {
                Debug.LogError("[TileManager] [CreateBehaviour] Exception occurred: {0}", ex.Message);
                return null;
            }
        }

        /**
         * @brief Initializes the TileManager for an entity.
         * 
         * @param entity The Entity associated with the TileManager behavior.
         * @note Initializes the tile grid and resets level completion state.
         */
        public static void Init(Entity entity)
        {
            Debug.LogInfo("[TileManager] Initializing TileManager for entity {0}", entity.ID);
            Instance?.InitializeGrid();
            if (Instance != null)
            {
                Instance._isLevelCompleted = false; // Reset level completion flag
            }
        }

        /**
         * @brief Updates the TileManager state.
         * 
         * @param entity The Entity associated with the TileManager behavior.
         * @param deltaTime The time elapsed since the last update in seconds.
         * @note Manages tile highlighting, player movement, enemy hover highlights, and level 
         * completion checks; skips updates if certain UI layers are active.
         */
        public static void Update(Entity entity, float deltaTime)
        {
            // If the pause/main/story menu is currently active, do NOT process any of the player's logic
            if (InternalCalls.LevelSwitch_GetLayerStatus("PauseMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("MainMenuLayer") ||
                InternalCalls.LevelSwitch_GetLayerStatus("StoryLayer"))
                return;

            Instance?.UpdateTileHighlight(entity);

            if (entity == null)
            {
                Debug.LogError("[TileManager] Update called with null entity.");
                return;
            }

            var playerEntity = UnitManager.Instance.GetPlayerEntity();
            if (playerEntity == null)
            {
                Debug.LogError("[TileManager] Player entity not found during update.");
                return;
            }

            if (Instance == null)
            {
                Debug.LogError("[TileManager] TileManager instance is null.");
                return;
            }

            var playerBehaviour = GameLogicSystem.Instance.GetBehaviour<Player>(playerEntity);
            bool playerMoving = playerBehaviour.isMoving;
            if (UnitManager.Instance.CurrentTurn == TurnType.PlayerTurn && !playerMoving)
            {
                Instance.HighlightReachableTiles(playerEntity);
                // Check if the player has reached the end goal tile
                Vector2D playerPosition = new Vector2D();
                InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);
                Entity playerTile = Instance.GetTileAtPosition(playerPosition);

                //if (playerTile == Instance.GetEndGoalTile()) // If player reaches the end tile
                //{
                //    Debug.LogInfo("[TileManager] Player reached the end goal. Level completed!");
                //    Instance.SetLevelCompleted(true); // Mark level as completed
                //}
            }
            else
            {
                Instance.ClearAllHighlights();
            }

            var enemies = UnitManager.Instance.GetEnemies();
            bool anyEnemyHovered = false;

            foreach (var enemy in enemies)
            {
                bool isHovered = Input.IsMouseOverEntity(enemy.ID);
                if (isHovered)
                {
                    // Highlight BFS for this hovered enemy
                    Instance.HighlightEnemyTiles(enemy);
                    Debug.LogInfo("[TileManager] HighlightEnemyTiles for {0}.", enemy.ID);
                    anyEnemyHovered = true;
                }
            }

            if (!anyEnemyHovered)
            {
                Instance.ClearAllEnemyHighlights();
            }
        }

        /**
         * @brief Cleans up the TileManager instance.
         * 
         * @param entity The Entity associated with the TileManager behavior.
         * @note Clears the grid tiles dictionary and nullifies the singleton instance.
         */
        public static void End(Entity entity)
        {
            Instance?._gridTiles.Clear();
            Instance = null;
            Debug.LogInfo("[TileManager] TileManager cleanup complete for entity {0}.", entity.ID);
        }

        /**
         * @brief Initializes the tile grid by populating it with tile entities.
         * 
         * @note Links tiles with their neighbors and identifies the end goal tile based on the hole entity's position.
         */
        private void InitializeGrid()
        {
            var allTileEntities = Entity.GetEntitiesWithECSComponentName("struct FruitLoops::TileComponent");

            // 1. Get the hole entity
            _holeEntity = new Entity(InternalCalls.Entity_GetEntityByName("hole"));
            if (_holeEntity == null || _holeEntity.ID == 0)
            {
                Debug.LogError("[TileManager] Hole entity not found or invalid. Cannot designate end goal.");
                return;
            }

            // 2. Retrieve hole entity position
            Vector2D holePosition = new Vector2D();
            InternalCalls.Transform_GetTranslation(_holeEntity.ID, out holePosition);

            // 3. Populate _gridTiles and find the end goal
            foreach (var tileEntity in allTileEntities)
            {
                // Retrieve tile position
                Vector2D tilePosition = new Vector2D();
                InternalCalls.TileComponent_GetPosition(tileEntity.ID, out tilePosition);

                // Add tile to dictionary
                if (!_gridTiles.ContainsKey(tilePosition))
                {
                    _gridTiles[tilePosition] = tileEntity;
                }
                else
                {
                    Debug.LogWarning("[TileManager] Duplicate tile detected at position {0}", tilePosition);
                }

                // Check if this tile matches the hole's position
                if (Math.Abs(tilePosition.X - holePosition.X) < 0.01f
                    && Math.Abs(tilePosition.Y - holePosition.Y) < 0.01f)
                {
                    _endgoalTile = tileEntity;
                    Debug.LogInfo($"[TileManager] Found end goal tile at position {tilePosition}, Entity: {tileEntity.ID}");
                }
            }

            Debug.LogInfo("[TileManager] Linking neighbors...");
            LinkNeighbors();
        }

        /**
         * @brief Links each tile with its neighboring tiles.
         * 
         * @note Uses predefined offsets to identify adjacent tiles and establishes neighbor relationships.
         */
        private void LinkNeighbors()
        {
            foreach (var kvp in _gridTiles)
            {
                var position = kvp.Key;
                var tileEntity = kvp.Value;

                if (tileEntity.ID == 0)
                {
                    Debug.LogWarning("[TileManager] Skipping invalid tile entity with ID 0 at position {0}.", position);
                    continue;
                }

                InternalCalls.TileComponent_ClearNeighbors(tileEntity.ID);

                Vector2D[] neighborOffsets = new Vector2D[]
                {
                    new Vector2D(0, 110),   // Up
                    new Vector2D(110, 0),   // Right
                    new Vector2D(0, -110),  // Down
                    new Vector2D(-110, 0)   // Left
                };

                foreach (var offset in neighborOffsets)
                {
                    var neighborPos = position + offset;

                    if (_gridTiles.TryGetValue(neighborPos, out var neighborEntity))
                    {
                        if (neighborEntity.ID == 0)
                        {
                            continue;
                        }

                        InternalCalls.TileComponent_AddNeighbor(tileEntity.ID, neighborEntity.ID);
                    }
                }
            }
        }

        /**
         * @brief Checks if a tile is walkable.
         * 
         * @param tileEntity The Entity representing the tile to check.
         * @return True if the tile is walkable, false otherwise.
         */
        public bool IsTileWalkable(Entity tileEntity)
        {
            return InternalCalls.TileComponent_GetIsWalkable(tileEntity.ID);
        }

        /**
         * @brief Gets the center position of a tile.
         * 
         * @param tileEntity The Entity representing the tile.
         * @return The Vector2D position of the tile's center.
         */
        public Vector2D GetTileCenter(Entity tileEntity)
        {
            InternalCalls.TileComponent_GetPosition(tileEntity.ID, out Vector2D tilePosition);
            return tilePosition;
        }

        /**
         * @brief Retrieves all walkable tiles in the grid.
         * 
         * @return A List of Entity objects representing walkable tiles.
         */
        public List<Entity> GetWalkableTiles()
        {
            var walkableTiles = new List<Entity>();
            foreach (var entity in _gridTiles.Values)
            {
                if (InternalCalls.TileComponent_GetIsWalkable(entity.ID))
                {
                    walkableTiles.Add(entity);
                }
            }
            return walkableTiles;
        }

        /**
         * @brief Retrieves all tiles in the grid.
         * 
         * @return A List of Entity objects representing all tiles.
         */
        public List<Entity> GetAllTiles()
        {
            return new List<Entity>(_gridTiles.Values); // Return all tiles as a list
        }

        /**
         * @brief Gets the end goal tile.
         * 
         * @return The Entity representing the end goal tile, set during grid initialization.
         */
        public Entity GetEndGoalTile()
        {
            return _endgoalTile;
        }

        /**
         * @brief Retrieves the tile at a specific position.
         * 
         * @param position The Vector2D position to query.
         * @return The Entity at the given position, or null if none exists.
         */
        public Entity GetTileAtPosition(Vector2D position)
        {
            if (_gridTiles.TryGetValue(position, out var entity))
            {
                return entity;
            }
            return null;
        }

        /**
         * @brief Gets the occupant of a tile at a specific position.
         * 
         * @param position The Vector2D position to query.
         * @return The Entity occupying the tile, or null if unoccupied or no tile exists.
         */
        public Entity GetOccupant(Vector2D position)
        {
            if (_gridTiles.TryGetValue(position, out var entity))
            {
                ulong occupantID = InternalCalls.TileComponent_GetOccupant(entity.ID);
                return occupantID != 0 ? new Entity(occupantID) : null;
            }
            return null;
        }

        /**
         * @brief Checks if a target entity is on a walkable tile.
         * 
         * @param target The Entity to check.
         * @return True if the target is on a walkable tile, false otherwise.
         */
        public bool IsTargetOnWalkableTile(Entity target)
        {
            Entity targetTile = GetTileAtPosition(GetTileCenter(target));

            if (targetTile == null)
            {
                Debug.LogWarning("[IsTargetOnWalkableTile] Target is not on a valid tile.");
                return false;
            }

            bool isWalkable = IsTileWalkable(targetTile);

            if (!isWalkable)
            {
                Debug.LogWarning($"[IsTargetOnWalkableTile] Target {target.ID} is on a non-walkable tile {targetTile.ID}.");
            }

            return isWalkable;
        }

        // -----------------------[ ASTAR PATHFINDING METHODS ]----------------------

        /**
         * @brief Executes A* pathfinding between two tiles.
         * 
         * @param startTileEntity The starting tile Entity.
         * @param endTileEntity The destination tile Entity.
         * @param allowNonWalkableEnd If true, allows the end tile to be non-walkable.
         * @return A List of Entity objects representing the path from start to end.
         * @note Uses Manhattan distance as heuristic; returns empty list if no path is found.
         */
        public List<Entity> ExecuteAStarPathfinding(Entity startTileEntity, Entity endTileEntity, bool allowNonWalkableEnd = false)
        {
            var path = new List<Entity>();

            if (startTileEntity == null || endTileEntity == null)
            {
                Debug.LogWarning("[TileManager] Invalid start or end tile for pathfinding.");
                return path;
            }

            if (startTileEntity == endTileEntity)
            {
                path.Add(startTileEntity);
                Debug.LogInfo("[TileManager] Start and end tile are the same: Tile {0}.", startTileEntity.ID);
                return path;
            }

            var cameFrom = new Dictionary<Entity, Entity>();
            var gScore = new Dictionary<Entity, float> { [startTileEntity] = 0 };
            var fScore = new Dictionary<Entity, float> { [startTileEntity] = Heuristic(startTileEntity, endTileEntity) };

            var openSet = new PriorityQueue<Entity>();
            openSet.Enqueue(startTileEntity, fScore[startTileEntity]);

            while (openSet.Count > 0)
            {
                var (current, _) = openSet.Dequeue(); // Ignore the priority value

                if (current == endTileEntity)
                {
                    while (current != null)
                    {
                        path.Add(current);
                        current = cameFrom.ContainsKey(current) ? cameFrom[current] : null;
                    }

                    path.Reverse();
                    return path;
                }

                InternalCalls.TileComponent_GetPosition(current.ID, out Vector2D currentPosition);

                Vector2D[] neighborOffsets = {
                    new Vector2D(0, 110),   // Up
                    new Vector2D(110, 0),   // Right
                    new Vector2D(0, -110),  // Down
                    new Vector2D(-110, 0)   // Left
                };

                foreach (var offset in neighborOffsets)
                {
                    var neighborPosition = currentPosition + offset;
                    var neighbor = TileManager.Instance.GetTileAtPosition(neighborPosition);

                    if (neighbor == null || (!TileManager.Instance.IsTileWalkable(neighbor) && neighbor != endTileEntity && !allowNonWalkableEnd))
                        continue;

                    float tentativeGScore = gScore[current] + 1;

                    if (!gScore.ContainsKey(neighbor) || tentativeGScore < gScore[neighbor])
                    {
                        cameFrom[neighbor] = current;
                        gScore[neighbor] = tentativeGScore;
                        fScore[neighbor] = tentativeGScore + Heuristic(neighbor, endTileEntity);

                        if (!openSet.Contains(neighbor))
                        {
                            openSet.Enqueue(neighbor, fScore[neighbor]);
                        }
                    }
                }
            }

            Debug.LogWarning("[TileManager] Pathfinding failed. No path found.");
            return path;
        }

        /**
         * @brief Calculates the heuristic cost between two tiles using Manhattan distance.
         * 
         * @param a The first tile Entity.
         * @param b The second tile Entity.
         * @return The heuristic cost as a float.
         */
        private float Heuristic(Entity a, Entity b)
        {
            var aPos = GetTileCenter(a);
            var bPos = GetTileCenter(b);
            return Math.Abs(aPos.X - bPos.X) + Math.Abs(aPos.Y - bPos.Y); // Manhattan distance
        }

        /**
         * @brief Clears the occupant of a tile at a specific position.
         * 
         * @param position The Vector2D position of the tile to clear.
         */
        public void ClearOccupant(Vector2D position)
        {
            if (_gridTiles.TryGetValue(position, out var tileEntity))
            {
                InternalCalls.TileComponent_SetOccupant(tileEntity.ID, 0);
                Debug.LogInfo("[TileManager] Cleared occupant of tile at position {0}", position);
            }
            else
            {
                Debug.LogWarning("[TileManager] No tile found at position {0} to clear occupant.", position);
            }
        }

        // -----------------------[ BFS HIGHLIGHT METHODS ]----------------------

        private Entity _lastHoveredTile = null; // Store last hovered tile
        private bool _isHighlightingReachableTiles = false; // Track if reachable tiles are being highlighted

        /**
         * @brief Updates tile highlighting based on mouse hover and player reachability.
         * 
         * @param tileEntity The Entity associated with the TileManager behavior.
         * @note Highlights reachable tiles and paths during player's turn; plays hover sound when applicable.
         */
        private void UpdateTileHighlight(Entity tileEntity)
        {
            // Ensure it's the player's turn and reachable tiles are being highlighted
            if (UnitManager.Instance.CurrentTurn != TurnType.PlayerTurn || !_isHighlightingReachableTiles)
            {
                ClearPathHighlights();
                HideTileHighlight();
                return;
            }

            bool anyTileHovered = false;
            bool anyReachableTileHovered = false;

            _tileHighlightEntity = new Entity(InternalCalls.Entity_GetEntityByName("TileHighlight"));

            // Ensure player entity exists
            var playerEntity = UnitManager.Instance.GetPlayerEntity();
            if (playerEntity == null)
            {
                Debug.LogWarning("[TileManager] Player entity not found.");
                return;
            }

            _tutorialBackground = new Entity(InternalCalls.Entity_GetEntityByName("TutorialBackScreenButton"));
			_tutorialChest = new Entity(InternalCalls.Entity_GetEntityByName("ChesterChicken"));

			// Get reachable tiles and player tile
			List<Entity> reachableTiles = GetReachableTiles(playerEntity);
            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);
            Entity playerTile = GetTileAtPosition(playerPosition);

            Entity finalHoveredTile = null;
            List<Entity> pathToHoveredTile = new List<Entity>();

			// Iterate over all tiles to detect hover state
			foreach (var tile in _gridTiles.Values)
            {
                if (InternalCalls.Input_IsMouseOverEntity(tile.ID))
                {
					if (!_isLevelCompleted &&
						tile.ID != 0 &&
						tile.ID != _tutorialBackground.ID &&
						tile.ID != _tutorialChest.ID &&
						(_lastHoveredTile == null || _lastHoveredTile.ID != tile.ID))
					{
						float hoverVolume = 0.002f; // Adjust this if needed
                        if (hoverVolume > 0.001f)  // Prevent playing sound at zero volume
                        {
                            InternalCalls.Audio_PlaySound("SFX_TileHover", hoverVolume);
                        }
                        _lastHoveredTile = tile; // Update last hovered tile
                    }

                    HighLightTile(tile, _tileHighlightEntity);
                    anyTileHovered = true;

                    // Identify final hovered tile if it is within reachable tiles
                    if (reachableTiles.Contains(tile) && tile.ID != playerTile.ID)
                    {
                        finalHoveredTile = tile;
                        anyReachableTileHovered = true;
                    }
                }
            }

            // If a valid hovered tile exists, compute path and highlight it
            if (finalHoveredTile != null)
            {
                pathToHoveredTile = ExecuteAStarPathfinding(playerTile, finalHoveredTile, allowNonWalkableEnd: false);
                HighlightPath(pathToHoveredTile, playerTile, finalHoveredTile);
            }
            else
            {
                ClearPathHighlights();
            }

            // If no tile is hovered, hide TileHighlight
            if (!anyTileHovered)
            {
                HideTileHighlight();
            }

            // If no reachable tile is hovered, hide MoveToTileHighlight
            if (!anyReachableTileHovered)
            {
                ClearPathHighlights();
            }
        }

        /**
         * @brief Highlights the path to a hovered tile during player movement.
         * 
         * @param path The List of Entity objects representing the path.
         * @param playerTile The Entity representing the player's current tile.
         * @param hoveredTile The Entity representing the hovered tile.
         * @note Uses MoveToTileHighlight entities to visualize the path, scaling dots smaller except for the destination.
         */
        private void HighlightPath(List<Entity> path, Entity playerTile, Entity hoveredTile)
        {
            // Ensure we have enough MoveToTileHighlight entities for the path length (excluding player tile)
            while (_moveToTileEntities.Count < path.Count - 1)
            {
                string entityName = $"MoveToTileHighlight{_moveToTileEntities.Count:00}";
                Entity newHighlight = new Entity(InternalCalls.Entity_GetEntityByName(entityName));

                if (newHighlight.ID == 0)
                {
                    Debug.LogError($"[TileManager] {entityName} entity not found.");
                    return;
                }

                _moveToTileEntities.Add(newHighlight);
            }

            int highlightIndex = 0;

            // Position and scale each moveDot entity correctly along the path
            foreach (var pathTile in path)
            {
                if (pathTile.ID == playerTile.ID) continue; // Skip highlighting the player's current tile

                if (highlightIndex >= _moveToTileEntities.Count)
                    break;

                Entity highlightEntity = _moveToTileEntities[highlightIndex];

                // Make all dots small except the last one (hovered tile)
                bool isLastTile = pathTile.ID == hoveredTile.ID;
                HighLightTile(pathTile, highlightEntity, smallerScale: !isLastTile);

                highlightIndex++;
            }

            // Hide any extra highlight entities not used in the current path
            for (int i = highlightIndex; i < _moveToTileEntities.Count; i++)
            {
                Vector2D zeroScale = Vector2D.Zero;
                InternalCalls.Transform_SetScale(_moveToTileEntities[i].ID, ref zeroScale);
            }
        }

        /**
         * @brief Clears all path highlights.
         * 
         * @note Sets the scale of all MoveToTileHighlight entities to zero.
         */
        private void ClearPathHighlights()
        {
            foreach (var highlightEntity in _moveToTileEntities)
            {
                Vector2D zeroScale = Vector2D.Zero;
                InternalCalls.Transform_SetScale(highlightEntity.ID, ref zeroScale);
            }
        }

        /**
         * @brief Highlights an individual tile with a specified highlight entity.
         * 
         * @param tileEntity The Entity representing the tile to highlight.
         * @param highlightEntity The Entity used to display the highlight.
         * @param smallerScale If true, reduces the highlight scale by half.
         */
        public void HighLightTile(Entity tileEntity, Entity highlightEntity, bool smallerScale = false)
        {
            InternalCalls.Transform_GetTranslation(tileEntity.ID, out Vector2D tilePosition);
            InternalCalls.Transform_GetScale(tileEntity.ID, out Vector2D tileScale);

            if (smallerScale)
            {
                tileScale *= 0.5f; // Make all path dots smaller, except the last one
            }

            InternalCalls.Transform_SetTranslation(highlightEntity.ID, ref tilePosition);
            InternalCalls.Transform_SetScale(highlightEntity.ID, ref tileScale);
        }

        /**
         * @brief Sets the level completion state.
         * 
         * @param completed True to mark the level as completed, false otherwise.
         */
        public void SetLevelCompleted(bool completed)
        {
            _isLevelCompleted = completed;
            Debug.LogInfo("[TileManager] Level completion state updated: " + completed);
        }

        /**
         * @brief Hides the tile highlight entity.
         * 
         * @note Sets the scale of the TileHighlight entity to zero.
         */
        private void HideTileHighlight()
        {
            if (_tileHighlightEntity == null)
            {
                _tileHighlightEntity = new Entity(InternalCalls.Entity_GetEntityByName("TileHighlight"));
                if (_tileHighlightEntity == null)
                {
                    Debug.LogError("[TileManager] Level1_Game_TileHighlight entity not found.");
                    return;
                }
            }

            Vector2D zeroScale = Vector2D.Zero;
            InternalCalls.Transform_SetScale(_tileHighlightEntity.ID, ref zeroScale);
        }

		public void HideAllTileHighlights()
		{
			ClearPathHighlights();
			HideTileHighlight();
		}

		/**
         * @brief Gets the list of tiles reachable by the player based on energy.
         * 
         * @param playerEntity The Entity representing the player.
         * @return A List of Entity objects representing reachable tiles.
         * @note Uses BFS to determine reachable tiles within the player's energy limit.
         */
		private List<Entity> GetReachableTiles(Entity playerEntity)
        {
            List<Entity> reachableTiles = new List<Entity>();

            if (playerEntity == null)
                return reachableTiles;

            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);
            Entity playerTile = GetTileAtPosition(playerPosition);

            if (playerTile == null)
                return reachableTiles;

            int playerEnergy = InternalCalls.UnitComponent_GetEnergy(playerEntity.ID);
            if (playerEnergy <= 0)
                return reachableTiles;

            PriorityQueue<Entity> queue = new PriorityQueue<Entity>();
            queue.Enqueue(playerTile, -playerEnergy); // Start BFS
            reachableTiles.Add(playerTile);

            while (queue.Count > 0)
            {
                var (currentTile, currentPriority) = queue.Dequeue();
                int remainingEnergy = (int)(-currentPriority);
                if (remainingEnergy <= 0) continue;

                InternalCalls.TileComponent_GetPosition(currentTile.ID, out Vector2D currentPosition);
                Vector2D[] neighborOffsets = {
                    new Vector2D(0, 110),   // Up
                    new Vector2D(110, 0),   // Right
                    new Vector2D(0, -110),  // Down
                    new Vector2D(-110, 0)   // Left
                };

                foreach (var offset in neighborOffsets)
                {
                    Vector2D neighborPos = currentPosition + offset;
                    Entity neighborTile = GetTileAtPosition(neighborPos);

                    if (neighborTile != null && IsTileWalkable(neighborTile) && !reachableTiles.Contains(neighborTile))
                    {
                        queue.Enqueue(neighborTile, -(remainingEnergy - 1));
                        reachableTiles.Add(neighborTile);
                    }
                }
            }

            return reachableTiles;
        }

        /**
         * @brief Highlights tiles reachable by the player based on energy.
         * 
         * @param playerEntity The Entity representing the player.
         * @note Uses BFS to highlight tiles within the player's energy range.
         */
        public void HighlightReachableTiles(Entity playerEntity)
        {
            if (playerEntity == null)
            {
                Debug.LogWarning("[TileManager] No player entity found.");
                _isHighlightingReachableTiles = false;
                ClearAllHighlights();
                return;
            }

            int playerEnergy = InternalCalls.UnitComponent_GetEnergy(playerEntity.ID);
            if (playerEnergy <= 0)
            {
                Debug.LogInfo("[TileManager] Player has no energy left. Disabling highlight.");
                _isHighlightingReachableTiles = false;
                ClearAllHighlights();
                return;
            }

            Vector2D playerPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(playerEntity.ID, ref playerPosition);
            Entity playerTile = GetTileAtPosition(playerPosition);

            if (playerTile == null)
            {
                _isHighlightingReachableTiles = false;
                ClearAllHighlights();
                return;
            }

            _isHighlightingReachableTiles = true;

            foreach (var tile in _gridTiles.Values)
            {
                InternalCalls.TileComponent_SetIsHighlighted(tile.ID, false);
            }

            PriorityQueue<Entity> queue = new PriorityQueue<Entity>();
            List<Entity> visited = new List<Entity>();

            queue.Enqueue(playerTile, -playerEnergy); // Start BFS
            visited.Add(playerTile);

            while (queue.Count > 0)
            {
                var (currentTile, currentPriority) = queue.Dequeue();
                InternalCalls.TileComponent_SetIsHighlighted(currentTile.ID, true);

                int remainingEnergy = (int)(-currentPriority);
                if (remainingEnergy <= 0) continue;

                InternalCalls.TileComponent_GetPosition(currentTile.ID, out Vector2D currentPosition);
                Vector2D[] neighborOffsets = {
                    new Vector2D(0, 110),   // Up
                    new Vector2D(110, 0),   // Right
                    new Vector2D(0, -110),  // Down
                    new Vector2D(-110, 0)   // Left
                };

                foreach (var offset in neighborOffsets)
                {
                    Vector2D neighborPos = currentPosition + offset;
                    Entity neighborTile = GetTileAtPosition(neighborPos);

                    if (neighborTile != null && IsTileWalkable(neighborTile) && !visited.Contains(neighborTile))
                    {
                        queue.Enqueue(neighborTile, -(remainingEnergy - 1));
                        visited.Add(neighborTile);
                    }
                }
            }
        }

        /**
         * @brief Highlights tiles reachable by an enemy based on energy.
         * 
         * @param enemyEntity The Entity representing the enemy.
         * @note Uses BFS to highlight tiles within the enemy's energy range, distinct from player highlights.
         */
        public void HighlightEnemyTiles(Entity enemyEntity)
        {
            if (enemyEntity == null)
            {
                Debug.LogWarning("[TileManager] No enemy entity found.");
                return;
            }

            Vector2D enemyPosition = new Vector2D();
            InternalCalls.UnitComponent_GetPosition(enemyEntity.ID, ref enemyPosition);
            Entity enemyTile = GetTileAtPosition(enemyPosition);

            if (enemyTile == null)
                return;

            int enemyEnergy = InternalCalls.UnitComponent_GetEnergy(enemyEntity.ID);
            if (enemyEnergy <= 0)
            {
                ClearAllEnemyHighlights();
                return;
            }

            foreach (var tile in _gridTiles.Values)
            {
                InternalCalls.TileComponent_SetIsHighlightedEnemy(tile.ID, false);
            }

            PriorityQueue<Entity> queue = new PriorityQueue<Entity>();
            List<Entity> visited = new List<Entity>();

            queue.Enqueue(enemyTile, -enemyEnergy);
            visited.Add(enemyTile);

            while (queue.Count > 0)
            {
                var (currentTile, currentPriority) = queue.Dequeue();
                InternalCalls.TileComponent_SetIsHighlightedEnemy(currentTile.ID, true);

                int remainingEnergy = (int)(-currentPriority);
                if (remainingEnergy <= 0) continue;

                InternalCalls.TileComponent_GetPosition(currentTile.ID, out Vector2D currentPosition);
                Vector2D[] neighborOffsets = {
                    new Vector2D(0, 110),   // Up
                    new Vector2D(110, 0),   // Right
                    new Vector2D(0, -110),  // Down
                    new Vector2D(-110, 0)   // Left
                };

                foreach (var offset in neighborOffsets)
                {
                    Vector2D neighborPos = currentPosition + offset;
                    Entity neighborTile = GetTileAtPosition(neighborPos);

                    if (neighborTile != null && IsTileWalkable(neighborTile) && !visited.Contains(neighborTile))
                    {
                        int newPriority = -(remainingEnergy - 1);
                        queue.Enqueue(neighborTile, newPriority);
                        visited.Add(neighborTile);
                    }
                }
            }
        }

        /**
         * @brief Clears all player tile highlights.
         * 
         * @note Disables highlighting flag and resets all tile highlight states.
         */
        private void ClearAllHighlights()
        {
            _isHighlightingReachableTiles = false;

            foreach (var tile in _gridTiles.Values)
            {
                InternalCalls.TileComponent_SetIsHighlighted(tile.ID, false);
            }

            ClearPathHighlights();
        }

        /**
         * @brief Clears all enemy tile highlights.
         * 
         * @note Resets all enemy-specific highlight states.
         */
        private void ClearAllEnemyHighlights()
        {
            foreach (var tile in _gridTiles.Values)
            {
                InternalCalls.TileComponent_SetIsHighlightedEnemy(tile.ID, false);
            }
        }

        // -----------------------[ CUSTOM PRIORITY QUEUE IMPLEMENTATION ]----------------------

        /**
         * @brief A simple priority queue implementation for managing entities with priorities.
         * 
         * @note Uses a list-based approach with linear search for dequeuing lowest-priority items.
         */
        public class PriorityQueue<T>
        {
            private List<(T Item, float Priority)> _elements = new List<(T Item, float Priority)>();

            /** @brief Gets the number of elements in the priority queue. */
            public int Count => _elements.Count;

            /**
             * @brief Adds an item to the priority queue with a specified priority.
             * 
             * @param item The item to enqueue.
             * @param priority The priority value (lower values indicate higher priority).
             */
            public void Enqueue(T item, float priority)
            {
                _elements.Add((item, priority));
            }

            /**
             * @brief Removes and returns the item with the lowest priority value.
             * 
             * @return A tuple containing the item and its priority.
             * @note Throws InvalidOperationException if the queue is empty.
             */
            public (T, float) Dequeue()
            {
                if (_elements.Count == 0)
                    throw new InvalidOperationException("The priority queue is empty.");

                int bestIndex = 0;
                for (int i = 1; i < _elements.Count; i++)
                {
                    if (_elements[i].Priority < _elements[bestIndex].Priority)
                        bestIndex = i;
                }

                var bestItem = _elements[bestIndex];
                _elements.RemoveAt(bestIndex);
                return (bestItem.Item, bestItem.Priority);
            }

            /**
             * @brief Checks if an item exists in the priority queue.
             * 
             * @param item The item to check for.
             * @return True if the item is in the queue, false otherwise.
             */
            public bool Contains(T item)
            {
                foreach (var element in _elements)
                {
                    if (EqualityComparer<T>.Default.Equals(element.Item, item))
                        return true;
                }
                return false;
            }
        }
    }
}