#pragma once

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
	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	// Core variables
	float x;
	float y;

	// Overloaded functions
	// Addition
	friend Vector2 operator+(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x + vec2.x;
		output.y = vec1.y + vec2.y;
		return output;
	}

	friend Vector2 operator+(const Vector2 vec1, const float scalar)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x + scalar;
		output.y = vec1.y + scalar;
		return output;
	}

	// Subtraction
	friend Vector2 operator-(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x - vec2.x;
		output.y = vec1.y - vec2.y;
		return output;
	}

	friend Vector2 operator-(const Vector2 vec1, const float scalar)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x - scalar;
		output.y = vec1.y - scalar;
		return output;
	}

	// Multiplication
	friend Vector2 operator*(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x * vec2.x;
		output.y = vec1.y * vec2.y;
		return output;
	}

	friend Vector2 operator*(const Vector2 vec1, const float scalar)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x * scalar;
		output.y = vec1.y * scalar;
		return output;
	}

	// Division
	friend Vector2 operator/(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x / vec2.x;
		output.y = vec1.y / vec2.y;
		return output;
	}

	friend Vector2 operator/(const Vector2 vec1, const float scalar)
	{
		Vector2 output(0.0, 0.0);
		output.x = vec1.x / scalar;
		output.y = vec1.y / scalar;
		return output;
	}

	float& operator[](int index)
	{
		if (index > 1 || index < 0)
		{
			throw("out of range");
			float x;
			return x;
		}
		else if (index == 0)
		{
			return x;
		}
		else if (index == 1)
		{
			return y;
		}
	}

	const float& operator[](int index) const
	{
		if (index > 1 || index < 0)
		{
			throw("out of range");
			float x;
			return x;
		}
		else if (index == 0)
		{
			return x;
		}
		else if (index == 1)
		{
			return y;
		}
	}
};