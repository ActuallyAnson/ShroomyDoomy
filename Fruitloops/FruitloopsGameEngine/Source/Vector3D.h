/******************************************************************************/
/*!
\file  Vector3D.h
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief   This file contains the declaration of the Vector3D class, representing
         a 3D vector with x, y, and z components. It provides basic vector operations
         such as addition, subtraction, scalar multiplication, and division, along
         with dot product and cross product calculations. The class also includes
         methods for calculating the vector's magnitude (length) and normalizing it
         to a unit vector.

         Operator overloads are provided for equality checks, stream output,
         and inequality comparisons.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

class Vector3D {
public:
    float x, y, z;

    /**************************************************************************
    * @brief
    *    Default constructor for the Vector3D class. Initializes the vector
    *    components to (0, 0, 0).
    *************************************************************************/
    Vector3D();

    /**************************************************************************
    * @brief
    *    Parameterized constructor for the Vector3D class. Initializes the
    *    vector with the specified x, y, and z components.
    *
    * @param x
    *    The x-coordinate of the vector.
    * @param y
    *    The y-coordinate of the vector.
    * @param z
    *    The z-coordinate of the vector.
    *************************************************************************/
    Vector3D(float x, float y, float z);

    /**************************************************************************
    * @brief
    *    Adds two vectors component-wise.
    *
    * @param other
    *    The vector to add to the current vector.
    *
    * @return
    *    A new Vector3D representing the result of the addition.
    *************************************************************************/
    Vector3D operator+(const Vector3D& other) const;

    /**************************************************************************
    * @brief
    *    Subtracts one vector from another component-wise.
    *
    * @param other
    *    The vector to subtract from the current vector.
    *
    * @return
    *    A new Vector3D representing the result of the subtraction.
    *************************************************************************/
    Vector3D operator-(const Vector3D& other) const;

    /**************************************************************************
    * @brief
    *    Multiplies the vector by a scalar value.
    *
    * @param scalar
    *    The scalar value to multiply the vector by.
    *
    * @return
    *    A new Vector3D representing the result of the multiplication.
    *************************************************************************/
    Vector3D operator*(float scalar) const;

    /**************************************************************************
    * @brief
    *    Divides the vector by a scalar value.
    *
    * @param scalar
    *    The scalar value to divide the vector by.
    *
    * @return
    *    A new Vector3D representing the result of the division.
    *************************************************************************/
    Vector3D operator/(float scalar) const;

    /**************************************************************************
    * @brief
    *    Computes the dot product of two vectors.
    *
    * @param other
    *    The vector to compute the dot product with.
    *
    * @return
    *    The dot product of the two vectors.
    *************************************************************************/
    float Dot(const Vector3D& other) const;

    /**************************************************************************
    * @brief
    *    Computes the cross product of two vectors.
    *
    * @param other
    *    The vector to compute the cross product with.
    *
    * @return
    *    A new Vector3D representing the result of the cross product.
    *************************************************************************/
    Vector3D Cross(const Vector3D& other) const;

    /**************************************************************************
    * @brief
    *    Calculates the length (magnitude) of the vector.
    *
    * @return
    *    The length of the vector.
    *************************************************************************/
    float Length() const;

    /**************************************************************************
    * @brief
    *    Normalizes the vector, converting it to a unit vector (a vector with
    *    length 1) pointing in the same direction.
    *
    * @return
    *    A new Vector3D representing the normalized vector.
    *************************************************************************/
    Vector3D Normalize() const;

    /**************************************************************************
    * @brief
    *    Overloads the stream insertion operator (<<) for easy output of the
    *    vector's components (x, y, z).
    *
    * @param os
    *    The output stream object (e.g., //std::cout).
    * @param vec
    *    The vector to output.
    *
    * @return
    *    The output stream object with the vector's components appended.
    *************************************************************************/
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& vec);

    /**************************************************************************
    * @brief
    *    Overloads the equality operator (==) to check if two vectors are equal
    *    component-wise.
    *
    * @param other
    *    The vector to compare with the current vector.
    *
    * @return
    *    True if the two vectors are equal component-wise, false otherwise.
    *************************************************************************/
    bool operator==(const Vector3D& other) const;

    /**************************************************************************
    * @brief
    *    Overloads the inequality operator (!=) to check if two vectors are not
    *    equal component-wise.
    *
    * @param other
    *    The vector to compare with the current vector.
    *
    * @return
    *    True if the two vectors are not equal component-wise, false otherwise.
    *************************************************************************/
    bool operator!=(const Vector3D& other) const;
};