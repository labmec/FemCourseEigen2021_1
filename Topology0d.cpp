/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Topology0d.h"
///\cond
#include <iostream>
///\endcond

// Number of sides associated with 1D elements
const int Topology0d::nSides;

// Number of corner nodes associated with 1D elements
const int Topology0d::nCorners;

// Dimension of 1D elements
const int Topology0d::Dimension;



int Topology0d::NSideNodes(int side) {
    if(side != 0) DebugStop();
    return 1;
}

int Topology0d::SideNodeLocIndex(int side, int node) {
    if (side != 0 || node != 0) DebugStop();
    return 0;
}

MElementType Topology0d::Type() {

    return EPoint;

}
