#include "area.h"
#include <stdio.h>
#include <assert.h>


// is the distance between p and the line (p1,p2) less than max_dist?
static bool check_distance(point p1, point p2, point p, double max_dist)
{
    // (p1,p2) rotated counterclockwise by 90 degree
    point n = perp(p2-p1);

    // r = dist(p,(p1,p2))*len(n) */
    double r = dot(n, p-p1);

    // check distance; avoid square root
    return r*r <= n.len2() * max_dist * max_dist;
}


/**
 * Calculates intersection point of (p1,p2) and (p3,p4).
 *
 * @param p1  startpoint of first line segment
 * @param p2  endpoint of first line segment
 * @param p3  startpoint of second line segment
 * @param p4  startpoint of second line segment
 * @param s   the intersection point is returned
 *
 * @return true=OK (output in s), false=no intersection point
 */
static bool cut(point p1, point p2, point p3, point p4, point& s)
{
    point p21 = p2 - p1;
    point p43 = p4 - p3;
    point p31 = p3 - p1;

    double det = p21.y*p43.x - p21.x*p43.y;
    if (det==0)
        return false;

    double det1 = p31.y*p43.x - p31.x*p43.y;
    double det2 = p21.x*p31.y - p21.y*p31.x;

    if (det>0 && (det1<0 || det1>det || det2<0 || det2>det))
        return false;
    if (det<0 && (det1>0 || det1<det || det2>0 || det2<det))
        return false;

    det1 /= det;

    s.x = p1.x + det1*p21.x;
    s.y = p1.y + det1*p21.y;

    return true;
}



/**
 * Calculates the area between two simple polylines and checks their
 * maximal distance from each other against a limit.
 *
 * The polylines must have identical start points (p[0]==q[0])
 * and identlical end points (p[p_cnt-1]==q[q_cnt-1]).
 *
 * The polygon defined by p and q is decomposed into simple pieces.
 * The area of every simple subpolygon is then calculated by the
 * formula 2A = sum(x[i]*(y[i-1]-y[i+1])).
 *
 * The algorithm searches for intersection points of p and q by
 * a sweep line like approach in linear time. It will find all
 * intersection points, if
 *
 * (1) The polylines p and q do not intersect themselves.
 *
 * (2) The points of p and q are visited in some linear order '<'
 *     such that the segments (p[i-1],p[i]) and (q[j-1],q[j]) do
 *     not intersect if p[i] '<' q[j-1] or q[j] '<' p[i-1].
 *
 * Condition (2) makes sure that the polylines do not reverse "direction".
 * Then the number of intersection points is at most linear.
 *
 * The implementation works as follows:
 * p and q are pointers to the current point of each polyline. The algorithm
 * keeps track of which point is 'behind' the other and always advances the
 * pointer to that point. A line perpendicular to the current p point is
 * used to check if the other point is still behind.
 *
 * For each point visited, its distance to the current segment of the opposite
 * path is checked against a limit.
 *
 *
 * @param p          first polyline with points p[0]..p[p_cnt-1]
 * @param p_cnt      number of points of p
 * @param q          second polyline with points q[0]..q[q_cnt-1]
 * @param q_cnt      number of points of q
 * @param max_dist   the maximal distance of the points of one polyline to
 *                   the other polyline
 * @param area       the area between the two polygons (return)
 *
 * @retval true      if the maximal distance between the polylines is less than
 *                   max_dist (the area between the polylines is returned in area)
 * @retval false     otherwise
 */
bool calc_area(const node* p, int p_cnt, const point* q, int q_cnt, double max_dist, double& area)
{
    point lp, lq;   // p[-1], q[-1]
    double x;       // start point (x coordinate)
    double yp, yq;  // p[-2].y, q[-2].y
    bool  p_behind; // true: p has to be advanced; false: q has to be advanced
    double a = 0;   // area of subpolygon
    
    area = 0;       // initialize area
    
    // p_cnt and q_cnt shall point to the last point of each path
    --p_cnt;
    --q_cnt;

    // we need at least two points in each path
    if (p_cnt<=0 || q_cnt<=0)
        return false;
    
    assert(p[0]==q[0]);
    assert(p[p_cnt]==q[q_cnt]);

    x = p->x;
    lp = *p++; --p_cnt;
    lq = *q++; --q_cnt;

    // lp==lq !

    // p '<' q ?
    p_behind = dot(*q-*p, *p-lp) > 0;

    while (p_cnt>0 || q_cnt>0)
    {
        // points are available in at least one path

        point s;    // intersection point

        // which pointer has to be advanced
        // special cases: p_cnt==0 --> advance_p==false
        //                q_cnt==0 --> advance_p==true
        bool  advance_p = (p_behind && p_cnt>0) || q_cnt==0;

        // printf("advance_p=%d *p=(%f,%f) *q=(%f,%f)\n", advance_p, p->x, p->y, q->x, q->y);
        
        if (advance_p)
        {
            // p '<' q --> check and advance p
            if (check_distance(lq, *q, *p, max_dist)==false)
                return false;

            yp = lp.y;
            lp = *p++; --p_cnt;

            // p '<' q ?
            //p_behind = dot(*p-*q, *q-lq) < 0;
            
            // determine which pointer to advance in the next round
            if (p_cnt>0)
                p_behind = dot(*p-*q, p[1]-lp) < 0;
        }
        else
        {
            // q '<' p --> check and advance q
            if (check_distance(lp, *p, *q, max_dist)==false)
                return false;

            yq = lq.y;
            lq = *q++; --q_cnt;

            // p '<' q ?
            //p_behind = dot(*q-*p, *p-lp) > 0;

            // determine which pointer to advance in the next round
            if (p_cnt>0)
                p_behind = dot(*q-*p, p[1]-lp) > 0;
        }

        // check for intersection
        if ( cut(lp, *p, lq, *q, s) )
        {
            // calculate area of subpolygon
            if (advance_p)
                a += (lp.x-x)*(s.y-yp);
            else
                a += (lq.x-x)*(yq-s.y);
            
            // finalize area calculation
            a += (s.x-x)*(lq.y-lp.y);

            // printf("subpolygon finished: a=%f s=(%f,%f)\n", a, s.x, s.y);

            // accumulate area
            if (a<0)
                a = -a;
            area += a;
            a = 0;

            // initialize new subpolygon
            x = s.x;
            lp = lq = s;
        }
        else
        {
            // no intersection, calculate area
            if (advance_p)
                a += (lp.x-x)*(p->y-yp);
            else
                a += (lq.x-x)*(yq-q->y);
        }
    }

    area /= 2;
    
    return true;
}
