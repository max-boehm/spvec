#include <algorithm>

using namespace std;

#include "shortest_path.h"


const int NIL = -1;

// used to represent infinity
#ifndef INFINITY
const double INFINITY = (double)1e20;
#endif



bool shortest_path::calculate()
{
    // initialize start node
    p[0].cost = 0;
    p[0].pred = NIL;
    p[0].in_deg = 0;

    for (int j=1; j<(int)p.size(); j++)
    {
        int missed = 0;         // count successive missing edges

        // initialize node
        p[j].cost = INFINITY;
        p[j].pred = NIL;
        p[j].in_deg = 0;

        for (int i=j-1; i>=0; i--)
        {
            double c;

            // check edge (i,j)

            if (i < j-par.sp_depth_limit)
                break;                  // again ?!

            // calculate the cost of this edge
            if (cost(i, j, c) == false)
            {
                // edge (i,j) does not exist
                if (++missed >= par.sp_missed_limit)
                    break;
            }
            else
            {
                missed = 0;

                p[j].in_deg++;

                // does edge (i,j) belong to the shortest path tree?
                if (p[i].cost + c < p[j].cost)
                {
                    p[j].cost = p[i].cost + c;
                    p[j].pred = i;

                    update(j);
                }
            }
        }
    }

    return true;
}


double shortest_path::extract(path& q) const
{
    q.clear();

    int i = p.size()-1;

    while (i != NIL)
    {
        q.push_back(p[i]);
        i = p[i].pred;
    }

    double cost = q.empty() ? 0 : q[0].cost;

    reverse(q.begin(), q.end());

    return cost;
}
