#pragma once
/******************************************************************************/
/*!
\file    ConfigData.h
\project FruitLoops
\author  Reagan Tang
\date    Dec 03, 2024
\brief   This file declares global configuration variables under the
         FruitLoops::ConfigData namespace. These variables include window
         dimensions, UI offsets, title, background music path, and font
         rendering properties.

         The ConfigData namespace serves as a central location for
         initialisation and access to key runtime configuration values that
         influence rendering, layout, and presentation.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
namespace FruitLoops {
    namespace ConfigData {
        // Window Properties
        extern int width;
        extern int height;
        extern int imGuiTitleWidthOffset;
        extern int imGuiTitleHeightOffset;
        extern int fontDetail;
        extern std::string title;
        // Background music
        extern std::string BGM;
        // Graphics config
        extern Vector3D fontColor;
        
    };
}