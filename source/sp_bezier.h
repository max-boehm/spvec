#ifndef _SP_BEZIER_H_
#define _SP_BEZIER_H_

#include "shortest_path.h"


class sp_bezier : public shortest_path
{
private:
    // for remembering the results of fit_bezier()
    bool  is_bezier;
    point xy[2];

public:
    sp_bezier(const parameter& par, path& p) : shortest_path(par, p) {}

    virtual bool cost(int i, int j, double& cost);
    virtual void update(int i);

    bool fit_bezier(int i, int j, double& area, point* xy);
    bool fit_bezier2(int i, int j, double& area, point* xy);
};


bool intermediate_points(const path& p, path& q, double corner_angle);

#endif
