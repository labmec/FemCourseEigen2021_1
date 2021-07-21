/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Topology1d.h"
///\cond
#include <iostream>
///\endcond

// Number of sides associated with 1D elements
const int Topology1d::nSides;

// Number of corner nodes associated with 1D elements
const int Topology1d::nCorners;

// Dimension of 1D elements
const int Topology1d::Dimension;



int Topology1d::NSideNodes(int side) {
    if(side < 0 || side > 2) DebugStop();
    static int nsidenodes[3] = {1, 1, 2};
    return nsidenodes[side];
}

int Topology1d::SideNodeLocIndex(int side, int node) {
    if (side < 2 && node == 0) return side;
    if (side == 2 && node < 2) return node;
    std::cout << "Topology1d::SideNodeIndex inconsistent side or node " << side
            << ' ' << node << std::endl;
    DebugStop();
    return -1;
}

MElementType Topology1d::Type() {

    return EOned;

}
