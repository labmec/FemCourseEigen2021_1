/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TopologyTriangle.h"
///\cond
#include <iostream>
///\endcond

// Number of sides associated with triangle elements elements
const int TopologyTriangle::nSides;

// Number of corner nodes associated with triangle elements
const int TopologyTriangle::nCorners;

// Dimension of triangle elements
const int TopologyTriangle::Dimension;


int TopologyTriangle::NSideNodes(int side) {
    int nsidenodes[7] = {1, 1, 1, 2, 2, 2, 3};
    return nsidenodes[side];
}

int TopologyTriangle::SideNodeLocIndex(int side, int node) {
    if (side < 3 && node == 0) return side;
    if (side >= 3 && side < 6 && node < 2) return (side - 3 + node) % 3;
    if (side == 6 && node < 3) return node;
    std::cout << "TopologyTriangle::SideNodeIndex inconsistent side or node " << side
            << ' ' << node << std::endl;
    return -1;
}

MElementType TopologyTriangle::Type() {
    return ETriangle;

}
