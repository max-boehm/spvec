#ifndef _PARAMETER_H_
#define _PARAMETER_H_


class parameter
{
public:                         // attributes are public!
    int    tr_middle_points;    // insert points in the middle
    int    sp_depth_limit;      // j-i <= sp_depth_limit
    int    sp_missed_limit;     // 
    double l_max_distance;
    double l_cost_segment;
    double l_cost_distance;
    double l_cost_area;
    int    b_fit_heuristic;     // 1 = tangent, 2 = least squares
    double b_corner_angle;      // in degrees
    double b_max_distance;
    double b_cost_curve;
    double b_cost_area;
    double b_cost_segment;
    int    svg_points;          // flags
    int    svg_lines1;
    int    svg_lines2;
    int    svg_curves;
    int    svg_control;

public:
    parameter();
    bool parse(const char* str);
    bool load(const char* filename);
    bool save(const char* filename) const;
};

#endif
