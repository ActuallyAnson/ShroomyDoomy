/******************************************************************************
/*!
\file  Input.cs
\Proj name  Shroomy Doomy
\author  Gareth
\date    Feb 03, 2024
\brief  This file implements the Input static class, which provides methods
       for querying user input such as mouse buttons, keyboard keys, cursor position,
       and mouse scroll data.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;
using System.Numerics; // For Vector2 (replace with a custom Vector2D if necessary)

namespace FruitLoops
{
    /**
     * @brief Provides static methods for handling user input.
     *
     * This class interfaces with internal engine calls to provide information
     * about mouse and keyboard input.
     */
    public static class Input
	{
        /**
         * @brief Checks if the specified mouse button is currently pressed.
         *
         * @param button The mouse button to check (default is 0 for left mouse button).
         * @return True if the specified mouse button is pressed; otherwise, false.
         */
        public static bool IsMouseButtonDown(int button = 0) // Default: left mouse button
		{
			return InternalCalls.Input_IsMouseButtonDown(button);
		}

        // Overload function
        public static bool IsMouseOverEntity(Entity entity)
        {
            if (entity == null || entity.ID == 0)
                return false;

            // 1. Retrieve entity's position and scale
            Vector2D entityPos = new Vector2D();
            Vector2D entityScale = new Vector2D();
            try
            {
                InternalCalls.Transform_GetTranslation(entity.ID, out entityPos);
                InternalCalls.Transform_GetScale(entity.ID, out entityScale);
            }
            catch (Exception ex)
            {
                Debug.LogError("[Input] Entity {0} is missing Transform components: {1}", entity.ID, ex.Message);
                return false;
            }

            // 2. Retrieve cursor position
            Vector2D cursorPos = GetCursorPosition();

            // 3. Calculate bounding box
            float left = entityPos.X - (entityScale.X / 2);
            float right = entityPos.X + (entityScale.X / 2);
            float bottom = entityPos.Y - (entityScale.Y / 2);
            float top = entityPos.Y + (entityScale.Y / 2);

            // 4. Check if cursor lies within bounding box
            return (cursorPos.X >= left && cursorPos.X <= right &&
                    cursorPos.Y >= bottom && cursorPos.Y <= top);
        }

        /**
         * @brief Checks if the mouse cursor is hovering over a specific entity.
         *
         * @param entity The unique identifier of the entity to check.
         * @return True if the mouse cursor is over the entity; otherwise, false.
         */
        public static bool IsMouseOverEntity(ulong entity)
		{
			if (entity == 0) return false;

			// Use your engine's logic to determine if the cursor is over the entity
			return InternalCalls.Input_IsMouseOverEntity(entity);
		}

        /**
         * @brief Checks if a specific key is currently pressed.
         *
         * @param keyCode The code of the key to check.
         * @return True if the specified key is pressed; otherwise, false.
         */
        public static bool IsKeyPressed(int keyCode)
		{
			return InternalCalls.Input_IsKeyPressed(keyCode);
		}

        /**
         * @brief Checks if a specific key was released.
         *
         * @param keyCode The code of the key to check.
         * @return True if the specified key was released; otherwise, false.
         */
        public static bool IsKeyReleased(int keyCode)
		{
			return InternalCalls.Input_IsKeyReleased(keyCode);
		}

        /**
         * @brief Checks if a specific key is being held down.
         *
         * @param keyCode The code of the key to check.
         * @return True if the specified key is held down; otherwise, false.
         */
        public static bool IsKeyHeld(int keyCode)
		{
			return InternalCalls.Input_IsKeyHeld(keyCode);
		}

        /**
         * @brief Checks if any mouse button is currently pressed.
         *
         * @return True if any mouse button is pressed; otherwise, false.
         */
        public static bool IsAnyMouseButtonPressed()
		{
			return InternalCalls.Input_IsAnyMouseButtonPressed();
		}

        /**
         * @brief Retrieves the current cursor position in world coordinates.
         *
         * @return A Vector2D representing the current cursor position.
         */
        public static Vector2D GetCursorPosition()
		{
			InternalCalls.Input_GetCursorPosition(out Vector2D cursorPosition);
			return cursorPosition;
		}

        /**
         * @brief Retrieves the mouse scroll Y offset.
         *
         * @return The mouse scroll offset along the Y-axis.
         */
        public static int GetMouseScrollY()
		{
			return InternalCalls.Input_GetMouseScrollY();
		}

	}
}
