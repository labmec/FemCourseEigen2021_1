/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "IntRule.h"
///\cond
#include <iostream> 
#include <vector> 
#include <cmath>
#include <iomanip>
///\endcond

using namespace std;

IntRule::IntRule() : fPoints(), fWeights(){
        fOrder = 0;


}

IntRule::IntRule(int order) {
    fOrder = order;
}

IntRule::~IntRule() {

}

IntRule::IntRule(const IntRule& copy) {
    fOrder = copy.fOrder;
    fPoints = copy.fPoints;
    fWeights = copy.fWeights;
}

IntRule &IntRule::operator=(const IntRule &cp) {
    fOrder = cp.fOrder;
    fPoints = cp.fPoints;
    fWeights = cp.fWeights;
    return *this;
}

int IntRule::NPoints() const {
    return fWeights.size();
}

void IntRule::Print(std::ostream &out) const {
    VecDouble co(fPoints.cols());
    double w;

    for (int i = 0; i < NPoints(); i++) {
        Point(i, co, w);

        int dim = co.size();

        out << "ponto: " << i << endl;

        for (int j = 0; j < dim; j++) {
            out << "coord: " << setprecision(10) << co[j] << endl;
        }
        out << "peso: " << setprecision(10) << w << endl << endl;
    }
}

void IntRule::Point(int p, VecDouble& co, double& w) const {
    int dim = co.size();

    for (int i = 0; i < dim; i++) {
        co[i] = fPoints(p, i);
    }
    w = fWeights[p];
}
