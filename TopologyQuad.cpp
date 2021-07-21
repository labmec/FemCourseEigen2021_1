/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TopologyQuad.h"
///\cond
#include <iostream>
///\endcond

// Number of sides associated with quadrilateral elements elements
const int TopologyQuad::nSides;

// Number of corner nodes associated with quadrilateral elements
const int TopologyQuad::nCorners;

// Dimension of quadrilateral elements
const int TopologyQuad::Dimension;


int TopologyQuad::NSideNodes(int side) {
    static int nsidenodes[9] = {1, 1, 1, 1, 2, 2, 2, 2, 4};
    return nsidenodes[side];
}

int TopologyQuad::SideNodeLocIndex(int side, int node) {
    if (side < 4 && node == 0) return side;
    if (side >= 4 && side < 8 && node < 2) return (side + node) % 4;
    if (side == 8 && node < 4) return node;
    std::cout << "TopologyQuad::SideNodeIndex inconsistent side or node " << side
            << ' ' << node << std::endl;
    return -1;
}

MElementType TopologyQuad::Type() {

    return EQuadrilateral;

}
