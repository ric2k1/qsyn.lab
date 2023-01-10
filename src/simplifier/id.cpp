/****************************************************************************
  FileName     [ id.cpp ]
  PackageName  [ simplifier ]
  Synopsis     [ Identity Removal Rule Definition ]
  Author       [ Cheng-Hua Lu ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <vector>

#include "zxRules.h"
using namespace std;

extern size_t verbose;

/**
 * @brief Finds non-interacting identity vertices.
 *        (Check PyZX/pyzx/rules.py/match_ids_parallel for more details)
 *
 * @param g
 */
void IdRemoval::match(ZXGraph* g) {
    _matchTypeVec.clear();
    // if (verbose >= 8) g->printVertices();
    // size_t cnt = 0;

    unordered_set<ZXVertex*> taken;

    for (const auto& v : g->getVertices()) {
        if (taken.contains(v)) continue;

        const Neighbors& nebs = v->getNeighbors();
        if (v->getPhase() != Phase(0)) continue;
        if (v->getType() != VertexType::Z && v->getType() != VertexType::X) continue;
        if (nebs.size() != 2) continue;

        NeighborPair nbp0 = *(nebs.begin());
        NeighborPair nbp1 = *next(nebs.begin());

        EdgeType etype = (nbp0.second == nbp1.second) ? EdgeType::SIMPLE : EdgeType::HADAMARD;

        _matchTypeVec.emplace_back(v, nbp0.first, nbp1.first, etype);
        taken.insert(v);
        taken.insert(nbp0.first);
        taken.insert(nbp1.first);
    }
    setMatchTypeVecNum(_matchTypeVec.size());
}

/**
 * @brief Generate Rewrite format from `_matchTypeVec`
 *        (Check PyZX/pyzx/rules.py/remove_ids for more details)
 *
 * @param g
 */
void IdRemoval::rewrite(ZXGraph* g) {
    reset();

    for (const auto& [v, n0, n1, et] : _matchTypeVec) {
        _removeVertices.push_back(v);
        if (n0 == n1) {
            n0->setPhase(n0->getPhase() + Phase(1));
            continue;
        }
        _edgeTableKeys.emplace_back(n0, n1);
        if (et == EdgeType::SIMPLE) {
            _edgeTableValues.emplace_back(1, 0);
        } else {
            _edgeTableValues.emplace_back(0, 1);
        }
    }
}