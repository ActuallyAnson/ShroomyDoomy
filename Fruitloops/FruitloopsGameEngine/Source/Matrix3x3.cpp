/******************************************************************************/
/*!
\file  Matrix3x3.cpp
\Proj name  Shroomy Doomy
\author  Benjamin Lau
\date    Oct 04, 2024
\brief  This file contains the implementation of the Matrix3x3 class, responsible
        for providing matrix operations including identity, translation, scaling,
        rotation, transposition, and inversion, used in 2D transformations.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "Matrix3x3.h"
#include "Log.h"

namespace FruitLoops {

/*!
* \brief Default constructor that initializes all elements of the matrix to 0.
*/
    Matrix3x3::Matrix3x3() {
        for (int i = 0; i < 9; i++) {
            data.m[i] = 0.0f;
        }
    }

/*!
* \brief Parameterized constructor that initializes the matrix with specified values.
* \param m00, m01, m02 Elements of the first row.
* \param m10, m11, m12 Elements of the second row.
* \param m20, m21, m22 Elements of the third row.
*/
    Matrix3x3::Matrix3x3(float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22) {
        data.Mat.m00 = m00; data.Mat.m01 = m01; data.Mat.m02 = m02;
        data.Mat.m10 = m10; data.Mat.m11 = m11; data.Mat.m12 = m12;
        data.Mat.m20 = m20; data.Mat.m21 = m21; data.Mat.m22 = m22;
    }

/*!
* \brief Constructs a matrix by copying values from a given array.
* \param array A pointer to an array of 9 floats.
*/
    Matrix3x3::Matrix3x3(const float* array) {
        for (int i = 0; i < 9; i++) {
            data.m[i] = array[i];
        }
    }

/*!
* \brief Accesses an element of the matrix for modification.
* \param row The row index (0-based).
* \param col The column index (0-based).
* \return A reference to the specified matrix element.
*/
    float& Matrix3x3::operator()(int row, int col) {
        return data.m[row * 3 + col];
    }

/*!
* \brief Accesses an element of the matrix for reading.
* \param row The row index (0-based).
* \param col The column index (0-based).
* \return The value of the specified matrix element.
*/
    float Matrix3x3::operator()(int row, int col) const {
        return data.m[row * 3 + col];
    }

/*!
* \brief Sets the given matrix to an identity matrix.
* \param result The matrix to modify.
*/
    void Matrix3x3Identity(Matrix3x3& result) {
        result.data.Mat.m00 = 1.0f; result.data.Mat.m01 = 0.0f; result.data.Mat.m02 = 0.0f;
        result.data.Mat.m10 = 0.0f; result.data.Mat.m11 = 1.0f; result.data.Mat.m12 = 0.0f;
        result.data.Mat.m20 = 0.0f; result.data.Mat.m21 = 0.0f; result.data.Mat.m22 = 1.0f;
    }

/*!
* \brief Applies a 2D translation transformation to the given matrix.
* \param result The matrix to modify.
* \param x Translation offset along the x-axis.
* \param y Translation offset along the y-axis.
*/
    void Matrix3x3Translate(Matrix3x3& result, float x, float y) {
        Matrix3x3Identity(result);
        result.data.Mat.m02 = x;
        result.data.Mat.m12 = y;

        LogMatrix("Applied translation: x = {}, y = {}", x, y);
    }

/*!
* \brief Applies a 2D scaling transformation to the given matrix.
* \param result The matrix to modify.
* \param x Scaling factor along the x-axis.
* \param y Scaling factor along the y-axis.
*/
    void Matrix3x3Scale(Matrix3x3& result, float x, float y) {
        result.data.Mat.m00 = x; result.data.Mat.m01 = 0.0f; result.data.Mat.m02 = 0.0f;
        result.data.Mat.m10 = 0.0f; result.data.Mat.m11 = y; result.data.Mat.m12 = 0.0f;
        result.data.Mat.m20 = 0.0f; result.data.Mat.m21 = 0.0f; result.data.Mat.m22 = 1.0f;
    }

/*!
* \brief Applies a 2D rotation transformation in radians to the given matrix.
* \param result The matrix to modify.
* \param angle The rotation angle in radians.
*/
    void Matrix3x3RotRad(Matrix3x3& result, float angle) {
        float cosin = std::cos(angle);
        float sine = std::sin(angle);
        result.data.Mat.m00 = cosin; result.data.Mat.m01 = -sine;  result.data.Mat.m02 = 0.0f;
        result.data.Mat.m10 = sine;  result.data.Mat.m11 = cosin; result.data.Mat.m12 = 0.0f;
        result.data.Mat.m20 = 0.0f; result.data.Mat.m21 = 0.0f; result.data.Mat.m22 = 1.0f;
    }

/*!
* \brief Applies a 2D rotation transformation in degrees to the given matrix.
* \param result The matrix to modify.
* \param angle The rotation angle in degrees.
*/
    void Matrix3x3RotDeg(Matrix3x3& result, float angle) {
        float radian = angle * (static_cast<float>(M_PI) / 180.0f);
        Matrix3x3RotRad(result, radian);
    }

/*!
* \brief Computes the transpose of a given matrix.
* \param result The matrix to store the transposed result.
* \param inputMatrix The matrix to transpose.
*/
    void Matrix3x3Transpose(Matrix3x3& result, const Matrix3x3& inputMatrix) {
        result.data.Mat.m00 = inputMatrix.data.Mat.m00; result.data.Mat.m01 = inputMatrix.data.Mat.m10; result.data.Mat.m02 = inputMatrix.data.Mat.m20;
        result.data.Mat.m10 = inputMatrix.data.Mat.m01; result.data.Mat.m11 = inputMatrix.data.Mat.m11; result.data.Mat.m12 = inputMatrix.data.Mat.m21;
        result.data.Mat.m20 = inputMatrix.data.Mat.m02; result.data.Mat.m21 = inputMatrix.data.Mat.m12; result.data.Mat.m22 = inputMatrix.data.Mat.m22;
    }

/*!
* \brief Computes the inverse of a given matrix and its determinant.
* \param result The matrix to store the inverse.
* \param determinant The determinant of the input matrix.
* \param inputMatrix The matrix to invert.
*/
    void Matrix3x3Inverse(Matrix3x3& result, float& determinant, const Matrix3x3& inputMatrix) {
        determinant = inputMatrix.data.Mat.m00 * (inputMatrix.data.Mat.m11 * inputMatrix.data.Mat.m22 - inputMatrix.data.Mat.m12 * inputMatrix.data.Mat.m21) -
            inputMatrix.data.Mat.m01 * (inputMatrix.data.Mat.m10 * inputMatrix.data.Mat.m22 - inputMatrix.data.Mat.m12 * inputMatrix.data.Mat.m20) +
            inputMatrix.data.Mat.m02 * (inputMatrix.data.Mat.m10 * inputMatrix.data.Mat.m21 - inputMatrix.data.Mat.m11 * inputMatrix.data.Mat.m20);

        if (determinant == 0.0f) {
            Matrix3x3Identity(result);
            return;
        }

        Matrix3x3 cofactor;
        cofactor.data.Mat.m00 = +(inputMatrix.data.Mat.m11 * inputMatrix.data.Mat.m22 - inputMatrix.data.Mat.m21 * inputMatrix.data.Mat.m12);
        cofactor.data.Mat.m01 = -(inputMatrix.data.Mat.m10 * inputMatrix.data.Mat.m22 - inputMatrix.data.Mat.m20 * inputMatrix.data.Mat.m12);
        cofactor.data.Mat.m02 = +(inputMatrix.data.Mat.m10 * inputMatrix.data.Mat.m21 - inputMatrix.data.Mat.m20 * inputMatrix.data.Mat.m11);
        cofactor.data.Mat.m10 = -(inputMatrix.data.Mat.m01 * inputMatrix.data.Mat.m22 - inputMatrix.data.Mat.m21 * inputMatrix.data.Mat.m02);
        cofactor.data.Mat.m11 = +(inputMatrix.data.Mat.m00 * inputMatrix.data.Mat.m22 - inputMatrix.data.Mat.m20 * inputMatrix.data.Mat.m02);
        cofactor.data.Mat.m12 = -(inputMatrix.data.Mat.m00 * inputMatrix.data.Mat.m21 - inputMatrix.data.Mat.m20 * inputMatrix.data.Mat.m01);
        cofactor.data.Mat.m20 = +(inputMatrix.data.Mat.m01 * inputMatrix.data.Mat.m12 - inputMatrix.data.Mat.m11 * inputMatrix.data.Mat.m02);
        cofactor.data.Mat.m21 = -(inputMatrix.data.Mat.m00 * inputMatrix.data.Mat.m12 - inputMatrix.data.Mat.m10 * inputMatrix.data.Mat.m02);
        cofactor.data.Mat.m22 = +(inputMatrix.data.Mat.m00 * inputMatrix.data.Mat.m11 - inputMatrix.data.Mat.m10 * inputMatrix.data.Mat.m01);

        Matrix3x3 adjugate;
        Matrix3x3Transpose(adjugate, cofactor);

        for (int i = 0; i < 9; ++i) {
            result.data.m[i] = adjugate.data.m[i] / determinant;
        }
    }

/*!
* \brief Overloads the assignment operator for matrix assignment.
* \param rhs The matrix to assign from.
* \return A reference to the modified matrix.
*/
    Matrix3x3& Matrix3x3::operator=(const Matrix3x3& rhs) {
        if (this == &rhs) {
            return *this;
        }
        for (int i = 0; i < 9; ++i) {
            data.m[i] = rhs.data.m[i];
        }
        return *this;
    }

/*!
* \brief Overloads the multiplication-assignment operator for matrix multiplication.
* \param rhs The matrix to multiply with.
* \return A reference to the modified matrix.
*/
    Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& rhs) {
        Matrix3x3 result;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result(row, col) = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    result(row, col) += (*this)(row, k) * rhs(k, col);
                }
            }
        }
        *this = result;
        return *this;
    }
}