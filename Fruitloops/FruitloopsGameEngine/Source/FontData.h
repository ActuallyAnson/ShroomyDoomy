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

namespace FruitLoops {

    struct FontDataComponent{
        std::string font;
        std::string text;
        float scale;
    };

}