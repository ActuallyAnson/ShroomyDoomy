#pragma once

/**
 * @file GameTypes.h
 * @author Anson Teng
 * @date  Nov 8, 2024
 * @brief Defines key enumerations used throughout the game, including event types, object types, and state management.
 *
 * This file contains important enumerations that categorize various game entities and events. These enums are used across
 * different parts of the game engine to represent events, object types, and states of the game world.
 *
 * Enumerations:
 * - EventID: Represents various events triggered in the game (e.g., object spawn, item collected, turn start/end).
 * - ObjectType: Defines different types of objects (e.g., main character, monsters, walls, collectibles, menus).
 * - ObjectState: Describes the different states of objects like players and monsters (e.g., idle, moving, death).
 * - TurnState: Indicates whose turn it is in turn-based mechanics (e.g., player or enemy turn).
 * 
 * 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */


namespace FruitLoops {
	enum class EventID
	{
		SetLoadingScreenEvent,
		ActivateTutorial,
		Cooking,			// cooking event,		// collect the food or something
		TurnEnd,
		TurnStart,
		ItemCollected,       // collect the food or something
		ObjectSpawned,
		ObjectDeleted,
		KeyPress,
		AnimationComponentChange,
		RigidBodyComponentChange,
		FontSpawned,
		FontDeleted,
		DrawGrid,
		ObjectSelected

	};

	enum class ObjectType
	{
		FLOOR = 0,//tiles
		MAIN_CHARACTER,

		//enemies
		MONSTER,
		MONSTER_PATROL,
		MONSTER_GUIDE,

		//walls
		WALL,
		WALL_HORIZONTAL,
		WALL_VERTICLE,

		//doors if have
		ENTRANCE_DOOR,
		EXIT_DOOR,
		LEVEL_DOOR,

		//background
		BACKGROUND,

		KEY_OBJECTIVE,
		COLLECTIBLES,

		//TUTORIAL LEVEL
		TUTORIAL,
		TUTORIAL_INSTRUCTION,

		//Main Menu
		MAIN_MENU_BG,
		PLAY,
		OPTION,
		CONTROL,
		CREDIT,
		QUIT,

		//Level Select Menu
		LEVELSELECT_MENU_BG,
		LEVEL1,
		LEVEL2,
		LEVEL3,
		LEVEL_BACK,

		//How to play Menu
		CONTROL_MENU_BG_1,
		CONTROL_MENU_BG_2,
		CONTROL_MENU_BG_3,
		CONTROL_NEXT,
		CONTROL_BACK,

		//Quit Menu
		QUIT_MENU_BG,
		QUIT_YES,
		QUIT_NO,

		//Pause Menu
		PAUSE_RESUME,
		PAUSE_QUIT,
		PAUSE_MAINMENU,


		//Defeat Screen
		DEFEAT_BG,
		DEFEAT_RESTART,
		DEFEAT_MAINMENU,

		//Victory Screen
		VICTORY_BG,
		VICTORY_CONTINUE,
		VICTORY_RESTART,
		VICTORY_MAINMENU,

		//UI
		UI_HP_ICON,
		UI_OBJECTIVE_ICON,
		UI_HP_COUNTER,


		//LoadingLogo
		LOADING_LOGO,
	};

	enum class ObjectState {
		PLAYER_IDLE = 0,
		PLAYER_MOVING,
		PLAYER_DEATH,

		MONSTER_MOVING,
		MONSTER_IDLE,
	};

	enum class TurnState {
		PlayerTurn,
		EnemyTurn
	};

}