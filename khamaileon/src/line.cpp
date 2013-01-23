/*********************************************************************************
 *
 * This file is part of eSRO.
 *
 * eSRO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * eSRO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright ⓒ 2013  Rafael Dominguez (npcdoom)
 *
 *********************************************************************************/

#include "line.hpp"
#include <cassert>
#include <cmath>

////////////////////////////////////////////////////////////
// vector2 class
//

// Constructor with initializing double values
vector2::vector2(double inX, double inY)
{
  x = inX;
  y = inY;
}

// Constructor with initializing vector2
vector2::vector2(const vector2 &v)
{
  x = v.x;
  y = v.y;
}

// Array indexing
double &vector2::operator [] (unsigned int i)
{
  assert (i<2);

  return *(&x+i);
}

// Array indexing
const double &vector2::operator [] (unsigned int i) const
{
  assert (i<2);

  return *(&x+i);
}

// Assign
vector2 &vector2::operator = (const vector2 &v)
{
  x = v.x;
  y = v.y;

  return *this;
}

// Add a vector2 to this one
vector2 &vector2::operator += (const vector2 &v)
{
  x += v.x;
  y += v.y;

  return *this;
}

// Subtract a vector2 from this one
vector2 &vector2::operator -= (const vector2 &v)
{
  x -= v.x;
  y -= v.y;

  return *this;
}

// Multiply the vector2 by a double
vector2 &vector2::operator *= (double f)
{
  x *= f;
  y *= f;

  return *this;
}

// Divide the vector2 by a double
vector2 &vector2::operator /= (double f)
{
  x /= f;
  y /= f;

  return *this;
}

// Are these two vector2's equal?
bool operator == (const vector2 &a, const vector2 &b)
{
  return ((a.x == b.x) && (a.y == b.y));
}

// Are these two vector2's not equal?
bool operator != (const vector2 &a, const vector2 &b)
{
  return ((a.x != b.x) || (a.y != b.y));
}

// Add two vector2's
vector2 operator + (const vector2 &a, const vector2 &b)
{
  vector2 ret(a);

  ret += b;

  return ret;
}

// Subtract one vector2 from another
vector2 operator - (const vector2 &a, const vector2 &b)
{
  vector2 ret(a);

  ret -= b;

  return ret;
}

// Multiply vector2 by a double
vector2 operator * (const vector2 &v, double f)
{
  return vector2(f * v.x, f * v.y);
}

// Multiply vector2 by a double
vector2 operator * (double f, const vector2 &v)
{
  return vector2(f * v.x, f * v.y);
}

// Divide vector2 by a double
vector2 operator / (const vector2 &v, double f)
{
  return vector2(v.x / f, v.y / f);
}

// Negate a vector2
vector2 operator - (const vector2 &a)
{
  return vector2(-a.x, -a.y);
}

// Get length of a vector2
double vector2::length() const
{
  return (double) sqrt(x*x + y*y);
}

// Get squared length of a vector2
double vector2::lengthSqr() const
{
  return (x*x + y*y);
}

// Does vector2 equal (0, 0)?
bool vector2::isZero() const
{
  return ((x == 0.0F) && (y == 0.0F));
}

// Normalize a vector2
vector2 &vector2::normalize()
{
  double m = length();

  if (m > 0.0F)
    m = 1.0F / m;
  else
    m = 0.0F;
  x *= m;
  y *= m;

  return *this;
}

Line::Line (double X1, double Y1, double Z1, double X2, double Y2, double Z2)
    : x1(X1), y1(Y1), z1(Z1), x2(X2), y2(Y2), z2(Z2)
{
}

bool Line::Contains (const Point &point) const
{
    return x1 <= point.X && x2 >= point.X && y1 <= point.Y && y2 >= point.Y;
}

bool Line::isEndpoint(const Point &point) const
{
    return ((point.X == x1 && point.Y == y1) || (point.X == x2 && point.Y == y2));
}

double Line::Distance(const Point &point) const
{
    if (!m_NormalCalculated)
    {
        ComputeNormal();
    }

    return (point.X-x1)*m_Normal.x+(point.Y-y1)*m_Normal.y;

}

Line::POINT_CLASSIFICATION Line::ClassifyPoint (const Point &point, double Epsilon) const
{
    POINT_CLASSIFICATION Result = ON_LINE;

    double distance = (point.X-x1)*(y2-y1)-(x2-x1)*(point.Y-y1);

    if (distance > Epsilon)
    {
        Result = RIGHT_SIDE;
    }
    else if (distance < -Epsilon)
    {
        Result = LEFT_SIDE;
    }

    return(Result);
}

/*	SegmentIntersection
------------------------------------------------------------------------------------------

    Determines if two segments intersect, and if so the point of intersection. The current
    member line is considered line AB and the incomming parameter is considered line CD for
    the purpose of the utilized equations.

    A = PointA of the member line
    B = PointB of the member line
    C = PointA of the provided line
    D = PointB of the provided line

------------------------------------------------------------------------------------------
*/

Line::LINE_CLASSIFICATION Line::Intersection (const Line &line, Point *pIntersectPoint) const
{
    double Ay_minus_Cy = y1 - line.y1;
    double Dx_minus_Cx = line.x2 - line.x1;
    double Ax_minus_Cx = x1 - line.x1;
    double Dy_minus_Cy = line.y2 - line.y1;
    double Bx_minus_Ax = x2 - x1;
    double By_minus_Ay = y2 - y1;

    double Numerator = (Ay_minus_Cy * Dx_minus_Cx) - (Ax_minus_Cx * Dy_minus_Cy);
    double Denominator = (Bx_minus_Ax * Dy_minus_Cy) - (By_minus_Ay * Dx_minus_Cx);

    // if lines do not intersect, return now
    if (!Denominator)
    {
        if (!Numerator)
        {
            return COLLINEAR;
        }

        return PARALELL;
    }

    double FactorAB = Numerator / Denominator;
    double FactorCD = ((Ay_minus_Cy * Bx_minus_Ax) - (Ax_minus_Cx * By_minus_Ay)) / Denominator;

    // if an interection point was provided, fill it in now
    if (pIntersectPoint)
    {
        pIntersectPoint->X = (x1 + (FactorAB * Bx_minus_Ax));
        pIntersectPoint->Y = (y1 + (FactorAB * By_minus_Ay));
    }

    // now determine the type of intersection
    if ((FactorAB >= 0.0f) && (FactorAB <= 1.0f) && (FactorCD >= 0.0f) && (FactorCD <= 1.0f))
    {
        return SEGMENTS_INTERSECT;
    }
    else if ((FactorCD >= 0.0f) && (FactorCD <= 1.0f))
    {
        return (A_BISECTS_B);
    }
    else if ((FactorAB >= 0.0f) && (FactorAB <= 1.0f))
    {
        return (B_BISECTS_A);
    }

    return LINES_INTERSECT;
}

void Line::ComputeNormal() const
{
    //
    // Get Normailized direction from A to B
    //
    m_Normal = vector2(x2-x1,y2-y1);
    m_Normal.normalize();

    //
    // Rotate by -90 degrees to get normal of line
    //
    double OldYValue = m_Normal[1];
    m_Normal[1] = -m_Normal[0];
    m_Normal[0] = OldYValue;
    m_NormalCalculated = true;

}

