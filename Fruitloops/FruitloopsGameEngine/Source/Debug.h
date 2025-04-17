/******************************************************************************/
/*!
\file  Debug.h
\Proj name  Shroomy Doomy
\author  Reagan Tang
\date    Dec 03, 2024
\brief   This file declares the Debug class and its functionalities, 
         including initialization and management of a GLFW error window, 
         displaying error popups, and managing resources. The Debug class 
         aids in debugging by providing visual and console feedback during 
         runtime.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"

namespace FruitLoops {
    /*!
     * \class Debug
     * \brief The Debug class provides functionalities to assist in debugging, such as
     *        managing a GLFW-based error window, displaying error popups, and showing
     *        runtime performance metrics like FPS.
     */
    class Debug {
    private:
        static std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> window;

    public:
        /*!
         * \brief Initializes the Debug system, setting up a GLFW error window for use during
         *        runtime debugging. The error window remains hidden until explicitly triggered.
         */
        static void Init();

        /*!
         * \brief Displays an error popup window with a specified error message. The popup uses
         *        a GLFW window and provides visual feedback about the error.
         * \param errorMessage The error message to display in the popup window.
         */
        static void DisplayErrorPopup(const std::string& errorMessage);

        /*!
         * \brief Cleans up resources allocated by the Debug system, including destroying
         *        the GLFW error window and terminating the GLFW library.
         */
        static void CleanUp();  // Ensure it's accessible

        /*!
         * \brief Displays the current frames per second (FPS) using ImGui. This function
         *        is useful for monitoring the performance of the application during runtime.
         */
        static void ShowFPS();
    };
}