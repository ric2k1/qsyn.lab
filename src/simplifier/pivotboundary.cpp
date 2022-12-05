/****************************************************************************
  FileName     [ pivot.cpp ]
  PackageName  [ simplifier ]
  Synopsis     [ Pivot Rule Definition ]
  Author       [ Cheng-Hua Lu ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <numbers>
#include <vector>

#include "zxRules.h"
using namespace std;

extern size_t verbose;

void PivotBoundary::preprocess(ZXGraph* g) {
    for (auto& v : this->_boundaries) {
        auto& [nb, etype] = v->getFirstNeighbor();
        g->addBuffer(v, nb, etype);
    }
    for (auto& m : this->_matchTypeVec) {
        if (!m[0]->hasNPiPhase()) g->transferPhase(m[0]);
        if (!m[1]->hasNPiPhase()) g->transferPhase(m[1]);
    }
}

/**
 * @brief Finds matchings of the pivot-boundary rule.
 *        Precondition: the graph is graph-like
 *
 * @param g
 */
void PivotBoundary::match(ZXGraph* g) {
    this->_matchTypeVec.clear(); 
    this->_boundaries.clear();
    if(verbose >= 8) g->printVertices();
    
    unordered_set<ZXVertex*> taken;
    vector<ZXVertex*> b0;
    auto matchBoundary = [&taken, &b0, this](ZXVertex* v){

        b0.clear();
        ZXVertex* vs = v->getFirstNeighbor().first;
        if (taken.contains(vs)) return;

        if (!vs->isZ()) {
            taken.insert(vs);
            return;
        }

        ZXVertex* vt = nullptr;
        for (auto& [nb, etype] : vs->getNeighbors()) {
            if (nb->isBoundary()) continue;
            if (!nb->hasNPiPhase()) continue;
            if (etype != EdgeType::HADAMARD) continue;
            vt = nb;
            break;
        }
        if (vt == nullptr) return;

        // check vs is only connected to boundary, or connected to Z-spider by H-edge
        for (auto& [nb, etype] : vs->getNeighbors()) {
            if (nb->isBoundary()) {
                b0.push_back(nb);
                continue;
            }
            if (!nb->isZ() || etype != EdgeType::HADAMARD) return;
        }
        // check vt is only connected to Z-spider by H-edge
        for (auto& [nb, etype] : vt->getNeighbors()) {
            if (!nb->isZ() || etype != EdgeType::HADAMARD) return;
        }
        if (b0.size() > 1) return;

        taken.insert(vs);
        taken.insert(vt);
        for (auto& [nb, _] : vs->getNeighbors()) taken.insert(nb);
        for (auto& [nb, _] : vt->getNeighbors()) taken.insert(nb);

        this->_matchTypeVec.push_back({vs, vt});
        this->_boundaries.insert(this->_boundaries.end(), b0.begin(), b0.end());

    };

    for (auto& v : g->getInputs())  matchBoundary(v);
    for (auto& v : g->getOutputs()) matchBoundary(v);

    setMatchTypeVecNum(_matchTypeVec.size());
}