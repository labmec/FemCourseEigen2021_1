/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Analysis.h"
#include "Assemble.h"
#include "CompMesh.h"
#include "GeoMesh.h"
#include "CompElement.h"
#include "GeoElement.h"
#include "VTKGeoMesh.h"
#include "PostProcessTemplate.h"
#include <Eigen/SparseLU>

using namespace std;

Analysis::Analysis() {
    cmesh = 0;
    Solution.resize(0, 0);
    GlobalSystem.resize(0, 0);
    RightHandSide.resize(0, 0);

}

Analysis::Analysis(const Analysis &cp) {
    cmesh = cp.cmesh;
    Solution = cp.Solution;
    GlobalSystem = cp.GlobalSystem;
    RightHandSide = cp.RightHandSide;
}

Analysis &Analysis::operator=(const Analysis &cp) {
    cmesh = cp.cmesh;
    Solution = cp.Solution;
    GlobalSystem = cp.GlobalSystem;
    RightHandSide = cp.RightHandSide;
    return *this;
}

Analysis::~Analysis() {
}

Analysis::Analysis(CompMesh *mesh) {
    cmesh = mesh;
}

void Analysis::SetMesh(CompMesh *mesh) {
    cmesh = mesh;
}

CompMesh *Analysis::Mesh() const {
    return cmesh;
}

void Analysis::RunSimulation() {
    Assemble assemb(cmesh);

    int ne = assemb.NEquations();
    SparseMat K(ne, ne);
    MatrixDouble F(ne, 1);

    K.setZero();
    F.setZero();

    assemb.Compute(K, F);
    std::cout << "Assemble done!" << std::endl;

    GlobalSystem = K;
    RightHandSide = F;

    std::cout << "Computing solution..." << std::endl;
    
    SparseLU<SparseMat, COLAMDOrdering<int> >   solver;
    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern(K); 
    // Compute the numerical factorization
    solver.factorize(K);
    if(solver.lastErrorMessage().size()){
        std::cout << "\n\n=====> Solver has thrown the following error: " << std::endl;
        std::cout << solver.lastErrorMessage() << std::endl;
        DebugStop();
    }
    //Use the factors to solve the linear system 
    Solution = solver.solve(F); 

    std::cout << "Solution computed!" << std::endl;
    
    int solsize = Solution.rows();
    VecDouble sol(solsize);
    
    for (int i = 0; i < solsize; i++) {
        sol[i] = Solution(i, 0);
    }
    cmesh->LoadSolution(sol);
}

void Analysis::PostProcessSolution(const std::string &filename, PostProcess &defPostProc) const {
    VTKGeoMesh::PrintSolVTK(cmesh, defPostProc, filename);
}

VecDouble Analysis::PostProcessError(std::ostream &out, PostProcess &defPostProc) const {
    std::cout << "Computing error..." << endl;

    VecDouble values(10);
    VecDouble errors(10);
    values.setZero();
    errors.setZero();
    std::function<void (const VecDouble &loc, VecDouble &result, MatrixDouble & deriv) > fExact;

    int64_t nel = cmesh->GetElementVec().size();
    GeoMesh *gmesh = cmesh->GetGeoMesh();
    int dim = gmesh->Dimension();

    for (int64_t i = 0; i < nel; i++) {
        CompElement *el = cmesh->GetElement(i);
	GeoElement *gel = el->GetGeoElement();
	if(gel->Dimension() != dim) continue;
        if (el) {
            if (el->GetStatement()->GetMatID() == 1) {
                errors.setZero();
                fExact = defPostProc.GetExact();
                el->EvaluateError(fExact, errors);
                int nerrors = errors.size();
                if(values.size() != nerrors)
                {
                	values.resize(nerrors);
                	values.setZero();
                }
                for (int ier = 0; ier < nerrors; ier++) {
                    values[ier] += errors[ier] * errors[ier];
                }
            }
        }
    }

    int nerrors = errors.size();
    VecDouble ervec(nerrors);
    ervec.setConstant(-10.);

    if (nerrors < 3) {
        out << endl << "Analysis::PostProcessError - At least 3 norms are expected." << endl;
        for (int ier = 0; ier < nerrors; ier++)
            out << endl << "error " << ier << "  = " << sqrt(values[ier]);
    } else {
        out << "\n# Error #" << endl;
        out << "L2-Norm (u): " << sqrt(values[0]) << endl;
        out << "L2-Norm (grad u):" << sqrt(values[1]) << endl;
        out << "H1-Norm (u): " << sqrt(values[2]) << endl;
        for (int ier = 3; ier < nerrors; ier++)
            out << "other norms = " << sqrt(values[ier]) << endl;
    }
    // Returns the calculated errors.
    for (int i = 0; i < nerrors; i++) {
        ervec[i] = sqrt(values[i]);
    }


    return ervec;
}

