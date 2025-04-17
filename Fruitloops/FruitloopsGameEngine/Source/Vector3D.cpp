/******************************************************************************/
/*!
\file  Vector3D.cpp
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief   This file contains the implementation of the Vector3D class, representing
         a 3D vector and providing operations such as vector addition, subtraction,
         scalar multiplication, division, dot product, and cross product. It also
         includes functionality for normalizing vectors, calculating their magnitude,
         and comparing equality between vectors. A custom square root function is used
         for calculating the magnitude to ensure precision.

         Additionally, the file implements operator overloads for easy output via
         streams and comparison between vectors.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "Vector3D.h"
/*!
 * \brief A custom square root function for precise magnitude calculations.
 * \param number The number to compute the square root of.
 * \return The square root of the number, or -1.0f for invalid inputs.
 */
float CustomSqrt(float number) {
    if (number < 0) {
        return -1.0f; // Return negative for invalid inputs (you can handle this as needed)
    }
    float x = number;
    float epsilon = 0.0001f; // The precision of the result
    while ((x - number / x) > epsilon) {
        x = (x + number / x) / 2;
    }
    return x;
}

/*!
 * \brief Default constructor that initializes a 3D vector to (0, 0, 0).
 */
Vector3D::Vector3D() : x(0), y(0), z(0) {}

/*!
 * \brief Constructs a 3D vector with specified x, y, and z components.
 * \param x The x-component of the vector.
 * \param y The y-component of the vector.
 * \param z The z-component of the vector.
 */
Vector3D::Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

/*!
 * \brief Adds two 3D vectors.
 * \param other The vector to add.
 * \return A new vector resulting from the addition.
 */
Vector3D Vector3D::operator+(const Vector3D& other) const {
    float newX = x + other.x;
    float newY = y + other.y;
    float newZ = z + other.z;
    return Vector3D(newX, newY, newZ);
}

/*!
 * \brief Subtracts another vector from this vector.
 * \param other The vector to subtract.
 * \return A new vector resulting from the subtraction.
 */
Vector3D Vector3D::operator-(const Vector3D& other) const {
    float newX = x - other.x;
    float newY = y - other.y;
    float newZ = z - other.z;
    return Vector3D(newX, newY, newZ);
}

/*!
 * \brief Multiplies this vector by a scalar.
 * \param scalar The scalar value to multiply by.
 * \return A new vector resulting from the multiplication.
 */
Vector3D Vector3D::operator*(float scalar) const {
    float newX = x * scalar;
    float newY = y * scalar;
    float newZ = z * scalar;
    return Vector3D(newX, newY, newZ);
}

/*!
 * \brief Divides this vector by a scalar.
 * \param scalar The scalar value to divide by.
 * \return A new vector resulting from the division.
 */
Vector3D Vector3D::operator/(float scalar) const {
    float newX = x / scalar;
    float newY = y / scalar;
    float newZ = z / scalar;
    return Vector3D(newX, newY, newZ);
}

/*!
 * \brief Calculates the dot product of two vectors.
 * \param other The vector to calculate the dot product with.
 * \return The dot product of the two vectors.
 */
float Vector3D::Dot(const Vector3D& other) const {
    float dotProduct = x * other.x + y * other.y + z * other.z;
    return dotProduct;
}

/*!
 * \brief Calculates the cross product of two vectors.
 * \param other The vector to calculate the cross product with.
 * \return A new vector resulting from the cross product.
 */
Vector3D Vector3D::Cross(const Vector3D& other) const {
    float crossX = y * other.z - z * other.y;
    float crossY = z * other.x - x * other.z;
    float crossZ = x * other.y - y * other.x;
    return Vector3D(crossX, crossY, crossZ);
}

/*!
 * \brief Calculates the length (magnitude) of the vector.
 * \return The length of the vector.
 */
float Vector3D::Length() const {
    float lengthSquared = x * x + y * y + z * z;
    return CustomSqrt(lengthSquared);
}

/*!
 * \brief Normalizes the vector, returning a unit vector in the same direction.
 * \return A normalized vector or a zero vector if the length is zero.
 */
Vector3D Vector3D::Normalize() const {
    float length = Length();  // Use custom length calculation
    if (length != 0) {
        float newX = x / length;
        float newY = y / length;
        float newZ = z / length;
        return Vector3D(newX, newY, newZ);
    }
    return Vector3D(0, 0, 0);  // Return zero vector if the length is zero
}

/*!
 * \brief Outputs the vector's components to a stream.
 * \param os The output stream.
 * \param vec The vector to output.
 * \return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const Vector3D& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

/*!
 * \brief Checks if two vectors are equal.
 * \param other The vector to compare with.
 * \return True if the vectors are equal, false otherwise.
 */
bool Vector3D::operator==(const Vector3D& other) const {
    return x == other.x && y == other.y && z == other.z;
}

/*!
 * \brief Checks if two vectors are not equal.
 * \param other The vector to compare with.
 * \return True if the vectors are not equal, false otherwise.
 */
bool Vector3D::operator!=(const Vector3D& other) const {
    return !(*this == other);
}