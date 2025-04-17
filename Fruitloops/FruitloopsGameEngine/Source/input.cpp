/****************************************************************************/
/*!
\file		input.cpp
\Proj name  Fruitloops
\author 	Zhang TongYan
\date   	Oct 03, 2024
\brief		This file contains the definition of the InputSystem class which
            manages user input and system events for the Fruitloops project.
            It provides mechanisms to handle keyboard, mouse, and other device
            inputs using callbacks and maintains state relevant to input devices. for Renderer class, used for batch rendering
All content (C) 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /****************************************************************************/

#include "pch.h"
#include "Input.h"
#include "WindowsWindow.h"
#include "Log.h"
#include "Graphics.h"
#include "Core.h"
#include <glm/gtc/matrix_transform.hpp>

namespace FruitLoops {
    
    GLdouble InputSystem::fps;            
    GLdouble InputSystem::deltaTime;  

    GLint InputSystem::windowWidth = ConfigData::width;  
    GLint InputSystem::windowHeight = ConfigData::height; 
    GLint InputSystem::frameWidth;   
    GLint InputSystem::frameHeight;  
    GLint InputSystem::mouseScrollY = 0;
    
    ImVec2 InputSystem::framebufferWindowPos = ImVec2(0, 0);
    ImVec2 InputSystem::framebufferWindowSize = ImVec2(0, 0);

    GLboolean InputSystem::windowHovered = false;
    GLboolean InputSystem::mousestateL = GL_FALSE;

    bool InputSystem::FStoggleState = false;
    bool InputSystem::InitOnce = false;

    std::unordered_map<int, bool> InputSystem::keyStates;
    InputSystem* inputSystem = NULL;

    /*!
    \fn InputSystem::InputSystem()
    \brief Constructor that initializes the InputSystem and sets it as the global instance.
    */
    InputSystem::InputSystem()
    {
        inputSystem = this;
    }

    /*!
    \fn InputSystem::~InputSystem()
    \brief Destructor that resets the global instance pointer.
    */
    InputSystem::~InputSystem() {
        inputSystem = nullptr;
    }

    /*!
    \brief Initializes the input system and sets up event callbacks for the window.
    */    
    void InputSystem::Initialize() {
        if (!windowSystem) {
#ifdef _DEBUG
            spdlog::error("windowSystem is null in InputSystem::Initialize! Initialization failed.");
#endif
            return;
        }

        GLFWwindow* glfwWindow = windowSystem->GetWindow();
        if (!glfwWindow) {
#ifdef _DEBUG
            spdlog::error("GLFW window is null in InputSystem::Initialize!");
#endif
            return;
        }

        glfwSetWindowUserPointer(glfwWindow, this);  // Set the user pointer for callbacks
#ifdef _DEBUG
        spdlog::info("Set GLFW user pointer for InputSystem: {}", static_cast<void*>(glfwWindow));
#endif

        // Setup callbacks for the GLFW window
        setup_event_callbacks(glfwWindow);

        glfwSetWindowUserPointer(windowSystem->GetWindow(), this);
        windowWidth = ConfigData::width;
        windowHeight = ConfigData::height;
        setup_event_callbacks(windowSystem->GetWindow());
    }

    /*!
    \brief Registers various input callbacks with GLFW for the given window.
    \param window Pointer to the GLFW window for which callbacks are to be set.
    */
    void InputSystem::setup_event_callbacks(GLFWwindow* window) {
        glfwSetFramebufferSizeCallback(window, InputSystem::fbsize_cb); // Window resize callback
        glfwSetKeyCallback(window, InputSystem::key_cb);    // Keyboard input callback
        glfwSetMouseButtonCallback(window, InputSystem::mousebutton_cb);    // Mouse button input callback
        //glfwSetCursorPosCallback(window, InputSystem::mousepos_cb); // Mouse movement callback
        glfwSetScrollCallback(window, InputSystem::mousescroll_cb); // Mouse scroll callback
    }

    /*!
    \brief Cleans up GLFW resources before termination of the application.
    */
    void InputSystem::cleanup() {
        glfwTerminate();
    }

    /*!
    \brief Checks if a specific key is pressed.
    \param keyCode The key code to check.
    \return True if the key is pressed, otherwise false.
    */
    bool InputSystem::IsKeyPressed(int keyCode) {
        // Return the key state if it exists in the map
        auto it = keyStates.find(keyCode);
        return it != keyStates.end() && it->second;
    }

    /*!
    \brief Handles keyboard input events.
    \param pwin Pointer to the GLFW window.
    \param key The key involved in the event.
    \param scancode Platform-specific scancode.
    \param action The type of action (press, release, repeat).
    \param mod Modifier keys state.
    */
    void InputSystem::key_cb(GLFWwindow* pwin, int key, int scancode, int action, int mod) {
        
        UNUSED_PARAM(scancode);
        UNUSED_PARAM(mod);
        UNUSED_PARAM(pwin);
        if (!PauseManager::ShouldPauseInputs()) {
            InputSystem* self = static_cast<InputSystem*>(glfwGetWindowUserPointer(windowSystem->GetWindow()));

            // DEBUG
#ifdef _DEBUG
            ImGui_ImplGlfw_KeyCallback(pwin, key, scancode, action, mod);
            // check if imgui is capturing keyboard
            if (ImGui::GetIO().WantCaptureKeyboard) {
                return;  // If ImGui is handling the input, don't process it in the game
            }
            if (GLFW_PRESS == action) {
                logInput("Key pressed");
            }
            else if (GLFW_REPEAT == action) {
                logInput("Key repeatedly pressed");
            }
            else if (GLFW_RELEASE == action) {
                logInput("Key released");
            }
#endif

            if (GLFW_PRESS == action)
            {
                self->NotifyObservers(new KeyPressMessage(key, action));

                if (key == GLFW_KEY_P || key == GLFW_KEY_L || key == GLFW_KEY_M) {
                    if (keyStates[key]) {
                        keyStates[key] = false;
                    }
                    else {
                        keyStates[key] = true;
                    }
                }
                else if (key == GLFW_KEY_1) {

                }
                else if (key == GLFW_KEY_2) {

                }
                else if (key == GLFW_KEY_3) {

                }
                else if (key == GLFW_KEY_4) {

                }
                else if (key == GLFW_KEY_F) {
                    audioSystem->PauseAll();
                }
                else if (key == GLFW_KEY_G) {
                    audioSystem->ResumeAll();
                }

                else if (key == GLFW_KEY_N) {
                    audioSystem->ResumeAll();
                }

                else {
                    keyStates[key] = true;
                }
            }

            if (GLFW_REPEAT == action) {
                keyStates[key] = true;
                self->NotifyObservers(new KeyPressMessage(key, action));
            }

            if (GLFW_RELEASE == action)
            {
                self->NotifyObservers(new KeyPressMessage(key, action));
                if (!(key == GLFW_KEY_P || key == GLFW_KEY_L || key == GLFW_KEY_M)) {
                    keyStates[key] = false;
                }

            }
        }
    }

    /*!
    \brief Handles mouse button events.
    \param pwin Pointer to the GLFW window.
    \param button The mouse button involved in the event.
    \param action The type of action (press or release).
    \param mod Modifier keys state.
    */
    void InputSystem::mousebutton_cb(GLFWwindow* pwin, int button, int action, int mod) {

        
        // Pass mouse button input to ImGui
        #ifdef _DEBUG
        ImGui_ImplGlfw_MouseButtonCallback(pwin, button, action, mod);
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS) {
                logInput("Left mouse button pressed");
            }

            if (action == GLFW_RELEASE) {
                logInput("Left mouse button released");
            }
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS) {
                logInput("Right mouse button pressed");
            }
            if (action == GLFW_RELEASE) {
                logInput("Right mouse button released");
            }
            break;
        default:
            break;
        }
        #endif

        UNUSED_PARAM(pwin);
        UNUSED_PARAM(mod);

        if (action == GLFW_PRESS)
            mouseButtonStates[button] = true;
        else if (action == GLFW_RELEASE)
            mouseButtonStates[button] = false;

        // game input handling logic
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS) {
                mousestateL = true; // Set left mouse button state
            }

            if (action == GLFW_RELEASE) {
                mousestateL = false; // reset left mouse button state
            }
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS) {
                // do nth yet
            }
            if (action == GLFW_RELEASE) {
                // do nth yet
            }
            break;

        default:
            break;
        }
    }
        

    /*!
    \brief Handles mouse position events.
    \param pwin Pointer to the GLFW window.
    \param xpos The x-coordinate of the cursor position.
    \param ypos The y-coordinate of the cursor position.
    */
    void InputSystem::mousepos_cb(GLFWwindow* pwin, double xpos, double ypos) {

        #ifdef _DEBUG
        logInput("Mouse cursor position: ({:.2f}, {:.2f})", xpos, ypos);
        ImGui_ImplGlfw_CursorPosCallback(pwin, xpos, ypos);
        #endif
        UNUSED_PARAM(xpos);
        UNUSED_PARAM(ypos);
        UNUSED_PARAM(pwin);
    }

    /*!
    \brief Handles mouse scroll events.
    \param pwin Pointer to the GLFW window.
    \param xoffset Horizontal scroll offset.
    \param yoffset Vertical scroll offset.
    */
    void InputSystem::mousescroll_cb(GLFWwindow* pwin, double xoffset, double yoffset) {

        #ifdef _DEBUG
        // Pass scroll input to ImGui
        ImGui_ImplGlfw_ScrollCallback(pwin, xoffset, yoffset);
        logInput("Mouse scroll wheel offset : ({:.2f}, {:.2f})", xoffset, yoffset);
        #endif
        UNUSED_PARAM(xoffset);
        UNUSED_PARAM(pwin);
        if (windowHovered) {
            mouseScrollY += static_cast<GLint>(yoffset);
        }
    }

    /*!
    \brief Handles GLFW errors and logs them for debugging.
    \param error Error code.
    \param description Error description.
    */
    void InputSystem::error_cb( int error, char const* description) {

        UNUSED_PARAM(error);
        UNUSED_PARAM(description);

        #ifdef _DEBUG
        std::cerr << "GLFW error: " << description << std::endl;
        #endif
    }

    /*!
    \brief Handles framebuffer size changes.
    \param ptr_win Pointer to the GLFW window.
    \param w New width of the framebuffer.
    \param h New height of the framebuffer.
    */
    void InputSystem::fbsize_cb(GLFWwindow* ptr_win, int w, int h) {

        UNUSED_PARAM(ptr_win);

        #ifdef _DEBUG
        logInput("fbsize_cb getting called!!!");
        #endif
        
        InputSystem::windowWidth = w;
        InputSystem::windowHeight = h;
    }

    /*!
    \brief Updates the input system every frame.
    \param deltaTime Time elapsed since the last update.
    */
    void InputSystem::Update(float dt) {

        UNUSED_PARAM(dt);
        glm::vec3 ndcBottomLeft(-1.0f, -1.0f, 1.0f);
        glm::vec3 ndcTopRight(1.0f, 1.0f, 1.0f);

        // Use the inverse of the world-to-NDC matrix to transform NDC corners to world space
        glm::mat3 inverseCameraMatrix = glm::inverse(GRAPHICS->camera2d.worldToNDCMatrix);

        glm::vec3 worldBottomLeft = inverseCameraMatrix * ndcBottomLeft;
        glm::vec3 worldTopRight = inverseCameraMatrix * ndcTopRight;

        // Calculate logical frame dimensions
        frameWidth = static_cast<GLint>(worldTopRight.x - worldBottomLeft.x);
        frameHeight = static_cast<GLint>(worldTopRight.y - worldBottomLeft.y);
        update_time(1.0);
    }

    /*!
    \brief Calculates time between frames and updates FPS.
    \param fps_calc_interval Interval in seconds to calculate FPS.
    */
    void InputSystem::update_time(double fps_calc_interval) {
        // Static variables to keep track of time
        static double prev_time = glfwGetTime();
        double curr_time = glfwGetTime();
        deltaTime = curr_time - prev_time;     // Calculate delta time
        prev_time = curr_time;  // Update previous time

        // fps calculations
        static double count = 0.0; // Frame counter
        static double start_time = glfwGetTime();   // Start time of the frame
        double elapsed_time = curr_time - start_time;   // Total elapsed time

        ++count;

        // Update FPS every fps_calc_interval seconds (clamped between 0 and 10)
        fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
        fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
        if (elapsed_time > fps_calc_interval) {
            InputSystem::fps = count / elapsed_time;    // Update FPS
            start_time = curr_time;     // Reset start time
            count = 0.0;        // Reset frame counter
        }
    }

    /*!
    \brief Retrieves the cursor position in normalized device coordinates.
    \return The cursor position as a Vector2D.
    */
    Vector2D InputSystem::GetCursorPosition() {
        double mouseX, mouseY;
        glfwGetCursorPos(windowSystem->GetWindow(), &mouseX, &mouseY);

        #ifdef _DEBUG
        // Adjust mouse position to be relative to the framebuffer window
        mouseX -= framebufferWindowPos.x + ConfigData::imGuiTitleWidthOffset;
        mouseY -= framebufferWindowPos.y + ConfigData::imGuiTitleHeightOffset;

        // Flip the y-coordinate (ImGui origin is top-left, OpenGL is bottom-left)
        mouseY = framebufferWindowSize.y - mouseY;

        float normalizedX = static_cast<float>(mouseX) / framebufferWindowSize.x;
        float normalizedY = static_cast<float>(mouseY) / framebufferWindowSize.y;
        #else
        // Flip the y-coordinate (GLFW provides top-left origin, OpenGL uses bottom-left)
        mouseY = windowHeight - mouseY;

        float normalizedX = static_cast<float>(mouseX) / windowWidth;
        float normalizedY = static_cast<float>(mouseY) / windowHeight;
        #endif

        // Map normalized coordinates to NDC [-1, 1]
        float ndcX = normalizedX * 2.0f - 1.0f;
        float ndcY = normalizedY * 2.0f - 1.0f;

        // Transform NDC coordinates to world coordinates using the inverse camera matrix
        glm::vec3 ndcPos(ndcX, ndcY, 1.0f);
        glm::mat3 inverseCameraMatrix = glm::inverse(GRAPHICS->camera2d.worldToNDCMatrix);
        glm::vec3 worldPos = inverseCameraMatrix * ndcPos;

        return Vector2D(worldPos.x, worldPos.y);
    }

    std::unordered_map<int, bool> InputSystem::keyPrevStates;
    std::unordered_map<int, bool> InputSystem::mouseButtonStates;
    std::unordered_map<int, bool> InputSystem::mousePrevButtonStates;

    bool InputSystem::IsKeyReleased(int keyCode) {
        auto it = keyPrevStates.find(keyCode);
        return it != keyPrevStates.end() && it->second && !keyStates[keyCode];
    }

    bool InputSystem::IsKeyHeld(int keyCode) {
        auto it = keyStates.find(keyCode);
        return it != keyStates.end() && it->second && keyPrevStates[keyCode];
    }

    bool InputSystem::IsAnyMouseButtonPressed() {
        for (const auto& buttonState : mouseButtonStates) {
            if (buttonState.second) return true;
        }
        return false;
    }

/*!
* \brief Handles input for toggling full-screen mode.
*/
    void InputSystem::HandleFullScreenInput() {
        if (glfwGetKey(windowSystem->GetWindow(), GLFW_KEY_F11) == GLFW_PRESS || (InitOnce == 1)) {
            if (!FStoggleState) {  // Ensure only one toggle per key press
#ifdef _DEBUG
                spdlog::info("F11 key pressed. Toggling full-screen mode.");
#endif
                windowSystem->ToggleFullScreen();
                FStoggleState = true;
                InitOnce = 0;
            }
        }

        if (glfwGetKey(windowSystem->GetWindow(), GLFW_KEY_F11) == GLFW_RELEASE) {
            FStoggleState = false;  // Reset toggle detection on key release
        }
    }
}
