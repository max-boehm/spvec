#include <stdio.h>

#include "parameter.h"


// Constructor: initialize parameters
parameter::parameter()
{
    tr_middle_points = 1;
    sp_depth_limit = 500;
    sp_missed_limit = 10;

    l_max_distance = 1;
    l_cost_segment = 10;
    l_cost_distance = 0;
    l_cost_area = 1;

    b_fit_heuristic = 1;
    b_corner_angle = 90;
    b_max_distance = 1.1;
    b_cost_curve = 20;
    b_cost_area = 1;
    b_cost_segment = 40;

    svg_points = 0;
    svg_lines1 = 0;
    svg_lines2 = 1;
    svg_curves = 1;
    svg_control = 1;
}


// parse string "name=value" and set parameter.
bool parameter::parse(const char* str)
{
    return
        sscanf(str, "tr_middle_points=%d", &tr_middle_points)==1 ||
        sscanf(str, "sp_depth_limit=%d", &sp_depth_limit)==1 ||
        sscanf(str, "sp_missed_limit=%d", &sp_missed_limit)==1 ||
        sscanf(str, "l_max_distance=%lf", &l_max_distance)==1 ||
        sscanf(str, "l_cost_segment=%lf", &l_cost_segment)==1 ||
        sscanf(str, "l_cost_distance=%lf", &l_cost_distance)==1 ||
        sscanf(str, "l_cost_area=%lf", &l_cost_area)==1 ||
        sscanf(str, "b_fit_heuristic=%d", &b_fit_heuristic)==1 ||
        sscanf(str, "b_corner_angle=%lf", &b_corner_angle)==1 ||
        sscanf(str, "b_max_distance=%lf", &b_max_distance)==1 ||
        sscanf(str, "b_cost_curve=%lf", &b_cost_curve)==1 ||
        sscanf(str, "b_cost_area=%lf", &b_cost_area)==1 ||
        sscanf(str, "b_cost_segment=%lf", &b_cost_segment)==1 ||
        sscanf(str, "svg_points=%d", &svg_points)==1 ||
        sscanf(str, "svg_lines1=%d", &svg_lines1)==1 ||
        sscanf(str, "svg_lines2=%d", &svg_lines2)==1 ||
        sscanf(str, "svg_curves=%d", &svg_curves)==1 ||
        sscanf(str, "svg_control=%d", &svg_control)==1;
}


bool parameter::load(const char* filename)
{
    char line[100];
    bool flag = true;
    
    FILE* f = fopen(filename, "r");
    if (f==NULL)
        return false;

    while (fgets(line, sizeof(line), f)!=NULL)
    {
        if (line[0]=='#')
            continue;

        if (parse(line)==false)
        {
            fprintf(stderr, "can't parse: %s", line);
            flag = false;
        }
    }

    fclose(f);
    return flag;
}


bool parameter::save(const char* filename) const
{
    FILE* f = fopen(filename, "w");
    if (f==NULL)
        return false;

    fprintf(f, "tr_middle_points=%d\n", tr_middle_points);
    fprintf(f, "sp_depth_limit=%d\n", sp_depth_limit);
    fprintf(f, "sp_missed_limit=%d\n", sp_missed_limit);
    fprintf(f, "l_max_distance=%f\n", l_max_distance);
    fprintf(f, "l_cost_segment=%f\n", l_cost_segment);
    fprintf(f, "l_cost_distance=%f\n", l_cost_distance);
    fprintf(f, "l_cost_area=%f\n", l_cost_area);
    fprintf(f, "b_fit_heuristic=%d\n", b_fit_heuristic);
    fprintf(f, "b_corner_angle=%f\n", b_corner_angle);
    fprintf(f, "b_max_distance=%f\n", b_max_distance);
    fprintf(f, "b_cost_curve=%f\n", b_cost_curve);
    fprintf(f, "b_cost_area=%f\n", b_cost_area);
    fprintf(f, "b_cost_segment=%f\n", b_cost_segment);
    fprintf(f, "svg_points=%d\n", svg_points);
    fprintf(f, "svg_lines1=%d\n", svg_lines1);
    fprintf(f, "svg_lines2=%d\n", svg_lines2);
    fprintf(f, "svg_curves=%d\n", svg_curves);
    fprintf(f, "svg_control=%d\n", svg_control);

    return fclose(f)==0;
}
