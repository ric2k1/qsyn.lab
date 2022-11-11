// /****************************************************************************
//   FileName     [ hrule.cpp ]
//   PackageName  [ simplifier ]
//   Synopsis     [ Hadamard Rule Definition ]
//   Author       [ Cheng-Hua Lu ]
//   Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
// ****************************************************************************/


#include <iostream>
#include <vector>
#include "zxRules.h"
using namespace std;

extern size_t verbose;


/**
 * @brief Matches all the H-boxes with arity 2 and phase 1, i.e. all the Hadamard gates.
 * 
 * @param g 
 */
void HRule::match(ZXGraph* g){
    _matchTypeVec.clear();
    if(verbose >= 8) g->printVertices();
    
    unordered_map<size_t, size_t> id2idx;
    // for(size_t i = 0; i < g->getNumVertices_depr(); i++) id2idx[g->getVertices_depr()[i]->getId()] = i;
    size_t cnt = 0;
    for(const auto& v: g->getVertices()){
        id2idx[v->getId()] = cnt;
        cnt++;
    }
    // Find all H-boxes
    vector<bool> taken(g->getNumVertices(), false);
    vector<bool> inMatches(g->getNumVertices(), false);

    for(const auto& v: g->getVertices()){
        if(v->getType() == VertexType::H_BOX && v->getNumNeighbors() == 2){
            NeighborPair nbp0 = *(v->getNeighbors().begin());
            NeighborPair nbp1 = *next(v->getNeighbors().begin());
            size_t n0 = id2idx[nbp0.first->getId()], n1 = id2idx[nbp1.first->getId()];
            if(taken[n0] || taken[n1]) continue;
            if(!inMatches[n0] && !inMatches[n1]){
                _matchTypeVec.push_back(v);
                inMatches[id2idx[v->getId()]] = true;
                taken[n0] = true;
                taken[n1] = true;
            }
        }
    }
    // for(size_t i = 0; i < g->getNumVertices_depr(); i++){
    //     if(g->getVertices_depr()[i]->getType() == VertexType::H_BOX && g->getVertices_depr()[i]->getNumNeighbors_depr() == 2){
    //         vector<ZXVertex*> neighbors = g->getVertices_depr()[i]->getNeighbors_depr();
    //         size_t n0 = id2idx[neighbors[0]->getId()], n1 = id2idx[neighbors[1]->getId()];
    //         if(taken[n0] || taken[n1]) continue;
    //         if(!inMatches[n0] && !inMatches[n1]){
    //             _matchTypeVec.push_back(g->getVertices_depr()[i]);
    //             inMatches[id2idx[g->getVertices_depr()[i]->getId()]] = true;
    //             taken[n0] = true;
    //             taken[n1] = true;
    //         }
    //     }
    // }
    setMatchTypeVecNum(_matchTypeVec.size());
}

/**
 * @brief Convert v in matches to Hadamard edge.
 * 
 * @param g 
 */
void HRule::rewrite(ZXGraph* g){
    reset();
    setRemoveVertices(_matchTypeVec);
    
    for(size_t i = 0; i < _matchTypeVec.size(); i++){
        // Only two neighbors which is ensured
        vector<ZXVertex*> ns; vector<EdgeType> ets;

        for(auto& itr : _matchTypeVec[i]->getNeighbors()){
            ns.push_back(itr.first);
            ets.push_back(itr.second);
        } 
        
        _edgeTableKeys.push_back(make_pair(ns[0], ns[1]));
        if(ets[0] == ets[1]) _edgeTableValues.push_back(make_pair(0,1));
        else _edgeTableValues.push_back(make_pair(1,0));

        //! TODO Correct for the sqrt(2) difference in H-boxes and H-edges
    }
}

