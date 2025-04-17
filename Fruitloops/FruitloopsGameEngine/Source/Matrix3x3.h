/******************************************************************************/
/*!
\file  Matrix3x3.h
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief   This file contains the declaration of the Matrix3x3 class and several
         non-member functions for performing matrix operations such as identity
         creation, translation, scaling, rotation, transposition, and inversion.
         The Matrix3x3 class represents a 3x3 matrix used primarily for 2D
         transformations in games.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

namespace FruitLoops {

    class Matrix3x3 {
    public:
        // Storage for matrix
        union MatrixData {
            struct Mat{
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
            } Mat;
            float m[9];        // Flat array
            float m2[3][3];    // 2D array
        } data;

        /**************************************************************************
        * @brief
        *    Default constructor for the Matrix3x3 class. Initializes all elements
        *    in the matrix to 0.
        *************************************************************************/
        Matrix3x3();

        /**************************************************************************
        * @brief
        *    Parameterized constructor for the Matrix3x3 class. Initializes the matrix
        *    with the specified values.
        *
        * @param m00, m01, m02
        *    Values for the first row of the matrix.
        * @param m10, m11, m12
        *    Values for the second row of the matrix.
        * @param m20, m21, m22
        *    Values for the third row of the matrix.
        *************************************************************************/
        Matrix3x3(float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22);

        /**************************************************************************
        * @brief
        *    Constructor for the Matrix3x3 class that initializes the matrix using
        *    an array of 9 floats.
        *
        * @param pArr
        *    Pointer to an array of 9 floats used to initialize the matrix.
        *************************************************************************/
        Matrix3x3(const float* pArr);

        /**************************************************************************
        * @brief
        *    Accessor for a specific element in the matrix (non-const).
        *
        * @param row
        *    Row index of the element (0-2).
        * @param col
        *    Column index of the element (0-2).
        *
        * @return
        *    A reference to the specified element in the matrix.
        *************************************************************************/
        float& operator()(int row, int col);

        /**************************************************************************
        * @brief
        *    Accessor for a specific element in the matrix (const).
        *
        * @param row
        *    Row index of the element (0-2).
        * @param col
        *    Column index of the element (0-2).
        *
        * @return
        *    The value of the specified element in the matrix.
        *************************************************************************/
        float operator()(int row, int col) const;

        /**************************************************************************
        * @brief
        *    Assignment operator for copying data from another Matrix3x3 instance.
        *
        * @param rhs
        *    The Matrix3x3 object to copy from.
        *
        * @return
        *    A reference to the current Matrix3x3 object.
        *************************************************************************/
        Matrix3x3& operator=(const Matrix3x3& rhs);

        /**************************************************************************
        * @brief
        *    Multiplies the current matrix by another Matrix3x3 and assigns the result
        *    to the current matrix.
        *
        * @param rhs
        *    The Matrix3x3 object to multiply with.
        *
        * @return
        *    A reference to the current Matrix3x3 object after multiplication.
        *************************************************************************/
        Matrix3x3& operator*=(const Matrix3x3& rhs);
    };

    /**************************************************************************
       * @brief
       *    Sets the given matrix to the identity matrix (3x3).
       *
       * @param result
       *    The matrix that will be set to the identity matrix.
       *************************************************************************/
    void Matrix3x3Identity(Matrix3x3& result);

    /**************************************************************************
    * @brief
    *    Creates a 2D translation matrix and stores it in the result matrix.
    *
    * @param result
    *    The matrix that will store the translation transformation.
    * @param x
    *    The translation along the x-axis.
    * @param y
    *    The translation along the y-axis.
    *************************************************************************/
    void Matrix3x3Translate(Matrix3x3& result, float x, float y);

    /**************************************************************************
    * @brief
    *    Creates a 2D scaling matrix and stores it in the result matrix.
    *
    * @param result
    *    The matrix that will store the scaling transformation.
    * @param x
    *    The scaling factor along the x-axis.
    * @param y
    *    The scaling factor along the y-axis.
    *************************************************************************/
    void Matrix3x3Scale(Matrix3x3& result, float x, float y);

    /**************************************************************************
    * @brief
    *    Creates a rotation matrix (in radians) and stores it in the result matrix.
    *
    * @param result
    *    The matrix that will store the rotation transformation.
    * @param angle
    *    The angle of rotation in radians.
    *************************************************************************/
    void Matrix3x3RotRad(Matrix3x3& result, float angle);

    /**************************************************************************
    * @brief
    *    Creates a rotation matrix (in degrees) and stores it in the result matrix.
    *    Converts degrees to radians internally.
    *
    * @param result
    *    The matrix that will store the rotation transformation.
    * @param angle
    *    The angle of rotation in degrees.
    *************************************************************************/
    void Matrix3x3RotDeg(Matrix3x3& result, float angle);

    /**************************************************************************
    * @brief
    *    Transposes the input matrix and stores the result in the result matrix.
    *
    * @param result
    *    The matrix that will store the transposed matrix.
    * @param inputMatrix
    *    The matrix to be transposed.
    *************************************************************************/
    void Matrix3x3Transpose(Matrix3x3& result, const Matrix3x3& inputMatrix);

    /**************************************************************************
    * @brief
    *    Inverts the input matrix and stores the result in the result matrix.
    *    If the matrix is not invertible (determinant is 0), sets the result to
    *    the identity matrix.
    *
    * @param result
    *    The matrix that will store the inverse of the input matrix.
    * @param determinant
    *    The determinant of the input matrix.
    * @param inputMatrix
    *    The matrix to be inverted.
    *************************************************************************/
    void Matrix3x3Inverse(Matrix3x3& result, float& determinant, const Matrix3x3& inputMatrix);
}
