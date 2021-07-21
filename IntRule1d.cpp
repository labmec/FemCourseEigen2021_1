/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "IntRule1d.h"
///\cond
#include <iostream> 
#include <vector>
#include <math.h>
#include <cmath>
///\endcond
using namespace std;

#define PI 3.141592654

IntRule1d::IntRule1d(){

}

IntRule1d::IntRule1d(int order) : IntRule(order) {
    DebugStop();
}

void IntRule1d::SetOrder(int order) {
    fOrder = order;
    if(order < 0 || order > MaxOrder()) DebugStop();
    switch (order) {
    case 0:
    case 1:
        fPoints.resize(1,1);
        fWeights.resize(1);
        fPoints(0, 0) = 0.;
        fWeights(0) = 2.;

            break;
    case 2:
    case 3:
        fPoints.resize(2, 1);
        fWeights.resize(2);

        fPoints(0, 0) = -1. / sqrt(3.);
        fWeights[0] = 1.;

        fPoints(1, 0) = 1. / sqrt(3.);
        fWeights[1] = 1.;

        break;
    case 4:
    case 5:
        fPoints.resize(3, 1);
        fWeights.resize(3);

        fPoints(0, 0) = -sqrt(3. / 5.);
        fWeights[0] = 5. / 9.;

        fPoints(1, 0) = 0.;
        fWeights[1] = 8. / 9.;

        fPoints(2, 0) = sqrt(3. / 5.);
        fWeights[2] = 5. / 9.;
        break;

    default:
        break;
    }
    DebugStop();
}

void IntRule1d::gauleg(const double x1, const double x2, VecDouble &co, VecDouble &w){
    int n = w.size();

    double EPS = 1.0e-14;
    int m, j, i;
    double z1, z, xm, xl, pp, p3, p2, p1;    
    
    m = (n + 1) / 2;
    xm = 0.5 * (x2 + x1);
    xl = 0.5 * (x2 - x1);
    for (i = 0; i < m; i++) {
        z = cos(PI * (i + 0.75) / (n + 0.5));
        do {
            p1 = 1.0;
            p2 = 0.0;
            for (j = 0; j < n; j++) {
                p3 = p2;
                p2 = p1;
                p1 = ((2.0 * j + 1.0) * z * p2 - j * p3) / (j + 1);
            }
            pp = n * (z * p1 - p2) / (z * z - 1.0);
            z1 = z;
            z = z1 - p1 / pp;
        } while (fabs(z - z1) > EPS);
        co[i] = xm - xl*z;
        co[n - 1 - i] = xm + xl*z;
        w[i] = 2.0 * xl / ((1.0 - z * z) * pp * pp);
        w[n - 1 - i] = w[i];
    }
    
}

