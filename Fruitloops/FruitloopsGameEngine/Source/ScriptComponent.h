/******************************************************************************/
/*!
\file    ScriptComponent.h
\project FruitLoops
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains the declaration of the ScriptComponent struct,
         which represents the scripting functionality for an entity in the
         game. It holds the name of the associated script and a reference
         to the script instance.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once
#include "ScriptSystem.h"

namespace FruitLoops
{
    struct ScriptComponent
    {
        std::string Name;                          // Name of the script (matches C# class)
        bool isActive;
        Ref<ScriptInstance> Instance = nullptr;    // Instance of the script class

        ScriptComponent() = default;
        ScriptComponent(const std::string& name)
            : Name(name), isActive(true)
        {
            // Do not call UpdateScriptInstance here to avoid dependency issues.
        }
    };
}
