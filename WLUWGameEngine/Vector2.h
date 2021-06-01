#pragma once

#include "Header.h"

class Vector2
{
private:
	float x;
	float y;

public:
	friend Vector2 operator+(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output;
		output.x = vec1.x + vec2.x;
		output.y = vec1.y + vec2.y;
		return output;
	}

	friend Vector2 operator-(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output;
		output.x = vec1.x - vec2.x;
		output.y = vec1.y - vec2.y;
		return output;
	}

	friend Vector2 operator*(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output;
		output.x = vec1.x * vec2.x;
		output.y = vec1.y * vec2.y;
		return output;
	}

	friend Vector2 operator/(const Vector2 vec1, const Vector2 vec2)
	{
		Vector2 output;
		output.x = vec1.x / vec2.x;
		output.y = vec1.y / vec2.y;
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

