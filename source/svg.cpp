#include <assert.h>

#include "svg.h"


bool svg::open(const char* filename)
{
    if (f!=NULL)
        fclose(f);

    f = fopen(filename, "w");
    
    return f!=NULL;
}


bool svg::close()
{
    if (f==NULL)
        return false;
    
    int ret = fclose(f);
    f = NULL;

    return ret==0;
}


void svg::write_header(int w, int h)
{
    if (f==NULL)
        return;
    
    fprintf(f, "<?xml version=\"1.0\" standalone=\"yes\"?>\n");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"%d\" height=\"%d\">\n", w, h);
    fprintf(f, "<g id=\"all\">\n");

    // marker definition
    fprintf(f,
        "<defs>\n"

        "<marker id=\"blue\" "
        "viewBox=\"0 0 10 10\" refX=\"5\" refY=\"5\" "
        "markerUnits=\"strokeWidth\" "
        "markerWidth=\"3\" markerHeight=\"3\">\n"
        "<path fill=\"blue\" stroke=\"none\" d=\"M 0 0 L 10 0 L 10 10 L 0 10 z\" />\n"
        "</marker>\n"
        
        "<marker id=\"red\" "
        "viewBox=\"0 0 10 10\" refX=\"5\" refY=\"5\" "
        "markerUnits=\"userSpaceOnUse\" "
        "markerWidth=\"0.5\" markerHeight=\"0.5\">\n"
        "<path fill=\"red\" stroke=\"none\" d=\"M 0 0 L 10 0 L 10 10 L 0 10 z\" />\n"
        "</marker>\n"
        
        "<marker id=\"control\" "
        "viewBox=\"0 0 10 10\" refX=\"5\" refY=\"5\" "
        "markerUnits=\"userSpaceOnUse\" "
        "markerWidth=\"1\" markerHeight=\"1\">\n"
        "<path stroke=\"green\" stroke-width=\"1\" fill=\"none\" d=\"M 5 0 L 5 10 M 0 5 L 10 5 z\" />\n"
        "</marker>\n"
        
        "</defs>\n");
}


void svg::write_image(int w, int h, const char* filename)
{
    if (f==NULL)
        return;
    
    fprintf(f,
        "<image x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" xlink:href=\"%s\" "
        "style=\"image-rendering: crisp-edges;\" />\n", w, h, filename);
}


void svg::write_bezier(point* b, const char* color)
{
    fprintf(f, "<path style=\"fill:none; stroke:%s; stroke-width:0.05\" ", color);

    fprintf(f, "d=\"M%.1f %.1f C%.1f %.1f %.1f %.1f %.1f %.1f\" />\n",
        b[0].x, b[0].y, b[1].x, b[1].y, b[2].x, b[2].y, b[3].x, b[3].y);
}


void svg::write_path(const path& p, const char* color, double stroke_width, int flags)
{
    if (f==NULL || p.empty())
        return;
    
    if (flags&SVG_FILL)
        fprintf(f, "<path style=\"fill:%s; stroke:none\" ", color);
    else
        fprintf(f, "<path style=\"fill:none; stroke:%s; stroke-width:%.1f\" ", color, stroke_width);

    if (flags&SVG_MARKER)
        fprintf(f, "marker-mid=\"url(#%s)\" marker-start=\"url(#%s)\" marker-end=\"url(#%s)\" ",
        color, color, color);

    fprintf(f, "d=\"M%.1f %.1f ", p[0].x, p[0].y);

    for (path::const_iterator pi=p.begin(); ++pi!=p.end(); )
    {
        if ((pi->flag&BEZIER) && (flags&SVG_CURVES))
            fprintf(f, "C%.1f %.1f %.1f %.1f %.1f %.1f ",
            pi->xy[0].x, pi->xy[0].y, pi->xy[1].x, pi->xy[1].y, pi->x, pi->y);
        else if ((pi->flag&BEZIER)==0 && (flags&SVG_LINES))
            fprintf(f, "L%.1f %.1f ", pi->x, pi->y);
        else
            fprintf(f, "M%.1f %.1f ", pi->x, pi->y);
    }

    fprintf(f, "\" />\n");

    if (flags&SVG_TEXT)
    {
        int i;
        int b_cnt=0;
        for (i=1; i<(int)p.size(); i++)
        {
            //fprintf(f, "<text x=\"%.1f\" y=\"%.1f\" font-size=\"2\">%.1f (%d,%d)</text>\n",
            //p[i].x+1, p[i].y, p[i].cost - p[i-1].cost, p[i].in_deg, p[i].flag);
            fprintf(f, "<text x=\"%.1f\" y=\"%.1f\" font-size=\"1.5\">%.1f</text>\n",
            p[i].x+0.4, p[i].y-0.7, p[i].cost - p[i-1].cost);

            if (p[i].flag&BEZIER)
                b_cnt++;
        }

        i--;
        
        fprintf(f, "<text x=\"%.1f\" y=\"%.1f\" font-size=\"3\">%.1f #%d,%d</text>\n",
            p[i].x, p[i].y-4, p[i].cost, b_cnt, (int)p.size()-1-b_cnt);
        
    }
}


void svg::write_control_points(const path& p)
{
    if (f==NULL || p.empty())
        return;
    
    fprintf(f,
        "<g style=\"fill:none; stroke:green; stroke-width:0.05; stroke-dasharray:0.5,0.5\" "
        "marker-end=\"url(#control)\">\n");

    for (int i=1; i<(int)p.size(); i++)
    {
        const point* xy = p[i].xy;
        if (p[i].flag&BEZIER)
        {
            fprintf(f, "<path d=\"M%.1f %.1f L%.1f %.1f\" />\n", p[i-1].x, p[i-1].y, xy[0].x, xy[0].y);
            fprintf(f, "<path d=\"M%.1f %.1f L%.1f %.1f\" />\n", p[i].x, p[i].y, xy[1].x, xy[1].y);
        }
    }

    fprintf(f, "</g>\n");
}


void svg::write_tree(const path& p)
{
    if (f==NULL || p.empty())
        return;
    
    fprintf(f, "<path style=\"fill:none; stroke:blue; stroke-width:0.1\" ");

    fprintf(f, "marker-mid=\"url(#blue)\" marker-start=\"url(#blue)\" marker-end=\"url(#blue)\" d=\"");

    for (int i=p.size()-1; i>0; i--)
    {
        int j = p[i].pred;
        
        assert(j>=0);

        fprintf(f, "M%.1f %.1f L%.1f %.1f ", p[j].x, p[j].y, p[i].x, p[i].y);
    }

    fprintf(f, "\" />\n");
}


void svg::write_end()
{
    if (f==NULL)
        return;
    
    fprintf(f, "</g>\n</svg>\n");
}
