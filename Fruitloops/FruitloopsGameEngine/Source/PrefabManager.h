#pragma once
/******************************************************************************/
/*!
\file    PrefabManager.cpp
\project FruitLoops
\author  Anson Teng
\date    Dec 03, 2024
\brief   This file declares static member variables for the PrefabManager class,
		 which handles the editing modes such as move, scale, and rotate
		 during level or entity manipulation. These modes are used in the
		 editor or debug UI to control prefab transformations.

		 All flags are initialised to false by default.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
namespace FruitLoops {
	class PrefabManager {
	public:
		// interaction modes
		static bool moveMode;
		static bool scaleMode;
		static bool rotateMode;

		// setters
		static void SetMoveMode(bool enabled) {
			moveMode = enabled;
		}

		static void SetScaleMode(bool enabled) {
			scaleMode = enabled;
		}

		static void SetRotateMode(bool enabled) {
			rotateMode = enabled;
		}

		// Getters
		static bool IsMoveMode() {
			return moveMode;
		}

		static bool IsScaleMode() {
			return scaleMode;
		}

		static bool IsRotateMode() {
			return rotateMode;
		}
	};
}