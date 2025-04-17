#pragma once
/******************************************************************************/
/*!
\file    LevelManager.h
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Declares the LevelManager class, which manages transitions and logic
		 between different levels and UI states within the FruitLoops engine.

This class handles key level transitions such as switching between tutorial,
main menu, Level 1, and Level 2. It supports runtime transitions for gameplay
as well as editor-driven transitions. Key responsibilities include toggling
UI visibility, activating script and tile components, managing music transitions,
and coordinating logic proxy systems.

The LevelManager is also responsible for reinitialising rendering and script
systems when levels are loaded or reloaded. It ensures a consistent and clean
game state across level boundaries.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

namespace FruitLoops {
	class LevelManager {

	public:

		LevelManager();
		~LevelManager();
		void Level1ToLevel2();
		void Level2ToLevel1();
		void Level1ToTutorial();
		void TutorialToLevel1();
		void GameStart(Layer* layer);
		void TogglePauseMenu();
		void MainMenuToLevel1();
		void ReturnMainMenu();
		void PauseMenuToMainMenu();
		void ReloadCurrLevel();
		void MainMenuToTutorial();
		int GetCurrLevel() { return currLevel; }
	private:
		// 0 - Tutorial, 1 - Level1, 2 - Level2
		int currLevel = 1;
	};

	extern LevelManager* levelManager;
}
