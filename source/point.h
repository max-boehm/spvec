#ifndef _POINT_H_
#define _POINT_H_

#include <math.h>


class point
{
public:
    double x;
    double y;

    point() {}

    point(double x, double y)
    {
        this->x = x;
        this->y = y;
    }

    // vector addition
    point operator+(point p) const
    {
        return point(x+p.x, y+p.y);
    }

    // vector difference
    point operator-(point p) const
    {
        return point(x-p.x, y-p.y);
    }

    // length squared
    double len2() const
    {
        return x*x+y*y;
    }

    // length
    double len() const
    {
        return sqrt(x*x+y*y);
    }

    bool operator==(const point& p) const
    {
        return x==p.x && y==p.y;
    }
};


// scalar multiplication
inline point operator*(double a, point p)
{
    return point(a*p.x, a*p.y);
}

inline point operator*(point p, double a)
{
    return point(a*p.x, a*p.y);
}



// inner dot product
inline double dot(point a, point b)
{
    return a.x*b.x + a.y*b.y;
}

// perpendicular product
inline double cross(point a, point b)
{
    return a.x*b.y - a.y*b.x;
}

// normal (perpendicular) vector
inline point perp(point p)
{
    return point(-p.y, p.x);
}

// cosine of angle between a and b
inline double cos_alpha(point a, point b)
{
    return dot(a, b) / sqrt(a.len2()*b.len2());
}


#endif
