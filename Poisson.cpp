/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Poisson.h"
///\cond
#include <functional>
#include <string.h>
///\endcond

Poisson::Poisson() {
}

Poisson::Poisson(int materialid, MatrixDouble &perm) {
    permeability = perm;
    this->SetMatID(materialid);
}

Poisson::Poisson(const Poisson &copy) {
    permeability = copy.permeability;
    forceFunction = copy.forceFunction;
}

Poisson &Poisson::operator=(const Poisson &copy) {
    permeability = copy.permeability;
    forceFunction = copy.forceFunction;
    return *this;
}

Poisson *Poisson::Clone() const {
    return new Poisson(*this);
}

Poisson::~Poisson() {
}

MatrixDouble Poisson::GetPermeability() const {
    return permeability;
}

void Poisson::SetPermeability(const MatrixDouble &perm) {
    permeability = perm;
}

int Poisson::NEvalErrors() const {
    return 3;
}

int Poisson::VariableIndex(const PostProcVar var) const {
    return int(var);
}

Poisson::PostProcVar Poisson::VariableIndex(const std::string &name) {
    if (!strcmp("Sol", name.c_str())) return ESol;
    if (!strcmp("DSol", name.c_str())) return EDSol;
    if (!strcmp("Flux", name.c_str())) return EFlux;
    if (!strcmp("Force", name.c_str())) return EForce;
    if (!strcmp("SolExact", name.c_str())) return ESolExact;
    if (!strcmp("DSolExact", name.c_str())) return EDSolExact;
    else {
        std::cout << "variable "<< name << " not implemented" << std::endl;
    }
    // Code should not reach this point. This return is only here to stop compiler warnings.
    DebugStop();
    return ENone;
}

int Poisson::NSolutionVariables(const PostProcVar var) {
    if (var == ESol) return this->NState();
    if (var == EDSol) return 3;
    if (var == EFlux) return 3;
    if (var == EForce) return this->NState();
    if (var == ESolExact) return this->NState();
    if (var == EDSolExact) return 3;
    else {
        std::cout << "variable not implemented" << std::endl;
    }
    // Code should not reach this point. This return is only here to stop compiler warnings.
    DebugStop();
    return -1;
}

void Poisson::ContributeError(IntPointData &data, VecDouble &u_exact, MatrixDouble &du_exact, VecDouble &errors) const {
    errors.resize(NEvalErrors());
    errors.setZero();
    MatrixDouble gradu;
    MatrixDouble axes = data.axes;

    VecDouble u = data.solution;
    MatrixDouble dudx = data.dsoldx;

    this->Axes2XYZ(dudx, gradu, axes);

    double diff = 0.0;
    for (int i = 0; i < this->NState(); i++) {
        diff = (u[i] - u_exact[i]);
        errors[0] += diff*diff;
    }

    errors[1] = 0.;
    int dim = this->Dimension();
    int nstate = this->NState();
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < nstate; j++) {
            diff = (gradu(i, j) - du_exact(i, j));
            errors[1] += diff*diff;
        }

    }
    errors[2] = errors[0] + errors[1];
}

void Poisson::Contribute(IntPointData &data, double weight, MatrixDouble &EK, MatrixDouble &EF) const {

    VecDouble phi = data.phi;
    MatrixDouble dphi = data.dphidx;
    MatrixDouble axes = data.axes;
    MatrixDouble dphi2, dphi3;

    dphi2 = data.axes.transpose()*data.dphidx;
    dphi3 = dphi2.transpose();

    MatrixDouble perm(3, 3);
    perm = this->GetPermeability();
    double res = 0.;

    auto force = this->GetForceFunction();
    if(force)
    {
        VecDouble resloc(1);
        force(data.x, resloc);
        res = resloc[0];
    }

    //+++++++++++++++++
    // Please implement me
    std::cout << "\nPLEASE IMPLEMENT ME\n" << __PRETTY_FUNCTION__ << std::endl;
    DebugStop();
    //+++++++++++++++++
}

void Poisson::PostProcessSolution(const IntPointData &data, const int var, VecDouble &Solout) const {
    MatrixDouble gradudx, flux;
    gradudx = data.axes.transpose()*data.dsoldx;
    flux = -permeability*gradudx;
    
    int nstate = this->NState();
    if(nstate != 1) DebugStop();

    switch (var) {
        case 0: //None
        {
            std::cout << " Var index not implemented " << std::endl;
            DebugStop();
        }

        case 1: //ESol
        {
            Solout = data.solution;
        }
            break;

        case 2: //EDSol
        {
            //+++++++++++++++++
            // Please implement me
            // DebugStop();
            Solout.resize(3);
            for (int i = 0; i < 3; i++) {
                Solout[i] = gradudx(i, 0);
            }
            //+++++++++++++++++
        }
            break;
        case 3: //EFlux
        {
            //+++++++++++++++++
            // Please implement me
            // DebugStop();
            Solout.resize(3);
            for (int i = 0; i < 3; i++) {
                Solout[i] = flux(i, 0);
            }
        }
            break;

        case 4: //EForce
        {
            //+++++++++++++++++
            // Please implement me
            // DebugStop();
            Solout.resize(nstate);
            if(forceFunction) this->forceFunction(data.x, Solout);
            else Solout.setZero();
            //+++++++++++++++++
        }
            break;

        case 5: //ESolExact
        {
            //+++++++++++++++++
            // Please implement me
            // DebugStop();
            Solout.resize(nstate);
            VecDouble sol(nstate);
            MatrixDouble dsol(3, nstate);
            if(SolutionExact) this->SolutionExact(data.x, Solout, dsol);
            else Solout.setZero();
            //+++++++++++++++++
        }
            break;
        case 6: //EDSolExact
        {
            //+++++++++++++++++
            // Please implement me
            // DebugStop();
            Solout.resize(3);
            VecDouble sol(nstate);
            MatrixDouble dsol(3, nstate);
            if(SolutionExact) this->SolutionExact(data.x, sol, dsol);
            else dsol.setZero();

            for (int i = 0; i < 3; i++) {
                Solout[i] = dsol(i, 0);
            }
            //+++++++++++++++++
        }
            break;
        default:
        {
            std::cout << " Var index not implemented " << std::endl;
            DebugStop();
        }
    }   
}

double Poisson::Inner(MatrixDouble &S, MatrixDouble & T) const {
    double inner = 0;
    for (int i = 0; i < S.rows(); i++) {
        for (int j = 0; j < S.cols(); j++) {
            inner += S(i, j) * T(i, j);
        }
    }
    return inner;
}
