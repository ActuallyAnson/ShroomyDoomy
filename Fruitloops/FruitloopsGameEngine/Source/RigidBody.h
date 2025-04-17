#pragma once
#include "Vector2D.h"
/******************************************************************************/
/*!
\file    RigidBody.h
\project FruitLoops
\author  Reagan Tang
\date    Dec 03, 2024
\brief   This file declares the RigidBody struct, which represents the
         physical properties of a 2D game object, including its velocity,
         position, and mass.

         The RigidBody component is used in the ECS system to simulate
         basic physics behaviour such as linear movement, collisions, and
         response to forces.

         It is intended to be updated during the physics step of the game loop.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
namespace FruitLoops {
    struct RigidBody {

        FruitLoops::Vector2D velocity;
        FruitLoops::Vector2D position;
        float mass = 0; // Mass of the body
    };
}

