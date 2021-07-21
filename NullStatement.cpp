/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "NullStatement.h"

///\cond
#include <functional>
#include <string.h>
///\endcond

NullStatement::NullStatement() {
}

NullStatement::NullStatement(int materialid) {
    this->SetMatID(materialid);
}

NullStatement::NullStatement(const NullStatement &copy) : MathStatement(copy) {
}

NullStatement &NullStatement::operator=(const NullStatement &copy) {
    MathStatement::operator=(copy);
    return *this;
}

NullStatement *NullStatement::Clone() const {
    return new NullStatement(*this);
}

NullStatement::~NullStatement() {
}



int NullStatement::VariableIndex(const PostProcVar var) const {
    if (var == ENone) return ENone;
    if (var == ESol) return ESol;
    if (var == EDSol) return EDSol;
    // Code should not reach this point. This return is only here to stop compiler warnings.
    DebugStop();
    return -1;
}

NullStatement::PostProcVar NullStatement::VariableIndex(const std::string &name) {
    if (!strcmp("Sol", name.c_str())) return ESol;
    if (!strcmp("DSol", name.c_str())) return EDSol;
    else {
        std::cout << "variable not implemented" << std::endl;
    }
    // Code should not reach this point. This return is only here to stop compiler warnings.
    DebugStop();
    return ENone;
}

int NullStatement::NSolutionVariables(const PostProcVar var) {
    if (var == ESol) return this->NState();
    if (var == EDSol) return this->Dimension();
    else {
        std::cout << "variable not implemented" << std::endl;
    }
    // Code should not reach this point. This return is only here to stop compiler warnings.
    DebugStop();
    return -1;
}


void NullStatement::Contribute(IntPointData &data, double weight, MatrixDouble &EK, MatrixDouble &EF) const {
    EK.setZero();
    EF.setZero();
}

void NullStatement::PostProcessSolution(const IntPointData &data, const int var, VecDouble &Solout) const {
    VecDouble sol = data.solution;
    int solsize = sol.size();
    int rows = data.dsoldx.rows();
    int cols = data.dsoldx.cols();
    MatrixDouble gradu(rows, cols);
    gradu = data.dsoldx;

    int nstate = this->NState();

    switch (var) {
        case 0: //None
        {
            std::cout << " Var index not implemented " << std::endl;
            DebugStop();
        }

        case 1: //ESol
        {
            Solout.resize(nstate);
            for (int i = 0; i < nstate; i++) {
                Solout[i] = sol[i];
            }
        }
            break;

        case 2: //EDSol
        {
            Solout.resize(rows * cols);
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    Solout[i * cols + j] = gradu(i, j);
                }
            }

        }
            break;

        default:
        {
            std::cout << " Var index not implemented " << std::endl;
            DebugStop();
        }
    }
}

