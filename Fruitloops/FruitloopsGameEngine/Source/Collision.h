#pragma once
/******************************************************************************/
/*!
\file    Collision.h
\project FruitLoops
\author  Reagan Tang
\date    April 5, 2025
\brief   Declares core 2D collision detection routines and bounding structures
         for the FruitLoops engine.

This header provides definitions for a set of geometric collision checks used
throughout the engine, including bounding box overlap, circle intersection,
ray-box collision, and point-in-polygon tests. These utilities are essential
for gameplay logic, selection mechanics, and spatial queries in both
interactive runtime and editor modes.

Included features:
- Axis-Aligned Bounding Box (AABB) structure (`MinMax2D`)
- Circle, ray, and line collision checks
- Point containment and intersection logic
- Modular utility functions for extensible collision behaviour

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
namespace FruitLoops
{
    /**
     * @brief Represents a 2D bounding box with minimum and maximum corners.
     */
    struct MinMax2D
    {
        Vector2D min;  // Min value (bottom-left)
        Vector2D max;  // Max value (top-right)

        // Constructor
        /**
         * @brief Default constructor. Initializes both min and max to (0, 0).
         */
        MinMax2D() : min(0, 0), max(0, 0) {}

        /**
         * @brief Constructor that initializes the bounding box with specific min and max values.
         *
         * @param minVal The minimum value (bottom-left corner).
         * @param maxVal The maximum value (top-right corner).
         */
        MinMax2D(const Vector2D& minVal, const Vector2D& maxVal) : min(minVal), max(maxVal) {}

        /**
         * @brief Ensures the bounding box is valid by swapping min and max if necessary.
         *        If min.x > max.x or min.y > max.y, the values are swapped.
         */
        void EnsureValid();
    };

    /**
     * @brief Checks if two 2D bounding boxes are colliding.
     *
     * @param box1 The first bounding box.
     * @param box2 The second bounding box.
     * @return True if the bounding boxes are colliding, false otherwise.
     */
    bool IsBoundingBoxCollision(const MinMax2D& box1, const MinMax2D& box2);

    /**
     * @brief Checks if two circles are colliding based on their centers and radii.
     *
     * @param center1 The center of the first circle.
     * @param radius1 The radius of the first circle.
     * @param center2 The center of the second circle.
     * @param radius2 The radius of the second circle.
     * @return True if the circles are colliding, false otherwise.
     */
    bool IsCircleCollision(const Vector2D& center1, float radius1, const Vector2D& center2, float radius2);

    /**
     * @brief Checks if a circle and a 2D bounding box are colliding.
     *
     * @param circleCenter The center of the circle.
     * @param radius The radius of the circle.
     * @param box The 2D bounding box.
     * @return True if the circle and the bounding box are colliding, false otherwise.
     */
    bool IsCircleBoxCollision(const Vector2D& circleCenter, float radius, const MinMax2D& box);

    /**
     * @brief Checks if a point is inside a 2D bounding box.
     *
     * @param point The point to check.
     * @param box The 2D bounding box.
     * @return True if the point is inside the bounding box, false otherwise.
     */
    bool IsPointInsideBox(const Vector2D& point, const MinMax2D& box);

    /**
     * @brief Checks if a line segment intersects a 2D bounding box.
     *
     * @param lineStart The starting point of the line.
     * @param lineEnd The ending point of the line.
     * @param box The 2D bounding box.
     * @return True if the line intersects the bounding box, false otherwise.
     */
    bool IsLineBoxCollision(const Vector2D& lineStart, const Vector2D& lineEnd, const MinMax2D& box);

    /**
     * @brief Checks if a ray intersects a 2D bounding box.
     *
     * @param rayOrigin The origin of the ray.
     * @param rayDir The direction of the ray.
     * @param box The 2D bounding box.
     * @param tmin The minimum distance along the ray where the intersection occurs (output).
     * @return True if the ray intersects the bounding box, false otherwise.
     */
    bool IsRayBoxCollision(const Vector2D& rayOrigin, const Vector2D& rayDir, const MinMax2D& box, float& tmin);

    /**
     * @brief Checks if a line segment intersects a circle.
     *
     * @param lineStart The starting point of the line.
     * @param lineEnd The ending point of the line.
     * @param circleCenter The center of the circle.
     * @param radius The radius of the circle.
     * @return True if the line intersects the circle, false otherwise.
     */
    bool IsLineCircleCollision(const Vector2D& lineStart, const Vector2D& lineEnd, const Vector2D& circleCenter, float radius);

    /**
     * @brief Checks if a point is inside a polygon.
     *
     * @param polygon The vertices of the polygon (in 2D space).
     * @param point The point to check.
     * @return True if the point is inside the polygon, false otherwise.
     */
    bool IsPointInsidePolygon(const std::vector<Vector2D>& polygon, const Vector2D& point);

    //MinMax2D GetEntityBoundingBox(Entity entity);

    //bool IsCursorOverBoundingBox(const Transform& transform);
}
