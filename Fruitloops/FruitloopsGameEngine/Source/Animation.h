/******************************************************************************/
/*!
\file  Material.h
\Proj name  Shroomy Doomy
\author  Zhang TongYan
\date    Oct 03, 2024
\brief
This file contains the declaration of the Material struct and its associated JSON
serialization and deserialization functions.
The Material struct is used for handling shader references, object names, colors,
and texture IDs in the game.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#pragma once

#include <nlohmann/json.hpp>

namespace FruitLoops {

    struct AnimationComponent {
        std::vector<std::string> AnimationName;  // Texture ID for the animation
        bool isActive = true;
    };

}