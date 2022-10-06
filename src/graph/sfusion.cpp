/****************************************************************************
  FileName     [ sfusion.cpp ]
  PackageName  [ graph ]
  Synopsis     [ Spider Fusion Rule Definition ]
  Author       [ Cheng-Hua Lu ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/


#include <iostream>
#include <vector>
#include "zxRules.h"
using namespace std;

extern size_t verbose;

/**
 * @brief Finds non-interacting matchings of the spider fusion rule.
 *        (Check PyZX/pyzx/rules.py/match_spider_parallel for more details)
 * 
 * @param g 
 */
void SpiderFusion::match(ZXGraph* g){
  _matchTypeVec.clear();
  vector<EdgePair> Edges = g->getEdges();
  unordered_map<EdgePair, size_t> Edge2idx;
  for(size_t i = 0; i < g->getNumEdges(); i++) Edge2idx[makeEdgeKey(Edges[i])] = i;
  vector<bool> validEdge(g->getNumEdges(), true);
  
  for(size_t i = 0; i < g->getNumEdges(); i++){
    // for(size_t k=0;k<validEdge.size();k++){
    //   cout << validEdge[k] << " ";
    // }
    if(!validEdge[Edge2idx[makeEdgeKey(Edges[i])]]) {
      if(verbose >=7) cout << Edges[i].first.first->getId() << "--" << Edges[i].first.second->getId() << " is discarded." << endl;
      continue;
    }
    validEdge[Edge2idx[makeEdgeKey(Edges[i])]] = false;
    if(*(Edges[i].second) != EdgeType::SIMPLE) continue;
    ZXVertex* v0 = Edges[i].first.first;
    ZXVertex* v1 = Edges[i].first.second;
    if( (v0 -> getType() == v1 -> getType()) && (v0 -> getType() == VertexType::X || v0 -> getType() == VertexType::Z)){
      NeighborMap v0n = v0->getNeighborMap();
      for(auto itr = v0n.begin(); itr!=v0n.end(); ++itr){
        validEdge[Edge2idx[makeEdgeKey(v0, itr->first, itr->second)]] = false;
      }
      NeighborMap v1n = v1->getNeighborMap();
      for(auto itr = v1n.begin(); itr!=v1n.end(); ++itr){
        validEdge[Edge2idx[makeEdgeKey(v1, itr->first, itr->second)]] = false;
      }
      _matchTypeVec.push_back(make_pair(v0, v1));
    }
  }
  setMatchTypeVecNum(_matchTypeVec.size());
  // while (num == -1 or i < num) and len(candidates) > 0:
  //       e = candidates.pop()
  //       if g.edge_type(e) != EdgeType.SIMPLE: continue
  //       v0, v1 = g.edge_st(e)
  //       v0t = types[v0]
  //       v1t = types[v1]
  //       if (v0t == v1t and vertex_is_zx(v0t)):
  //               i += 1
  //               for v in g.neighbors(v0):
  //                   for c in g.incident_edges(v): candidates.discard(c)
  //               for v in g.neighbors(v1):
  //                   for c in g.incident_edges(v): candidates.discard(c)
  //               m.append((v0,v1))
  //   return m
}

/**
 * @brief Generate Rewrite format from `_matchTypeVec`
 *        (Check PyZX/pyzx/rules.py/spider for more details)
 * 
 * @param g 
 */
void SpiderFusion::rewrite(ZXGraph* g){
    reset();
    //TODO: Rewrite _removeVertices, _removeEdges, _edgeTableKeys, _edgeTableValues
    //* _removeVertices: all ZXVertex* must be removed from ZXGraph this cycle.
    //* _removeEdges: all EdgePair must be removed from ZXGraph this cycle.
    //* (EdgeTable: Key(ZXVertex* vs, ZXVertex* vt), Value(int s, int h))
    //* _edgeTableKeys: A pair of ZXVertex* like (ZXVertex* vs, ZXVertex* vt), which you would like to add #s EdgeType::SIMPLE between them and #h EdgeType::HADAMARD between them
    //* _edgeTableValues: A pair of int like (int s, int h), which means #s EdgeType::SIMPLE and #h EdgeType::HADAMARD
    for(size_t i=0; i<_matchTypeVec.size(); i++){
      // Merge phase
      _matchTypeVec[i].first -> setPhase(_matchTypeVec[i].first -> getPhase() + _matchTypeVec[i].second -> getPhase());
      
      // Merge
      ZXVertex* v0 = _matchTypeVec[i].first;
      ZXVertex* v1 = _matchTypeVec[i].second;
      vector<ZXVertex*> v1n = v1->getNeighbors();
      unordered_map<ZXVertex*,bool> done;
      done.clear();
      for(size_t i=0; i < v1n.size(); i++) done[v1n[i]] = false;
      for(size_t i=0; i < v1n.size(); i++){
        if(done[v1n[i]]) continue;
        NeighborMap neighbor = v1->getNeighborMap();
        auto neighborItr = neighbor.equal_range(v1n[i]);
        int hadamardcount = 0;
        int simplecount = 0;
        auto map = v1->getNeighborMap();
        // for (auto ittr = map.begin(); ittr != map.end(); ittr++){
        //   cout << ittr->first->getId() << " ** ";
        // }

        for(auto itr = neighborItr.first; itr != neighborItr.second; ++itr){
          if(*(itr->second) == EdgeType::HADAMARD) hadamardcount++;
          if(*(itr->second) == EdgeType::SIMPLE) simplecount++;
          // cout << "@@@@@@" << endl;
        }
        _edgeTableKeys.push_back(make_pair(v0, v1n[i]));
        _edgeTableValues.push_back(make_pair((v1n[i]==v0) ? 0 : simplecount, hadamardcount));
        _removeVertices.push_back(v1);

        // cout << _removeVertices[0] -> getId() << endl;
        done[v1n[i]] = true;
      }
    }

}


//? (Check PyZX/pyzx/rules.py/unspider if have done the above functions)
