/******************************************************************************/
/*!
\file  Matrix4x4.cpp
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief   This file contains the implementation of the Matrix4x4 class, which
         represents a 4x4 matrix commonly used in 3D graphics transformations.
         It provides operations such as matrix multiplication, scalar multiplication,
         matrix inversion, transposition, and determinant calculation. The file also
         includes helper functions to calculate 3x3 minors, which are necessary for
         the determinant and inverse computations.

         The class supports basic matrix operations and transformations for game
         development and graphics applications.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "Matrix4x4.h"
#include <stdexcept>  // For throwing exceptions

/*!
 * \brief Default constructor that initializes the matrix to the identity matrix.
 */
Matrix4x4::Matrix4x4() {
    *this = Identity();
}

/*!
 * \brief Constructs a matrix with the specified elements.
 * \param m00, m01, m02, m03 Elements of the first row.
 * \param m10, m11, m12, m13 Elements of the second row.
 * \param m20, m21, m22, m23 Elements of the third row.
 * \param m30, m31, m32, m33 Elements of the fourth row.
 */
Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33) {
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
    m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

/*!
 * \brief Multiplies this matrix with another matrix and returns the result.
 * \param rhs The matrix to multiply with.
 * \return The resulting matrix after multiplication.
 */
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const {
    Matrix4x4 result;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result.m[row][col] = 0;
            for (int k = 0; k < 4; ++k) {
                result.m[row][col] += m[row][k] * rhs.m[k][col];
            }
        }
    }
    return result;
}

/*!
 * \brief Multiplies this matrix with another matrix and assigns the result to itself.
 * \param rhs The matrix to multiply with.
 * \return A reference to the modified matrix.
 */
Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs) {
    *this = *this * rhs;
    return *this;
}

/*!
 * \brief Multiplies this matrix by a scalar value and returns the result.
 * \param scalar The scalar value to multiply by.
 * \return The resulting matrix after scalar multiplication.
 */
Matrix4x4 Matrix4x4::operator*(float scalar) const {
    Matrix4x4 result;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result.m[row][col] = m[row][col] * scalar;
        }
    }
    return result;
}

/*!
 * \brief Divides this matrix by a scalar value and returns the result.
 * \param scalar The scalar value to divide by.
 * \return The resulting matrix after scalar division.
 * \throws std::invalid_argument if scalar is zero.
 */
Matrix4x4 Matrix4x4::operator/(float scalar) const {
    if (scalar == 0) {
        throw std::invalid_argument("Division by zero.");
    }
    Matrix4x4 result;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result.m[row][col] = m[row][col] / scalar;
        }
    }
    return result;
}

/*!
 * \brief Returns an identity matrix.
 * \return A 4x4 identity matrix.
 */
Matrix4x4 Matrix4x4::Identity() {
    return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

/*!
 * \brief Returns the transpose of the given matrix.
 * \param mat The matrix to transpose.
 * \return The transposed matrix.
 */
Matrix4x4 Matrix4x4::Transpose(const Matrix4x4& mat) {
    Matrix4x4 result;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result.m[col][row] = mat.m[row][col];
        }
    }
    return result;
}

/*!
 * \brief Computes the determinant of a 3x3 matrix.
 * \param m00, m01, m02 Elements of the first row.
 * \param m10, m11, m12 Elements of the second row.
 * \param m20, m21, m22 Elements of the third row.
 * \return The determinant of the 3x3 matrix.
 */
float Determinant3x3(float m00, float m01, float m02,
    float m10, float m11, float m12,
    float m20, float m21, float m22) {
    return m00 * (m11 * m22 - m12 * m21) -
        m01 * (m10 * m22 - m12 * m20) +
        m02 * (m10 * m21 - m11 * m20);
}

/*!
 * \brief Computes the determinant of a 4x4 matrix.
 * \param mat The matrix to compute the determinant of.
 * \return The determinant of the matrix.
 */
float Matrix4x4::Determinant(const Matrix4x4& mat) {
    float det0 = Determinant3x3(mat.m[1][1], mat.m[1][2], mat.m[1][3],
        mat.m[2][1], mat.m[2][2], mat.m[2][3],
        mat.m[3][1], mat.m[3][2], mat.m[3][3]);

    float det1 = Determinant3x3(mat.m[1][0], mat.m[1][2], mat.m[1][3],
        mat.m[2][0], mat.m[2][2], mat.m[2][3],
        mat.m[3][0], mat.m[3][2], mat.m[3][3]);

    float det2 = Determinant3x3(mat.m[1][0], mat.m[1][1], mat.m[1][3],
        mat.m[2][0], mat.m[2][1], mat.m[2][3],
        mat.m[3][0], mat.m[3][1], mat.m[3][3]);

    float det3 = Determinant3x3(mat.m[1][0], mat.m[1][1], mat.m[1][2],
        mat.m[2][0], mat.m[2][1], mat.m[2][2],
        mat.m[3][0], mat.m[3][1], mat.m[3][2]);

    return mat.m[0][0] * det0 - mat.m[0][1] * det1 + mat.m[0][2] * det2 - mat.m[0][3] * det3;
}

/*!
 * \brief Computes the inverse of a 4x4 matrix.
 * \param mat The matrix to invert.
 * \return The inverse of the matrix.
 * \throws std::invalid_argument if the matrix is singular (non-invertible).
 */
Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& mat) {
    float det = Determinant(mat);
    if (det == 0.0f) {
        throw std::invalid_argument("Matrix is singular and cannot be inverted.");
    }

    Matrix4x4 result;

    result.m[0][0] = Determinant3x3(mat.m[1][1], mat.m[1][2], mat.m[1][3],
        mat.m[2][1], mat.m[2][2], mat.m[2][3],
        mat.m[3][1], mat.m[3][2], mat.m[3][3]) / det;

    result.m[0][1] = -Determinant3x3(mat.m[1][0], mat.m[1][2], mat.m[1][3],
        mat.m[2][0], mat.m[2][2], mat.m[2][3],
        mat.m[3][0], mat.m[3][2], mat.m[3][3]) / det;

    result.m[0][2] = Determinant3x3(mat.m[1][0], mat.m[1][1], mat.m[1][3],
        mat.m[2][0], mat.m[2][1], mat.m[2][3],
        mat.m[3][0], mat.m[3][1], mat.m[3][3]) / det;

    result.m[0][3] = -Determinant3x3(mat.m[1][0], mat.m[1][1], mat.m[1][2],
        mat.m[2][0], mat.m[2][1], mat.m[2][2],
        mat.m[3][0], mat.m[3][1], mat.m[3][2]) / det;

    result = Transpose(result);
    return result;
}

/*!
 * \brief Provides non-const access to a row of the matrix.
 * \param index The row index (0-based).
 * \return A pointer to the first element of the specified row.
 */
float* Matrix4x4::operator[](int index) {
    return m[index];
}

/*!
 * \brief Provides const access to a row of the matrix.
 * \param index The row index (0-based).
 * \return A const pointer to the first element of the specified row.
 */
const float* Matrix4x4::operator[](int index) const {
    return m[index];
}