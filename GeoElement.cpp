/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "GeoElementSide.h"
///\cond
#include <set>
///\endcond

GeoElement::GeoElement() : GMesh(0), MaterialId(-1), Reference(0), Index(-1) {

}

GeoElement::GeoElement(int materialid, GeoMesh *mesh, int index) {
    MaterialId = materialid;
    GMesh = mesh;
    Index = index;
}

GeoElement::GeoElement(const GeoElement &copy) {
    GMesh = copy.GMesh;
    MaterialId = copy.MaterialId;
    Reference = copy.Reference;
    Index = copy.Index;
}

GeoElement::~GeoElement() {

}
// Return the enumerated element type
MElementType GeoElement::Type(int side) const
{
    int nsides = NSides();
    if(side == nsides-1) return Type();
    int nsidenodes = NSideNodes(side);
    switch (nsidenodes) {
        case 1:
            return EPoint;
            break;
        case 2:
            return EOned;
        case 3:
            return ETriangle;
            
        default:
            DebugStop();
            break;
    }
    return EPoint;
}

void GeoElement::Jacobian(const MatrixDouble &gradx, MatrixDouble &jac, MatrixDouble &axes, double &detjac, MatrixDouble &jacinv) const {
    int nrows = gradx.rows ();
    int ncols = gradx.cols();
    int dim = jac.rows ();
    jac.resize(dim, dim);
    axes.resize(dim, 3);
    jacinv.resize(dim, dim);
    jac.setZero();

    switch (dim) {
        case 0:
            detjac = 1.;
            break;
        case 1:
        {
            axes.resize(dim, 3);
            VecDouble v_1(3);
            v_1.setZero();

            for (int i = 0; i < nrows; i++) {
                v_1[i] = gradx(i, 0);
            }

            double norm_v_1 = 0.;
            for (int i = 0; i < nrows; i++) {
                norm_v_1 += v_1[i] * v_1[i];
            }

            norm_v_1 = sqrt(norm_v_1);
            jac(0, 0) = norm_v_1;
            detjac = norm_v_1;
            jacinv(0, 0) = 1.0 / detjac;

            detjac = fabs(detjac);

            for (int i = 0; i < 3; i++) {
                axes(0, i) = v_1[i] / norm_v_1;
            }
        }
            break;
        case 2:
        {
            VecDouble v_1(3), v_2(3);
            VecDouble v_1_til(3), v_2_til(3);
            v_1.setZero();
            v_2.setZero();
            v_1_til.setZero();
            v_2_til.setZero();

            for (int i = 0; i < nrows; i++) {
                v_1[i] = gradx(i, 0);
                v_2[i] = gradx(i, 1);
            }

            double norm_v_1_til = 0.0;
            double norm_v_2_til = 0.0;
            double v_1_dot_v_2 = 0.0;

            for (int i = 0; i < 3; i++) {
                norm_v_1_til += v_1[i] * v_1[i];
                v_1_dot_v_2 += v_1[i] * v_2[i];
            }
            norm_v_1_til = sqrt(norm_v_1_til);

            for (int i = 0; i < 3; i++) {
                v_1_til[i] = v_1[i] / norm_v_1_til;
                v_2_til[i] = v_2[i] - v_1_dot_v_2 * v_1_til[i] / norm_v_1_til;
                norm_v_2_til += v_2_til[i] * v_2_til[i];
            }
            norm_v_2_til = sqrt(norm_v_2_til);


            jac(0, 0) = norm_v_1_til;
            jac(0, 1) = v_1_dot_v_2 / norm_v_1_til;
            jac(1, 1) = norm_v_2_til;

            detjac = jac(0, 0) * jac(1, 1) - jac(1, 0) * jac(0, 1);

            jacinv(0, 0) = +jac(1, 1) / detjac;
            jacinv(1, 1) = +jac(0, 0) / detjac;
            jacinv(0, 1) = -jac(0, 1) / detjac;
            jacinv(1, 0) = -jac(1, 0) / detjac;

            detjac = fabs(detjac);

            for (int i = 0; i < 3; i++) {
                v_2_til[i] /= norm_v_2_til;
                axes(0, i) = v_1_til[i];
                axes(1, i) = v_2_til[i];
            }
        }
            break;
        case 3:
        {
            axes.resize(dim, 3);

            for (int i = 0; i < nrows; i++) {
                jac(i, 0) = gradx(i, 0);
                jac(i, 1) = gradx(i, 1);
                jac(i, 2) = gradx(i, 2);
            }

            detjac -= jac(0, 2) * jac(1, 1) * jac(2, 0); //- a02 a11 a20
            detjac += jac(0, 1) * jac(1, 2) * jac(2, 0); //+ a01 a12 a20
            detjac += jac(0, 2) * jac(1, 0) * jac(2, 1); //+ a02 a10 a21
            detjac -= jac(0, 0) * jac(1, 2) * jac(2, 1); //- a00 a12 a21
            detjac -= jac(0, 1) * jac(1, 0) * jac(2, 2); //- a01 a10 a22
            detjac += jac(0, 0) * jac(1, 1) * jac(2, 2); //+ a00 a11 a22

            jacinv(0, 0) = (-jac(1, 2) * jac(2, 1) + jac(1, 1) * jac(2, 2)) / detjac; //-a12 a21 + a11 a22
            jacinv(0, 1) = (jac(0, 2) * jac(2, 1) - jac(0, 1) * jac(2, 2)) / detjac; //a02 a21 - a01 a22
            jacinv(0, 2) = (-jac(0, 2) * jac(1, 1) + jac(0, 1) * jac(1, 2)) / detjac; //-a02 a11 + a01 a12
            jacinv(1, 0) = (jac(1, 2) * jac(2, 0) - jac(1, 0) * jac(2, 2)) / detjac; //a12 a20 - a10 a22
            jacinv(1, 1) = (-jac(0, 2) * jac(2, 0) + jac(0, 0) * jac(2, 2)) / detjac; //-a02 a20 + a00 a22
            jacinv(1, 2) = (jac(0, 2) * jac(1, 0) - jac(0, 0) * jac(1, 2)) / detjac; //a02 a10 - a00 a12
            jacinv(2, 0) = (-jac(1, 1) * jac(2, 0) + jac(1, 0) * jac(2, 1)) / detjac; //-a11 a20 + a10 a21
            jacinv(2, 1) = (jac(0, 1) * jac(2, 0) - jac(0, 0) * jac(2, 1)) / detjac; //a01 a20 - a00 a21
            jacinv(2, 2) = (-jac(0, 1) * jac(1, 0) + jac(0, 0) * jac(1, 1)) / detjac; //-a01 a10 + a00 a11

            detjac = fabs(detjac);

            axes.setZero();
            axes(0, 0) = 1.0;
            axes(1, 1) = 1.0;
            axes(2, 2) = 1.0;
        }
            break;
    }

}

static bool CheckQuadConsistency(const VecInt &one, const VecInt &two)
{
    // same orientation
    for(int rot = 0; rot<4; rot++)
    {
        bool check = true;
        for(int i=0; i<4; i++)
        {
            if(one[i] != two[(i+rot)%4]) {
                check = false;
                break;
            }
        }
        if(check) return true;
    }
    // oposite rotation
    for(int rot = 0; rot<4; rot++)
    {
        bool check = true;
        for(int i=0; i<4; i++)
        {
            if(one[i] != two[(4-i+rot)%4]) {
                check = false;
                break;
            }
        }
        if(check) return true;
    }
    return false;
}

int GeoElement::WhichSide(VecInt &SideNodeIds) const {
    int64_t cap = SideNodeIds.size();
    std::set<int> neighids;
    neighids.insert(&SideNodeIds[0], &SideNodeIds[0]+cap);
    
    int nums = NSides();
    for (int side = 0; side < nums; side++) {
        if (NSideNodes(side) == cap) {
            if(SideIsUndefined(side)) DebugStop();
            std::set<int> mynodes;
            for(int n=0; n<cap; n++) mynodes.insert(SideNodeIndex(side, n));
            if(mynodes == neighids)
            {
                if(cap == 4)
                {
                    VecInt mynodes(4);
                    for(int n=0; n<4; n++) mynodes[n] = SideNodeIndex(side, n);
                    if(CheckQuadConsistency(SideNodeIds, mynodes) == false) DebugStop();
                }
                return side;
            }
        }
    }
    return -1;
}

void GeoElement::Print(std::ostream &out) const {
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
