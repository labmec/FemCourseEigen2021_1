/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "IntRule0d.h"
///\cond
#include <iostream> 
#include <vector>
#include <math.h>
#include <cmath>
///\endcond
using namespace std;


IntRule0d::IntRule0d(){

}

IntRule0d::IntRule0d(int order) {
    if (order != 0) {
        DebugStop();
    }

    SetOrder(order);

}


void IntRule0d::SetOrder(int order) {
    fOrder = order;  
    
    if (order != 0) {
        DebugStop();
    }

    int npoints = 1;
    
    fPoints.resize(npoints, 0);
    fWeights.resize(npoints);
    fWeights[0] = 1.;
    
}
