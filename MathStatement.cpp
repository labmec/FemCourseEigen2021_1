/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "MathStatement.h"

double MathStatement::gBigNumber = 1.e12;

MathStatement::MathStatement() : MathDim(0) {
}

MathStatement::MathStatement(const MathStatement &copy) {
    matid = copy.matid;
}

MathStatement &MathStatement::operator=(const MathStatement &copy) {
    matid = copy.matid;
    return *this;
}

MathStatement::~MathStatement() {
}

void MathStatement::Axes2XYZ(const MatrixDouble &dudaxes, MatrixDouble &dudx, const MatrixDouble &axesv) const {
        MatrixDouble axesT;
        axesT = axesv.transpose();

        if (dudx.rows() != axesT.rows() || dudx.cols() != dudaxes.cols()) {
            dudx.resize(axesT.rows(), dudaxes.cols());
        }
        dudx = axesT*dudaxes;
}

void MathStatement::Print(std::ostream &out) {
    out << "Material: " << this->GetMatID() << std::endl;
    out << "Big number: " << gBigNumber << std::endl << std::endl;

}
