#include <assert.h>

#include "bezier.h"


static double tab[64+1][4];


// Calculate Bernstein polynomials of degree 3
// B(i,t) = choose(3,i) * t^i * (1-t)^(3-i)
static void init_tab()
{
    for (int i=0; i<=64; i++)
    {
        double t = (double)i/64;
        tab[i][0] =     (1-t)*(1-t)*(1-t);
        tab[i][1] = 3 * t*(1-t)*(1-t);
        tab[i][2] = 3 * t*t*(1-t);
        tab[i][3] =     t*t*t;
    }
}


/**
 * Calculates cnt+1 points p[0]..p[cnt] of the Bézier curve b[0]..b[3].
 * (cnt=1,2,4,8,16,32,64)
 *
 * p[t] = sum_{i=0..3} B(i,t/cnt)*b[i]
 */
void bezier_points(point* b, point* p, int cnt)
{
    assert(cnt==1||cnt==2||cnt==4||cnt==8||cnt==16||cnt==32||cnt==64);

    static bool initialized = false;
    if (initialized == false)
    {
        init_tab();
        initialized = true;
    }

    int d = 4*64/cnt;
    double* c = (double*)tab;

    while (cnt >= 0)
    {
        *p++ = b[0]*c[0] + b[1]*c[1] + b[2]*c[2] + b[3]*c[3];
        c += d;
        cnt--;
    }
}



