#ifndef _SP_LINES_H_
#define _SP_LINES_H_

#include "shortest_path.h"


class sp_lines : public shortest_path
{
public:
    sp_lines(const parameter& par, path& p) : shortest_path(par, p) {}

    virtual bool cost(int i, int j, double& cost);
    virtual void update(int i) {}
};

#endif
