#include "tracer.h"


void tracer::init()
{
    posx = 0;
    posy = 0;
    color = map.bit_is_set(0, 0);

    // initialize (and clear) bitmap to mark contours already traced
    bb.init(map.get_width(), map.get_height());
}


bool tracer::get_next_contour(int& x, int& y)
{
    while (posy < map.get_height())
    {
        if (color)
            posx = map.next_bit_clr(posx, posy);
        else
            posx = map.next_bit_set(posx, posy);

        if (posx < map.get_width())
        {
            color = !color;

            if (bb.bit_is_set(posx, posy)==false)
            {
                // contour has not yet been traced
                x = posx;
                y = posy;
                return true;
            }
        }
        else
        {
            // next row
            if (++posy >= map.get_height())
                break;
            color = map.bit_is_set(0, posy);
            posx = 0;
        }
    }

    return false;
}


void tracer::trace_points(int x, int y, bool middle_points, path& p)
{
    // The contour is traced clockwise starting at (x,y). The direction to
    // follow is determined by sampling 2x2 bits at the current position.
    // The sampled bits are interpreted as a 4 bit integer value:
    //
    // index = 8*B[x-1,y-1] + 4*B[x,y-1] + 2*B[x-1,y] + B[x,y]
    //
    // +---------+-------+
    // |(x-1,y-1)|(x,y-1)|           x' = x + dx[index]
    // +---------O-------+           y' = y + dy[index]
    // | (x-1,y) | (x,y) |
    // +---------+-------+
    //
    //                 index:   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    //
    //       2x2 pixel block:  00 00 00 00 01 01 01 01 10 10 10 10 11 11 11 11
    //                         00 01 10 11 00 01 10 11 00 01 10 11 00 01 10 11
    //
    //   direction to follow:   -  r  d  r  u  u  ?  u  l  ?  d  r  l  l  d  -
    //
    static char dx[16] =      { 0, 1, 0, 1, 0, 0, 0, 0,-1, 0, 0, 1,-1,-1, 0, 0};
    static char dy[16] =      { 0, 0, 1, 0,-1,-1, 0,-1, 0, 0, 1, 0, 0, 0, 1, 0};
    static char second[16] =  { 0, 4, 1, 4, 8, 8, 0, 8, 2, 0, 1, 4, 2, 2, 1, 0};
    
    int sx = x;
    int sy = y;
    int last = 2;       // previous direction (down)

    assert((x==0 && map.bit_is_set(x, y)) || (x>0 && map.bit_is_set(x-1, y)!=map.bit_is_set(x, y)));

    p.clear();

    p.push_back(node(x, y));
    
    do
    {
        // index = 8*B[x-1,y-1] + 4*B[x,y-1] + 2*B[x-1,y] + B[x,y]
        int index = map.get_point4(x, y);
        
        if (x<bb.get_width() && y<bb.get_height())
            bb.set_bit(x,y);
        
        // special pattern?
        if (index==6 || index==9)
            index = second[last];
        
        if (middle_points)
            p.push_back(node(x + 0.5 * dx[index], y + 0.5 * dy[index]));
        
        x += dx[index];
        y += dy[index];
        last = index;
        
        p.push_back(node(x, y));
    }
    while (x!=sx || y!=sy);
}

