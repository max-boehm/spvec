#include <math.h>
#include <assert.h>

#include "sp_bezier.h"

#include "bezier.h"
#include "area.h"



// insert intermediate points and set flags (CORNER, MIDDLE)
bool intermediate_points(const path& p, path& q, double corner_angle)
{
    q.clear();

    int last = p.size()-1;

    // p must have at least 2 points
    if (last <= 0)
        return false;

    // p must be a closed path
    if (p[0].x!=p[last].x || p[0].y!=p[last].y)
        return false;

    double cos_corner = cos( (180-corner_angle) * (3.1415926/180.0) );

    //       m2,len2
    // p[0] ---------> p[1]
    point m2   = p[1] - p[0];
    double len2 = m2.len();
    double pos = 0;
    
    for (int i=1; i<(int)p.size(); i++)
    {
        point m1   = m2;
        double len1 = len2;

        if (i==last)
            m2 = p[1] - p[0];
        else
            m2 = p[i+1] - p[i];
        len2 = m2.len();

        // accumulate length
        pos += len1;

        //         m1,len1   pos   m2,len2
        // p[i-1] ---------> p[i] ---------> p[i+1]

        // p[i-1] ----+---- p[i] --------- p[i+1]
        q.push_back(node(p[i]-m1*0.5, MIDDLE, pos-len1*0.5));

        if (len1 > 2*len2)
        {
            double a = 0.5*len2/len1;
            // p[i-1] ------------+-- p[i] ----- p[i+1]
            q.push_back(node(p[i]-m1*a, MIDDLE, pos-len2*0.5));
        }

        // p[i]
        q.push_back(node(p[i], cos_alpha(m1, m2)>cos_corner ? 0 : CORNER, pos));
        
        if (len2 > 2*len1)
        {
            double a = 0.5*len1/len2;
            // p[i-1] ---- p[i] --+------------ p[i+1]
            q.push_back(node(p[i]+m2*a, MIDDLE, pos+len1*0.5));
        }
    }

    q.push_back(node(q[0], MIDDLE, pos+q[0].pos));

    return true;
}


// solve linear equation ax+by=c, dx+ey=f by Cramer's Rule
static bool linear_equation(double a, double b, double c, double d, double e, double f, double& x, double& y)
{
    double detx = c*e - f*b;
    double dety = a*f - d*c;
    double det  = a*e - d*b;
    
    if (det == 0)
        return false;

    x = detx / det;
    y = dety / det;

    return true;
}


static bool calc_b12(point* b, point m1, point m2, point pt, point mt)
{
    double s, t;

    double r1 = cross(m1, mt);
    double r2 = cross(m2, mt);

    point help = 4*pt - b[0] - b[3];

    if (linear_equation(2*r1, r2, cross(help-2*b[0], mt), r1, 2*r2, cross(help-2*b[3], mt), s, t)==false)
        return false;

    b[1] = b[0] + s*m1;
    b[2] = b[3] + t*m2;

    return true;
}


/**
 * Fit a Bézier curve into the path p[i]...p[j].
 *
 * Heuristik: fit curve such that
 * - segment (p[i],p[i+1]) is the start tangent
 * - segment (p[j-1],p[j]) is the end tangent
 * - line through point p with direction m is tangent to the curve at t=0.5
 *
 * 3..4 candidates (p,m) in the middle of the polygon are tested.
 *
 * Constraints:
 *
 */
bool sp_bezier::fit_bezier(int i, int j, double& area, point* xy)
{
    point b[4];

    assert(i+1<j);  // at least one intermediate point

    area = (double)1e20;

    b[0] = p[i];
    b[3] = p[j];

    point m1 = p[i+1]-p[i];
    point m2 = p[j-1]-p[j];

    // constraint: angle(m1,seg)<=90° and angle(m2,-seg)<=90°
    point seg = p[j]-p[i];
    if (dot(m1, seg)<0 || dot(m2, seg)>0)
        return false;

    // squared limit for distance of control points
    double max_len2 = p[j].pos - p[i].pos;
    max_len2 *= max_len2;
    double min_len2 = max_len2 * 0.01;

    int cnt = j-i-1;            // number of intermediate points
    if (cnt > 4)
        cnt = 4-(cnt&1);        // at most 4 candidate points
    
    // loop through candidate points p[k]
    for (int k=i+(j-i+1-cnt)/2; cnt--; k++)
    {
        assert(k>i && k<j);

        // the line pt+r*mt shall be the tangent of the curve at b(t=0.5)
        point pt = p[k]*(2/3.0F)+p[k+1]*(1/3.0F);
        point mt = p[k+1]-p[k-1];

        /*
        // dist(line, p[k]) > b_max_distance?
        // -> shift line, such that dist(..) = b_max_distance * 0.8
        double len = mt.len();
        double d = cross(mt, p[k]-pt)/len;
        if (d > par.b_max_distance * 0.8)
            pt = p[k] - perp(mt) * (par.b_max_distance/len) * 0.8;
        else if (d < -par.b_max_distance)
            pt = p[k] + perp(mt) * (par.b_max_distance/len) * 0.8;
        */
        
        // calculate control points
        if (calc_b12(b, m1, m2, pt, mt)==false)
            continue;

        point bm1 = b[1]-b[0];
        point bm2 = b[2]-b[3];

        // constraint: direction of control points ok?
        if (dot(bm1,m1)<0 || dot(bm2,m2)<0)
            continue;

        // constraint: distance of control points within limit?
        double len2 = bm1.len2();
        if (len2>max_len2 || len2<min_len2)
            continue;
        len2 = bm2.len2();
        if (len2>max_len2 || len2<min_len2)
            continue;
        //if (bm1.len2()>max_len2 || bm2.len2()>max_len2)
        //    continue;

        point bp[16+1];
        bezier_points(b, bp, 16);

        double a;
        // constraint: maximal distance of curve to polyline ok?
        if (calc_area(&p[i], j-i+1, bp, 17, par.b_max_distance, a)==false)
            continue;

        // is better?
        if (a < area)
        {
            area = a;
            xy[0] = b[1];
            xy[1] = b[2];
        }
    }

    return area < (double)1e20;
}


bool sp_bezier::cost(int i, int j, double& cost)
{
    int not_middle = 0;
    
    for (int k=i+1; k<j; k++)
        if ((p[k].flag&MIDDLE)==0)
            not_middle++;

    if (not_middle==0)              // straight line
    {
        is_bezier = false;
        cost = par.b_cost_segment;  // cost per segment
        //printf("i=%d j=%d line cost=%f\n", i, j, cost);
        
        p[j].in_deg--;              // do not count straight lines
        
        return true;
    }

    assert(not_middle>0);

    // try to fit a curve only, if start and end points are CORNER or MIDDLE
    if ((p[i].flag&(CORNER|MIDDLE)) && (p[j].flag&(CORNER|MIDDLE)))
    {
        double area;
        bool ret = false;

        if (par.b_fit_heuristic==1)             // tangent heuristic
        {
            ret = fit_bezier(i, j, area, xy);
        }

        if (ret==false)
        {
            //printf("i=%d j=%d -> no curve fits\n", i, j);
            return false;
        }

        is_bezier = true;
        cost = par.b_cost_curve + par.b_cost_area * area;
        //printf("i=%d j=%d curve cost=%f\n", i, j, cost);
        return true;
    }

    //printf("i=%d j=%d -> not tried\n", i, j);
    // no success
    return false;
}


void sp_bezier::update(int i)
{
    if (is_bezier)
    {
        p[i].flag |= BEZIER;
        p[i].xy[0] = xy[0];
        p[i].xy[1] = xy[1];
    }
}

