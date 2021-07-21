/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "CompMesh.h"
#include "GeoMesh.h"
#include "MathStatement.h"
#include "GeoElement.h"
#include "CompElement.h"
#include "CompElementTemplate.h"

CompMesh::CompMesh() : geomesh(0), compelements(0), dofs(0), mathstatements(0), solution(0) {

}

CompMesh::CompMesh(const CompMesh &copy) {
    geomesh = copy.geomesh;
    compelements = copy.compelements;
    dofs = copy.dofs;
    mathstatements = copy.mathstatements;
    solution = copy.solution;
    DefaultOrder = copy.DefaultOrder;
}

CompMesh::CompMesh(GeoMesh *gmesh) {
    this->SetGeoMesh(gmesh);
    this->SetNumberElement(gmesh->NumElements());
}

CompMesh::~CompMesh() {
}

GeoMesh *CompMesh::GetGeoMesh() const {
    return geomesh;
}

void CompMesh::SetGeoMesh(GeoMesh *gmesh) {
    geomesh = gmesh;
}

void CompMesh::SetNumberElement(int64_t nelem) {
    compelements.resize(nelem);
}

void CompMesh::SetNumberDOF(int64_t ndof) {
    dofs.resize(ndof);
}

void CompMesh::SetNumberMath(int nmath) {
    mathstatements.resize(nmath);
}

void CompMesh::SetElement(int64_t elindex, CompElement *cel) {
    compelements[elindex] = cel;
}

void CompMesh::SetDOF(int64_t index, const DOF &dof) {
    dofs[index] = dof;
}

void CompMesh::SetMathStatement(int index, MathStatement *math) {
    if(mathstatements.size() < index+1) mathstatements.resize(index+1);
    mathstatements[index] = math;
}

DOF &CompMesh::GetDOF(int64_t dofindex) {
    return dofs[dofindex];
}

CompElement *CompMesh::GetElement(int64_t elindex) const {
    return compelements[elindex];
}

MathStatement *CompMesh::GetMath(int matindex) const {
    return mathstatements[matindex];
}

const std::vector<CompElement *> &CompMesh::GetElementVec() const {
    return compelements;
}

const std::vector<DOF> &CompMesh::GetDOFVec() const {
    return dofs;
}

const std::vector<MathStatement *> &CompMesh::GetMathVec() const {
    return mathstatements;
}

void CompMesh::SetElementVec(const std::vector<CompElement *> &vec) {
    int c = vec.size();

    for (int i = 0; i < c; i++) {
        compelements[i] = vec[i];
    }
}

void CompMesh::SetDOFVec(const std::vector<DOF> &dofvec) {
    int c = dofvec.size();

    for (int i = 0; i < c; i++) {
        dofs[i] = dofvec[i];
    }
}

void CompMesh::SetMathVec(const std::vector<MathStatement *> &mathvec) {
    int c = mathvec.size();
    mathstatements.resize(c);
    for (int i = 0; i < c; i++) {
        mathstatements[i] = mathvec[i];
    }
}

void CompMesh::AutoBuild() {
    int64_t i;
    int64_t nelem = this->GetElementVec().size();

    for (i = 0; i < nelem; i++) {
        GeoElement *gel = this->GetGeoMesh()->Element(i);
        if(!gel)
        {
            std::cout << "Null pointer as geometric element\n";
            DebugStop();
        }
        CompElement *cel = CreateCompEl(gel, this, i);
        this->SetElement(i, cel);
    }
    this->Resequence();
}

void CompMesh::Resequence() {
    int64_t ndof = this->GetNumberDOF();
    int64_t fe = 0;

    for (int i = 0; i < ndof; i++) {
        this->GetDOF(i).SetFirstEquation(fe);
        int nshape = this->GetDOF(i).GetNShape();
        int nstate = this->GetDOF(i).GetNState();
        int result = nshape * nstate;
        fe += result;
    }
    solution.resize(fe);
    solution.setZero();
}

void CompMesh::Resequence(VecInt &DOFindices) {
    int64_t ndofel = DOFindices.size();
    int nelem = this->GetElementVec().size();
    int64_t fe = 0;

    for (int i = 0; i < nelem; i++) {
        for (int j = 0; j < ndofel; j++) {
            int dofindex = DOFindices[j];
            this->GetDOF(dofindex).SetFirstEquation(fe);
            int nshape = this->GetDOF(dofindex).GetNShape();
            int nstate = this->GetDOF(dofindex).GetNState();
            int result = nshape * nstate;
            fe += result;
        }
    }
}

VecDouble &CompMesh::Solution() {
    return solution;
}

void CompMesh::LoadSolution(VecDouble &Sol) {
    solution = Sol;
//    solution.resize(Sol.size());
//    for (int64_t i = 0; i < Sol.size(); i++) {
//        solution[i] = Sol[i];
//    }
}

void CompMesh::Print(std::ostream & out) {
    out << "\n\t\tCOMPUTATIONAL MESH INFORMATION\n\n";

    out << "Number of DOFs:\t\t" << this->GetNumberDOF() << std::endl;
    out << "Number of elements:\t" << this->GetElementVec().size() << std::endl;
    out << "Number of materials:\t" << this->GetMathVec().size() << std::endl;
    out << "Dimension of the mesh:\t" << this->GetElementVec()[0]->Dimension() << std::endl;

    int64_t i;

    out << "\n\tDOFs Information" << std::endl;
    int64_t ndof = this->GetNumberDOF();
    for (i = 0; i < ndof; i++) {
        out << "DOF index: " << i << "\t\t";
        this->GetDOF(i).Print(*this,out);
    }
    
    out << "\n\tComputable Element Information" << std::endl;
    int64_t nelem = this->GetElementVec().size();
    for (i = 0; i < nelem; i++) {
        if (!this->GetElement(i)) continue;
        CompElement *cel = this->GetElement(i);
        out << "Element index: " << i << std::endl;
        cel->Print(out);
    }
    
    out << "\n\tMaterial Information" << std::endl;
    int64_t nummat = this->GetMathVec().size();
    for (i = 0; i < nummat; i++) {
        MathStatement *mat = this->GetMath(i);
        out << "Material index: " << i << std::endl;
        if (!mat) {
            DebugStop();
        }
        mat->Print(out);
    }
}
