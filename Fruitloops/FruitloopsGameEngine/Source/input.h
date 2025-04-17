/****************************************************************************/
/*!
\file		input.h
\Proj name  Fruitloops
\author 	Zhang TongYan
\date   	Oct 03, 2024
\brief		This file contains the declarations of the InputSystem class which
			manages user input and system events for the Fruitloops project.
			It provides mechanisms to handle keyboard, mouse, and other device
			inputs using callbacks and maintains state relevant to input devices. for Renderer class, used for batch rendering
All content (C) 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /****************************************************************************/
#pragma once

#include "Subject.h"
#include "KeyCodes.h"

namespace FruitLoops {

	// The InputSystem class handles user input and events within the game.
	// It inherits from EngineSystems, which likely provides basic system functionality.
	class InputSystem :  public Subject, public EngineSystems {
		
	public:
		// Constructor and destructor
		InputSystem();		// Initializes the input system
		~InputSystem();		// Cleans up resources when the input system is destroyed

		// Static function to clean up resources when necessary
		static void cleanup();


		/*!
		 \brief Callback for handling GLFW errors.
		 \param error Error code provided by GLFW.
		 \param description Descriptive error message.
		*/
		static void error_cb(int error, char const* description);
		
		/*!
		\brief Callback for handling framebuffer size changes.
		\param ptr_win Pointer to the GLFW window.
		\param width New width of the framebuffer.
		\param height New height of the framebuffer.
		*/
		static void fbsize_cb(GLFWwindow* ptr_win, int width, int height);
		
		/*!
		 \brief Callback for keyboard input events.
		 \param pwin Pointer to the GLFW window.
		 \param key Key code of the pressed key.
		 \param scancode Platform-specific scancode.
		 \param action Action (press/release/repeat).
		 \param mod Modifier keys.
		*/
		static void key_cb(GLFWwindow* pwin, int key, int scancode, int action, int mod);
		
		/*!
		 \brief Callback for mouse button events.
		 \param pwin Pointer to the GLFW window.
		 \param button Mouse button code.
		 \param action Action (press/release).
		 \param mod Modifier keys.
		*/
		static void mousebutton_cb(GLFWwindow* pwin, int button, int action, int mod);
		
		/*!
		 \brief Callback for mouse scroll events.
		 \param pwin Pointer to the GLFW window.
		 \param xoffset Horizontal scroll offset.
		 \param yoffset Vertical scroll offset.
		*/
		static void mousescroll_cb(GLFWwindow* pwin, double xoffset, double yoffset);
		
		/*!
		 \brief Callback for mouse position events.
		 \param pwin Pointer to the GLFW window.
		 \param xpos New x-coordinate of the cursor.
		 \param ypos New y-coordinate of the cursor.
		*/
		static void mousepos_cb(GLFWwindow* pwin, double xpos, double ypos);
		
		/*!
		\brief Sets up callbacks for handling GLFW input and window events.
		\param window Pointer to the GLFW window for which events are to be handled.
		*/
		static void setup_event_callbacks(GLFWwindow* window);

		/*!
		\brief Initializes the InputSystem, setting up necessary state and registering callbacks.
		*/
		void Initialize() override;

		/*!
		\brief Updates the InputSystem by processing input events.
		\param deltaTime Time elapsed since the last frame, used for time-sensitive updates.
		*/
		void Update(float deltaTime) override;

		/*!
		\brief Updates time-related calculations, such as FPS.
		\param fpsCalcInt Interval in seconds to calculate FPS.
		*/
		static void update_time(double fpsCalcInt = 1.0);

		/*!
		\brief Checks if a specific key is pressed.
		\param keyCode The key code to check.
		\return True if the key is pressed, false otherwise.
		*/
		bool IsKeyPressed(int keyCode);

		/*!
		\brief Retrieves the current position of the cursor.
		\return The current cursor position as a Vector2D object.
		*/
		static Vector2D InputSystem::GetCursorPosition();
		//static float InputSystem::GetCursorPosition_X(const Vector2D& cursor_pos);
		//static float InputSystem::GetCursorPosition_Y(const Vector2D& cursor_pos);

		/*!
		 * \brief Handles input for toggling full-screen mode.
		 */
		static void HandleFullScreenInput(); // Handle full-screen input


		// Static variables to store window properties and input states

		bool IsKeyReleased(int keyCode);
		bool IsKeyHeld(int keyCode);
		bool IsAnyMouseButtonPressed();

		// Window width and height
		static GLint windowWidth;  // GLFW window width
		static GLint windowHeight; // GLFW window height
		static GLint frameWidth;   // Framebuffer (Viewport) width
		static GLint frameHeight;  // Framebuffer (Viewport) height
		// FPS (frames per second) and deltaTime (time between frames)
		static GLdouble fps;
		static GLdouble deltaTime; // time taken to complete most recent game loop

		// Mouse and keyboard states

		// Whether the left mouse button is pressed (GL_TRUE if pressed, GL_FALSE if not)
		static GLboolean mousestateL;
		static GLboolean windowHovered;
		// Mouse scroll state (stores vertical scroll offset)
		static GLint mouseScrollY;

		static ImVec2 framebufferWindowPos;
		static ImVec2 framebufferWindowSize;

		static std::unordered_map<int, bool> keyStates;
		static std::unordered_map<int, bool> keyPrevStates;         // Stores previous key states
		static std::unordered_map<int, bool> mouseButtonStates;     // Stores current mouse button states
		static std::unordered_map<int, bool> mousePrevButtonStates; // Stores previous mouse button states

		static bool FStoggleState; // Track the state of the F11 key press
		static bool InitOnce;
	}; 

	// Global pointer to the InputSystem instance
	extern InputSystem* inputSystem;

}