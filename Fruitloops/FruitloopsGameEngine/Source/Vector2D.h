/******************************************************************************/
/*!
\file  Vector2D.h
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief   This file contains the declaration of the Vector2D class, which represents
         a 2D vector. The class provides various operator overloads for arithmetic
         operations (addition, subtraction, multiplication, division) and utility
         functions to calculate vector length, distance, dot product, cross product,
         and normalization. It also includes static functions for setting or zeroing vectors.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

namespace FruitLoops {

    class Vector2D {
    public:
        float x, y;
        
        float magnitude() const;
        // Returns normalized vector without modifying the original
        Vector2D normalized() const;

        // Normalizes this vector in place
        void normalize();
        /**************************************************************************
        * @brief
        *    Constructor for the Vector2D class. Initializes the vector with the
        *    given x and y values. If no values are provided, initializes to (0, 0).
        *
        * @param x
        *    The x-coordinate of the vector.
        * @param y
        *    The y-coordinate of the vector.
        *************************************************************************/
        Vector2D(float x = 0, float y = 0);

        /**************************************************************************
        * @brief
        *    Adds another Vector2D to the current vector.
        *
        * @param rhs
        *    The vector to add to the current vector.
        *
        * @return
        *    A reference to the modified vector after addition.
        *************************************************************************/
        Vector2D& operator += (const Vector2D& rhs);

        /**************************************************************************
        * @brief
        *    Subtracts another Vector2D from the current vector.
        *
        * @param rhs
        *    The vector to subtract from the current vector.
        *
        * @return
        *    A reference to the modified vector after subtraction.
        *************************************************************************/
        Vector2D& operator -= (const Vector2D& rhs);

        /**************************************************************************
        * @brief
        *    Multiplies the current vector by a scalar value.
        *
        * @param rhs
        *    The scalar value to multiply the vector by.
        *
        * @return
        *    A reference to the modified vector after multiplication.
        *************************************************************************/
        Vector2D& operator *= (float rhs);

        /**************************************************************************
        * @brief
        *    Divides the current vector by a scalar value.
        *
        * @param rhs
        *    The scalar value to divide the vector by.
        *
        * @return
        *    A reference to the modified vector after division.
        *************************************************************************/
        Vector2D& operator /= (float rhs);

        bool operator!=(const Vector2D& other) const;

        /**************************************************************************
        * @brief
        *    Negates the vector (reverses its direction).
        *
        * @return
        *    A new vector that is the negation of the current vector.
        *************************************************************************/
        Vector2D operator - () const;

        /**************************************************************************
        * @brief
        *    Subtracts one vector from another.
        *
        * @param rhs
        *    The vector to subtract.
        *
        * @return
        *    A new vector representing the result of the subtraction.
        *************************************************************************/
        Vector2D operator - (const Vector2D& rhs) const;

        /**************************************************************************
        * @brief
        *    Adds two vectors together.
        *
        * @param rhs
        *    The vector to add.
        *
        * @return
        *    A new vector representing the result of the addition.
        *************************************************************************/
        Vector2D operator + (const Vector2D& rhs) const;

        /**************************************************************************
        * @brief
        *    Divides the vector by a scalar value.
        *
        * @param rhs
        *    The scalar value to divide the vector by.
        *
        * @return
        *    A new vector representing the result of the division.
        *************************************************************************/
        Vector2D operator / (float rhs) const;

        /**************************************************************************
        * @brief
        *    Multiplies the vector by a scalar value.
        *
        * @param rhs
        *    The scalar value to multiply the vector by.
        *
        * @return
        *    A new vector representing the result of the multiplication.
        *************************************************************************/
        Vector2D operator * (float rhs) const;


        bool operator!= (float rhs) const;
        /**************************************************************************
        * @brief
        *    Computes the length (magnitude) of a vector.
        *
        * @param v
        *    The vector whose length is to be computed.
        *
        * @return
        *    The length of the vector.
        *************************************************************************/
        static float Length(const Vector2D& v);

        /**************************************************************************
        * @brief
        *    Computes the squared length (magnitude) of a vector.
        *    This is faster than calculating the actual length as it avoids the square root.
        *
        * @param v
        *    The vector whose squared length is to be computed.
        *
        * @return
        *    The squared length of the vector.
        *************************************************************************/
        static float SquareLength(const Vector2D& v);

        /**************************************************************************
        * @brief
        *    Normalizes the vector, converting it to a unit vector (a vector with
        *    length 1) and stores the result in the provided result vector.
        *
        * @param result
        *    The vector to store the normalized result.
        * @param v
        *    The vector to be normalized.
        *************************************************************************/
        static void Normalize(Vector2D& result, const Vector2D& v);

        /**************************************************************************
        * @brief
        *    Computes the distance between two vectors.
        *
        * @param vector1
        *    The first vector.
        * @param vector2
        *    The second vector.
        *
        * @return
        *    The distance between the two vectors.
        *************************************************************************/
        static float Distance(const Vector2D& vector1, const Vector2D& vector2);

        /**************************************************************************
        * @brief
        *    Computes the squared distance between two vectors.
        *    This is faster than calculating the actual distance as it avoids the square root.
        *
        * @param vector1
        *    The first vector.
        * @param vector2
        *    The second vector.
        *
        * @return
        *    The squared distance between the two vectors.
        *************************************************************************/
        static float SquareDistance(const Vector2D& vector1, const Vector2D& vector2);

        /**************************************************************************
        * @brief
        *    Computes the dot product of two vectors.
        *
        * @param vector1
        *    The first vector.
        * @param vector2
        *    The second vector.
        *
        * @return
        *    The dot product of the two vectors.
        *************************************************************************/
        static float DotProduct(const Vector2D& vector1, const Vector2D& vector2);

        /**************************************************************************
        * @brief
        *    Computes the magnitude of the cross product between two vectors.
        *    Since this is a 2D vector, it returns the magnitude of the resulting
        *    scalar from the 2D cross product.
        *
        * @param vector1
        *    The first vector.
        * @param vector2
        *    The second vector.
        *
        * @return
        *    The magnitude of the cross product.
        *************************************************************************/
        static float CrossProductMag(const Vector2D& vector1, const Vector2D& vector2);

        /**************************************************************************
        * @brief
        *    Sets the values of a vector.
        *
        * @param Result
        *    The vector to set.
        * @param x
        *    The x-coordinate to set.
        * @param y
        *    The y-coordinate to set.
        *************************************************************************/
        static void Vec2Set(Vector2D& Result, float x, float y);

        /**************************************************************************
        * @brief
        *    Sets the vector to (0, 0).
        *
        * @param Result
        *    The vector to zero out.
        *************************************************************************/
        static void Vec2Zero(Vector2D& Result);

        bool operator<(const Vector2D& other) const;

    };
}