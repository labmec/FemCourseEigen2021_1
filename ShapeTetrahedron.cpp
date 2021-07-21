/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ShapeTetrahedron.h"

void ShapeTetrahedron::Shape(const VecDouble &xi, VecInt &orders, VecDouble &phi, MatrixDouble &dphi) {
    int nshape = NShapeFunctions(orders);

    phi[0] = 1 - xi[0] - xi[1] - xi[2];
    phi[1] = xi[0];
    phi[2] = xi[1];
    phi[3] = xi[2];

    dphi(0, 0) = -1.0;
    dphi(0, 1) = 1.0;
    dphi(0, 2) = 0.0;
    dphi(0, 3) = 0.0;

    dphi(1, 0) = -1.0;
    dphi(1, 1) = 0.0;
    dphi(1, 2) = 1.0;
    dphi(1, 3) = 0.0;

    dphi(2, 0) = -1.0;
    dphi(2, 1) = 0.0;
    dphi(2, 2) = 0.0;
    dphi(2, 3) = 1.0;

    int count = 4;
    int is;
    for (is = 4; is < 10; is++) {
        if(orders[is] == 2)
        {
            int is1 = SideNodeLocIndex(is, 0);
            int is2 = SideNodeLocIndex(is, 1);
            phi[is] = 4. *phi[is1] * phi[is2];
            dphi(0, is) = 4. * (dphi(0, is1) * phi[is2] + phi[is1] * dphi(0, is2));
            dphi(1, is) = 4. * (dphi(1, is1) * phi[is2] + phi[is1] * dphi(1, is2));
            dphi(2, is) = 4. * (dphi(2, is1) * phi[is2] + phi[is1] * dphi(2, is2));
            count++;
        } else if (orders[is] != 1) DebugStop();
    }
    if(count != nshape) DebugStop();
    for(int is = 10 ; is< nSides; is++) if(orders[is] != 1 && orders[is] != 2) DebugStop();
}

int ShapeTetrahedron::NShapeFunctions(int side, int order) {
    if(side < 0 || side >= nSides) DebugStop();
    if(order != 1 && order != 2) DebugStop();
    if (side < 4) return 1;
    if (side < 10) return order - 1;
    return 0;
}

int ShapeTetrahedron::NShapeFunctions(VecInt &orders) {
    if(orders.size() != nSides) DebugStop();
    int n = nSides;
    int nshape = 0;
    for (int i = 0; i < n; i++) {
        nshape += NShapeFunctions(i, orders[i]);
    }
    return nshape;
}
