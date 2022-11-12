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
		// Default constructor
		Vector2()
		{
			x = 0.0;
			y = 0.0;
		}

		// Constructor
		Vector2(double x, double y)
		{
			this->x = x;
			this->y = y;
		}

		// Core variables
		double x;
		double y;

		double size() const
		{
			return sqrt(pow(this->x, 2.0) + pow(this->y, 2.0));
		}

		double dot(Vector2 v) const
		{
			return this->x * v.x + this->y * v.y;
		}

		Vector2 normal() const
		{
			return Vector2(-this->y, this->x);
		}

		Vector2 normalized() const
		{
			return *this / this->size();
		}

		Vector2 projectOntoAxis(const Vector2 axis) const
		{
			if (*this == Vector2() || axis == Vector2())
				return Vector2();

			if (this->dot(axis) == 0.0)
				return Vector2();

			return axis * (this->dot(axis)) / (axis.dot(axis));
		}

		// Overloaded functions
		// Addition
		friend Vector2 operator+(const Vector2 vec1, const Vector2 vec2)
		{
			return Vector2(vec1.x + vec2.x, vec1.y + vec2.y);
		}

		friend Vector2 operator+(const Vector2 vec1, const double scalar)
		{
			return Vector2(vec1.x + scalar, vec1.y + scalar);
		}

		// Subtraction
		friend Vector2 operator-(const Vector2 vec1, const Vector2 vec2)
		{
			return vec1 + (-vec2);
		}

		friend Vector2 operator-(const Vector2 vec1, const double scalar)
		{
			return Vector2(vec1.x - scalar, vec1.y - scalar);
		}

		// Multiplication
		friend Vector2 operator*(const Vector2 vec1, const Vector2 vec2)
		{
			return Vector2(vec1.x * vec2.x, vec1.y * vec2.y);
		}

		friend Vector2 operator*(const Vector2 vec1, const double scalar)
		{
			return Vector2(vec1.x * scalar, vec1.y * scalar);
		}

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

		friend Vector2 operator/(const Vector2 vec1, const double scalar)
		{
			if (scalar == 0.0)
				throw("Division by zero");
			return Vector2(vec1.x / scalar, vec1.y / scalar);
		}

		// Negate
		friend Vector2 operator-(const Vector2 vec)
		{
			return Vector2(-vec.x, -vec.y);
		}

		// Equivalent
		friend bool operator==(const Vector2 vec1, const Vector2 vec2)
		{
			return vec1.x == vec2.x && vec1.y == vec2.y;
		}

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
			os << "[" << v.x << ", " << v.y << "]";
			return os;
		}
	};
}