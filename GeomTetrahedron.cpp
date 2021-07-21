/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "GeomTetrahedron.h"

GeomTetrahedron::GeomTetrahedron() {
}

GeomTetrahedron::~GeomTetrahedron() {
}

GeomTetrahedron::GeomTetrahedron(const GeomTetrahedron &copy) {
    fNodeIndices = copy.fNodeIndices;
}

GeomTetrahedron& GeomTetrahedron::operator=(const GeomTetrahedron& copy){
    fNodeIndices = copy.fNodeIndices;
    return *this;
}

void GeomTetrahedron::Shape(const VecDouble &xi, VecDouble &phi, MatrixDouble &dphi) {
    if(xi.size() != Dimension || phi.size() != nCorners || dphi.rows() != Dimension || dphi.cols() != nCorners) DebugStop();
    double qsi = xi[0];
    double eta = xi[1];
    double zeta = xi[2];

    phi[0] = 1.0 - qsi - eta - zeta;
    phi[1] = qsi;
    phi[2] = eta;
    phi[3] = zeta;

    dphi(0, 0) = -1.0;
    dphi(1, 0) = -1.0;
    dphi(2, 0) = -1.0;
    
    dphi(0, 1) = 1.0;
    dphi(1, 1) = 0.0;
    dphi(2, 1) = 0.0;
   
    dphi(0, 2) = 0.0;
    dphi(1, 2) = 1.0;
    dphi(2, 2) = 0.0;
    
    dphi(0, 3) = 0.0;
    dphi(1, 3) = 0.0;
    dphi(2, 3) = 1.0;
}

void GeomTetrahedron::X(const VecDouble &xi, MatrixDouble &NodeCo, VecDouble &x) {
    if(xi.size() != Dimension) DebugStop();
    if(x.size() != NodeCo.rows()) DebugStop();
    if(NodeCo.cols() != nCorners) DebugStop();
    VecDouble phi(nCorners);
    MatrixDouble dphi(Dimension, nCorners);
    
    
    Shape(xi, phi, dphi);
    int space = NodeCo.rows();

    for (int i = 0; i < Dimension; i++) {
        x[i] = 0.0;
        for (int j = 0; j < nCorners; j++) {
            x[i] += phi[j] * NodeCo(i, j);
        }
    }
}


void GeomTetrahedron::GradX(const VecDouble &xi, MatrixDouble &NodeCo, VecDouble &x, MatrixDouble &gradx) {
    if(xi.size() != Dimension) DebugStop();
    if(x.size() != NodeCo.rows()) DebugStop();
    if(NodeCo.cols() != nCorners) DebugStop();
    gradx.resize(3, 3);
    gradx.setZero();
    x.resize(3);
    x.setZero();
    int nrow = NodeCo.rows();
    int ncol = NodeCo.cols();

    VecDouble phi(nCorners);
    MatrixDouble dphi(Dimension, nCorners);
    Shape(xi, phi, dphi);
    for (int i = 0; i < nCorners; i++) {
        for (int j = 0; j < Dimension; j++) {
            x[j] += NodeCo(j,i) * phi[i];
            gradx(j, 0) += NodeCo(j, i) * dphi(0, i);
            gradx(j, 1) += NodeCo(j, i) * dphi(1, i);
            gradx(j, 2) += NodeCo(j, i) * dphi(2, i);
        }
    }
}


void GeomTetrahedron::SetNodes(const VecInt &nodes) {
    if(nodes.size() != nCorners) DebugStop();
    fNodeIndices = nodes;
}

void GeomTetrahedron::GetNodes(VecInt &nodes) const {
    nodes = fNodeIndices;
}

int GeomTetrahedron::NodeIndex(int node) const {
    return fNodeIndices[node];
}

int GeomTetrahedron::NumNodes() {
    return nCorners;

}

GeoElementSide GeomTetrahedron::Neighbour(int side) const {
    return fNeighbours[side];
}

void GeomTetrahedron::SetNeighbour(int side, const GeoElementSide &neighbour) {
    fNeighbours[side]=neighbour;
}
