/****************************************************************************
  FileName     [ pivotgadget.cpp ]
  PackageName  [ simplifier ]
  Synopsis     [ Pivot Gadget Rule Definition ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cstddef>  // for size_t
#include <iostream>

#include "gFlow.h"
#include "zxGraph.h"
#include "zxRules.h"

using namespace std;

extern size_t verbose;

/**
 * @brief Preprocess the matches so that it conforms with the rewrite functions
 *
 * @param g
 */
void PivotDegadget::preprocess(ZXGraph* g) {
    for (auto& v : _boundaries) {
        auto& [nb, etype] = v->getFirstNeighbor();
        g->addBuffer(v, nb, etype);
    }

    cout << "Finished Buffering Boundaries!" << endl;

    for (auto& [vt, vu] : _unfuseCandidates) {
        EdgeType etype = vt->isNeighbor(vu, EdgeType::SIMPLE) ? EdgeType::SIMPLE : EdgeType::HADAMARD;
        assert(vt->isNeighbor(vu, etype));
        ZXVertex* buffer1 = g->addBuffer(vu, vt, etype);
        g->addBuffer(buffer1, vt, EdgeType::HADAMARD);
        buffer1->setPhase(vt->getPhase());
        vt->setPhase(Phase(0));
    }
    cout << "Finished Unfusing!" << endl;
}

/**
 * @brief Find matchings of the pivot-gadget rule. Find the targets with non-Clifford and gadgetize them.
 *        Preconditions: Graph-like and no lcomp candidates
 *
 * @param g
 */
void PivotDegadget::match(ZXGraph* g) {
    this->_matchTypeVec.clear();
    this->_boundaries.clear();
    this->_unfuseCandidates.clear();

    unordered_set<ZXVertex*> taken;

    GFlow gflow{g};
    g->printVertices();
    gflow.doExtendedGFlow(true);

    gflow.calculate();
    gflow.print();

    assert(gflow.isValid());

    using MP = GFlow::MeasurementPlane;

    cout << "Start Matching..." << endl;

    for (auto const& [vs, plane] : gflow.getMeasurementPlanes()) {
        if (taken.contains(vs) || plane != MP::YZ) continue;
        ZXVertex* vt = nullptr;
        ZXVertex* vu = nullptr;
        for (auto const& v : gflow.getZCorrectionSet(vs)) {
            if (taken.contains(v) || gflow.getMeasurementPlane(v) != MP::XY) continue;
            if (vt == nullptr || v->hasNPiPhase()) vt = v;  // vt with 0 or pi phase takes priority
        }
        if (vt == nullptr) continue;

        for (auto const& v : gflow.getXCorrectionSet(vt)) {
            if (taken.contains(v) || gflow.getMeasurementPlane(v) != MP::XY) continue;
            vu = v;
        }
        if (vu == nullptr) continue;

        taken.insert(vs);
        taken.insert(vt);

        for (auto v : {vs, vt}) {
            for (auto& [nb, _] : v->getNeighbors()) {
                taken.insert(nb);
                if (nb->isBoundary() && nb != vu) _boundaries.push_back(nb);
            }
        }

        cout << "{vs, vt, vu} = {" << vs->getId() << ", "
             << vt->getId() << ", "
             << vu->getId() << "}\n";

        _matchTypeVec.push_back({vs, vt});
        _unfuseCandidates.push_back({vt, vu});
    }
    cout << "Finished Matching!" << endl;

    setMatchTypeVecNum(this->_matchTypeVec.size());
}
