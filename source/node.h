#ifndef _NODE_H_
#define _NODE_H_

#include "point.h"


#define CORNER 1        // point is allowed to become a corner
#define MIDDLE 2        // intermediate points on line segments
#define BEZIER 4        // is a bezier curve element


class node : public point
{
public:
    // double x;        // inherited
    // double y;        // inherited
    double cost;        // minimal cost
    int   pred;         // list of predecessors (used in shortest_path)
    int   in_deg;       // in degree
    
    int   flag;         // CORNER, MIDDLE, BEZIER
    double pos;         // accumulated lengths
    point xy[2];        // control points (BEZIER)

public:
    node() {}
    node(double x, double y) : point(x,y), flag(0) {}
    node(point p, int flag, double pos) : point(p.x,p.y), flag(flag), pos(pos) {}
};

#endif
