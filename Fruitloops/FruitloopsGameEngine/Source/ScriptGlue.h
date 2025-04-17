/******************************************************************************/
/*!
\file    ScriptGlue.h
\project FruitLoops
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains the declaration of the ScriptGlue class,
         which provides functionality to bind internal calls between
         the C++ game engine and the C# scripting environment. It serves
         as the interface for registering engine functionalities that can
         be accessed from the scripting layer.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

namespace FruitLoops {

	class ScriptGlue
	{
	public:
		static void RegisterFunctions();
	};
}