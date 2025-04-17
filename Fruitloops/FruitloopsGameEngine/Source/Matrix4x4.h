/******************************************************************************/
/*!
\file  Matrix4x4.h
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief   This file contains the declaration of the Matrix4x4 class, representing
         a 4x4 matrix. The class supports a variety of operations commonly needed
         in 3D transformations, including matrix multiplication, scalar operations,
         transposition, determinant calculation, and inversion. These functions
         are crucial for implementing rotation, scaling, and translation in 3D
         space, which are often used in game development and graphics systems.

         It also includes utility functions like identity matrix generation and
         element access via overloaded operators.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

class Matrix4x4 {
public:
    float m[4][4];  // 4x4 matrix stored as a 2D array

    /**************************************************************************
    * @brief
    *    Default constructor for Matrix4x4 class. Initializes the matrix to the
    *    identity matrix.
    *************************************************************************/
    Matrix4x4();

    /**************************************************************************
    * @brief
    *    Parameterized constructor for Matrix4x4 class. Initializes the matrix
    *    with the specified values for all elements.
    *
    * @param m00 - m33
    *    The values to initialize the matrix elements.
    *************************************************************************/
    Matrix4x4(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);

    /**************************************************************************
    * @brief
    *    Multiplies two 4x4 matrices and returns the result.
    *
    * @param rhs
    *    The matrix to multiply with the current matrix.
    *
    * @return
    *    A new Matrix4x4 that is the result of the multiplication.
    *************************************************************************/
    Matrix4x4 operator*(const Matrix4x4& rhs) const;

    /**************************************************************************
    * @brief
    *    Multiplies the current matrix with another matrix and assigns the result
    *    to the current matrix.
    *
    * @param rhs
    *    The matrix to multiply with.
    *
    * @return
    *    A reference to the modified Matrix4x4.
    *************************************************************************/
    Matrix4x4& operator*=(const Matrix4x4& rhs);

    /**************************************************************************
    * @brief
    *    Multiplies the matrix by a scalar value and returns the result.
    *
    * @param scalar
    *    The scalar value to multiply the matrix by.
    *
    * @return
    *    A new Matrix4x4 that is the result of the scalar multiplication.
    *************************************************************************/
    Matrix4x4 operator*(float scalar) const;

    /**************************************************************************
    * @brief
    *    Divides the matrix by a scalar value and returns the result.
    *
    * @param scalar
    *    The scalar value to divide the matrix by.
    *
    * @return
    *    A new Matrix4x4 that is the result of the scalar division.
    *
    * @throw std::invalid_argument
    *    If scalar value is zero, an exception is thrown to prevent division by zero.
    *************************************************************************/
    Matrix4x4 operator/(float scalar) const;

    /**************************************************************************
    * @brief
    *    Returns the identity matrix (a matrix with 1s on the diagonal and 0s elsewhere).
    *
    * @return
    *    A Matrix4x4 representing the identity matrix.
    *************************************************************************/
    static Matrix4x4 Identity();

    /**************************************************************************
    * @brief
    *    Transposes the given matrix, swapping rows and columns.
    *
    * @param mat
    *    The matrix to be transposed.
    *
    * @return
    *    A new Matrix4x4 that is the transposed version of the input matrix.
    *************************************************************************/
    static Matrix4x4 Transpose(const Matrix4x4& mat);

    /**************************************************************************
    * @brief
    *    Inverts the given matrix if it is invertible.
    *
    * @param mat
    *    The matrix to be inverted.
    *
    * @return
    *    A new Matrix4x4 that is the inverse of the input matrix.
    *
    * @throw std::invalid_argument
    *    If the matrix is not invertible (determinant is zero), an exception is thrown.
    *************************************************************************/
    static Matrix4x4 Inverse(const Matrix4x4& mat);

    /**************************************************************************
    * @brief
    *    Calculates the determinant of the given 4x4 matrix.
    *
    * @param mat
    *    The matrix whose determinant is to be calculated.
    *
    * @return
    *    The determinant of the matrix as a float.
    *************************************************************************/
    static float Determinant(const Matrix4x4& mat);

    /**************************************************************************
    * @brief
    *    Provides access to the matrix elements by row (non-const).
    *
    * @param index
    *    The row index (0-3).
    *
    * @return
    *    A pointer to the row of the matrix at the specified index.
    *************************************************************************/
    float* operator[](int index);

    /**************************************************************************
    * @brief
    *    Provides access to the matrix elements by row (const).
    *
    * @param index
    *    The row index (0-3).
    *
    * @return
    *    A const pointer to the row of the matrix at the specified index.
    *************************************************************************/
    const float* operator[](int index) const;
};