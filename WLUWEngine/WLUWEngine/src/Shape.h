/*****************************************************************//**
 * \file   Shape.h
 * \brief  Defines the info for a convex 2D polygon
 * 
 * \author Bassil Virk
 * \date   January 2022
 *********************************************************************/

#pragma once

#include <vector>

#include "SDL.h"
#include "Vector2.h"

namespace WLUW
{
	/**
	 * /enum WLUW::ShapeType
	 * /brief A enum class for the possible types of shapes
	 */
	enum class ShapeType {
		POLYGON,
		CIRCLE
	};

	/**
	 * \class Shape Shape.h
	 * \brief Class which defines a convex 2D polygon
	 */
	class Shape
	{
	public:
		/////////////////////
		//// Constructors
		/////////////////////

		/**
		 * \brief Default constructor
		 * 
		 * \param pos position of shape
		 */
		Shape(Vector2 pos = Vector2());

		/**
		 * \brief Copy constructor
		 *
		 * \param shape Shape to copy
		 */
		Shape(const Shape& shape);

		/**
		 * \brief Constructor which moves a set of points from a vector to the object. The points vector will be empty after this
		 *
		 * \param points vector of points defining polygon
		 * \param pos position of shape
		 */
		Shape(std::vector<Vector2>& points, Vector2 pos = Vector2());

		/**
		 * \brief Constructor for a circle shape with a given radius
		 *
		 * \param radius radius of circle to create
		 * \param pos position of circle
		 */
		Shape(float radius, Vector2 pos = Vector2());

		/**
		 * \brief Constructor for a shape with only the type defined
		 *
		 * \param type type of shape
		 * \param pos position of shape
		 */
		Shape(ShapeType type, Vector2 pos = Vector2());

		///////////////////////////
		//// Operators/Assignments
		///////////////////////////

		/**
		 * \brief Copy assignment operator
		 *
		 * \param type type of shape
		 * \param pos position of shape
		 */
		Shape& operator=(const Shape& other);

		/**
		 * \brief Move assignment operator
		 *
		 * \param type type of shape
		 * \param pos position of shape
		 */
		Shape& operator=(Shape&& other) noexcept;

		friend bool operator==(const Shape& lhs, const Shape& rhs);

		/////////////////////
		//// Methods
		/////////////////////

		/**
		 * \brief Checks if two shapes are colliding by projecting both shapes onto all axes (If the two shapes are overlapping on every single axes of both shapes, then they are colliding)
		 *
		 * \param a First shape
		 * \param b Second shape
		 * \return The MTV containing the axis with the minimum translation distance and the distance
		 */
		static std::pair<Vector2, float> checkCollision(const Shape& a, const Shape& b);

		/**
		 * \brief Calculate the normals of the edges of the shape and swap them into this->points
		 */
		void calcNormals();

		/**
		 * \brief Gets projection of shape onto an axis
		 *
		 * \param axis axis to project shape on to
		 * \return pair of floats representing the min and max of the projection
		 */
		std::pair<float, float> projectOntoAxis(Vector2 axis) const;

		/**
		 * \brief Equality check
		 *
		 * \param other: Shape to compare with
		 * \return True if all parameters and vectors are equal
		 */
		virtual bool isEqual(Shape other) const
		{
			return this->type == other.type
				&& this->pos == other.pos
				&& this->radius == other.radius
				&& this->points == other.points
				&& this->normals == other.normals;
		}

		/////////////////////
		//// Modifier Methods
		/////////////////////
		
		/**
		 * \brief Add point to shape. Point is appended to end of vector.
		 *
		 * \param point point to add to shape
		 */
		void addPoint(Vector2 point);

		/**
		 * \brief Insert point to shape. Point is inserted inside vector.
		 *
		 * \param point point to insert into shape
		 * \param index index to insert point at
		 */
		void insertPoint(Vector2 point, int index);

		/**
		 * \brief Remove point from shape
		 *
		 * \param index index of point to remove
		 * \return removed point
		 */
		Vector2 removePoint(int index);

		/**
		 * \brief Swap out a point in the shape with another
		 *
		 * \param index index of the point to swap out
		 * \param point point to swap out with
		 * \return point which was swapped out
		 */
		Vector2 swapPoint(int index, Vector2 point);

		/////////////////////
		//// Getter Methods
		/////////////////////
		
		/**\return type of shape */
		ShapeType getShapeType() const { return type; };

		/**\return shape position */
		Vector2 getPos() const { return pos; };
		
		/**\return radius, if type is circle */
		float getRadius() const { return radius; };

		/**\return points defining polygon */
		std::vector<Vector2> const& getPoints() const { return points; };

		/**\return normal vectors of edges */
		std::vector<Vector2> const& getNormals() const { return normals; };

		/**\return The center point of shape */
		Vector2 getCenter();

		/**\return The size of the shape's bounding box */
		float getBoundingBoxSize();

		/////////////////////
		//// Setter Methods
		/////////////////////

		void setPos(Vector2 pos) { this->pos = pos; }

	protected:
		ShapeType type;					/* Type of shape */
		Vector2 pos;					/* Position of shape */
		float radius;					/* Radius, if shape is circle */
		std::vector<Vector2> points;	/* Points defining polygon */
		std::vector<Vector2> normals;	/* List of unique normal vectors for every edge */
	};
}

