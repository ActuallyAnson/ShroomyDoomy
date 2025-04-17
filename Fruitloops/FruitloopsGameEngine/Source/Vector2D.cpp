/******************************************************************************/
/*!
\file  Vector2D.cpp
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief   This file contains the implementation of the Vector2D class, which represents
         a 2D vector and provides various vector operations including addition,
         subtraction, scaling, normalization, and distance calculation. It also
         includes functions for dot products, cross products, and setting vectors
         to zero or specific values.

All content � 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "Vector2D.h"
#include <cmath> // For std::sqrt

namespace FruitLoops {
    /*!
     * \brief Constructs a 2D vector with specified x and y components.
     * \param x The x-component of the vector.
     * \param y The y-component of the vector.
     */
    Vector2D::Vector2D(float x, float y) : x(x), y(y) {}

    float Vector2D::magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2D Vector2D::normalized() const {
        float m = magnitude();
        if (m == 0) return Vector2D(0, 0);
        return Vector2D(x / m, y / m);
    }

    void Vector2D::normalize() {
        float m = magnitude();
        if (m > 0) {
            x /= m;
            y /= m;
        }
    }

    /*!
     * \brief Adds another vector to this vector.
     * \param rhs The vector to add.
     * \return A reference to this vector after addition.
     */
    Vector2D& Vector2D::operator += (const Vector2D& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    /*!
     * \brief Subtracts another vector from this vector.
     * \param rhs The vector to subtract.
     * \return A reference to this vector after subtraction.
     */
    Vector2D& Vector2D::operator -= (const Vector2D& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    /*!
     * \brief Multiplies this vector by a scalar.
     * \param rhs The scalar value.
     * \return A reference to this vector after multiplication.
     */
    Vector2D& Vector2D::operator *= (float rhs) {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    /*!
     * \brief Divides this vector by a scalar.
     * \param rhs The scalar value.
     * \return A reference to this vector after division.
     */
    Vector2D& Vector2D::operator /= (float rhs) {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    /*!
     * \brief Checks if two vectors are not equal.
     * \param other The vector to compare with.
     * \return True if the vectors are not equal, false otherwise.
     */
    bool Vector2D::operator!=(const Vector2D& other) const {
        return x != other.x || y != other.y;
    }

    /*!
     * \brief Checks if two vectors are not equal.
     * \param other The vector to compare with.
     * \return True if the vectors are not equal, false otherwise.
     */
    Vector2D Vector2D::operator - () const {
        return Vector2D(-x, -y);
    }

    /*!
     * \brief Multiplies this vector by a scalar, returning a new vector.
     * \param rhs The scalar value.
     * \return A new vector after multiplication.
     */
    Vector2D Vector2D::operator * (float rhs) const {
        return Vector2D(x * rhs, y * rhs);
    }

    /*!
     * \brief Subtracts another vector from this vector, returning a new vector.
     * \param rhs The vector to subtract.
     * \return A new vector after subtraction.
     */
    Vector2D Vector2D::operator - (const Vector2D& rhs) const {
        return Vector2D(x - rhs.x, y - rhs.y);
    }

    /*!
     * \brief Adds another vector to this vector, returning a new vector.
     * \param rhs The vector to add.
     * \return A new vector after addition.
     */
    Vector2D Vector2D::operator + (const Vector2D& rhs) const {
        return Vector2D(x + rhs.x, y + rhs.y);
    }

    /*!
     * \brief Divides this vector by a scalar, returning a new vector.
     * \param rhs The scalar value.
     * \return A new vector after division.
     */
    Vector2D Vector2D::operator / (float rhs) const {
        return Vector2D(x / rhs, y / rhs);
    }

    /*!
     * \brief Calculates the length (magnitude) of a vector.
     * \param v The vector to calculate the length of.
     * \return The length of the vector.
     */
    float Vector2D::Length(const Vector2D& v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    /*!
     * \brief Calculates the square of the length of a vector.
     * \param v The vector to calculate the square length of.
     * \return The square length of the vector.
     */
    float Vector2D::SquareLength(const Vector2D& v) {
        return v.x * v.x + v.y * v.y;
    }

    /*!
     * \brief Normalizes a vector and stores the result.
     * \param result The vector to store the normalized result in.
     * \param v The vector to normalize.
     */
    void Vector2D::Normalize(Vector2D& result, const Vector2D& v) {
        float len = Length(v);
        if (len != 0) {
            result = v / len;
        }
    }

    /*!
     * \brief Calculates the distance between two vectors.
     * \param vector1 The first vector.
     * \param vector2 The second vector.
     * \return The distance between the two vectors.
     */
    float Vector2D::Distance(const Vector2D& vector1, const Vector2D& vector2) {
        return Length(vector1 - vector2);
    }

    /*!
     * \brief Calculates the squared distance between two vectors.
     * \param vector1 The first vector.
     * \param vector2 The second vector.
     * \return The squared distance between the two vectors.
     */
    float Vector2D::SquareDistance(const Vector2D& vector1, const Vector2D& vector2) {
        return SquareLength(vector1 - vector2);
    }

    /*!
     * \brief Calculates the dot product of two vectors.
     * \param vector1 The first vector.
     * \param vector2 The second vector.
     * \return The dot product of the two vectors.
     */
    float Vector2D::DotProduct(const Vector2D& vector1, const Vector2D& vector2) {
        return vector1.x * vector2.x + vector1.y * vector2.y;
    }

    /*!
     * \brief Calculates the magnitude of the cross product of two 2D vectors.
     * \param vector1 The first vector.
     * \param vector2 The second vector.
     * \return The magnitude of the cross product.
     */
    float Vector2D::CrossProductMag(const Vector2D& vector1, const Vector2D& vector2) {
        return vector1.x * vector2.y - vector1.y * vector2.x;
    }

    /*!
     * \brief Sets the components of a vector to the specified values.
     * \param Result The vector to set.
     * \param x The x-component.
     * \param y The y-component.
     */
    void Vector2D::Vec2Set(Vector2D& Result, float x, float y) {
        Result.x = x;
        Result.y = y;
    }

    /*!
     * \brief Sets the components of a vector to zero.
     * \param Result The vector to set to zero.
     */
    void Vector2D::Vec2Zero(Vector2D& Result) {
        Result.x = 0;
        Result.y = 0;
    }

    /*!
     * \brief Compares two vectors lexicographically.
     * \param other The vector to compare with.
     * \return True if this vector is less than the other, false otherwise.
     */
    bool Vector2D::operator<(const Vector2D& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }

}
