/**
 * @file ECSHub.cpp
 * @brief Central hub for managing the Entity-Component-System architecture.
 *
 * This file provides a placeholder for the main ECS architechture at the
 * header file
 *
 * Key functionalities include:
 * - Initializing and managing component, entity, and system managers.
 * - Creating and destroying entities while ensuring proper cleanup of
 *   components and systems.
 * - Adding and removing components from entities, updating their signatures
 *   accordingly.
 * - Retrieving entities that have specific components and checking for the
 *   presence of components on entities.
 *
 * @note The `ECSHub` class utilizes templates for flexibility and type safety
 *       when registering and managing components and systems.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */
#include "pch.h"
#include "ECSHub.h"

namespace FruitLoops {
    ECSHub* ecsHub = nullptr;
}