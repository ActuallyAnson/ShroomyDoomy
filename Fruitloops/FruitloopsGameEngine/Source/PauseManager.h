/******************************************************************************/
/*!
\file    PauseManager.h
\project FruitLoops
\author  Foo Ming Sheng
\date    April 5, 2025
\brief   Declares the PauseManager class, which manages global pause state
         logic based on manual triggers, window focus, and minimisation.

The PauseManager handles all pause-related logic in the FruitLoops engine. It
exposes functions for manually pausing and resuming the game, and integrates
with GLFW to automatically respond to window events like minimisation or loss
of focus. The manager also governs input locking and animation freezing during
paused states.

An ImGui-based overlay can be drawn using the DrawOverlay function, allowing
users to resume the game, access settings, or quit. All pause-related flags
are implemented as static members, ensuring consistent global state handling
throughout the engine.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#pragma once
#include <GLFW/glfw3.h>
#include "Audio.h"
#include "Log.h"

namespace FruitLoops {

    class PauseManager {
    public:
        /*!
         * \brief Activates manual pause state, typically triggered by the user.
         */
        static void OnManualPause();

        /*!
         * \brief Deactivates manual pause state, resuming normal pause conditions.
         */
        static void OffManualPause();

        /*!
         * \brief Checks if the game is currently paused.
         * \return True if the game is paused, false otherwise.
         */
        static bool IsPaused();

        /*!
         * \brief Handles the game's pause state when the window is minimized.
         *        Automatically pauses the game if minimized.
         */
        static void HandleMinimize();

        /*!
          * \brief Handles changes in the window's focus state.
          * \param window Pointer to the GLFW window instance.
          * Logs when the window loses or regains focus.
          */
        static void HandleFocus(GLFWwindow* window);

        /*!
         * \brief Pauses the game and all associated systems forcefully.
         */
        static void PauseGame();          // Force pause the game

        /*!
         * \brief Resumes the game and all associated systems forcefully.
         */
        static void ResumeGame();         // Force resume the game

        /*!
         * \brief Pauses the game manually, overriding other states.
         */
        static void ManualPauseGame();

        /*!
         * \brief Resumes the game manually, overriding other states.
         */
        static void ManualResumeGame();

        /*!
         * \brief Updates the game's pause state based on current conditions,
         *        including manual input, window focus, and minimization.
         */
        static void UpdatePauseState();

        /*!
         * \brief Draws an overlay for the pause menu, including options for resuming,
         *        settings, and quitting.
         * \param drawlist Pointer to ImDrawList for rendering.
         * \param framebufferWindowPos Position of the framebuffer window.
         * \param isPaused Reference to the game's pause state.
         */
        static void DrawOverlay(ImDrawList* drawlist, ImVec2& framebufferWindowPos, bool& isPaused);

        /*!
         * \brief Disables key input when certain conditions are met, such as during pause states.
         */
        static void DisableKeyInput();

        /*!
         * \brief Determines if animations should be frozen during a pause state.
         * \return True if animations should be frozen, false otherwise.
         */
        static bool ShouldFreezeAnimations();

        /*!
         * \brief Determines if input handling should be disabled during a pause state.
         * \return True if input should be disabled, false otherwise.
         */
        static bool ShouldPauseInputs();

    private:
        // Static variables for tracking pause states
        static bool m_IsPaused;          // Overall game pause state
        static bool m_IsManuallyPaused;  // Whether the user manually paused
        static bool m_IsMinimized;       // Whether the game is minimized
        static bool m_HasFocus;          // Whether the game window has focus
        static bool m_InputLock; // New flag
        static bool m_FreezeAnimations; // New flag
    };

} // namespace FruitLoops
