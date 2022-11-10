/*****************************************************************//**
 * \file   Shape.cpp
 * \brief  Implementation of shape class and collision detection
 *
 * \author Bassil Virk
 * \date   January 2022
 *********************************************************************/

#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>

#include "Shape.h"

using namespace WLUW;

// Aliases
using Axis = Vector2;
using Proj = std::pair<double, double>;
using MTV = std::pair<Vector2, double>;

WLUW::Shape::Shape(Vector2 pos)
    : type(ShapeType::POLYGON), radius(0.f), pos(pos)
{
}

WLUW::Shape::Shape(const Shape& shape) : type(shape.type), pos(shape.pos), radius(shape.radius)
{
    this->points = shape.points;
    this->normals = shape.normals;
}

WLUW::Shape::Shape(std::vector<Vector2>& points, Vector2 pos)
    : type(ShapeType::POLYGON), radius(0.f)
{
    this->points.insert(this->points.end(), std::make_move_iterator(points.begin()), std::make_move_iterator(points.end()));
    points.erase(points.begin(), points.end());
}

WLUW::Shape::Shape(double radius, Vector2 pos) : type(ShapeType::CIRCLE), radius(radius)
{
}

WLUW::Shape::Shape(ShapeType type, Vector2 pos) : type(type), radius(0.f)
{
}

Shape& WLUW::Shape::operator=(const Shape& other)
{
    this->type = other.type;
    this->pos = other.pos;
    this->radius = other.radius;
    this->points = other.points;
    this->normals = other.normals;

    return *this;
}

Shape& WLUW::Shape::operator=(Shape&& other) noexcept
{
    this->type = other.type;
    this->pos = other.pos;
    this->radius = other.radius;
    std::swap(this->points, other.points);
    std::swap(this->normals, other.normals);

    return *this;
}

bool WLUW::operator==(const Shape& lhs, const Shape& rhs)
{
    return lhs.isEqual(rhs);
}

/**
 * \brief Helper function which gets the length of the union of two ranges/projections
 *
 * \param a first range/projection
 * \param b second range/projection
 * \return length of the union of a and b, or NaN if no overlap
 */
bool isOverlapping(Proj a, Proj b)
{
    if (b.first > a.second || a.first > b.second)
        return false;
    return true;
}

/**
 * \brief Helper function which gets the length of the union of two ranges/projections
 *
 * \param a first range/projection
 * \param b second range/projection
 * \return length of the union of a and b, or NaN if no overlap
 */
double getOverlap(Proj a, Proj b)
{
    if (b.first > a.second || a.first > b.second)
    {
        throw("Proj a and b are not overlapping! Use isOverlapping() to assert that they overlap before calling getOverlap()");
        return std::numeric_limits<double>::quiet_NaN();
    }

    double start = std::max(a.first, b.first);
    double end = std::min(a.second, b.second);

    return end - start;
}

/**
 * \brief Helper function which calculates circle to circle collision
 * 
 * \param a first circle
 * \param b second circle
 * \return Axis between the centre of the two circles
 */
Axis calcCircleToCircleCollisionAxis(const Shape& a, const Shape& b)
{
    if (a.getShapeType() != ShapeType::CIRCLE || b.getShapeType() != ShapeType::CIRCLE)
    {
        throw("Atleast one of the shapes was not a circle");
        return Axis(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    }

    return a.getPos() - b.getPos();
}

/**
 * \brief Helper function which calculates circle to polygon collision
 *
 * \param circle circle shape
 * \param poly polygon shape
 * \return Axes to test for collision
 */
std::vector<Axis> calcCircleToPolygonCollisionAxis(const Shape& circle, const Shape& poly)
{
    if (circle.getShapeType() != ShapeType::CIRCLE)
    {
        throw("The provided circle is not of the circle shape type");
        return std::vector<Axis>();
    }
    if (poly.getShapeType() != ShapeType::POLYGON)
    {
        throw("The provided polygon is not of the polygon shape type");
        return std::vector<Axis>();
    }

    std::vector<Axis> axes;
    Vector2 circlePos = circle.getPos();

    // Generate all axes from centre of circle to polygon vertices
    for (auto &point : poly.getPoints())
    {
        axes.emplace_back(point - circlePos);
    }

    return axes;
}

MTV WLUW::Shape::checkCollision(const Shape& a, const Shape& b)
{
    double mtvOverlap = std::numeric_limits<double>::max();
    Axis mtvAxis;
    std::vector<Axis> allAxes, axes1, axes2;

    // Find collision axes
    // If both shapes are circles
    if (a.getShapeType() == ShapeType::CIRCLE && b.getShapeType() == ShapeType::CIRCLE)
    {
        Axis axis = calcCircleToCircleCollisionAxis(a, b);
        axes1.push_back(axis);
    }
    // If one of the shapes is a circle
    else if (a.getShapeType() == ShapeType::CIRCLE || b.getShapeType() == ShapeType::CIRCLE)
    {
        // If a is the circle
        if (a.getShapeType() == ShapeType::CIRCLE)
        {
            axes1 = calcCircleToPolygonCollisionAxis(a, b);
            axes2 = b.getNormals();
        }
        // If b is the circle
        else
        {
            axes1 = a.getNormals();
            axes2 = calcCircleToPolygonCollisionAxis(b, a);
        }
    }
    // Both shapes are polygons
    else
    {
        axes1 = a.getNormals();
        axes2 = b.getNormals();
    }

    allAxes = axes1;

    // Add all elements of axes2 into allAxes
    for (auto& axis : axes2)
    {
        // Don't add vector if any vector currently in newNormal is collinear to this one
        if (!std::any_of(
            allAxes.begin(),
            allAxes.end(),
            [=](Vector2 vec) { return (vec.dot(axis) == vec.size() * axis.size()) || (vec.dot(axis) == -vec.size() * axis.size()); }))
        {
            allAxes.push_back(std::move(axis));
        }
    }

    // Loop over the axes
    for (int i = 0; i < allAxes.size(); i++)
    {
        Axis axis = allAxes[i];
        // Project both shapes onto the axis
        Proj p1 = a.projectOntoAxis(axis);
        Proj p2 = b.projectOntoAxis(axis);

        // Do the projections overlap?
        // If so, we can guarantee that the shapes do not overlap
        if (!isOverlapping(p1, p2))
            return MTV(Axis(0, 0), 0);

        // Get overlapping distance
        double overlap = getOverlap(p1, p2);

        // Check for minimum
        // We want to return the smallest overlap length
        if (overlap < mtvOverlap)
        {
            // Then set this one as the smallest
            mtvOverlap = overlap;
            mtvAxis = axis;
        }
    }

    MTV mtv(mtvAxis, mtvOverlap);
    return mtv;
}

void WLUW::Shape::calcNormals()
{
    // Not enough points to calculate a normal
    if (this->type == ShapeType::POLYGON && this->points.size() <= 1)
    {
        throw("Not enough points to calculate normals");
        return;
    }

    // No need to precalculate the normals of a circle
    if (this->type == ShapeType::CIRCLE)
    {
        throw("Circles should not need their normals calculated");
        return;
    }

    std::vector<Vector2> newNormals;
    int const numPoints = this->points.size();

    // Calculate normals
    for (int i = 0; i < numPoints; i++)
    {
        Vector2 p1 = this->points[i];
        Vector2 p2 = this->points[(i + 1) % numPoints];

        Vector2 edge = p1 - p2;
        Vector2 normal = edge.normal().normalized();

        // Don't add vector if any vector currently in newNormal is collinear to this one
        if (!std::any_of(
            newNormals.begin(),
            newNormals.end(),
            [=](Vector2 vec) { return (vec.dot(normal) == vec.size() * normal.size()) || (vec.dot(normal) == -vec.size() * normal.size()); })
           )
        {
            newNormals.push_back(std::move(normal));
        }
    }

    // Swap in new normals
    std::swap(this->normals, newNormals);
}

Proj WLUW::Shape::projectOntoAxis(Vector2 axis) const
{
    // If shape is a polygon
    if (this->type == ShapeType::POLYGON)
    {
        // Not enough points
        if (this->points.size() <= 0)
        {
            throw("Not enough points");
            return std::make_pair(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
        }

        // Project the points onto the axis and keep the min and max points along the axis
        double min = axis.dot(this->points[0] + this->pos);
        double max = min;

        for (int i = 1; i < this->points.size(); i++)
        {
            double p = axis.dot(this->points[i] + this->pos);
            if (p < min)
                min = p;
            else if (p > max)
                max = p;
        }

        return Proj(min, max);
    }
    // Shape is a circle
    else
    {
        // Project either end of the circle onto the axis
        double min = axis.dot(this->pos + axis * this->radius);
        double max = axis.dot(this->pos - axis * this->radius);

        if (min > max) std::swap(min, max); // Swap if min is larger

        return Proj(min, max);
    }
}

void WLUW::Shape::addPoint(Vector2 point)
{
    this->points.push_back(point);
    if (points.size() > 1)
    {
        calcNormals(); // Recalculate normals
    }
}

void WLUW::Shape::insertPoint(Vector2 point, int index)
{
    if (index < 0 || index > this->points.size())
    {
        throw("out of range");
        return;
    }

    this->points.insert(this->points.begin() + index, point);
    if (points.size() > 1)
    {
        calcNormals(); // Recalculate normals
    }
}

Vector2 WLUW::Shape::removePoint(int index)
{
    if (index < 0 || index >= this->points.size())
    {
        throw("out of range");
        return Vector2();
    }

    // Test if getting the removed value doesn't result in garbage
    Vector2 removed = this->points.at(index);
    Vector2 returned = Vector2(removed.x, removed.y);

    this->points.erase(this->points.begin() + index);
    if (points.size() > 1)
    {
        calcNormals(); // Recalculate normals
    }

    return returned;
}

Vector2 WLUW::Shape::swapPoint(int index, Vector2 point)
{
    if (index < 0 || index >= this->points.size())
    {
        throw("out of range");
        return Vector2();
    }

    Vector2 returned = Vector2(point.x, point.y);
    std::swap(returned, this->points[index]);
    if (points.size() > 1)
    {
        calcNormals(); // Recalculate normals
    }

    return returned;
}
