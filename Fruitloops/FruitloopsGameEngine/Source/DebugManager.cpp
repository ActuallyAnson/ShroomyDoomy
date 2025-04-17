/******************************************************************************/
/*!
\file    DebugManager.cpp
\project Shroomy Doomy
\author  Reagan Tang
\date    Dec 03, 2024
\brief   This file defines the DebugManager class, which is responsible for
         toggling and updating debug features during runtime. These include
         displaying frames per second (FPS), collision data, and a visual grid
         overlay. It is designed to assist developers during development by
         providing helpful runtime debug information.

         Debug features are conditionally compiled using the DEBUG_MODE macro.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "DebugManager.h"
#include <iostream>

void DebugManager::toggleDebugMode() {
    debugModeEnabled = !debugModeEnabled;
    std::cout << "Debug Mode: " << (debugModeEnabled ? "ON" : "OFF") << std::endl;
}

void DebugManager::updateDebugFeatures() {
    if (debugModeEnabled) {
        displayFPS();
        displayCollisionData();
        renderGrid();
    }
}

void DebugManager::displayFPS() {
#ifdef DEBUG_MODE
    // Here, you would have code to calculate and display FPS
    std::cout << "FPS: [calculated FPS]" << std::endl;
#endif
}

void DebugManager::displayCollisionData() {
#ifdef DEBUG_MODE
    // Code to display collision data
    std::cout << "Collision Points: [details]" << std::endl;
#endif
}

void DebugManager::renderGrid() {
#ifdef DEBUG_MODE
    // Code to overlay a grid on the game screen
    std::cout << "Grid displayed on the game screen" << std::endl;
#endif
}