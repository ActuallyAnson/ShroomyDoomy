/******************************************************************************/
/*!
\file    DebugManager.h
\project Shroomy Doomy
\author  Reagan Tang
\date    Dec 03, 2024
\brief   This file declares the DebugManager class and its public interface
         for managing runtime debugging features. The class provides methods
         to toggle debug mode and display helpful development data such as
         FPS, collision information, and grid overlays.

         This system is intended to aid developers during runtime testing by
         enabling or disabling visual debug tools conditionally based on
         the DEBUG_MODE macro.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

class DebugManager {
public:
    void toggleDebugMode();
    void updateDebugFeatures();

    // Additional methods for specific debugging functions can be added here
    void displayFPS();
    void displayCollisionData();
    void renderGrid();

private:
    bool debugModeEnabled = false; // Track whether debug mode is currently enabled
};

#endif // DEBUG_MANAGER_H