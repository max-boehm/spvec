/*
 * Shortest Path based Raster to Vector Conversion
 * -----------------------------------------------
 *
 * This software belongs to the publication:
 *
 * M. Böhm:
 * Fast Raster to Vector Conversion based on Optimization and Heuristics,
 * proc. International Conference on Advances in the Internet, Processing,
 * Systems, and Interdisciplinary Research IPSI 2003, Sveti Stefan,
 * Montenegro, 2003.
 *
 * See: http://www.research-net.de/boehm/vec
 *
 * Copyright (c) Max Böhm, boehm@research-net.de, 2003
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "parameter.h"
#include "bitmap.h"
#include "tracer.h"
#include "path.h"
#include "svg.h"
#include "sp_lines.h"
#include "sp_bezier.h"

#include "bezier.h"
#include "area.h"


// return the area between polylines p and q
static bool calc_total_area(const path& p, const path& q, double max_dist, double& area)
{
    point* help = new point[q.size()];
    
    for (int i=0; i<(int)q.size(); i++)
        help[i] = q[i];

    // calculate area
    bool ret = calc_area(&p[0], p.size(), help, q.size(), max_dist*2, area);
    
    delete[] help;

    return ret;
}


int main(int argc, char** argv)
{
    const char* filename_png = "polygon.png";
    const char* filename_svg = "out.svg";
    parameter par;
    bitmap map;
    int rep = 1;          // repetitions for accurate time measurement

    if (!par.load("spvec.par"))
        par.save("spvec.par");

    // parse parameters
    for (int i=1; i<argc; i++)
    {
        if (strstr(argv[i],".png")!=NULL || strstr(argv[i],".PNG")!=NULL)
            filename_png = argv[i];
        else if (strstr(argv[i],".svg")!=NULL || strstr(argv[i],".SVG")!=NULL)
            filename_svg = argv[i];
        else
            par.parse(argv[i]);
    }

    int ret = map.init_from_png(filename_png);
    if (ret!=0)
        return ret;
    
    tracer t(map);

    svg s;
    s.open(filename_svg);
    s.write_header(map.get_width(), map.get_height());
    s.write_image(map.get_width(), map.get_height(), filename_png);
    
    long c1 = clock();
    
    long t1=0, t2=0;
    int  n=0, n1=0, n2=0, n3=0;
    double a1=0, a2=0;
	
    int x, y;
    while (t.get_next_contour(x, y))
    {
        //printf("contour found: %d %d\n", x, y);
        
        path p, l, l2, b;
        double cost;
        
        t.trace_points(x, y, par.tr_middle_points!=0, p);
        
        //printf("#points = %d\n", p.size());
        n += p.size() - 1;

        if (par.svg_points)
            s.write_path(p, "blue", 0.1F, SVG_LINES|SVG_MARKER);
            // s.write_path(p, "#B2B2B2", 0.1F, SVG_LINES|SVG_FILL);

        long c2 = clock();

        for (int i=0; i<rep; i++)
        {
            sp_lines spl(par, p);
            spl.calculate();
            cost = spl.extract(l);
        }

        if (par.svg_lines1)
        {
            // s.write_tree(p);
            s.write_path(l, "red", 0.1F, SVG_LINES|SVG_MARKER);
            // s.write_path(l, "red", 0.1F, SVG_LINES|SVG_MARKER|SVG_TEXT);
        }

        t1 += clock() - c2;

        double area;
        bool ret = calc_total_area(p, l, par.l_max_distance, area);

        double area1;
        bool ret1 = calc_total_area(l, p, par.l_max_distance, area1);

        //printf("#lines=%d   %5.2f ms   area=%5.2f  | ret=%d area=%5.2f | (%d %5.2f)\n",
        //    l.size()-1, (double)(clock()-c2)/rep,
        //    cost - (l.size()-1)*par.l_cost_segment, ret, area, ret1, area1);

        n1 += l.size()-1;
        a1 += cost - (l.size()-1)*par.l_cost_segment;

        c2 = clock();

        for (int i=0; i<rep; i++)
        {
            intermediate_points(l, l2, par.b_corner_angle);

            if (par.svg_lines2)
                s.write_path(l2, "blue", 0.1F, SVG_LINES|SVG_MARKER);

            sp_bezier spb(par, l2);
            spb.calculate();
            cost = spb.extract(b);
        }

        t2 += clock() - c2;

        if (par.svg_curves)
        {
            s.write_path(b, "green", 0.3F, SVG_CURVES);
            //s.write_path(b, "red", 0.3F, SVG_LINES|SVG_MARKER|SVG_TEXT);
            s.write_path(b, "red", 0.3F, SVG_LINES|SVG_MARKER);
            if (par.svg_control)
                s.write_control_points(b);
        }

        // count bezier curve segments
        int bezier = 0;
        for (int i=1; i<(int)b.size(); i++)
            if (b[i].flag & BEZIER)
                bezier++;

        //printf("#curves=%d line=%d   %5.2f ms   area=%5.2f\n", bezier, b.size()-bezier-1, (double)(clock()-c2)/rep,
        //    cost - (b.size()-bezier-1)*par.b_cost_segment - (bezier)*par.b_cost_curve);

        n2 += bezier;
        n3 += b.size()-bezier-1;
        a2 += cost - (b.size()-bezier-1)*par.b_cost_segment - bezier*par.b_cost_curve;
    }

    printf("%s %d | %d %.0f %.0fms | %d %d %.0f %.1fms\n",
        filename_png,
        n,
        n1, a1, (double)t1 * (1000.0 / CLOCKS_PER_SEC) / rep,
        n2, n3, a2, (double)t2 * (1000.0 / CLOCKS_PER_SEC) / rep);

    // printf("Zeit: %.3f s\n", double(clock()-c1)/CLOCKS_PER_SEC);
    
    s.write_end();
    s.close();
    
    return 0;
}
