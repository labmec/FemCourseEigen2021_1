/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "GeoElementTemplate.h"
#include "Geom0d.h"
#include "Geom1d.h"
#include "GeomQuad.h"
#include "GeomTriangle.h"
#include "GeomTetrahedron.h"
#include "GeoMesh.h"
///\cond
#include <math.h> 
#include <stdio.h>
///\endcond

using namespace std;

template<class TGeom>
GeoElementTemplate<TGeom>::GeoElementTemplate(const VecInt &nodeindices, int materialid, GeoMesh *gmesh, int index) : GeoElement(materialid, gmesh, index) {
    if(index >= gmesh->NumElements()) gmesh->SetNumElements(index + 1);
    if(nodeindices.size() != Geom.nCorners) DebugStop();
    Geom.SetNodes(nodeindices);
    for (int side = 0; side < TGeom::nSides; side++) {
        Geom.SetNeighbour(side, GeoElementSide(this, side));
    }
    gmesh->SetElement(index, this);
}

template<class TGeom>
GeoElementTemplate<TGeom>::GeoElementTemplate(const GeoElementTemplate &copy) {
    Geom = copy.Geom;
}

template<class TGeom>
GeoElementTemplate<TGeom> &GeoElementTemplate<TGeom>::operator=(const GeoElementTemplate &copy) {
    Geom = copy.Geom;
    return *this;
}

template<class TGeom>
MElementType GeoElementTemplate<TGeom>::Type() const {
    return TGeom::Type();
}

template<class TGeom>
void GeoElementTemplate<TGeom>::X(const VecDouble &xi, VecDouble &x) const{
    int NNodes = this->NNodes();
    int dim = GMesh->Dimension();
    MatrixDouble coord(dim, NNodes);

    int i, j;
    for (i = 0; i < NNodes; i++) {
        int index = this->NodeIndex(i);
        GeoNode node = GMesh->Node(index);
        for (j = 0; j < dim; j++) {
            coord(j, i) = node.Coord(j);
        }
    }
    x.setZero();
    Geom.X(xi, coord, x);
}

template<class TGeom>
void GeoElementTemplate<TGeom>::GradX(const VecDouble &xi, VecDouble &x, MatrixDouble &gradx) const
{
    int NNodes = this->NNodes();
    MatrixDouble coord(3, NNodes);

    int i, j;
    for (i = 0; i < NNodes; i++) {
        int index = this->NodeIndex(i);
        GeoNode node = GMesh->Node(index);
        for (j = 0; j < 3; j++) {
            coord(j, i) = node.Coord(j);
        }
    }
    x.setZero();
    gradx.setZero();
    Geom.GradX(xi, coord, x, gradx);
    // std::cout << "xi " << xi << "\nx " << x << std::endl;
}



template<class TGeom>
void GeoElementTemplate<TGeom>::Print(std::ostream &out) const
{
    out << "Number of nodes:\t" << this->NNodes() << std::endl;
    out << "Corner nodes:\t\t" << this->NCornerNodes() << std::endl;
    out << "Nodes indexes\t\t";

    for (int i = 0; i < this->NNodes(); i++) out << this->NodeIndex(i) << "   ";
    out << "\nNumber of sides:\t" << this->NSides() << std::endl;
    out << "Material Id:\t\t" << this->Material() << std::endl;

    int nsides = this->NSides();

    for (int i = 0; i < nsides; i++) {
        out << "Neighbours for side " << i << ":\t";
        GeoElementSide neighbour = Neighbour(i);
        GeoElementSide thisside((GeoElement *)this, i);
        if (!(neighbour.Element() != 0 && neighbour.Side() > -1)) {
            out << "No neighbour\n";
        } else {
            while (neighbour.Element() != thisside.Element() || neighbour.Side() != thisside.Side()) {
                out << neighbour.Element()->GetIndex() << "/" << neighbour.Side() << ' ';

                neighbour = neighbour.Neighbour();
            }
            out << std::endl;
        }
    }
}

template<class TGeom>
int GeoElementTemplate<TGeom>::SideIsUndefined(int side) const{
    if (side < 0 || side > NSides()) {
        std::cout << "GeoElementTemplate<TGeom>::SideIsUndefined - bad side: " << side << std::endl;
        DebugStop();
    }
    return (Geom.Neighbour(side).Side() == -1);
}


template class GeoElementTemplate<Geom0d>;
template class GeoElementTemplate<Geom1d>;
template class GeoElementTemplate<GeomQuad>;
template class GeoElementTemplate<GeomTriangle>;
template class GeoElementTemplate<GeomTetrahedron>;
