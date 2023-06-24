/****************************************************************************
  FileName     [ simplify.cpp ]
  PackageName  [ simplifier ]
  Synopsis     [ Define class Simplifier member functions ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include "simplify.h"

#include <cstddef>  // for size_t
#include <iomanip>
#include <iostream>
#include <map>

#include "gFlow.h"
#include "zxGraph.h"  // for ZXGraph

using namespace std;
extern size_t verbose;

/**
 * @brief Helper method for constructing simplification strategies based on the rules.
 *
 * @return int
 */
int Simplifier::simp() {
    if (_rule->getName() == "Hadamard Rule") {
        cerr << "Error: Please use `hadamardSimp` when using HRule." << endl;
        return 0;
    }
    int i = 0;
    vector<int> matches;
    if (verbose >= 5) cout << setw(30) << left << _rule->getName();
    if (verbose >= 8) cout << endl;
    while (true) {
        _rule->match(_simpGraph);
        if (_rule->getMatchTypeVecNum() <= 0)
            break;
        else
            matches.push_back(_rule->getMatchTypeVecNum());
        i += 1;

        if (verbose >= 8) cout << "\nIteration " << i << ":" << endl
                               << ">>>" << endl;
        rewrite();
        amend();
        if (verbose >= 8) cout << "<<<" << endl;
    }
    _recipe.emplace_back(_rule->getName(), matches);
    if (verbose >= 8) cout << "=> ";
    if (verbose >= 5) {
        cout << i << " iterations." << endl;
        for (size_t m = 0; m < matches.size(); m++) {
            cout << "  " << m + 1 << ") " << matches[m] << " matches" << endl;
        }
    }
    if (verbose >= 5) cout << "\n";
    return i;
}

/**
 *
 * @brief Convert as many Hadamards represented by H-boxes to Hadamard-edges.
 *
 * @return int
 */
int Simplifier::hadamardSimp() {
    if (_rule->getName() != "Hadamard Rule") {
        cerr << "Error: `hadamardSimp` is only for HRule." << endl;
        return 0;
    }
    int i = 0;
    vector<int> matches;
    if (verbose >= 5) cout << setw(30) << left << _rule->getName();
    if (verbose >= 8) cout << endl;
    while (true) {
        size_t vcount = _simpGraph->getNumVertices();

        _rule->match(_simpGraph);

        if (_rule->getMatchTypeVecNum() == 0)
            break;
        else
            matches.push_back(_rule->getMatchTypeVecNum());
        i += 1;

        if (verbose >= 8) cout << "\nIteration " << i << ":" << endl
                               << ">>>" << endl;
        rewrite();
        amend();
        if (verbose >= 8) cout << "<<<" << endl;
        if (_simpGraph->getNumVertices() >= vcount) break;
    }
    _recipe.emplace_back(_rule->getName(), matches);
    if (verbose >= 8) cout << "=> ";
    if (verbose >= 5) {
        cout << i << " iterations." << endl;
        for (size_t m = 0; m < matches.size(); m++) {
            cout << "  " << m + 1 << ") " << matches[m] << " matches" << endl;
        }
    }
    if (verbose >= 5) cout << "\n";
    return i;
}

/**
 * @brief Apply rule
 */
void Simplifier::amend() {
    for (size_t e = 0; e < _rule->getEdgeTableKeys().size(); e++) {
        ZXVertex* v = _rule->getEdgeTableKeys()[e].first;
        ZXVertex* v_n = _rule->getEdgeTableKeys()[e].second;
        int numSimpleEdges = _rule->getEdgeTableValues()[e].first;
        int numHadamardEdges = _rule->getEdgeTableValues()[e].second;

        if (numSimpleEdges) _simpGraph->addEdge(v, v_n, EdgeType::SIMPLE);
        if (numHadamardEdges) _simpGraph->addEdge(v, v_n, EdgeType::HADAMARD);
    }
    _simpGraph->removeEdges(_rule->getRemoveEdges());
    _simpGraph->removeVertices(_rule->getRemoveVertices());

    _simpGraph->removeIsolatedVertices();
}

// Basic rules simplification

/**
 * @brief Perform Bialgebra Rule
 *
 * @return int
 */
int Simplifier::bialgSimp() {
    this->setRule(make_unique<Bialgebra>());
    return this->simp();
}

/**
 * @brief Perform State Copy Rule
 *
 * @return int
 */
int Simplifier::copySimp() {
    if (!_simpGraph->isGraphLike()) return 0;
    this->setRule(make_unique<StateCopy>());
    return this->simp();
}

/**
 * @brief Perform Gadget Rule
 *
 * @return int
 */
int Simplifier::gadgetSimp() {
    this->setRule(make_unique<PhaseGadget>());
    return this->simp();
}

/**
 * @brief Perform Hadamard Fusion Rule
 *
 * @return int
 */
int Simplifier::hfusionSimp() {
    this->setRule(make_unique<HboxFusion>());
    return this->simp();
}

/**
 * @brief Perform Hadamard Rule
 *
 * @return int
 */
int Simplifier::hruleSimp() {
    this->setRule(make_unique<HRule>());
    return this->hadamardSimp();
}

/**
 * @brief Perform Identity Removal Rule
 *
 * @return int
 */
int Simplifier::idSimp() {
    this->setRule(make_unique<IdRemoval>());
    return this->simp();
}

/**
 * @brief Perform Local Complementation Rule
 *
 * @return int
 */
int Simplifier::lcompSimp() {
    this->setRule(make_unique<LComp>());
    return this->simp();
}

/**
 * @brief Perform Pivot Rule
 *
 * @return int
 */
int Simplifier::pivotSimp() {
    this->setRule(make_unique<Pivot>());
    return this->simp();
}

/**
 * @brief Perform Pivot Boundary Rule
 *
 * @return int
 */
int Simplifier::pivotBoundarySimp() {
    this->setRule(make_unique<PivotBoundary>());
    return this->simp();
}

/**
 * @brief Perform Pivot Gadget Rule
 *
 * @return int
 */
int Simplifier::pivotGadgetSimp() {
    this->setRule(make_unique<PivotGadget>());
    return this->simp();
}

/**
 * @brief Perform Degadgetize Rule
 *
 * @return int
 */
int Simplifier::degadgetizeSimp() {
    this->setRule(make_unique<PivotDegadget>());
    return this->simp();
}

/**
 * @brief Perform Spider Fusion Rule
 *
 * @return int
 */
int Simplifier::sfusionSimp() {
    this->setRule(make_unique<SpiderFusion>());
    return this->simp();
}

// action

/**
 * @brief Turn every red node(VertexType::X) into green node(VertexType::Z) by regular simple edges <--> hadamard edges.
 *
 */
void Simplifier::toGraph() {
    for (auto& v : _simpGraph->getVertices()) {
        if (v->getType() == VertexType::X) {
            _simpGraph->toggleEdges(v);
        }
    }
}

/**
 * @brief Turn green nodes into red nodes by color-changing vertices which greedily reducing the number of Hadamard-edges.
 *
 */
void Simplifier::toRGraph() {
    for (auto& v : _simpGraph->getVertices()) {
        if (v->getType() == VertexType::Z) {
            _simpGraph->toggleEdges(v);
        }
    }
}

/**
 * @brief Keep doing the simplifications `id_removal`, `s_fusion`, `pivot`, `lcomp` until none of them can be applied anymore.
 *
 * @return int
 */
int Simplifier::interiorCliffordSimp() {
    this->sfusionSimp();
    toGraph();
    
    int i = 0;
    while (true) {
        int i1 = this->idSimp();
        int i2 = this->sfusionSimp();
        int i3 = this->pivotSimp();
        int i4 = this->lcompSimp();
        if (i1 + i2 + i3 + i4 == 0) break;
        i += 1;
    }
    return i;
}

/**
 * @brief Perform `interior_clifford` and `pivot_boundary` iteratively until no pivot_boundary candidate is found
 *
 * @return int
 */
int Simplifier::cliffordSimp() {
    int i = 0;
    while (true) {
        i += this->interiorCliffordSimp();
        int i2 = this->pivotBoundarySimp();
        if (i2 == 0) break;
    }
    return i;
}

/**
 * @brief The main simplification routine
 *
 */
void Simplifier::fullReduce() {
    _simpGraph->addProcedure("FR");
    this->interiorCliffordSimp();
    this->pivotGadgetSimp();
    while (true) {
        this->cliffordSimp();
        int i = this->gadgetSimp();
        this->interiorCliffordSimp();
        int j = this->pivotGadgetSimp();
        if (i + j == 0) break;
    }
    this->printRecipe();
}

/**
 * @brief The reduce strategy with `state_copy` and `full_reduce`
 *
 */
void Simplifier::symbolicReduce() {
    this->interiorCliffordSimp();
    this->pivotGadgetSimp();
    this->copySimp();
    while (true) {
        this->cliffordSimp();
        int i = this->gadgetSimp();
        this->interiorCliffordSimp();
        int j = this->pivotGadgetSimp();
        this->copySimp();
        if (i + j == 0) break;
    }
    this->toRGraph();
}

/**
 * @brief Partition ZX-graph first to `d` sub ZX-graph. 
 *        Do the `fullReduce` for each sub ZX-graph and then recombine them in the correct order.
 *        (Optional) After combination, call `fullReduce` again.
 * 
 * @param d 
 */
void Simplifier::partitionReduce(int d) {
    // TODO: Start writing your code!
    // Hint: to trace through all vertices in a ZX-graph, you can try this:
    //       for(auto& v : _simpGraph->getVertices()){}
    //
    //       to get/set the `col` of a ZXVertex, you can use `v->getCol()`
    // 
    vector<ZXGraph* > vec;
    
    int maxCol = 0;
    for(auto& v : _simpGraph->getVertices()) {
        int col = v->getCol();
        if(col > maxCol) maxCol = col;
    }
    ///
    for(int idx = 0; idx < d; idx++) {
        ZXGraph* subGraph = new ZXGraph(idx);
        auto simpGraph = _simpGraph->copy();
        int splitSize = (maxCol+1) / d;
        int left = idx * splitSize, right = (idx + 1) * splitSize;
        ZXVertexList subGraphList, inputList, outputList;
        map<int, ZXVertex*> addedBoundaries;
        for(auto& v : simpGraph->getInputs()) {
            int col = v->getCol();
            if(col >= left && col < right) {
                inputList.insert(v);
            }
        }
        for(auto& v : simpGraph->getOutputs()) {
            int col = v->getCol();
            if(col >= left && col < right) {
                outputList.insert(v);
            }
        }
        for(auto& v : simpGraph->getVertices()) {
            int col = v->getCol();
            if(col >= left && col < right) {
                // insert node v into ZXGraph #idx
                subGraphList.insert(v);
                
                // add output or input vertecies
                auto nei = v->getCopiedNeighbors();
                
                for(auto& i : nei) {
                    int neiCol = i->getCol();
                    if(neiCol < left || neiCol >= right) {
                        // cout << "add neighbor " << i->getId() << endl;
                        if(!addedBoundaries.count(i->getId())) {
                            ZXVertex* bdr = new ZXVertex(i->getId(), i->getQubit(), VertexType::BOUNDARY, Phase(0), i->getCol());
                            bdr->addNeighbor(NeighborPair(v, EdgeType::SIMPLE));
                            v->removeNeighbor(NeighborPair(i, EdgeType::SIMPLE));
                            v->addNeighbor(NeighborPair(bdr, EdgeType::SIMPLE));
                            addedBoundaries[i->getId()] = bdr;
                        }
                        else {
                            auto bdr = addedBoundaries[i->getId()];
                            bdr->addNeighbor(NeighborPair(v, EdgeType::SIMPLE));
                            v->removeNeighbor(NeighborPair(i, EdgeType::SIMPLE));
                            v->addNeighbor(NeighborPair(bdr, EdgeType::SIMPLE));
                        }
                    }
                }
            }
        }
        
        for(auto& v : addedBoundaries) {
            auto bdr = v.second;
            subGraphList.insert(bdr);
            if(bdr->getCol() < left) inputList.insert(bdr);
            else outputList.insert(bdr);
        }
        
        subGraph->setVertices(subGraphList);
        subGraph = subGraph->copy();
        subGraph->setInputs(inputList);
        subGraph->setOutputs(outputList);
        
        // normalize id
        int id = 0;
        for(auto& v : subGraph->getVertices()) {
            v->setId(id++);
        }
        
        Simplifier* simp = new Simplifier((std::unique_ptr<ZXRule>)this->getRule(), subGraph);
        simp->fullReduce();
        
        vec.push_back(subGraph);
    }
    
    auto composedGraph = vec[0];
    for(unsigned long i = 1; i < vec.size(); ++i) {
        composedGraph = composedGraph->compose(vec[i]);
    }
    _simpGraph = composedGraph;
    this->fullReduce();
    
    // A simple method:
    //  (1) Trace through all vertices and use a new vector(vector<ZXGraph* > vec) to store the i-th sub-graph vertices
    //  (2) As a reminder, after each cut of the ZX-graph, you should remember to add outputs to the cut sub-graph, and add inputs to the remain one. (This step is a little tricky!)
    //  (3) For each sub ZX-graph, call `fullReduce` to optimize them respectively
    //  (4) Use `compose` defined in `src/graph/zxGraphAction.cpp` to combine each sub ZX-graph in the correct order.
    //  (5) (Optional) After combination, call `fullReduce` again to make the ZX-graph optimal.
    //
    // ?? This is one of the methods. Try to implement it, optimize it or find out your own solution! ?Ҫ?Ҫ?Ҫ Good Luck!
    
    
}


/**
 * @brief Print recipe of Simplifier
 *
 */
void Simplifier::printRecipe() {
    if (verbose <= 3) {
        if (verbose == 0)
            return;
        if (verbose == 1) {
            cout << "\nAll rules applied:\n";
            unordered_set<string> rules;
            for (size_t i = 0; i < _recipe.size(); i++) {
                if (get<1>(_recipe[i]).size() != 0) {
                    if (rules.find(get<0>(_recipe[i])) == rules.end()) {
                        cout << "(" << rules.size() + 1 << ") " << get<0>(_recipe[i]) << endl;
                        rules.insert(get<0>(_recipe[i]));
                    }
                }
            }
        } else {
            cout << "\nAll rules applied in order:\n";
            for (size_t i = 0; i < _recipe.size(); i++) {
                if (get<1>(_recipe[i]).size() != 0) {
                    cout << setw(30) << left << get<0>(_recipe[i]) << get<1>(_recipe[i]).size() << " iterations." << endl;
                    if (verbose == 3) {
                        for (size_t j = 0; j < get<1>(_recipe[i]).size(); j++) {
                            cout << "  " << j + 1 << ") " << get<1>(_recipe[i])[j] << " matches" << endl;
                        }
                    }
                }
            }
        }
    }
}