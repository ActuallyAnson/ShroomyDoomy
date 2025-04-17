/******************************************************************************
/*!
\file  MathUtils.cs
\Proj name  Shroomy Doomy
\author  Gareth
\date    Feb 03, 2024
\brief  This file implements the MathUtils class, a utility class that provides
        mathematical helper functions, including value clamping.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
public static class MathUtils
{
    /**
 * @brief Clamps a float value within a specified range.
 *
 * Ensures that the given value remains between the defined minimum and maximum bounds.
 *
 * @param value The input value to clamp.
 * @param min The minimum allowable value.
 * @param max The maximum allowable value.
 * @return The clamped value, constrained between min and max.
 */
    public static float Clamp(float value, float min, float max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
}
