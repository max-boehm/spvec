#ifndef _SVG_H_
#define _SVG_H_

#include <stdio.h>

#include "path.h"


// flags used in write_path()
#define SVG_LINES       1
#define SVG_CURVES      2
#define SVG_FILL        4
#define SVG_MARKER      8
#define SVG_TEXT       16


class svg
{
private:
    FILE* f;

public:
    svg() : f(NULL) {}

    bool open(const char* filename);
    bool close();

    void write_header(int w, int h);
    void write_image(int w, int h, const char* filename);
    void write_bezier(point* b, const char* color);
    void write_path(const path& p, const char* color, double stroke_width, int flags);
    void write_control_points(const path& p);
    void write_tree(const path& p);
    void write_end();
};

#endif
