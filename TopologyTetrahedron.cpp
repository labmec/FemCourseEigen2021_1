/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TopologyTetrahedron.h"

// Number of sides associated with tetrahedron elements elements
const int TopologyTetrahedron::nSides;

// Number of corner nodes associated with tetrahedron elements
const int TopologyTetrahedron::nCorners;

// Dimension of tetrahedron elements
const int TopologyTetrahedron::Dimension;


int TopologyTetrahedron::NSideNodes(int side) {
    int nsidenodes[15] = {1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4};
    return nsidenodes[side];
}

int TopologyTetrahedron::SideNodeLocIndex(int side, int node) {
    int SideNodes[6][2] = {
        {0, 1},
        {1, 2},
        {2, 0},
        {0, 3},
        {1, 3},
        {2, 3}
    };
    int FaceNodes[4][3] = {
        {0, 1, 2},
        {0, 1, 3},
        {1, 2, 3},
        {0, 2, 3}
    };

    if (side < 4 && node == 0) return side;
    if (side >= 4 && side < 10 && node < 2) return SideNodes[side - 4][node];
    if (side >= 10 && side < 14 && node < 3) return FaceNodes[side - 10][node];
    if (side == 14 && node < 4) return node;
    std::cout << "ShapeTetrahedron::SideNodeIndex inconsistent side or node " << side
            << ' ' << node << std::endl;
    DebugStop();
    return -1;
}

MElementType TopologyTetrahedron::Type() {

    return ETetraedro;

}
