/******************************************************************************/
/*!
\file    LogicProxySystem.h
\project FruitLoops
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains the declaration of the LogicProxySystem class,
         which acts as a bridge between the game engine and the game logic
         system. It initializes, updates, and ends the game logic by calling
         corresponding static methods in the scripting system.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once
#include "System.h"
#include "ScriptSystem.h"

namespace FruitLoops {

    class LogicProxySystem {
    public:
        static void Initialize()  {
            ScriptSystem::CallStaticMethod("FruitLoops.GameLogicSystem", "Init", nullptr);
        }

        static void Update(float deltaTime)  {

            void* param = &deltaTime;
            ScriptSystem::CallStaticMethod("FruitLoops.GameLogicSystem", "Update", &param);
        }

        static void End() {
            ScriptSystem::CallStaticMethod("FruitLoops.GameLogicSystem", "End", nullptr);
        }
    };
}