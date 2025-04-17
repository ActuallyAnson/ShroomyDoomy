/******************************************************************************/
/*!
\file  Debug.cpp
\Proj name  Shroomy Doomy
\author  Reagan Tang
\date    Dec 03, 2024
\brief   This file defines the Debug class and its functionalities, 
         including initialization and management of a GLFW error window, 
         displaying error popups, and managing resources. The Debug class 
         aids in debugging by providing visual and console feedback during 
         runtime.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#include "pch.h"
#include "Debug.h"

namespace FruitLoops {
    std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> FruitLoops::Debug::window(nullptr, glfwDestroyWindow);

    /*!
     * \brief Initializes the Debug system, including setting up a GLFW error window.
     *        The error window is created but hidden by default until an error occurs.
     */
    void Debug::Init() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }
        GLFWwindow* raw_window = glfwCreateWindow(400, 200, "Error", nullptr, nullptr);
        window.reset(raw_window);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwHideWindow(window.get());
    }

    /*!
     * \brief Displays an error popup with the specified message. The popup is shown
     *        using a GLFW window, and the error message is printed to the console as well.
     * \param errorMessage The error message to display in the popup.
     */
    void Debug::DisplayErrorPopup(const std::string& errorMessage) {
        if (!window.get()) {
            std::cerr << "Error: GLFW window pointer is null." << std::endl;
            return;
        }

        std::cerr << "Error Popup: " << errorMessage << std::endl;

        glfwShowWindow(window.get());
        glfwMakeContextCurrent(window.get());
        glViewport(0, 0, 400, 200);

        while (!glfwWindowShouldClose(window.get())) {
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

            glBegin(GL_QUADS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(-0.9f, -0.5f);
            glVertex2f(0.9f, -0.5f);
            glVertex2f(0.9f, 0.5f);
            glVertex2f(-0.9f, 0.5f);
            glEnd();

            //glfwSwapBuffers(window.get());
            glfwPollEvents();
        }

        //glfwHideWindow(window);  // Hide window after displaying error
    }

    /*!
     * \brief Cleans up resources used by the Debug system, including destroying the
     *        GLFW error window and terminating GLFW.
     */
    void Debug::CleanUp() {
        window.reset();  // Automatically calls glfwDestroyWindow
        glfwTerminate();
    }

    /*!
    * \brief Displays the current frames per second (FPS) using ImGui. This function
    *        provides a runtime performance metric for debugging purposes.
    */
    void Debug::ShowFPS() {
        ImGui::GetIO().Framerate;
    }
}
