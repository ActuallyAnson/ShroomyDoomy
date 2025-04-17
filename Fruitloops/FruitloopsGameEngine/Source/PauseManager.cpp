/******************************************************************************/
/*!
\file    PauseManager.cpp
\project FruitLoops
\author  Foo Ming Sheng
\date    April 5, 2025
\brief   Implements the PauseManager class, which controls game pausing logic
         based on manual input, window focus, and minimisation events.

This module manages global pause and resume states for the FruitLoops engine.
It integrates with GLFW and ImGui to detect system events such as window
minimisation or loss of focus and handles pausing or resuming all audio and
game logic accordingly. It also provides the ability to freeze animations or
disable inputs when the game is paused.

The PauseManager offers an ImGui-based overlay for the pause menu and exposes
static flags that other engine subsystems can query to determine pause-related
behaviour. Manual and automatic pause sources are distinguished for more
granular control.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "PauseManager.h"
#include "Audio.h"  // Ensure this is included for audio control

namespace FruitLoops {

    // Initialize static variables
    bool PauseManager::m_IsPaused = false;
    bool PauseManager::m_IsManuallyPaused = false;
    bool PauseManager::m_IsMinimized = false;
    bool PauseManager::m_HasFocus = true;
    bool PauseManager::m_FreezeAnimations = false; // New flag
    bool PauseManager::m_InputLock = false; // New flag

    /*!
     * \brief Updates the pause state based on manual, minimized, and focus conditions.
     */
    void PauseManager::UpdatePauseState() {
        bool manual = m_IsManuallyPaused;

        // Manual is 1, m_ispaused is 0
        if (manual && !m_IsPaused) {
            ManualPauseGame();
        }
        else if (!manual && m_IsPaused) {
            ManualResumeGame();
        }
    }

    /*!
     * \brief Draws a pause overlay on the screen with pause menu options.
     * \param drawList Pointer to the ImDrawList for rendering.
     * \param framebufferWindowPos Position of the framebuffer window.
     * \param isPaused Boolean indicating if the game is paused.
     */
    void PauseManager::DrawOverlay(ImDrawList* drawList, ImVec2& framebufferWindowPos, bool& isPaused) {
        // Draw Pause Screen Overlay
        drawList = ImGui::GetWindowDrawList();

        // Draw semi-transparent background over framebuffer
        drawList->AddRectFilled(
            framebufferWindowPos,
            ImVec2(framebufferWindowPos.x + InputSystem::framebufferWindowSize.x, framebufferWindowPos.y + InputSystem::framebufferWindowSize.y),
            IM_COL32(0, 0, 0, 150)  // Semi-transparent black
        );

        // Center the pause menu content within the viewport
        ImVec2 pauseTextPos = ImVec2(
            framebufferWindowPos.x + InputSystem::framebufferWindowSize.x / 2.0f,
            framebufferWindowPos.y + InputSystem::framebufferWindowSize.y / 2.0f - 50
        );

        // Center the text and buttons horizontally within the viewport
        drawList->AddText(ImVec2(pauseTextPos.x - ImGui::CalcTextSize("Game Paused").x / 2, pauseTextPos.y), IM_COL32(255, 255, 255, 255), "Game Paused");

        ImVec2 buttonSize = ImVec2(200, 50);
        ImVec2 buttonPos = ImVec2(pauseTextPos.x - buttonSize.x / 2, pauseTextPos.y + 30);

        ImGui::SetCursorScreenPos(buttonPos);
        if (ImGui::Button("Resume", buttonSize)) {
            isPaused = false;  // Unpause the game
        }

        buttonPos.y += buttonSize.y + 10;
        ImGui::SetCursorScreenPos(buttonPos);
        if (ImGui::Button("Settings", buttonSize)) {
            // Open settings menu (implement separately if needed)
        }

        buttonPos.y += buttonSize.y + 10;
        ImGui::SetCursorScreenPos(buttonPos);
        if (ImGui::Button("Quit", buttonSize)) {
            // Implement quit functionality, e.g., set a quit flag or close the window
            // GameActive = false;
        }
    }

    /*!
     * \brief Disables key input while the game is paused.
     */
    void PauseManager::DisableKeyInput()
    {
    }

    /*!
     * \brief Activates manual pause for the game.
     */
    void PauseManager::OnManualPause() {
        m_IsManuallyPaused = true; // Turns true, manually pause turns true
        UpdatePauseState();
    }

    /*!
     * \brief Deactivates manual pause for the game.
     */
    void PauseManager::OffManualPause() {
        m_IsManuallyPaused = false; // Turns true, manually pause turns true
        UpdatePauseState();
    }

    /*!
     * \brief Checks if the game is currently paused.
     * \return True if the game is paused, false otherwise.
     */
    bool PauseManager::IsPaused() {
        return m_IsPaused;
    }

    /*!
     * \brief Pauses the game and all associated systems.
     */
    void PauseManager::PauseGame() {
        if (!m_IsPaused) {
            m_IsPaused = true;
            audioSystem->PauseAll();
            logPause("Game paused.");
        }
    }

    /*!
     * \brief Manually pauses the game, regardless of other states.
     */
    void PauseManager::ManualPauseGame() {
        m_IsPaused = true;
        audioSystem->PauseAll();
        logPause("Game Manually paused.");
    }

    /*!
     * \brief Resumes the game if it meets conditions such as not being manually paused or minimized.
     */
    void PauseManager::ResumeGame() {
        if (m_IsPaused && !m_IsManuallyPaused && !m_IsMinimized && m_HasFocus) {
            m_IsPaused = false;
            audioSystem->ResumeAll();
            logPause("Game resumed.");
        }
    }

    /*!
     * \brief Manually resumes the game, overriding other pause states.
     */
    void PauseManager::ManualResumeGame() {
        m_IsPaused = false;
        audioSystem->ResumeAll();
        logPause("Game Manually paused.");
    }

    /*!
     * \brief Handles pause state when the game window is minimized.
     */
    void PauseManager::HandleMinimize() {
        GLFWwindow* glfwWindow = windowSystem->GetGLFWWindow();

        int isMinimized = glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED);
        int width, height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        if (isMinimized || (width == 0 && height == 0)) {
            if (!m_IsMinimized) {
                m_IsMinimized = true;
                ImGuiPanels::OnIsPaused();
#ifdef DEBUG

                spdlog::info("Game minimized.");
#endif // DEBUG
                UpdatePauseState();
            }
        }
        else {
            if (m_IsMinimized) {
                m_IsMinimized = false;
                //ImGuiPanels::OffIsPaused();
#ifdef DEBUG

                spdlog::info("Game restored from minimize.");
#endif // DEBUG
                UpdatePauseState();
            }
        }
    }

    /*!
     * \brief Handles pause state when the game window gains or loses focus.
     * \param window Pointer to the GLFW window.
     */
    void PauseManager::HandleFocus(GLFWwindow* window) {
        int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);

        if (!focused) {
            if (m_HasFocus) {
                m_HasFocus = false;
                ImGuiPanels::OnIsPaused();
                logPause("Focus lost.");
                UpdatePauseState();
            }
        }
        else {
            if (!m_HasFocus) {
                m_HasFocus = true;
                logPause("Focus lost.");
                UpdatePauseState();
            }
        }
    }

    /*!
     * \brief Checks if animations should be frozen based on pause state.
     * \return True if animations should be frozen, false otherwise.
     */
    bool PauseManager::ShouldFreezeAnimations() {
        return m_FreezeAnimations;
    }

    /*!
     * \brief Checks if input handling should be paused.
     * \return True if input should be paused, false otherwise.
     */
    bool PauseManager::ShouldPauseInputs()
    {
        return m_InputLock;
    }

} // namespace FruitLoops
