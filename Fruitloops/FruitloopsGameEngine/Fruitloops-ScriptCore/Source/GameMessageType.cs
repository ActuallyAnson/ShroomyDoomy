/******************************************************************************
/*!
\file  GameMessageType.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 11, 2025
\brief  Defines message types for the game's observer pattern.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

namespace FruitLoops
{
    /**
     * @brief Message types for the observer pattern.
     */
    public enum GameMessageType
    {
        PlayerMoved,
        PlayerAttacked,
        PlayerDied,
        PlayerTurnStarted,
        PlayerTurnEnded,
        PlayerHealthChanged,
        PlayerEnergyChanged,

        EnemyMoved,
        EnemyAttacked,
        EnemyDied,
        EnemyTurnStarted,
        EnemyTurnEnded,
        EnemyHealthChanged,
        EnemyEnergyChanged,


        GameStart,
        GameOver,
        TurnChanged,

        PlayerActionFailed,

        TileInteraction,
        TileStateChanged,

        PathCheck,
        RangedAttackStarted,
        RangeCheck,

        EmptyTileDetected,
        TileScanned,
        EntityDetected,
        EnvironmentScanCompleted,
        EnvironmentScanStarted,
        NoTileDetected,

        AllEnemiesDefeated,
        EnemyInitialized,
        ObstacleInitialized,
        PlayerInitialized,
        LevelComplete,
        GameManagerInitialized,
        GameManagerCleanup,

        EnemyActionFinished,
        TileOccupied,


        UIUpdateStarted,
        UIUpdateCompleted,
        AttackRangeCheck,
        GameError,
        UIElementsPositioned,
        UIInteraction,

        EntityCreationStarted,
        EntityCreationCompleted,
        EntityInitializationStarted,
        EntityInitializationCompleted,
        UIError,


        AnimationStateChanged,
        EnemyActionStarted,
        EntityStateChanged,
        InventoryUpdated,
        EntityStatusUpdated,
        EntityProximityChanged,
        EntityStatusUpdate,

        EnemyEnergyCalculation,
        EnemyEnergyInsufficient,
        EnemyMovementStarted,
        PathfindingStarted,
        PathfindingCompleted,
        PathfindingError,
        PathfindingFailed,
        EnemyMovementFinished,
        EnemyMovementUpdate,
        EnemyReachedTile,
        EntityCleanupStarted,
        EntityCleanupCompleted,
        PathError,
        EntityStatsReset,
        GameWarning,
        DirectionChange,
        EnemyStatus,

        EnemyCleanUp,
        TimedAction,
    }
}