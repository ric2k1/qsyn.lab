/****************************************************************************
  FileName     [ zxTraverse.cpp ]
  PackageName  [ graph ]
  Synopsis     [ Traversal functions for ZX ]
  Author       [ Chin-Yi Cheng ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include <algorithm>
#include "zxGraph.h"
#include "util.h"

using namespace std;
extern size_t verbose;

void ZXGraph::updateTopoOrder()
{
    _topoOrder.clear();
    _globalDFScounter++;
    for (size_t i = 0; i < _inputs_depr.size(); i++){
        if (!_inputs_depr[i]->isVisited(_globalDFScounter))
            DFS(_inputs_depr[i]);
    }
    for (size_t i = 0; i < _outputs_depr.size(); i++){
        if (!_outputs_depr[i]->isVisited(_globalDFScounter))
            DFS(_outputs_depr[i]);
    }
    reverse(_topoOrder.begin(), _topoOrder.end());
    if (verbose >= 7) {
        cout << "Topological order from first input: ";
        for (size_t j = 0; j < _topoOrder.size(); j++){
            cout << _topoOrder[j]->getId() << " ";
        }
        cout << "\nSize of topological order: " << _topoOrder.size() << endl;
    }
    // assert(_topoOrder.size() == _vertices.size());
}
void ZXGraph::DFS(ZXVertex *currentVertex)
{
    currentVertex->setVisited(_globalDFScounter);
    // Original
    // vector<NeighborPair_depr> neighbors = currentVertex->getNeighbors();
    // for (size_t i = 0; i < neighbors.size(); i++){
    //     if (!(neighbors[i].first->isVisited(_globalDFScounter)))
    //         DFS(neighbors[i].first);
    // }

    // NeighberMap
    NeighborMap_depr neighborMap = currentVertex->getNeighborMap();
    for(auto itr = neighborMap.begin(); itr != neighborMap.end(); itr++){
        if(!(itr->first->isVisited(_globalDFScounter))) DFS(itr->first);
    }

    _topoOrder.push_back(currentVertex);
}