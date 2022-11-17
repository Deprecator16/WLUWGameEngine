/*****************************************************************//**
 * \file   Vector2.h
 * \brief  2D Vector class and auxillaries
 * 
 * \author Noah Willsher
 * \date   May 2021
 *********************************************************************/
#pragma once

#include <iostream>
#include <math.h>
#include <limits>

namespace WLUW
{
	class Vector2
	{
	public:
		enum Orientation
		{
			COLLINEAR = 0,
			CLOCKWISE,
			COUNTERCLOCKWISE
		};

		// Constructors
		Vector2() { x = 0.0; y = 0.0; }
		Vector2(double x, double y) { this->x = x; this->y = y; }

		// Core variables
		double x;
		double y;

		// Helper functions
		double size() const { return sqrt(pow(this->x, 2.0) + pow(this->y, 2.0)); }
		double dot(Vector2 v) const { return this->x * v.x + this->y * v.y; }
		Vector2 normal() const { return Vector2(-this->y, this->x); }
		Vector2 normalized() const
		{
			if (this->size() == 0.0)
				return Vector2();
			return *this / this->size();
		}

		Vector2 projectOntoAxis(const Vector2 axis) const { return this->dot(axis) == 0.0 ? Vector2() : axis * (this->dot(axis)) / (axis.dot(axis)); }

		/**
		 * \brief Helper function that returns the orientation of an ordered triplet
		 *
		 * \param point1: First point in the triplet
		 * \param point2: Second point in the triplet
		 * \param point3: Third point in the triplet
		 * \return Orientation object specifying the orientation of the given ordered triplet, or COLLINEAR if collinear
		 */
		static Orientation getOrientation(Vector2 point1, Vector2 point2, Vector2 point3)
		{
			double val = (point2.y - point1.y) * (point3.x - point2.x) - (point2.x - point1.x) * (point3.y - point2.y);

			if (abs(val) < 0.0001) // Make sure to compare to an episilon value (Acceptable error), otherwise the algorithm is buggy
				return Orientation::COLLINEAR;

			return (val > 0) ? Orientation::CLOCKWISE : Orientation::COUNTERCLOCKWISE;
		}

		// Overloads
		// Addition
		friend Vector2 operator+(const Vector2 vec1, const Vector2 vec2) { return Vector2(vec1.x + vec2.x, vec1.y + vec2.y); }
		friend Vector2 operator+(const Vector2 vec, const double scalar) { return Vector2(vec.x + scalar, vec.y + scalar); }
		Vector2& operator+=(const Vector2& other) { this->x += other.x; this->y += other.y; return *this; }
		Vector2& operator+=(const double& scalar) { this->x += scalar; this->y += scalar; return *this; }

		// Subtraction
		friend Vector2 operator-(const Vector2 vec1, const Vector2 vec2) { return vec1 + (-vec2); }
		friend Vector2 operator-(const Vector2 vec, const double scalar) { return Vector2(vec.x - scalar, vec.y - scalar); }
		Vector2& operator-=(const Vector2& other) { this->x -= other.x; this->y -= other.y; return *this; }
		Vector2& operator-=(const double& scalar) { this->x -= scalar; this->y -= scalar; return *this; }

		// Multiplication
		friend Vector2 operator*(const Vector2 vec1, const Vector2 vec2) { return Vector2(vec1.x * vec2.x, vec1.y * vec2.y); }
		friend Vector2 operator*(const Vector2 vec, const double scalar) { return Vector2(vec.x * scalar, vec.y * scalar); }
		Vector2& operator*=(const double& scalar) { this->x *= scalar; this->y *= scalar; return *this; }

		// Division
		friend Vector2 operator/(const Vector2 vec1, const Vector2 vec2)
		{
			if (vec2 == Vector2())
				throw("Division by zero");
			if (vec2.x == 0.0)
				return Vector2(0.0, vec1.y / vec2.y);
			if (vec2.y == 0.0)
				return Vector2(vec1.x / vec2.x, 0.0);
			return Vector2(vec1.x / vec2.x, vec1.y / vec2.y);
		}
		friend Vector2 operator/(const Vector2 vec, const double scalar)
		{
			if (scalar == 0.0)
				throw("Division by zero");
			return Vector2(vec.x / scalar, vec.y / scalar);
		}
		Vector2& operator/=(const double& scalar)
		{
			if (scalar == 0.0)
				throw("Division by zero");
			this->x /= scalar;
			this->y /= scalar;
			return *this;
		}

		// Negate
		friend Vector2 operator-(const Vector2 vec) { return Vector2(-vec.x, -vec.y); }

		// Equivalent
		friend bool operator==(const Vector2 vec1, const Vector2 vec2) { return vec1.x == vec2.x && vec1.y == vec2.y; }

		// Index
		double& operator[](int index)
		{
			if (index > 1 || index < 0)
			{
				throw("out of range");
			}
			
			if (index == 0) return x;
			else return y;
		}
		const double& operator[](int index) const
		{
			if (index > 1 || index < 0)
			{
				throw("out of range");
			}

			else if (index == 0) return x;
			else return y;
		}

		// Output
		friend std::ostream& operator<<(std::ostream& os, const Vector2& v)
		{
			os << "(" << v.x << ", " << v.y << ")";
			return os;
		}
	};
}