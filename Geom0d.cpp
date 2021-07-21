/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Geom0d.h"

Geom0d::Geom0d() {
}

Geom0d::~Geom0d() {
}

Geom0d::Geom0d(const Geom0d &copy) {
    fNodeIndices = copy.fNodeIndices;
}

Geom0d& Geom0d::operator=(const Geom0d& copy) {
    fNodeIndices = copy.fNodeIndices;
    return *this;
}

void Geom0d::Shape(const VecDouble &xi, VecDouble &phi, MatrixDouble &dphi) {
    if(xi.size() != Dimension || phi.size() != nCorners || dphi.rows() != Dimension || dphi.cols() != nCorners) DebugStop();
    phi[0] = 1.;
}

void Geom0d::X(const VecDouble &xi, MatrixDouble &NodeCo, VecDouble &x) {
    if(xi.size() != Dimension) DebugStop();
    if(x.size() < NodeCo.rows()) DebugStop();
    if(NodeCo.cols() != nCorners) DebugStop();
    int nrow = NodeCo.rows();
    for (int i = 0; i < nrow; i++) {
        x[i] = NodeCo(i, 0);
    }
}

void Geom0d::GradX(const VecDouble &xi, MatrixDouble &NodeCo, VecDouble &x, MatrixDouble &gradx) {
    if(xi.size() != Dimension) DebugStop();
    if(x.size() != NodeCo.rows()) DebugStop();
    if(NodeCo.cols() != nCorners) DebugStop();
    int nrow = NodeCo.rows();
    int ncol = nCorners;

    gradx.resize(nrow, 1);
    gradx.setZero();

    VecDouble phi(1);
    MatrixDouble dphi(Dimension, 1);
    Shape(xi, phi, dphi);
    for (int i = 0; i < ncol; i++) {
        for (int j = 0; j < nrow; j++) {
            x[i] = NodeCo(i, 0);
        }
    }
}

void Geom0d::SetNodes(const VecInt &nodes) {
    if(nodes.rows() != 1) DebugStop();
    fNodeIndices = nodes;
}

void Geom0d::GetNodes(VecInt &nodes) const {
    nodes = fNodeIndices;
}

int Geom0d::NodeIndex(int node) const {
    if(node != 0) DebugStop();
    return fNodeIndices[node];
}

int Geom0d::NumNodes(){
    return nCorners;    
}

GeoElementSide Geom0d::Neighbour(int side) const{
    if(side != 0) DebugStop();
    return fNeighbours[side];
}

void Geom0d::SetNeighbour(int side, const GeoElementSide &neighbour) {
    if(side != 0) DebugStop();
    fNeighbours[side]=neighbour;
}
