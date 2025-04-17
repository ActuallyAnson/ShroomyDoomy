/**
 * @file Transform.h
 * @brief Defines the Transform structure for entity positioning and scaling.
 *
 * This header file declares the `Transform` struct, which represents an
 * entity's position, orientation, and scale in 2D space using the
 * `Vector2D` class. It also includes serialization and deserialization
 * functions for converting the `Transform` to and from JSON format
 * using the `nlohmann::json` library.
 *
 * Key functionalities include:
 * - Defining the `Transform` struct with scale, orientation, and position
 *   vectors.
 * - Serialization of `Transform` objects to JSON format for easy data
 *   exchange.
 * - Deserialization of JSON data into `Transform` objects for loading
 *   configurations or state.
 *
 * @note The `Transform` struct is designed to work in conjunction with
 *       the Entity-Component-System (ECS) architecture, allowing for
 *       flexible manipulation of entity properties.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace FruitLoops {

    struct Transform {
      Vector2D scale;
      Vector2D orientation;
      Vector2D position;
    };

}