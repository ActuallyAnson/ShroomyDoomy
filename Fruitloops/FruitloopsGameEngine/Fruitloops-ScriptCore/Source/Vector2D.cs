/******************************************************************************
/*!
\file  Vector2D.cs
\Proj name  Shroomy Doomy
\author  Anson Teng
\date    March 14, 2025
\brief  Implements a 2D vector struct with mathematical operations, properties,
        and utility methods for game development.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

using System;

namespace FruitLoops
{
    /**
     * @brief Represents a 2D vector with X and Y components, providing mathematical
     * operations and utility functions.
     * 
     * @note Supports vector arithmetic, normalization, distance calculations, dot and
     * cross products, and equality comparisons.
     */
    public struct Vector2D
    {
        /** @brief Gets or sets the X component of the vector. */
        public float X { get; set; }

        /** @brief Gets or sets the Y component of the vector. */
        public float Y { get; set; }

        /**
         * @brief Constructs a new Vector2D with specified X and Y components.
         * 
         * @param x The X component value.
         * @param y The Y component value.
         */
        public Vector2D(float x, float y)
        {
            X = x;
            Y = y;
        }

        /**
         * @brief Adds two Vector2D instances component-wise.
         * 
         * @param a The first vector.
         * @param b The second vector.
         * @return A new Vector2D representing the sum of a and b.
         */
        public static Vector2D operator +(Vector2D a, Vector2D b)
        {
            return new Vector2D(a.X + b.X, a.Y + b.Y);
        }

        /**
         * @brief Subtracts one Vector2D from another component-wise.
         * 
         * @param a The vector to subtract from.
         * @param b The vector to subtract.
         * @return A new Vector2D representing the difference of a and b.
         */
        public static Vector2D operator -(Vector2D a, Vector2D b)
        {
            return new Vector2D(a.X - b.X, a.Y - b.Y);
        }

        /**
         * @brief Negates a Vector2D component-wise.
         * 
         * @param a The vector to negate.
         * @return A new Vector2D with negated X and Y components.
         */
        public static Vector2D operator -(Vector2D a)
        {
            return new Vector2D(-a.X, -a.Y);
        }

        /**
         * @brief Multiplies a Vector2D by a scalar value.
         * 
         * @param a The vector to scale.
         * @param scalar The scalar value to multiply by.
         * @return A new Vector2D scaled by the scalar.
         */
        public static Vector2D operator *(Vector2D a, float scalar)
        {
            return new Vector2D(a.X * scalar, a.Y * scalar);
        }

        /**
         * @brief Multiplies a scalar value by a Vector2D.
         * 
         * @param scalar The scalar value to multiply by.
         * @param a The vector to scale.
         * @return A new Vector2D scaled by the scalar.
         */
        public static Vector2D operator *(float scalar, Vector2D a)
        {
            return new Vector2D(a.X * scalar, a.Y * scalar);
        }

        /**
         * @brief Divides a Vector2D by a scalar value.
         * 
         * @param a The vector to divide.
         * @param scalar The scalar value to divide by.
         * @return A new Vector2D divided by the scalar.
         * @note Throws DivideByZeroException if scalar is zero.
         */
        public static Vector2D operator /(Vector2D a, float scalar)
        {
            if (scalar == 0)
                throw new DivideByZeroException("Division by zero is not allowed.");
            return new Vector2D(a.X / scalar, a.Y / scalar);
        }

        /**
         * @brief Compares two Vector2D instances for sorting purposes.
         * 
         * @param a The first vector.
         * @param b The second vector.
         * @return True if a is less than b, false otherwise.
         * @note Compares X first, then Y if X values are equal.
         */
        public static bool operator <(Vector2D a, Vector2D b)
        {
            return (a.X < b.X) || (a.X == b.X && a.Y < b.Y);
        }

        /**
         * @brief Compares two Vector2D instances for sorting purposes.
         * 
         * @param a The first vector.
         * @param b The second vector.
         * @return True if a is greater than b, false otherwise.
         * @note Compares X first, then Y if X values are equal.
         */
        public static bool operator >(Vector2D a, Vector2D b)
        {
            return (a.X > b.X) || (a.X == b.X && a.Y > b.Y);
        }

        /** @brief Gets the magnitude (length) of the vector. */
        public float Length => (float)Math.Sqrt(X * X + Y * Y);

        /** @brief Gets the squared magnitude of the vector. */
        public float SquareLength => X * X + Y * Y;

        /**
         * @brief Gets the normalized version of the vector.
         * 
         * @return A new Vector2D with unit length, or (0, 0) if length is zero.
         */
        public Vector2D Normalized
        {
            get
            {
                float len = Length;
                return len > 0 ? this / len : new Vector2D(0, 0);
            }
        }

        /**
         * @brief Calculates the length of a vector.
         * 
         * @param v The vector to measure.
         * @return The magnitude of the vector.
         */
        public static float GetLength(Vector2D v)
        {
            return v.Length;
        }

        /**
         * @brief Calculates the squared length of a vector.
         * 
         * @param v The vector to measure.
         * @return The squared magnitude of the vector.
         */
        public static float GetSquareLength(Vector2D v)
        {
            return v.SquareLength;
        }

        /**
         * @brief Normalizes a vector.
         * 
         * @param v The vector to normalize.
         * @return A new Vector2D with unit length, or (0, 0) if length is zero.
         */
        public static Vector2D Normalize(Vector2D v)
        {
            return v.Normalized;
        }

        /**
         * @brief Calculates the distance between two vectors.
         * 
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @return The distance between v1 and v2.
         */
        public static float Distance(Vector2D v1, Vector2D v2)
        {
            return (v1 - v2).Length;
        }

        /**
         * @brief Calculates the squared distance between two vectors.
         * 
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @return The squared distance between v1 and v2.
         */
        public static float SquareDistance(Vector2D v1, Vector2D v2)
        {
            return (v1 - v2).SquareLength;
        }

        /**
         * @brief Computes the dot product of two vectors.
         * 
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @return The dot product of v1 and v2.
         */
        public static float Dot(Vector2D v1, Vector2D v2)
        {
            return v1.X * v2.X + v1.Y * v2.Y;
        }

        /**
         * @brief Computes the 2D cross product (z-component) of two vectors.
         * 
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @return The cross product magnitude (scalar value).
         * @note In 2D, this represents the z-component of the 3D cross product.
         */
        public static float Cross(Vector2D v1, Vector2D v2)
        {
            return v1.X * v2.Y - v1.Y * v2.X;
        }

        /**
         * @brief Creates a new Vector2D with specified components.
         * 
         * @param x The X component value.
         * @param y The Y component value.
         * @return A new Vector2D instance with the given x and y values.
         */
        public static Vector2D Set(float x, float y)
        {
            return new Vector2D(x, y);
        }

        /** @brief Gets a Vector2D with both components set to zero. */
        public static Vector2D Zero => new Vector2D(0, 0);

        /**
         * @brief Checks if two Vector2D instances are approximately equal.
         * 
         * @param a The first vector.
         * @param b The second vector.
         * @return True if the vectors are equal within float.Epsilon, false otherwise.
         */
        public static bool operator ==(Vector2D a, Vector2D b)
        {
            return Math.Abs(a.X - b.X) < float.Epsilon && Math.Abs(a.Y - b.Y) < float.Epsilon;
        }

        /**
         * @brief Checks if two Vector2D instances are not equal.
         * 
         * @param a The first vector.
         * @param b The second vector.
         * @return True if the vectors are not equal, false otherwise.
         */
        public static bool operator !=(Vector2D a, Vector2D b)
        {
            return !(a == b);
        }

        /**
         * @brief Determines if this Vector2D equals another object.
         * 
         * @param obj The object to compare with.
         * @return True if obj is a Vector2D and equal to this instance, false otherwise.
         */
        public override bool Equals(object obj)
        {
            if (obj is Vector2D other)
            {
                return this == other;
            }
            return false;
        }

        /**
         * @brief Generates a hash code for this Vector2D.
         * 
         * @return An integer hash code based on X and Y components.
         */
        public override int GetHashCode()
        {
            unchecked
            {
                int hash = 17;
                hash = hash * 23 + X.GetHashCode();
                hash = hash * 23 + Y.GetHashCode();
                return hash;
            }
        }
    }
}