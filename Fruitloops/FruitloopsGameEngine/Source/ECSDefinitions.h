/**
 * @file ECSDefinitions.h
 * @brief Defines types and constants for managing entities and components.
 *
 * This header file contains type aliases and constants used in the
 * Entity-Component-System (ECS) framework of the FruitLoops engine.
 * It provides a structured approach to manage entity IDs, component types,
 * and their maximum limits within the game engine.
 *
 * Key functionalities include:
 * - Defining type aliases for `Entity` IDs and `ComponentType` identifiers.
 * - Setting maximum limits for the number of entities and component types
 *   that can exist simultaneously.
 * - Using a bitset to track component signatures for entities, enabling
 *   efficient component management and querying.
 *
 * @note These definitions are essential for the ECS framework, ensuring
 *       efficient management and retrieval of components associated with
 *       each entity.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once

namespace FruitLoops {
    using Entity = std::uint32_t;  // Type alias for entity IDs
    using ComponentType = std::uint8_t;  // Type alias for component types
    const Entity maxEntities = 3000;  // Maximum number of entities
    const std::uint8_t maxComponents = 20;  // Maximum number of component types
    using Signature = std::bitset<maxComponents>;  // Bitset for tracking component signatures

    constexpr ComponentType INVALID_COMPONENT_TYPE = std::numeric_limits<ComponentType>::max();
    constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();
}