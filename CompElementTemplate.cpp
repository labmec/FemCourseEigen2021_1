/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "CompElementTemplate.h"
#include "CompElement.h"
#include "CompMesh.h"
#include "DataTypes.h"
#include "CompMesh.h"
#include "Shape0d.h"
#include "Shape1d.h"
#include "ShapeQuad.h"
#include "ShapeTetrahedron.h"
#include "ShapeTriangle.h"
#include "GeoElement.h"
#include "MathStatement.h"
#include "GeoElementSide.h"
#include "DOF.h"

template<class Shape>
CompElementTemplate<Shape>::CompElementTemplate() : dofindexes(0) {
}

template<class Shape>
CompElementTemplate<Shape>::CompElementTemplate(int64_t ind, CompMesh *cmesh, GeoElement *geo) : CompElement(ind, cmesh, geo) {
    int64_t nel = 0;
    nel = cmesh->GetElementVec().size();
    cmesh->SetNumberElement(nel);
    cmesh->SetElement(ind, this);
    intrule.SetOrder(2 * cmesh->GetDefaultOrder());
    this->SetIntRule(&intrule);
    this->SetIndex(ind);
    this->SetCompMesh(cmesh);
    this->SetGeoElement(geo);
    geo->SetReference(this);

    int materialid = geo->Material();
    MathStatement *mat = cmesh->GetMath(materialid);
    this->SetStatement(mat);

    int nsides = geo->NSides();

    for (int i = 0; i < nsides; i++) {
        GeoElementSide gelside(this->GetGeoElement(), i);
        GeoElementSide neighbour = gelside.Neighbour();
        this->SetNDOF(nsides);

        if (gelside != neighbour && neighbour.Element()->GetIndex() < ind) {
            CompElement *cel = neighbour.Element()->GetReference();
            int64_t dofindex = cel->GetDOFIndex(neighbour.Side());
            this->SetDOFIndex(i, dofindex);
        } else {
            class DOF dof;
            int order = cmesh->GetDefaultOrder();
            if(i < geo->NCornerNodes()) order = 1;
            int nshape = ComputeNShapeFunctions(i, order);
            int nstate = mat->NState();
            dof.SetNShapeStateOrder(nshape, nstate, order);

            if(nstate != 1) DebugStop();
            int64_t ndof = cmesh->GetNumberDOF();
            ndof++;
            cmesh->SetNumberDOF(ndof);

            int64_t dofindex = ndof - 1;
            this->SetDOFIndex(i, dofindex);

            cmesh->SetDOF(ndof - 1, dof);
        }
    }
}

template<class Shape >
CompElementTemplate<Shape>::CompElementTemplate(const CompElementTemplate & copy) {
    dofindexes = copy.dofindexes;
    intrule = copy.intrule;
}

template<class Shape >
CompElementTemplate<Shape> &CompElementTemplate<Shape>::operator=(const CompElementTemplate & copy) {
    dofindexes = copy.dofindexes;
    intrule = copy.intrule;
    return *this;
}

template<class Shape >
CompElementTemplate<Shape>::~CompElementTemplate() {
}

template<class Shape >
CompElement * CompElementTemplate<Shape>::Clone() const {
    return new CompElementTemplate(*this);
}

template<class Shape>
void CompElementTemplate<Shape>::ShapeFunctions(const VecDouble &intpoint, VecDouble &phi, MatrixDouble & dphi) const {
    int nsides = this->NDOF();
    VecInt orders(nsides);
    for(int is=0; is<nsides; is++)
    {
        int doforder = GetDOF(is).GetOrder();
        orders[is] = doforder;
    }

    Shape::Shape(intpoint, orders, phi, dphi);
    orders.resize(0);
}

template<class Shape>
void CompElementTemplate<Shape>::GetMultiplyingCoeficients(VecDouble & coefs) const {
    int ndof = this->NDOF();

    int ncoef = 0;
    for (int64_t i = 0; i < ndof; i++) {
        int dofindex = dofindexes[i];
        class DOF dof = this->GetCompMesh()->GetDOF(dofindex);
        int dof_neq = dof.GetNShape()*dof.GetNState();
        ncoef+= dof_neq;
    }
    coefs.resize(ncoef);
    coefs.setZero();

    int ni = 0;
    for (int64_t i = 0; i < ndof; i++) {
        int dofindex = dofindexes[i];
        class DOF dof = this->GetCompMesh()->GetDOF(dofindex);
        int dof_first = dof.GetFirstEquation();
        int dof_neq = dof.GetNShape()*dof.GetNState();
        for (int j = 0; j < dof_neq; j++) {
            int iglob = dof_first + j;
            coefs[ni] = this->GetCompMesh()->Solution()[iglob];
            ni++;
        }
    }
}

template<class Shape>
int CompElementTemplate<Shape>::NShapeFunctions() const {
    int nsides = this->NDOF();
    VecInt orders(nsides);
    for(int is=0; is<nsides; is++)
    {
        orders[is] = GetDOF(is).GetOrder();
    }
    return Shape::NShapeFunctions(orders);
    orders.resize(0);
    // Code should not reach this point. This return is only here to stop compiler warnings.
    DebugStop();
    return -1;
}

template<class Shape>
void CompElementTemplate<Shape>::SetNDOF(int64_t ndof) {
    dofindexes.resize(ndof);
}

template<class Shape>
void CompElementTemplate<Shape>::SetDOFIndex(int i, int64_t dofindex) {
    dofindexes[i] = dofindex;
}

template<class Shape>
int64_t CompElementTemplate<Shape>::GetDOFIndex(int i) const {
    return dofindexes[i];
}

template<class Shape>
int64_t CompElementTemplate<Shape>::NDOF() const {
    return dofindexes.size();
}

template<class Shape>
int CompElementTemplate<Shape>::NShapeFunctions(int doflocindex) const {
    CompMesh *compmesh = GetCompMesh();
    return compmesh->GetDOF(doflocindex).GetNShape();
}

template<class Shape>
int CompElementTemplate<Shape>::ComputeNShapeFunctions(int doflocindex, int order) {
    dofindexes.resize(doflocindex + 1);
    dofindexes[doflocindex] = doflocindex;
    return Shape::NShapeFunctions(doflocindex, order);

}

template<class Shape>
void CompElementTemplate<Shape>::Print(std::ostream &out) {
    out << "fReference index: " << this->GetGeoElement()->GetIndex() << std::endl;
    if (this->GetStatement()) {
        out << "Material index: " << this->GetStatement()->GetMatID() << std::endl;
    } else {
        out << "No material\n";
    }

    out << "Number of DOFs: " << this->NDOF() << std::endl;
    out << "DOFs indexes : ";
    int nod;
    for (nod = 0; nod < this->NDOF(); nod++) {
        out << this->GetDOFIndex(nod) << ' ';
    }
    out << std::endl << std::endl;
}

CompElement *CreateCompEl(GeoElement *gel, CompMesh *mesh, int64_t index) {

    switch (gel->Type()) {
        case EPoint:
            return new CompElementTemplate<Shape0d> (index, mesh, gel);
            break;
        case EOned:
            return new CompElementTemplate<Shape1d> (index, mesh, gel);
            break;
        case EQuadrilateral:
            return new CompElementTemplate<ShapeQuad> (index, mesh, gel);
            break;
        case ETriangle:
            return new CompElementTemplate<ShapeTriangle> (index, mesh, gel);
            break;
        case ETetraedro:
            return new CompElementTemplate<ShapeTetrahedron> (index, mesh, gel);
        default:
            DebugStop();
    }
 
    // Code should not reach this point. This return is only here to stop compiler warnings.
    DebugStop();
    return NULL;
}

template class CompElementTemplate<Shape0d>;
template class CompElementTemplate<Shape1d>;
template class CompElementTemplate<ShapeQuad>;
template class CompElementTemplate<ShapeTriangle>;
template class CompElementTemplate<ShapeTetrahedron>;
