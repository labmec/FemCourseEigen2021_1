/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "GeoMesh.h"
#include "GeoElementSide.h"
///\cond
#include <stdio.h>
#include <algorithm>
///\endcond


GeoElementSide::GeoElementSide() {

}

GeoElementSide::GeoElementSide(const GeoElementSide &copy) {
    fElement = copy.fElement;
    fSide = copy.fSide;
}

GeoElementSide &GeoElementSide::operator=(const GeoElementSide &copy) {
    fElement = copy.fElement;
    fSide = copy.fSide;
    return *this;
}

GeoElementSide GeoElementSide::Neighbour() const {
    return fElement ? fElement->Neighbour(fSide) : GeoElementSide();
}

void GeoElementSide::SetNeighbour(const GeoElementSide &neighbour) {
    fElement->SetNeighbour(fSide, neighbour);
}

bool GeoElementSide::IsNeighbour(const GeoElementSide &candidate) const {
    if (candidate == *this) return true;
    GeoElementSide neighbour = Neighbour();
    if (!(neighbour.Element() != 0 && neighbour.Side() > -1)) return false;
    while ((neighbour == *this) == false) {
        if (candidate == neighbour) return true;
        neighbour = neighbour.Neighbour();
    }
    return false;
}

void GeoElementSide::IsertConnectivity(GeoElementSide &candidate) {
    if (this->IsNeighbour(candidate)) return;
    GeoElementSide neigh1 = Neighbour();
    GeoElementSide neigh2 = candidate.Neighbour();
    SetNeighbour(neigh2);
    candidate.SetNeighbour(neigh1);
}

void GeoElementSide::AllNeighbours(std::vector<GeoElementSide> &allneigh) const {
    
    if(fElement->NSideNodes(fSide) != 1) DebugStop();
    GeoElementSide neigh = Neighbour();

    int nodeindex = fElement->SideNodeIndex(fSide, 0);
    while (neigh != *this) {
        int neighnode = neigh.fElement->SideNodeIndex(neigh.fSide, 0);
        if(neighnode != nodeindex) DebugStop();
        allneigh.push_back(neigh);
        neigh = neigh.Neighbour();
    }
}

void GeoElementSide::ComputeNeighbours(std::vector<GeoElementSide> &compneigh) {
    if (fSide < fElement->NCornerNodes()) {
        AllNeighbours(compneigh);
        return;
    }
    int nsnodes = fElement->NSideNodes(fSide);
    std::vector<GeoElementSide> GeoElSideSet;
    std::vector<std::vector<int> > GeoElSet;
    GeoElSet.resize(nsnodes);
    int in;
    VecInt nodeindexes(nsnodes);
    for (in = 0; in < nsnodes; in++) {
        int nodeindex = fElement->SideNodeIndex(fSide, in);
        nodeindexes[in] = nodeindex;
    }
    // compute the neighbours along the cornernodes
    for (in = 0; in < nsnodes; in++) {
        int locindex = fElement->SideNodeLocIndex(fSide, in);
        int nodeindex = fElement->SideNodeIndex(fSide, in);
        GeoElSideSet.resize(0);
        GeoElementSide locside(fElement, locindex);
        
        int nodeindex_again = locside.Element()->SideNodeIndex(locside.Side(), 0);
        if(nodeindex != nodeindex_again) DebugStop();
        
        locside.AllNeighbours(GeoElSideSet);
        for(auto &gelside : GeoElSideSet)
        {
            int node = gelside.Element()->SideNodeIndex(gelside.Side(), 0);
            if(node != nodeindex) DebugStop();
        }
        int nel = GeoElSideSet.size();
        int el;
        for (el = 0; el < nel; el++) {
            GeoElSet[in].push_back(GeoElSideSet[el].Element()->GetIndex());
        }
        std::sort(GeoElSet[in].begin(), GeoElSet[in].end());
    }
    std::vector<int> result;
    std::vector<int> result_aux;
    // build the neighbours along the higher dimension sides
    switch (nsnodes) {
        case 1:
        {
            result = GeoElSet[0];
        }
            break;
        case 2:
            std::set_intersection(GeoElSet[0].begin(), GeoElSet[0].end(), GeoElSet[1].begin(), GeoElSet[1].end(), std::back_inserter(result));

            break;
        case 3:
            std::set_intersection(GeoElSet[0].begin(), GeoElSet[0].end(), GeoElSet[1].begin(), GeoElSet[1].end(), std::back_inserter(result_aux));
            std::set_intersection(result_aux.begin(), result_aux.end(), GeoElSet[2].begin(), GeoElSet[2].end(), std::back_inserter(result));
            break;
        case 4:
        {
            std::vector<int> inter1, inter2;
            std::set_intersection(GeoElSet[0].begin(), GeoElSet[0].end(), GeoElSet[2].begin(), GeoElSet[2].end(), std::back_inserter(inter1));
            if (inter1.size() == 0) break;
            std::set_intersection(GeoElSet[1].begin(), GeoElSet[1].end(), GeoElSet[3].begin(), GeoElSet[3].end(), std::back_inserter(inter2));
            if (inter2.size() == 0) break;
            std::set_intersection(inter1.begin(), inter1.end(), inter2.begin(), inter2.end(), std::back_inserter(result));
            inter1.clear();
            inter2.clear();
        }
            break;
        default:
        {
            std::vector<int> inter1, inter2;
            inter1 = GeoElSet[0];
            for (in = 0; in < nsnodes - 1; in++) {
                std::set_intersection(inter1.begin(), inter1.end(), GeoElSet[in + 1].begin(), GeoElSet[in + 1].end(), std::back_inserter(inter2));
                if (inter2.size() == 0) break;
                inter1 = inter2;
            }
            inter1.clear();
            inter2.clear();
            result = inter2;
        }
    }
    int el, nel = result.size();
    GeoMesh * geoMesh = fElement->GetMesh();
    for (el = 0; el < nel; el++) {
        GeoElement * gelResult = geoMesh->Element(result[el]);
        int whichSd = gelResult->WhichSide(nodeindexes);
        if(whichSd < 0)
        {
            std::cout << "nodeindexes " << nodeindexes << std::endl;
            std::cout << "neighbouring element index " << gelResult->GetIndex() << std::endl;

            std::cout << "neighbouring element nodes ";
            for(int i=0; i<gelResult->NNodes(); i++) std::cout << gelResult->NodeIndex(i) << " ";
            std::cout << std::endl;
            for(int i=0; i<nsnodes; i++)
            {
                auto gelset = GeoElSet[i];
                std::cout << "GeoElSet along node " << i << " is ";
                for(auto g : gelset) std::cout << g << " ";
                std::cout << std::endl;
            }
            DebugStop();
        }
        GeoElementSide gelside(gelResult,whichSd);
        if(gelside == *this) continue;
        if (whichSd > 0) {
            compneigh.push_back(GeoElementSide(gelResult, whichSd));
        }
    }
    GeoElSideSet.clear();
    GeoElSet.clear();
    nodeindexes.resize(0);
    result.clear();
    result_aux.clear();


}

// Print the element index and side
void GeoElementSide::Print(std::ostream &out) const
{
    if(!fElement)
    {
        out << "GeoElSide empty\n";
    }
    else
    {
        out << "elid " << fElement->GetIndex() << " side " << fSide << std::endl;
    }
}

