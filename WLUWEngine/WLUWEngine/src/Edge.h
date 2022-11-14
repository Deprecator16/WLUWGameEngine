#pragma once

#include "Vector2.h"

namespace WLUW
{
	class Edge
	{
	public:
		// Constructors
		Edge()
		{
			this->first = Vector2();
			this->second = Vector2();
		}
		Edge(Vector2 first, Vector2 second)
		{
			this->first = first;
			this->second = second;
		}

		Vector2 first;
		Vector2 second;

		// Helper functions
		Vector2 slope() { return second - first; }
		double size() { return (second - first).size(); }
		Vector2 midpoint() { return first + ((second - first) / 2.0); }
		Vector2 normal() const { return (second - first).normal(); }

		/**
		 * \brief Helper function that returns whether a given point lies on a given edge
		 *
		 * \param point: Specified point
		 * \param edge: Specified edge
		 * \return True if point lies on edge, false otherwise
		 */
		bool onSegment(Vector2& point)
		{
			// Check collinearity
			double val = (this->first.y - point.y) * (this->second.x - this->first.x) - (this->first.x - point.x) * (this->second.y - this->first.y);
			if (val != 0.0)
				return false;

			return (point - this->first).size() + (point - this->second).size() == (this->second - this->first).size();
		}

		/**
		 * \brief Helper function that returns whether two edges intersect
		 *
		 * \param edge1: First edge
		 * \param edge2: Second edge
		 * \return True if edge1 and edge2 intersect, false otherwise
		 */
		static bool areIntersecting(Edge edge1, Edge edge2)
		{
			// Check for parallelism
			if (edge1.second - edge1.first == edge2.second - edge2.first ||
				edge1.second - edge1.first == edge2.first - edge2.second)
				return false;

			// Find the four orientations needed for general and special cases
			Vector2::Orientation o1 = Vector2::getOrientation(edge1.first, edge1.second, edge2.first);
			Vector2::Orientation o2 = Vector2::getOrientation(edge1.first, edge1.second, edge2.second);
			Vector2::Orientation o3 = Vector2::getOrientation(edge2.first, edge2.second, edge1.first);
			Vector2::Orientation o4 = Vector2::getOrientation(edge2.first, edge2.second, edge1.second);

			// General case
			if (o1 != o2 && o3 != o4)
				return true;

			// Special Cases
			// edge2.first collinear with edge1
			if (o1 == 0 && edge1. onSegment(edge2.first)) return true;
			// edge2.second collinear with edge1
			if (o2 == 0 && edge1. onSegment(edge2.second)) return true;
			// edge1.first collinear with edge2
			if (o3 == 0 && edge2.onSegment(edge1.first)) return true;
			// edge1.second collinear with edge2
			if (o4 == 0 && edge2.onSegment(edge1.second)) return true;

			return false; // Doesn't fall in any of the above cases
		}

		/**
		 * \brief Helper function that returns the point of intersection between two edges
		 *
		 * \param edge1: First edge
		 * \param edge2: Second edge
		 * \return The point of intersection between edge1 and edge2
		 */
		static Vector2 getPointOfIntersection(Edge edge1, Edge edge2)
		{
			// Line 1 represented as a1x + b1y = c1
			double a1 = edge1.second.y - edge1.first.y;
			double b1 = edge1.first.x - edge1.second.x;
			double c1 = a1 * (edge1.first.x) + b1 * (edge1.first.y);

			// Line 2 represented as a2x + b2y = c2
			double a2 = edge2.second.y - edge2.first.y;
			double b2 = edge2.first.x - edge2.second.x;
			double c2 = a2 * (edge2.first.x) + b2 * (edge2.first.y);

			double determinant = a1 * b2 - a2 * b1;

			if (determinant == 0)
			{
				// The lines are parallel, no intersection
				// Return the point that is closest to the first point in edge1
				if ((edge2.first - edge1.first).size() <= (edge2.second - edge1.first).size())
					return edge2.first;
				return edge2.second;
			}
			else
			{
				// Get point of intersection
				double x = (b2 * c1 - b1 * c2) / determinant;
				double y = (a1 * c2 - a2 * c1) / determinant;

				return Vector2(x, y);
			}
		}

		// Overloads
		// Addition
		friend Edge operator+(const Edge edge1, const Edge edge2) { return Edge(edge1.first + edge2.first, edge1.second + edge2.second); }
		friend Edge operator+(const Edge edge, const Vector2 vec) { return Edge(edge.first + vec, edge.second + vec); }
		friend Edge operator+(const Edge edge, const double scalar) { return Edge(edge.first + scalar, edge.second + scalar); }
		Edge& operator+=(const Edge& other) { this->first += other.first; this->second += other.second; return *this; }
		Edge& operator+=(const Vector2& vec) { this->first += vec; this->second += vec; return *this; }
		Edge& operator+=(const double& scalar) { this->first += scalar; this->second += scalar; return *this; }

		// Subtraction
		friend Edge operator-(const Edge edge1, const Edge edge2) { return edge1 + (-edge2); }
		friend Edge operator-(const Edge edge, const Vector2 vec) { return Edge(edge.first - vec, edge.second - vec); }
		friend Edge operator-(const Edge edge, const double scalar) { return Edge(edge.first - scalar, edge.second - scalar); }
		Edge& operator-=(const Edge& other) { this->first -= other.first; this->second -= other.second; return *this; }
		Edge& operator-=(const Vector2& vec) { this->first -= vec; this->second -= vec; return *this; }
		Edge& operator-=(const double& scalar) { this->first -= scalar; this->second -= scalar; return *this; }

		// Multiplication
		friend Edge operator*(const Edge edge, const double scalar) { return Edge(edge.first * scalar, edge.second * scalar); }
		Edge& operator*=(const double& scalar) { this->first *= scalar; this->second *= scalar; return *this; }

		// Division
		friend Edge operator/(const Edge edge, const double scalar)
		{
			if (scalar == 0.0)
				throw("Division by zero");
			return Edge(edge.first / scalar, edge.second / scalar);
		}
		Edge& operator/=(const double& scalar)
		{
			if (scalar == 0.0)
				throw("Division by zero");
			this->first /= scalar;
			this->second /= scalar;
			return *this;
		}

		// Negate
		friend Edge operator-(const Edge edge) { return Edge(-edge.first, -edge.second); }

		// Equivalent
		friend bool operator==(const Edge edge1, const Edge edge2) { return edge1.first == edge2.first && edge1.second == edge2.second; }

		// Index
		Vector2& operator[](int index)
		{
			if (index > 1 || index < 0)
			{
				throw("out of range");
			}

			if (index == 0) return first;
			else return second;
		}
		const Vector2& operator[](int index) const
		{
			if (index > 1 || index < 0)
			{
				throw("out of range");
			}

			else if (index == 0) return first;
			else return second;
		}

		// Output
		friend std::ostream& operator<<(std::ostream& os, const Edge& e)
		{
			os << "[" << e.first << ", " << e.second << "]";
			return os;
		}
	};
}
