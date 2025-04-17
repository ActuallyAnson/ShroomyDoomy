/******************************************************************************/
/*!
\file    PrefabManager.cpp
\project FruitLoops
\author  Anson Teng
\date    Dec 03, 2024
\brief   This file defines static member variables for the PrefabManager class,
         which handles the editing modes such as move, scale, and rotate
         during level or entity manipulation. These modes are used in the
         editor or debug UI to control prefab transformations.

         All flags are initialised to false by default.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/

#include "pch.h"
#include "PrefabManager.h"

namespace FruitLoops {
    bool PrefabManager::moveMode = false;
    bool PrefabManager::scaleMode = false;
    bool PrefabManager::rotateMode = false;
}