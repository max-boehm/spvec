#ifndef _TRACER_H_
#define _TRACER_H_

#include "bitmap.h"
#include "path.h"


class tracer
{
private:
    const bitmap& map;
    
    // state
    bitmap bb;              // traced contours are marked in bb
    int    posx, posy;
    bool   color;

public:
    tracer(const bitmap& map) : map(map) { init(); }

    void init();
    bool get_next_contour(int& x, int& y);
    void trace_points(int x, int y, bool middle_points, path& p);
};

#endif
