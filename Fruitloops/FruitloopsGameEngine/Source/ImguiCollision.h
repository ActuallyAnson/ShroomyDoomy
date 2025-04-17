/******************************************************************************/
/*!
\file    DebugUtilities.h
\Proj    Shroomy Doomy
\author  Foo Ming Sheng
\date    Nov 08, 2024
\brief   This file contains utility functions and variables for debugging
         purposes in the FruitLoops namespace. It includes a display function
         for debug information and a function to check if the cursor is over
         a bounding box, as well as a toggle to show collision boxes.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#pragma once

#include "Collision.h"  // For MinMax2D and collision-related functions


namespace FruitLoops {
    static bool showCollisionBox = false;  // Set to true to always show the collision boxes

/**
* @brief Displays debug information for an object in an ImGui window.
*
* This function creates an ImGui window to display details about an object's
* position, scale, bounding box coordinates, and whether the cursor is within
* the bounding box.
*
* @param position The position of the object (x, y).
* @param scale The scale of the object (width, height).
* @param topLeft The top-left corner of the bounding box.
* @param bottomRight The bottom-right corner of the bounding box.
* @param isCursorOverBox Indicates whether the cursor is inside the bounding box.
*/
    void DisplayDebugInfo(const Vector2D& position, const Vector2D& scale, const Vector2D& topLeft, const Vector2D& bottomRight, bool isCursorOverBox);

/**
* @brief Checks if the cursor is within a specified bounding box.
*
* This function calculates whether the current cursor position lies within
* the rectangular bounding box defined by its top-left and bottom-right corners.
*
* @param topLeft The top-left corner of the bounding box.
* @param bottomRight The bottom-right corner of the bounding box.
* @return True if the cursor is inside the bounding box, false otherwise.
*/
    bool IsCursorOverBoundingBox(const ImVec2& topLeft, const ImVec2& bottomRight);
}  // namespace FruitLoops