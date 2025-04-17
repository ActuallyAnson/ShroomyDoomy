/**
 * @file Core.h
 * @brief Header file for the core functionality of the FruitloopEngine.
 *
 * This source file contains the definitions for the methods of the
 * `FruitloopEngine` class, which serves as the main manager for
 * the game engine. It initializes subsystems, manages the main game
 * loop, and provides a mechanism to shut down the engine cleanly.
 *
 * Key functionalities include:
 * - Engine initialization and subsystem setup (graphics, input, physics).
 * - The main game loop that handles events, updates, and rendering.
 * - Logging system performance and frame timings.
 * - Gracefully shutting down the engine.
 *
 * @note The engine relies on the ECS (Entity-Component-System) architecture
 *       for managing game objects and their behaviors.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#pragma once

#include "WindowsWindow.h"
#include "LayerStack.h"
#include "FrameBuffer.h"


namespace FruitLoops {
	/**
	 * The FruitloopEngine class manages all game systems, coordinates their updates,
	 * routes messages, and handles their lifecycle from creation to termination.
	 */
	class FruitloopEngine {
	public:

		FruitloopEngine(); // Constructor initializes the engine.
		~FruitloopEngine(); // Destructor cleans up resources.

		void GameLoop(); // Executes the main game loop until shutdown.
		void Initialize(); // Initializes the engine and its subsystems.
		void InitializeLayers(); // Initializes layers for rendering and UI management.
		void RestartGame(); // Restarts game through C# game logic side
		void Registration(); // Registers all components and systems with the ECS.
		void PushLayer(Layer* layer); // Adds a new layer to the engine's layer stack.
		void PushOverlay(Layer* layer); // Adds a new overlay on top of existing layers.
		void ShutDown(); // Shuts down the engine, stopping the game loop.

		bool& GetGameState() { return gameActive; } // Returns the active state of the game.
		bool& GetSetTutorialToLevel1() { return tutorialToLevel1; } // Returns the active state of the game.
		bool& GetSetLevel1ToLevel2() { return level1ToLevel2; } // Returns the active state of the game.
		bool& GetSetLevel2ToLevel1() { return level2ToLevel1; } // Returns the active state of the game.
		bool& GetSetLevel1ToTutorial() { return level1ToTutorial; } // Returns the active state of the game.
		bool& GetSetMainMenuToLevel1() { return mainMenuToLevel1; } // Returns the active state of the game.
		bool& GetSetReloadLevel() { return restartGame; } // Returns the active state of the game.
		bool& GetSetPauseMenuToMainMenu() { return pauseToMainMenu; } // Returns the active state of the game.
		bool& GetSetBackToMainMenu() { return returnMainMenu; } // Returns the active state of the game.
		bool& GetSetMainMenuToTutorial() { return menuToTutorial; } // Returns the active state of the game.

		//float GetDeltaTime() { return deltaTime.count(); }
		const std::unique_ptr<FrameBuffer>& GetFrameBuffer() const { return frameBuffer; } // Returns the framebuffer used for rendering.
		LayerStack& GetLayerStack() { return layerStack; } // Access the engine's layer stack.
		std::shared_ptr<EngineSystems> GetLogicSystem() { return coreLogicSystem; }
		std::shared_ptr<EngineSystems> GetScriptSystem() { return coreScriptSystem; }
		//message -- next week?

	private:
		//See if game is active
		bool gameActive; // Flag to check if the game is active.
		bool tutorialToLevel1 = false;
		bool level1ToLevel2 = false;
		bool level2ToLevel1 = false;
		bool level1ToTutorial = false;
		bool mainMenuToLevel1 = false;
		bool restartGame = false;
		bool pauseToMainMenu = false;
		bool returnMainMenu = false;
		bool menuToTutorial = false;
		// Subsystem pointers managed via smart pointers for automatic resource management.
		std::shared_ptr<EngineSystems> coreGraphicsSystem;
		std::shared_ptr<EngineSystems> corePhysicsSystem;
		std::shared_ptr<EngineSystems> coreScriptSystem;
		std::shared_ptr<EngineSystems> coreInputSystem;
		std::shared_ptr<EngineSystems> coreSoundSystem;
		std::shared_ptr<EngineSystems> coreLogicSystem;
		std::shared_ptr<EngineSystems> coreImGuiSystem;

		std::unique_ptr<WindowsWindow> window; // Unique pointer to the window management system.
		LayerStack layerStack; // Manages layers for UI and game object rendering.

		// framebuffer member

		std::unique_ptr<FrameBuffer> frameBuffer; // Framebuffer for handling complex rendering tasks.
	};

	//make it accessible from other files
	extern FruitloopEngine* coreEngine;
}