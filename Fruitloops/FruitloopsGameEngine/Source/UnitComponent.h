/******************************************************************************/
/*!
\file    UnitComponent.h
\project Shroomy Doomy
\author  Benjamin Lau
\date    Nov 08, 2024
\brief   This file contains the declaration of the UnitComponent struct, which
         represents a unit in the game. It stores data such as unit type, health,
         energy, position, and the tile the unit occupies. It also provides
         utility functions for managing unit state.

All content � 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

#include "ECSHub.h"

namespace FruitLoops {

    enum class UnitType {
        Player,
        Enemy
    };
    /**
     * @struct UnitComponent
     * @brief Holds attributes and state for a game unit.
     *
     * This structure encapsulates all the properties necessary for defining
     * a unit's behavior and status within the game, such as its type, health,
     * energy, and current position on the game grid.
     */
    struct UnitComponent {
        UnitType unitType;           // Player or Enemy
        int health;
        int maxHealth;
        int attackPower;
        int energy;
        int maxEnergy;
        bool isAlive;
        bool isMoving;
        int speed;
        Vector2D position;           // Current grid position
        Entity occupiedTile;         // Reference to the tile the unit currently occupies
        /**
         * @brief Default constructor initializes a unit as a player with default attributes.
         */
        UnitComponent()
            : unitType(UnitType::Player), health(0), maxHealth(0), attackPower(0), energy(0), maxEnergy(0), isAlive (true), isMoving(false), speed(0),
            position(Vector2D(0, 0)), occupiedTile(Entity()) {}
        /**
         * @brief Constructor to create a unit with specified attributes.
         * @param type The type of the unit.
         * @param hp Current health.
         * @param maxHp Maximum health.
         * @param attack Attack power.
         * @param enrgy Current energy.
         * @param maxEnrgy Maximum energy capacity.
         * @param pos Initial grid position.
         */
        UnitComponent(UnitType type, int hp, int maxHp, int attack, int enrgy, int maxEnrgy, bool isAlive, bool isMoving, int speed,const Vector2D& pos = Vector2D(0, 0))
            : unitType(type), health(hp), maxHealth(maxHp), attackPower(attack), energy(enrgy), maxEnergy(maxEnrgy), isAlive(isAlive), isMoving(isMoving), speed(speed),
            position(pos), occupiedTile(Entity()) {}
        /**
         * @brief Checks if the unit is alive by evaluating its health.
         * @return True if health is greater than zero, otherwise false.
         */
        bool IsAlive() const {
            return health > 0;
        }
        /**
         * @brief Resets the occupied tile reference to no entity.
         */
        void ResetOccupiedTile() {
            occupiedTile = Entity();
        }
    };
}