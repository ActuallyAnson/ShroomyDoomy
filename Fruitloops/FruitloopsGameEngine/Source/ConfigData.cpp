/******************************************************************************/
/*!
\file    ConfigData.cpp
\project FruitLoops
\author  Zhang TongYan
\date    Dec 03, 2024
\brief   This file defines global configuration variables within the
		 FruitLoops::ConfigData namespace. These variables govern various
		 runtime settings such as window dimensions, UI offsets, game title,
		 background music (BGM), and font rendering preferences.

		 These config values are typically initialised at startup and
		 referenced throughout the engine or application to maintain consistent
		 behaviour and appearance.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "ConfigData.h"
namespace FruitLoops {
	namespace ConfigData {
		// Window Properties
		int width = 0;
		int height = 0;
		int imGuiTitleHeightOffset = 0;
		int imGuiTitleWidthOffset = 0;
		std::string title = "";
		std::string BGM = "";

		// Graphics config
		Vector3D fontColor;
		int fontDetail = 0;
	}
	

}