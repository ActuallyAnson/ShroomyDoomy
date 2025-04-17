#pragma once

/******************************************************************************/
/*!
\file    CameraComponent.h
\project FruitLoops
\author  Zhang TongYan
\date    Dec 03, 2024
\brief   This file declares the CameraComponent struct, which defines the
		 parameters and control flags for a 2D/orthographic camera in the
		 game world.

		 The CameraComponent includes:
		 - Configurable window height and zoom limits.
		 - Movement speed and direction.
		 - Boolean flags for controlling camera behaviour (e.g., zoom, turn).
		 - Camera position and aspect ratio (AR).

		 This component is designed to be attached to ECS entities and
		 manipulated in real time using user inputs for smooth camera control.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include <GLEW/glew.h>

namespace FruitLoops {

    struct CameraComponent {
		GLint height{ 1000 };
		// Position of the camera in world space

		// window change parameters ...
		GLint minHeight{ 500 }, maxHeight{ 2000 };
		// height is increasing if 1 and decreasing if -1
		GLint heightChangeDir{ 1 };
		// increments by which window height is changed per Z key press
		GLint heightChangeVal{ 5 };
		// camera's speed when button U is pressed
		GLfloat linearSpeed{ 2.f };                   
		GLboolean camTypeFlag{ GL_FALSE }; // button V
		GLboolean zoomFlag{ GL_FALSE }; // button Z
		GLboolean leftTurnFlag{ GL_FALSE }; // button H
		GLboolean rightTurnFlag{ GL_FALSE }; // button K
		GLboolean moveFlag{ GL_FALSE }; // button U
		GLfloat ar;

		Vector2D position{ 0.0f, 0.0f };  // Default to the origin
    };
}