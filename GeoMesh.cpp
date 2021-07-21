/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "GeoMesh.h"
#include "GeoElementSide.h"
///\cond
#include <vector>
#include <stdio.h>
///\endcond

GeoMesh::GeoMesh() : Nodes(0), Elements(0) {
    Reference = 0;
    fDim = -1;
}

GeoMesh::GeoMesh(const GeoMesh &copy) {
    Elements = copy.Elements;
    Nodes = copy.Nodes;
    Reference = copy.Reference;
    fDim = copy.fDim;
}

GeoMesh &GeoMesh::operator=(const GeoMesh &copy) {

    int nnodes = copy.Nodes.size();
    this->Nodes.resize(nnodes);
    for (int64_t inodes = 0; inodes < nnodes; inodes++)
        this->Nodes[inodes] = copy.Nodes[inodes];

    this->Elements.resize(copy.Elements.size());
    for (int64_t iel = 0; iel < copy.Elements.size(); iel++)
        this->Elements[iel] = copy.Elements[iel];

    return *this;
}

void GeoMesh::SetNumNodes(int nnodes) {
    Nodes.resize(nnodes);
}

void GeoMesh::SetNumElements(int numelements) {
    Elements.resize(numelements);
}

int64_t GeoMesh::NumNodes() const {
    return Nodes.size();
}

int64_t GeoMesh::NumElements() const {
    return Elements.size();
}

const GeoNode &GeoMesh::Node(int64_t node) const {
    if(node <0 || node >= Nodes.size()) DebugStop();
    return Nodes[node];
}

GeoNode &GeoMesh::Node(int64_t node) {
    return Nodes[node];
}

void GeoMesh::SetElement(int64_t elindex, GeoElement *gel) {
    if(elindex >= Elements.size()) DebugStop();
    Elements[elindex] = gel;
}

GeoElement *GeoMesh::Element(int64_t elindex) const {
    if(elindex < 0 || elindex >= Elements.size()) DebugStop();
    return Elements[elindex];
}

void GeoMesh::BuildConnectivity() {
    // for each node : an element index / side connected to the node
    // if the value == -1, the datastructure hasn't been initialized yet
    auto numnodes = NumNodes();
    VecInt sides(numnodes);
    sides.setConstant(-1);
    VecInt vetor(numnodes);
    vetor.setConstant(-1);


    int64_t nelem = this->NumElements();
    int64_t iel = 0;

    // reset the connectivity
    for (iel = 0; iel < nelem; iel++) {
        GeoElement *gel = Elements[iel];
        if (!gel) continue;
        int nsides = gel->NSides();
        for(int is=0; is<nsides; is++)
        {
            GeoElementSide gelside(gel,is);
            gel->SetNeighbour(is, gelside);
        }
    }

    // set the connectivity along the nodes
    for (iel = 0; iel < nelem; iel++) {
        GeoElement *gel = Elements[iel];
        if (!gel) continue;
        int ncor = gel->NCornerNodes();
        int in = 0;
        for (in = 0; in < ncor; in++) {
            int64_t nodeindex = gel->NodeIndex(in);
            if (vetor[nodeindex] == -1) {
                vetor[nodeindex] = iel;
                sides[nodeindex] = in;

            } else {
                GeoElementSide one(gel, in);
                GeoElementSide two(Element(vetor[nodeindex]), sides[nodeindex]);

                GeoElementSide neighbour = one.Neighbour();
                if (neighbour.Element() == 0) DebugStop();

                if (!two.IsNeighbour(one)) {
                    one.IsertConnectivity(two);
                }
            }
        }
    }
    // at this point all neighbours along the nodes have been initialized
    
    // the line, triangle or quad neighbours are identified by the intersection of neighbours along nodes
    for (iel = 0; iel < nelem; iel++) {
        GeoElement *gel = Elements[iel];
        if (!gel) continue;
        int ncor = gel->NCornerNodes();
        int nsides = gel->NSides();
        int is;
        for (is = ncor; is < nsides; is++) {
            GeoElementSide gelside(gel, is);
            GeoElementSide neigh = gelside.Neighbour();
            if(gelside != neigh)
            {
                continue;
            }
            // the neighbours are the intersection of all neighbours along the nodes
            std::vector<GeoElementSide> neighbours;
            gelside.ComputeNeighbours(neighbours);
            int64_t nneigh = neighbours.size();
//            for(int in=0; in<nneigh; in++)
//            {
//                neighbours[in].Print(std::cout);
//            }
//            std::cout << std::endl;
            for (int in = 0; in < nneigh; in++) {
                gelside.IsertConnectivity(neighbours[in]);
            }
        }
    }
}

void GeoMesh::Print(std::ostream &out) const {
    out << "\n\t\tGEOMETRIC MESH INFORMATION\n\n";
    out << "Number of nodes:\t" << this->NumNodes() << std::endl;
    out << "Number of elements:\t" << this->NumElements() << std::endl;

    out << "\n\tNode Information" << std::endl;
    for (int i = 0; i < this->NumNodes(); i++) {
        out << "Node index: " << i << "\t\t";
        this->Node(i).Print(out);
    }

    out << "\n\tElement Information" << std::endl;
    for (int i = 0; i < this->NumElements(); i++) {
        out << "Element index:\t\t" << i << std::endl;
        this->Element(i)->Print(out);
        out << std::endl;
    }
}
