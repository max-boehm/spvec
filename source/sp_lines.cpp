#include <math.h>
#include <assert.h>

#include "sp_lines.h"


bool sp_lines::cost(int i, int j, double& cost)
{
    assert(i<j);

    point& p1 = p[i];
    point& p2 = p[j];
    point p21 = p2 - p1;
    
    // normal vector, perpendicular to segment (p1,p2)
    point n = perp(p21);

    // len = length(n)
    double len = n.len();
    
    // limit = len(n)*par.l_max_distance
    double limit = len * par.l_max_distance;

    // for bounding box
    double limit2 = len*len + limit;
    
    double sum = 0;
    int   cnt = 1;

    // loop through the intermediate points of the contour
    for (int k=i+1; k<j; k++)
    {
        point pk1 = p[k] - p1;

        // d = dist(p[k],segment) * length(n)
        double d = dot(n, pk1);
        if (d<0)
            d = -d;

        // d > par.l_max_distance ?
        if (d > limit)
            return false;

        sum += d;
        cnt++;

        // check bounding box
        // if (dot(p21, pk1) < -limit || dot(p21, p[k]-p2) > limit)
        //     return false;

        // check bounding box
        d = dot(p21, pk1);
        if ((d < -limit) || d > limit2)
            return false;
    }

    // approximate the average distance of contour points to segment
    double dist = sum / (cnt * len);
    // double dist = sum / cnt;
    
    // calculate the cost per segment: cost = k1 + dist * k2
    cost = par.l_cost_segment + par.l_cost_distance * dist + par.l_cost_area * (sum/cnt);

    return true;
}

