/******************************************************************************/
/*!
\file    Button.h
\project FruitLoops
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains the declaration of the ButtonComponent struct,
         which represents data and behavior for UI buttons. It manages
         properties like hover state, click state, and enabled status to
         facilitate button interactions in the game's UI.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

namespace FruitLoops {

    struct ButtonComponent {
        bool isHovered = false;
        bool isClicked = false;
        bool isEnabled = true;
    };
}