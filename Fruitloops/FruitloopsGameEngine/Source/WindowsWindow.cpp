/******************************************************************************/
/*!
\file  WindowsWindow.cpp
\Proj name  Shroomy Doomy
\author  Zhang TongYan
\date    Oct 01, 2024
\brief  This file contains the implementations for the WindowsWindow class 
        and related functions, responsible for creating, initializing, 
        updating, and managing the main game window, handling inputs, 
        and managing graphical context through GLFW and OpenGL.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#include "pch.h"
#include "Graphics.h"
#include "Input.h"
#include "WindowsWindow.h"
#include "AssetsManager.h"
#include "Log.h"

namespace FruitLoops {

	// Static variable to ensure GLFW is only initialized once
	static bool isGLFWInitialized = false;

	WindowsWindow* windowSystem = NULL;

	/***************************************************************************
	 * @brief Factory method to create a Window object
	 * @param props Properties for the window (title, width, height)
	 * @return Pointer to the created WindowsWindow object
	 **************************************************************************/
	WindowsWindow::WindowsWindow() {
		init(WindowProps());
	}
	/***************************************************************************
	 * @brief Constructor for the WindowsWindow class
	 * Initializes the window with the given properties
	 * @param props The properties for the window (title, width, height)
	 **************************************************************************/
	WindowsWindow::WindowsWindow(const WindowProps& props) {
		init(props);  // Call the initialization function
	}

	/***************************************************************************
	 * @brief Destructor for the WindowsWindow class
	 * Cleans up the window resources
	 **************************************************************************/
	WindowsWindow::~WindowsWindow() {
		end();  // Call the cleanup function
	}

	/***************************************************************************
	 * @brief Initializes the window with the given properties
	 * @param props Properties for the window (title, width, height)
	 **************************************************************************/
	void WindowsWindow::init(const WindowProps& props) {
		// Existing code for initializing the window...
		spdlog::info("Initializing GLFW window: {}x{}, Title: {}", props.Width, props.Height, props.Title);

		// Set the global window system pointer
		if (windowSystem) {
			spdlog::error("Multiple instances of WindowsWindow detected! This should not happen.");
			return;  // Exit to prevent overwriting the global pointer
		}

		windowSystem = this;  // Assign the global pointer
		spdlog::info("Set global windowSystem to this instance: {}", static_cast<void*>(windowPointer));

		// Continue with initialization...
		glfwSetWindowUserPointer(windowPointer, &windowData);  // User pointer for GLFW callbacks

		// Set window title, width, and height from props
		windowData.Title = props.Title;
		windowData.Width = props.Width;
		windowData.Height = props.Height;
		// Initialize GLFW if it hasn't been initialized yet
		if (!isGLFWInitialized) {
			if (!glfwInit()) {
				spdlog::error("GLFW init has failed - abort program!!!");
				isGLFWInitialized = false;
			}
			else {
				isGLFWInitialized = true;
			}
			
		}

		glfwSetErrorCallback(InputSystem::error_cb);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
		// Create a GLFW window with the specified width, height, and title
		// Logging of creation of window
		spdlog::info("Creating GLFW window with dimensions: {}x{}", props.Width, props.Height);


		windowPointer = glfwCreateWindow((int)props.Width, (int)props.Height, windowData.Title.c_str(), nullptr, nullptr);

		// ImGui Main Loop
		
		// Set the current context to this window

		// Logging for successful window
		if (windowPointer) {
			spdlog::info("GLFW window created successfully: {}x{}", props.Width, props.Height);
		}
		else {
			spdlog::error("Failed to create GLFW window with dimensions: {}x{}", props.Width, props.Height);
		}

		glfwMakeContextCurrent(windowPointer);

		// Set the user pointer somewhere during initialization.
		glfwSetWindowUserPointer(windowPointer, assetsManager);

		// Use a lambda to call the member function.
		glfwSetDropCallback(windowPointer, [](GLFWwindow* window, int count, const char** paths) {
			// Retrieve the assetsManager pointer from the window's user pointer.
			AssetsManager* assetsManagerPtr = static_cast<AssetsManager*>(glfwGetWindowUserPointer(window));
			if (assetsManagerPtr) {
				assetsManagerPtr->HandleDrop(window, count, paths);
			}
			});

		glfwSwapInterval(1);

		glfwSetInputMode(windowPointer, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		GLenum err = glewInit();
		if (GLEW_OK != err) {
			spdlog::error("Unable to initialize GLEW - error: {}", reinterpret_cast<const char*>(glewGetErrorString(err)));
			return;
		}
		if (GLEW_VERSION_4_5) {
			spdlog::info("Using GLEW version: {}", reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));
			spdlog::info("Driver supports OpenGL 4.5");

		}
		else {
			spdlog::warn("The driver may lack full compatibility with OpenGL 4.5, potentially limiting access to advanced features.");
		}
		// Store user data (window properties) in the GLFW window object
		glfwSetWindowUserPointer(windowPointer, &windowData);

		// Enable VSync by default
		SetVSync(true);
		windowSystem = this;
	}

	/***************************************************************************
	 * @brief Cleans up and destroys the window
	 **************************************************************************/
	void WindowsWindow::end() {
		// Destroy the GLFW window
		glfwTerminate();
	}

	/***************************************************************************
	 * @brief Handles window updates such as polling events and swapping buffers
	 **************************************************************************/
	void WindowsWindow::PollEvents(){
		glfwPollEvents();
	}

	/**************************************************************************
	* @brief
	*    Swaps the front and back buffers to display the rendered frame.
	*************************************************************************/
	void WindowsWindow::SwapBuffers() {
		glfwSwapBuffers(windowPointer);
	}

	/**************************************************************************
	 * @brief Enables or disables VSync (vertical synchronization)
	 * @param enabled True to enable VSync, false to disable
	 *************************************************************************/
	void WindowsWindow::SetVSync(bool enabled) {
		if (enabled) {
			// Enable VSync (locks the frame rate to the display's refresh rate)
			glfwSwapInterval(1);
		}
		else {
			// Disable VSync (unlocks the frame rate)
			glfwSwapInterval(0);
		}
		windowData.VSync = enabled;  // Update VSync status in the window data
	}

	/**************************************************************************
	 * @brief Checks if VSync is currently enabled
	 * @return True if VSync is enabled, false otherwise
	 *************************************************************************/
	bool WindowsWindow::IsVSync() const {
		return windowData.VSync;
	}

	GLFWwindow* WindowsWindow::GetGLFWWindow() const {
		if (!windowPointer) {
#ifdef _DEBUG
			spdlog::error("Window pointer is null! Ensure the window is properly initialized.");
#endif
		}
		return windowPointer;
	}

	// Enter full-screen mode
	void WindowsWindow::EnterFullScreen() {
		if (isFullScreen) return;  // Already in full-screen

		spdlog::info("Entering full-screen mode...");


		// Store current windowed mode position and size
		glfwGetWindowPos(windowPointer, &windowedX, &windowedY);
		glfwGetWindowSize(windowPointer, &windowedWidth, &windowedHeight);

		spdlog::info("Stored windowed mode position: ({}, {}), size: ({}x{})", windowedX, windowedY, windowedWidth, windowedHeight);


		// Get monitor and video mode
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (!monitor || !mode) {

			spdlog::error("Failed to get monitor or video mode.");

			return;
		}

		// Switch to full-screen
		glfwSetWindowMonitor(windowPointer, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		isFullScreen = true;

		spdlog::info("Entered full-screen mode.");

	}

	// Exit full-screen mode
	void WindowsWindow::ExitFullScreen() {
		if (!isFullScreen) return;  // Already in windowed mode


		spdlog::info("Exiting full-screen mode...");
		spdlog::info("Restoring windowed mode position: ({}, {}), size: ({}x{})",
			windowedX, windowedY, windowedWidth, windowedHeight);


		// Switch back to windowed mode
		glfwSetWindowMonitor(windowPointer, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
		isFullScreen = false;


		spdlog::info("Exited full-screen mode.");

	}

	// Toggle full-screen mode
	void WindowsWindow::ToggleFullScreen() {
		if (isFullScreen) {
			ExitFullScreen();
		}
		else {
			EnterFullScreen();
		}
	}

	// Check if the window is in full-screen mode
	bool WindowsWindow::IsFullScreen() const {
		return isFullScreen;
	}
}