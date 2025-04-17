/******************************************************************************/
/*!
\file  WindowsWindow.h
\Proj name  Shroomy Doomy
\author  Zhang TongYan
\date    Oct 01, 2024
\brief  This file contains the declarations to the implementations for the WindowsWindow class
		and related functions, responsible for creating, initializing,
		updating, and managing the main game window, handling inputs,
		and managing graphical context through GLFW and OpenGL.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#pragma once

namespace FruitLoops {

	struct WindowProps {
		std::string Title;
		int Width;
		int Height;

		WindowProps(const std::string& title = "Fruitloops Engine",
			unsigned int width = 1280,
			unsigned int height = 720) : Title(title), Width(width), Height(height) {}
	};
	/***************************************************************************
	 * @brief Windows-specific implementation of a window.
	 *
	 * This class provides the necessary functionality to create, manage, and control a window
	 * in a Windows environment using GLFW for OpenGL context handling and event polling.
	 **************************************************************************/
	class WindowsWindow {
	public:

		/***************************************************************************
		 * @brief Constructor for the WindowsWindow class.
		 * @param props Properties for the window (title, width, height).
		 **************************************************************************/
		WindowsWindow();

		/***************************************************************************
		* @brief Constructor for the WindowsWindow class
		* Initializes the window with the given properties
		* @param props The properties for the window (title, width, height)
		**************************************************************************/
		WindowsWindow(const WindowProps& props);

		/**************************************************************************
		 * @brief Destructor for the WindowsWindow class.
		 * Cleans up and destroys the window.
		 *************************************************************************/
		virtual ~WindowsWindow();

		/***************************************************************************
		* @brief Handles window updates such as polling events and swapping buffers
		**************************************************************************/
		void PollEvents();

		/**************************************************************************
		* @brief
		*    Swaps the front and back buffers to display the rendered frame.
		*************************************************************************/
		void SwapBuffers();

		/**************************************************************************
		 * @brief Gets the width of the window.
		 * @return The width of the window in pixels.
		 *************************************************************************/
		inline unsigned int GetWidth() const { return windowData.Width; };

		/**************************************************************************
		 * @brief Gets the height of the window.
		 * @return The height of the window in pixels.
		 *************************************************************************/
		inline unsigned int GetHeight() const { return windowData.Height; };

		inline GLFWwindow* GetWindow() const { return windowPointer; };

		/**************************************************************************
		 * @brief Sets the event callback function to handle window events (e.g., input, resizing).
		 * @param callback The event callback function to be used.
		 *************************************************************************/
		 // inline void SetEventCallback(const EventCallbackFn& callback) override { windowData.EventCallback = callback; };

		 /**************************************************************************
		  * @brief Enables or disables VSync (vertical synchronization).
		  * @param enabled True to enable VSync, false to disable.
		  *************************************************************************/
		void SetVSync(bool enabled);

		/**************************************************************************
		 * @brief Checks if VSync is enabled.
		 * @return True if VSync is enabled, false otherwise.
		 *************************************************************************/
		bool IsVSync() const;

		GLFWwindow* GetGLFWWindow() const;

		void EnterFullScreen();         // Function to enter full-screen mode
		void ExitFullScreen();          // Function to exit full-screen mode
		void ToggleFullScreen();        // Toggle between full-screen and windowed mode
		bool IsFullScreen() const;      // Check if the window is currently in full-screen mode

	private:
		/**************************************************************************
		 * @brief Initializes the window with the provided properties.
		 * @param props Properties for the window (title, width, height).
		 *************************************************************************/
		virtual void init(const WindowProps& props);

		/**************************************************************************
		 * @brief Cleans up the window, releasing any resources.
		 *************************************************************************/
		virtual void end();

	private:
		// Pointer to the GLFW window instance
		GLFWwindow* windowPointer;

		/**************************************************************************
		 * @brief Struct to hold window-specific data such as title, dimensions, and VSync status.
		 *************************************************************************/
		struct WindowData {
			std::string Title; //< The window title
			unsigned int Width, Height; //< Window dimensions in pixels
			bool VSync; //< Whether VSync is enabled

		};

		// Instance of the WindowData structure that stores the window's data
		WindowData windowData;

		bool isFullScreen = false;      // Track the full-screen state
		int windowedX = 0, windowedY = 0;         // Store the previous position of the window
		int windowedWidth = 0, windowedHeight = 0; // Store the previous size of the window

	};

	static WindowProps windowProperties;

	extern WindowsWindow* windowSystem;
}