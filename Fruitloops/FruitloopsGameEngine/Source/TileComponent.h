/******************************************************************************/
/*!
\file    TileComponent.h
\project Shroomy Doomy
\author  Benjamin Lau
\date    Nov 08, 2024
\brief   This file contains the declaration of the TileComponent struct,
         which represents data and behavior for a tile in the game grid. It
         manages properties like walkability, movement cost, neighbors, and
         occupant, as well as utility functions for tile interactions.

All content � 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once
#include "ECSDefinitions.h"

namespace FruitLoops {

    enum class TileType {
        Floor,
        Wall
    };

    struct TileComponent {
        bool walkable;
        bool highlighted;
        bool highlightedEnemy;
        bool isActive;
        Vector2D position;
        TileType tileType;
        std::vector<TileComponent*> neighbors;
        float movementCost;
        Entity occupant;

        TileComponent()
            : walkable(true),
            highlighted(false),
            highlightedEnemy(false),
            position(0, 0),
            tileType(TileType::Floor),
            movementCost(1.0f),
            occupant(Entity(0))
        {}

        TileComponent(bool isWalkable, bool isHighlighted, bool isHighlightedEnemy, const Vector2D& pos, TileType type, float cost = 1.0f)
            : walkable(isWalkable),
            highlighted(isHighlighted),
            highlightedEnemy(isHighlightedEnemy),
            isActive(true),
            position(pos),
            tileType(type),
            movementCost(cost),
            occupant(Entity(0))
        {}

        bool IsWalkable() const {
            return walkable;
        }

        void SetWalkable(bool isWalkable) {
            walkable = isWalkable;
        }

        void AddNeighbor(TileComponent* neighbor) {
            if (neighbor) {
                neighbors.push_back(neighbor);
            }
        }

        void ClearNeighbors() {
            neighbors.clear();
        }

        std::vector<TileComponent*> GetNeighbors() const {
            std::vector<TileComponent*> validNeighbors;
            for (auto* neighbor : neighbors) {
                if (neighbor) {
                    validNeighbors.push_back(neighbor);
                }
            }
            return validNeighbors;
        }

        float CostToEnter() const {
            return movementCost;
        }

        void SetOccupant(Entity entity) {
            occupant = entity;
        }

        Entity GetOccupant() const {
            return occupant;
        }

        void ClearOccupant() {
            occupant = Entity();
        }

        Vector2D GetCenterPosition() const {
            return position;
        }

        bool IsHighlighted() const {
            return highlighted;
        }

        void SetHighlighted(bool state) {
            highlighted = state;
        }

        bool IsHighlightedEnemy() const {
            return highlightedEnemy;
        }

        void SetHighlightedEnemy(bool state) {
            highlightedEnemy = state;
        }
    };
}
