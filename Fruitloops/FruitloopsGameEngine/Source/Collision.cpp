/******************************************************************************/
/*!
\file    Collision.cpp
\project FruitLoops
\author  Reagan Tang
\date    April 5, 2025
\brief   Implements 2D collision detection routines for the FruitLoops engine.

This file contains core geometric collision functions, including bounding box,
circle, polygon, ray, and line-based intersection checks. These functions form
the foundational building blocks for the engine’s physics, selection tools,
UI interactions, and gameplay collision handling.

Key features implemented include:
- Axis-Aligned Bounding Box (AABB) collision
- Circle-to-circle and circle-to-AABB collision
- Line and ray intersection with boxes and circles
- Point-in-polygon tests using ray casting

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "Collision.h"
#include "Input.h"

namespace FruitLoops
{
    /**
     * @brief Ensures that the min and max values of the 2D bounding box are valid.
     *        If the minimum value is greater than the maximum, it swaps them.
     */
    void MinMax2D::EnsureValid()
    {
        if (min.x > max.x) std::swap(min.x, max.x);
        if (min.y > max.y) std::swap(min.y, max.y);
    }

    /**
     * @brief Checks if two 2D bounding boxes are colliding.
     *
     * @param box1 The first bounding box.
     * @param box2 The second bounding box.
     * @return True if the bounding boxes are colliding, false otherwise.
     */
    bool IsBoundingBoxCollision(const MinMax2D& box1, const MinMax2D& box2)
    {
        if (box1.max.x < box2.min.x || box1.min.x > box2.max.x) return false;

        if (box1.max.y < box2.min.y || box1.min.y > box2.max.y) return false;

        return true;
    }

    /**
     * @brief Checks if two circles are colliding based on their centers and radii.
     *
     * @param center1 The center of the first circle.
     * @param radius1 The radius of the first circle.
     * @param center2 The center of the second circle.
     * @param radius2 The radius of the second circle.
     * @return True if the circles are colliding, false otherwise.
     */
    bool IsCircleCollision(const Vector2D& center1, float radius1, const Vector2D& center2, float radius2)
    {
        float distanceSquared = Vector2D::SquareDistance(center1, center2);

        float radiiSum = radius1 + radius2;

        return distanceSquared <= (radiiSum * radiiSum);
    }

    /**
     * @brief Checks if a circle and a 2D bounding box are colliding.
     *
     * @param circleCenter The center of the circle.
     * @param radius The radius of the circle.
     * @param box The bounding box.
     * @return True if the circle and the bounding box are colliding, false otherwise.
     */
    bool IsCircleBoxCollision(const Vector2D& circleCenter, float radius, const MinMax2D& box)
    {
        float closestX = circleCenter.x;
        float closestY = circleCenter.y;

        if (circleCenter.x < box.min.x) closestX = box.min.x;
        else if (circleCenter.x > box.max.x) closestX = box.max.x;

        if (circleCenter.y < box.min.y) closestY = box.min.y;
        else if (circleCenter.y > box.max.y) closestY = box.max.y;

        Vector2D closestPoint(closestX, closestY);
        float distanceSquared = Vector2D::SquareDistance(circleCenter, closestPoint);

        return distanceSquared <= (radius * radius);
    }

    /**
     * @brief Checks if a point is inside a 2D bounding box.
     *
     * @param point The point to check.
     * @param box The bounding box.
     * @return True if the point is inside the bounding box, false otherwise.
     */
    bool IsPointInsideBox(const Vector2D& point, const MinMax2D& box)
    {
        return (point.x >= box.min.x && point.x <= box.max.x &&
            point.y >= box.min.y && point.y <= box.max.y);
    }

    /**
     * @brief Checks if a line segment intersects a 2D bounding box.
     *
     * @param lineStart The starting point of the line.
     * @param lineEnd The ending point of the line.
     * @param box The bounding box.
     * @return True if the line intersects the bounding box, false otherwise.
     */
    bool IsLineBoxCollision(const Vector2D& lineStart, const Vector2D& lineEnd, const MinMax2D& box)
    {
        // Check for overlap in the x and y ranges of the box with the line
        if ((lineStart.x > box.max.x && lineEnd.x > box.max.x) || (lineStart.x < box.min.x && lineEnd.x < box.min.x)) return false;
        if ((lineStart.y > box.max.y && lineEnd.y > box.max.y) || (lineStart.y < box.min.y && lineEnd.y < box.min.y)) return false;

        // If no separation is found, the line intersects the box
        return true;
    }

    /**
     * @brief Checks if a ray intersects a 2D bounding box.
     *
     * @param rayOrigin The origin of the ray.
     * @param rayDir The direction of the ray.
     * @param box The bounding box.
     * @param tmin The minimum distance along the ray where the intersection occurs (output).
     * @return True if the ray intersects the bounding box, false otherwise.
     */
    bool IsRayBoxCollision(const Vector2D& rayOrigin, const Vector2D& rayDir, const MinMax2D& box, float& tmin)
    {
        Vector2D invDir(1.0f / rayDir.x, 1.0f / rayDir.y);

        float tx1 = (box.min.x - rayOrigin.x) * invDir.x;
        float tx2 = (box.max.x - rayOrigin.x) * invDir.x;

        tmin = std::min(tx1, tx2);
        float tmax = std::max(tx1, tx2);

        float ty1 = (box.min.y - rayOrigin.y) * invDir.y;
        float ty2 = (box.max.y - rayOrigin.y) * invDir.y;

        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));

        return tmax >= tmin;
    }

    /**
     * @brief Checks if a line segment intersects a circle.
     *
     * @param lineStart The starting point of the line.
     * @param lineEnd The ending point of the line.
     * @param circleCenter The center of the circle.
     * @param radius The radius of the circle.
     * @return True if the line intersects the circle, false otherwise.
     */
    bool IsLineCircleCollision(const Vector2D& lineStart, const Vector2D& lineEnd, const Vector2D& circleCenter, float radius)
    {
        Vector2D lineDir = lineEnd - lineStart;
        Vector2D lineToCircle = circleCenter - lineStart;

        // Use your SquareLength method for the vector's length squared
        float projectionLength = Vector2D::DotProduct(lineToCircle, lineDir) / Vector2D::SquareLength(lineDir);
        Vector2D closestPoint = lineStart + lineDir * projectionLength;

        return Vector2D::SquareDistance(closestPoint, circleCenter) <= (radius * radius);
    }

    /**
     * @brief Checks if a point is inside a polygon.
     *
     * @param polygon The vertices of the polygon (in 2D space).
     * @param point The point to check.
     * @return True if the point is inside the polygon, false otherwise.
     */
    bool IsPointInsidePolygon(const std::vector<Vector2D>& polygon, const Vector2D& point)
    {
        int i, j, nvert = static_cast<int>(polygon.size());
        bool inside = false;

        for (i = 0, j = nvert - 1; i < nvert; j = i++) {
            if (((polygon[i].y >= point.y) != (polygon[j].y >= point.y)) &&
                (point.x <= (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
                inside = !inside;
        }
        return inside;
    }

    //MinMax2D GetEntityBoundingBox(Entity entity) {
    //    // Retrieve the Transform component of the entity
    //    Transform& transform = ecsHub->GetComponent<Transform>(entity);

    //    // Calculate the bounding box based on position and scale
    //    MinMax2D box;
    //    Vector2D halfScale = transform.scale * 0.5f;

    //    // Calculate min and max corners based on center position and half the scale
    //    box.min = transform.position - halfScale;  // Bottom-left corner
    //    box.max = transform.position + halfScale;  // Top-right corner
    //    return box;
    //}

    //bool IsCursorOverBoundingBox(const Transform& transform) {
    //    Vector2D cursorScreenPos = InputSystem::GetCursorPosition();
    //    Vector2D cursorWorldPos = ScreenToWorld(cursorScreenPos);

    //    Vector2D halfSize = transform.scale * 0.5f;
    //    Vector2D topLeft = transform.position - halfSize;
    //    Vector2D bottomRight = transform.position + halfSize;

    //    return IsPointInBoundingBox(cursorWorldPos, topLeft, bottomRight);
    //}
}


