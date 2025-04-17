/******************************************************************************/
/*!
\file    DebugUtilities.cpp
\Proj    Shroomy Doomy
\author  Foo Ming Sheng
\date    Nov 08, 2024
\brief   Implements debugging functions for displaying object details and
         detecting cursor overlap with bounding boxes in FruitLoops.

         DisplayDebugInfo shows object position, scale, and bounding box
         coordinates in an ImGui window. IsCursorOverBoundingBox checks if
         the cursor is within given bounds for interactive debugging.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#include "pch.h"
#include "Collision.h"
#include "ImguiCollision.h"
#include "Log.h"

namespace FruitLoops {
/**
* @brief Displays debug information for an object in an ImGui window.
*
* This function creates an ImGui window that displays information such as
* object position, scale, bounding box coordinates, and whether the cursor
* is over the bounding box.
*
* @param position The position of the object (x, y).
* @param scale The scale of the object (width, height).
* @param topLeft The top-left corner of the bounding box.
* @param bottomRight The bottom-right corner of the bounding box.
* @param isCursorOverBox Indicates whether the cursor is inside the bounding box.
*/
    void DisplayDebugInfo(const Vector2D& position, const Vector2D& scale, const Vector2D& topLeft, const Vector2D& bottomRight, bool isCursorOverBox) {

        ImGui::Begin("Debug Info");  // Create a window to display debug info
        ImGui::Text("Object Position: (%.2f, %.2f)", position.x, position.y);
        ImGui::Text("Object Scale: (%.2f, %.2f)", scale.x, scale.y);
        ImGui::Separator(); // Adds a visual separator line
        ImGui::Text("Top Left Position (Red Box): (%.2f, %.2f)", topLeft.x, topLeft.y);
        ImGui::Text("Bottom Right Position (Red Box): (%.2f, %.2f)", bottomRight.x, bottomRight.y);

        // Display different text based on the cursor's position
        if (isCursorOverBox) {
            ImGui::Text("Cursor is over the bounding box!");
        }
        else {
            ImGui::Text("Cursor is outside the bounding box.");
        }

        ImGui::End();  // End the ImGui window
    }

/**
* @brief Checks if the cursor is over a specified bounding box.
*
* This function determines whether the current cursor position falls within
* the bounds of a given rectangular area, defined by its top-left and
* bottom-right corners.
*
* @param topLeft The top-left corner of the bounding box.
* @param bottomRight The bottom-right corner of the bounding box.
* @return True if the cursor is inside the bounding box, false otherwise.
*/
    bool IsCursorOverBoundingBox(const ImVec2& topLeft, const ImVec2& bottomRight) {
        ImVec2 cursorPos = ImGui::GetMousePos();
        logImgui("Testing for collision.\n");
        // Check if the cursor is within the bounding box
        return (cursorPos.x >= topLeft.x && cursorPos.x <= bottomRight.x &&
            cursorPos.y >= topLeft.y && cursorPos.y <= bottomRight.y);
    }

}
