#ifndef _SHORTEST_PATH_H_
#define _SHORTEST_PATH_H_


#include "parameter.h"
#include "path.h"


/**
 * Single-source shortest path algorithm in a directed acyclic graph (DAG).
 * (abstract base class).
 *
 * The path p[0]..p[n] define a topological sort of a DAG.
 *
 * Edges (i,j) from p[i] to p[j] exist, if
 * a) j-par.sp_depth_limit <= i < j
 * b) cost(i,j)==true
 * c) more than par.sp_missed_limit successive edges between i and j do not exist.
 *
 * The nodes are processed in sequential order. For each node j its
 * predecessors i are processed backwards until condition c) is reached.
 * cost(i,j) is calculated. If p[i].cost+cost(i,j)<p[j].cost then p[j].cost
 * is relaxed, the pointer p[j].back is set to p[i] and update(p[j]) is called.
 *
 */
class shortest_path
{
protected:
    const parameter& par;       // parameters
    path& p;                    // topological sort of the DAG

public:
    shortest_path(const parameter& par, path& p) : par(par), p(p) {}

    bool calculate();
    double extract(path& q) const;

    virtual bool cost(int i, int j, double& c) = 0;
    virtual void update(int i) = 0;
};

#endif
