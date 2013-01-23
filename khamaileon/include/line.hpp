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

#ifndef LINE_HPP
#define LINE_HPP

#include <stdint.h>

struct Point
{
    Point (double x = 0, double y = 0)
        : X(x), Y(y)
    {}

    double X;
    double Y;
};

class vector2
{
 public:
  // Members
  double x, y;

 public:
  // Constructors
  vector2() {};
  vector2(double inX, double inY);
  vector2(const vector2 &v);

 public:
  // Operators
  double                 &operator [] (unsigned int index);
  const double           &operator [] (unsigned int index) const;
  vector2               &operator =  (const vector2 &v);
  vector2               &operator += (const vector2 &v);
  vector2               &operator -= (const vector2 &v);
  vector2               &operator *= (double f);
  vector2               &operator /= (double f);
  friend bool           operator == (const vector2 &a, const vector2 &b);
  friend bool           operator != (const vector2 &a, const vector2 &b);
  friend vector2        operator - (const vector2 &a);
  friend vector2        operator + (const vector2 &a, const vector2 &b);
  friend vector2        operator - (const vector2 &a, const vector2 &b);
  friend vector2        operator * (const vector2 &v, double f);
  friend vector2        operator * (double f, const vector2 &v);
  friend vector2        operator / (const vector2 &v, double f);

 public:
  // Methods
  double                 length() const;
  double                 lengthSqr() const;
  bool                  isZero() const;
  vector2               &normalize();
};


struct Line
{
    enum POINT_CLASSIFICATION
    {
        ON_LINE,		// The point is on, or very near, the line
        LEFT_SIDE,		// looking from endpoint A to B, the test point is on the left
        RIGHT_SIDE		// looking from endpoint A to B, the test point is on the right
    };

    enum LINE_CLASSIFICATION
    {
        COLLINEAR,			// both lines are parallel and overlap each other
        LINES_INTERSECT,	// lines intersect, but their segments do not
        SEGMENTS_INTERSECT,	// both line segments bisect each other
        A_BISECTS_B,		// line segment B is crossed by line A
        B_BISECTS_A,		// line segment A is crossed by line B
        PARALELL			// the lines are paralell
    };

    Line () {}

    Line (double X1, double Y1, double Z1, double X2, double Y2, double Z2);

    bool Contains (const Point &point) const;

    bool isEndpoint (const Point &point) const;

    double Distance (const Point &point) const;

    POINT_CLASSIFICATION ClassifyPoint (const Point &point, double Epsilon = 0.0f) const;

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

    LINE_CLASSIFICATION Intersection (const Line &line, Point *pIntersectPoint= 0) const;

    double x1;
    double y1;
    double z1;
    double x2;
    double y2;
    double z2;

private:

    // ----- HELPER FUNCTIONS -------------
    void ComputeNormal() const;

    
    mutable vector2 m_Normal;	// 'normal' of the ray. A vector pointing to the right-hand side of the line
                                // when viewed from PointA towards PointB
    mutable bool m_NormalCalculated; // normals are only calculated on demand
    
};

#endif // LINE_HPP
