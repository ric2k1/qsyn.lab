/****************************************************************************
  FileName     [ zxGraphAction.cpp ]
  PackageName  [ graph ]
  Synopsis     [ Define class ZXGraph Action functions ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cstddef>  // for size_t
#include <iostream>
#include <vector>

#include "phase.h"       // for Phase
#include "textFormat.h"  // for TextFormat
#include "zxDef.h"       // for VertexType, VertexType::Z
#include "zxGraph.h"     // for ZXGraph, ZXVertex

using namespace std;
namespace TF = TextFormat;
extern size_t verbose;

/**
 * @brief Reset a ZX-graph (make empty)
 *
 */
void ZXGraph::reset() {
    for (auto& v : _vertices) delete v;
    _inputs.clear();
    _outputs.clear();
    _inputList.clear();
    _outputList.clear();
    _topoOrder.clear();
    _vertices.clear();
    _nextVId = 0;
    _globalTraCounter = 1;
}

/**
 * @brief Sort _inputs and _outputs of graph by qubit (ascending)
 *
 */
void ZXGraph::sortIOByQubit() {
    _inputs.sort([](ZXVertex* a, ZXVertex* b) { return a->getQubit() < b->getQubit(); });
    _outputs.sort([](ZXVertex* a, ZXVertex* b) { return a->getQubit() < b->getQubit(); });
}

/**
 * @brief Copy an identitcal ZX-graph
 *
 * @return ZXGraph*
 */
ZXGraph* ZXGraph::copy(bool doReordering) const {
    ZXGraph* newGraph = new ZXGraph(0);
    // Copy all vertices (included i/o) first
    unordered_map<ZXVertex*, ZXVertex*> oldV2newVMap;
    for (const auto& v : _vertices) {
        if (v->getType() == VertexType::BOUNDARY) {
            if (_inputs.contains(v))
                oldV2newVMap[v] = newGraph->addInput(v->getQubit(), true, v->getCol());
            else
                oldV2newVMap[v] = newGraph->addOutput(v->getQubit(), true, v->getCol());

        } else if (v->getType() == VertexType::Z || v->getType() == VertexType::X || v->getType() == VertexType::H_BOX) {
            oldV2newVMap[v] = newGraph->addVertex(v->getQubit(), v->getType(), v->getPhase(), true, v->getCol());
        }
    }

    if (!doReordering) {
        for (auto& [oldV, newV] : oldV2newVMap) {
            newV->setId(oldV->getId());
        }
        newGraph->_nextVId = _nextVId;
    }
    // Link all edges
    // cout << "Link all edges" << endl;
    // unordered_map<size_t, ZXVertex*> id2VertexMap = newGraph->id2VertexMap();
    forEachEdge([&oldV2newVMap, newGraph](const EdgePair& epair) {
        newGraph->addEdge(oldV2newVMap[epair.first.first], oldV2newVMap[epair.first.second], epair.second);
    });
    return newGraph;
}

/**
 * @brief Toggle EdgeType that connected to `v`. ( H -> S / S -> H)
 *        Ex: [(3, S), (4, H), (5, S)] -> [(3, H), (4, S), (5, H)]
 *
 * @param v
 */
void ZXGraph::toggleEdges(ZXVertex* v) {
    if (!v->isZ() && !v->isX()) return;
    Neighbors toggledNeighbors;
    for (auto& itr : v->getNeighbors()) {
        toggledNeighbors.insert(make_pair(itr.first, toggleEdge(itr.second)));
        itr.first->removeNeighbor(make_pair(v, itr.second));
        itr.first->addNeighbor(make_pair(v, toggleEdge(itr.second)));
    }
    v->setNeighbors(toggledNeighbors);
    v->setType(v->getType() == VertexType::Z ? VertexType::X : VertexType::Z);
}

/**
 * @brief Lift each vertex's qubit in ZX-graph with `n`.
 *        Ex: origin: 0 -> after lifting: n
 *
 * @param n
 */
void ZXGraph::liftQubit(const size_t& n) {
    for (const auto& v : _vertices) {
        v->setQubit(v->getQubit() + n);
    }

    unordered_map<size_t, ZXVertex*> newInputList, newOutputList;

    for_each(_inputList.begin(), _inputList.end(),
             [&n, &newInputList](pair<size_t, ZXVertex*> itr) {
                 newInputList[itr.first + n] = itr.second;
             });
    for_each(_outputList.begin(), _outputList.end(),
             [&n, &newOutputList](pair<size_t, ZXVertex*> itr) {
                 newOutputList[itr.first + n] = itr.second;
             });

    setInputList(newInputList);
    setOutputList(newOutputList);
}

/**
 * @brief Compose `target` to the original ZX-graph (horizontal concat)
 *
 * @param target
 * @return ZXGraph*
 */
ZXGraph* ZXGraph::compose(ZXGraph* target) {
    // Check ori-outputNum == target-inputNum
    if (this->getNumOutputs() != target->getNumInputs())
        cerr << "Error: The composing ZX-graph's #input is not equivalent to the original ZX-graph's #output." << endl;
    else {
        ZXGraph* copiedGraph = target->copy();

        // Get maximum column in `this`
        unsigned maxCol = 0;
        for (const auto& o : this->getOutputs()) {
            if (o->getCol() > maxCol) maxCol = o->getCol();
        }

        // Update `_id` and `_col` of copiedGraph to make them unique to the original graph
        for (const auto& v : copiedGraph->getVertices()) {
            v->setId(_nextVId);
            v->setCol(v->getCol() + maxCol + 1);
            _nextVId++;
        }

        // Sort ori-output and copy-input
        this->sortIOByQubit();
        copiedGraph->sortIOByQubit();

        // Change ori-output and copy-inputs' vt to Z and link them respectively
        auto itr_ori = _outputs.begin();
        auto itr_cop = copiedGraph->getInputs().begin();
        for (; itr_ori != _outputs.end(); ++itr_ori, ++itr_cop) {
            (*itr_ori)->setType(VertexType::Z);
            (*itr_cop)->setType(VertexType::Z);
            this->addEdge((*itr_ori), (*itr_cop), EdgeType::SIMPLE);
        }
        this->setOutputs(copiedGraph->getOutputs());
        this->addVertices(copiedGraph->getVertices());
        this->setOutputList(copiedGraph->getOutputList());
        copiedGraph->disownVertices();
        delete copiedGraph;
    }
    return this;
}

/**
 * @brief Tensor `target` to the original ZX-graph (vertical concat)
 *
 * @param target
 * @return ZXGraph*
 */
ZXGraph* ZXGraph::tensorProduct(ZXGraph* target) {
    ZXGraph* copiedGraph = target->copy();

    // Lift Qubit
    int oriMaxQubit = INT_MIN, oriMinQubit = INT_MAX;
    int copiedMinQubit = INT_MAX;
    for (const auto& i : getInputs()) {
        if (i->getQubit() > oriMaxQubit) oriMaxQubit = i->getQubit();
        if (i->getQubit() < oriMinQubit) oriMinQubit = i->getQubit();
    }
    for (const auto& i : getOutputs()) {
        if (i->getQubit() > oriMaxQubit) oriMaxQubit = i->getQubit();
        if (i->getQubit() < oriMinQubit) oriMinQubit = i->getQubit();
    }

    for (const auto& i : copiedGraph->getInputs()) {
        if (i->getQubit() < copiedMinQubit) copiedMinQubit = i->getQubit();
    }
    for (const auto& i : copiedGraph->getOutputs()) {
        if (i->getQubit() < copiedMinQubit) copiedMinQubit = i->getQubit();
    }
    size_t liftQ = (oriMaxQubit - oriMinQubit + 1) - copiedMinQubit;
    copiedGraph->liftQubit(liftQ);

    // Update Id of copiedGraph to make them unique to the original graph
    for (const auto& v : copiedGraph->getVertices()) {
        v->setId(_nextVId);
        _nextVId++;
    }

    // Merge copiedGraph to original graph
    this->addInputs(copiedGraph->getInputs());
    this->addOutputs(copiedGraph->getOutputs());
    this->addVertices(copiedGraph->getVertices());
    this->mergeInputList(copiedGraph->getInputList());
    this->mergeOutputList(copiedGraph->getOutputList());

    copiedGraph->disownVertices();
    delete copiedGraph;

    return this;
}

/**
 * @brief Check if v is a gadget leaf
 *
 * @param v
 * @return true
 * @return false
 */
bool ZXGraph::isGadgetLeaf(ZXVertex* v) const {
    if (v->getType() != VertexType::Z ||
        v->getNumNeighbors() != 1 ||
        v->getFirstNeighbor().first->getType() != VertexType::Z ||
        v->getFirstNeighbor().second != EdgeType::HADAMARD ||
        !v->getFirstNeighbor().first->hasNPiPhase()) {
        if (verbose >= 5) cout << "Note: (" << v->getId() << ") is not a gadget leaf vertex!" << endl;
        return false;
    }
    return true;
}

/**
 * @brief Check if v is a gadget axel
 *
 * @param v
 * @return true
 * @return false
 */
bool ZXGraph::isGadgetAxel(ZXVertex* v) const {
    return v->isZ() &&
           v->hasNPiPhase() &&
           any_of(v->getNeighbors().begin(), v->getNeighbors().end(),
                  [](const NeighborPair& nbp) {
                      return nbp.first->getNumNeighbors() == 1 && nbp.first->isZ() && nbp.second == EdgeType::HADAMARD;
                  });
}

bool ZXGraph::hasDanglingNeighbors(ZXVertex* v) const {
    return any_of(v->getNeighbors().begin(), v->getNeighbors().end(),
                  [](const NeighborPair& nbp) {
                      return nbp.first->getNumNeighbors() == 1;
                  });
}

/**
 * @brief Add phase gadget of phase `p` for each vertex in `verVec`.
 *
 * @param p
 * @param verVec
 */
void ZXGraph::addGadget(Phase p, const vector<ZXVertex*>& verVec) {
    for (size_t i = 0; i < verVec.size(); i++) {
        if (verVec[i]->getType() == VertexType::BOUNDARY || verVec[i]->getType() == VertexType::H_BOX) return;
    }

    ZXVertex* axel = addVertex(-1, VertexType::Z, Phase(0));
    ZXVertex* leaf = addVertex(-2, VertexType::Z, p);

    addEdge(axel, leaf, EdgeType::HADAMARD);
    for (const auto& v : verVec) addEdge(v, axel, EdgeType::HADAMARD);
    if (verbose >= 5) cout << "Add phase gadget (" << leaf->getId() << ") to graph!" << endl;
}

/**
 * @brief Remove phase gadget `v`. (Auto-check if `v` is a gadget first!)
 *
 * @param v
 */
void ZXGraph::removeGadget(ZXVertex* v) {
    if (!isGadgetLeaf(v)) return;
    ZXVertex* axel = v->getFirstNeighbor().first;
    removeVertex(axel);
    removeVertex(v);
}

/**
 * @brief Generate a id-2-ZXVertex* map
 *
 * @return unordered_map<size_t, ZXVertex*>
 */
unordered_map<size_t, ZXVertex*> ZXGraph::id2VertexMap() const {
    unordered_map<size_t, ZXVertex*> id2VertexMap;
    for (const auto& v : _vertices) id2VertexMap[v->getId()] = v;
    return id2VertexMap;
}

/**
 * @brief Disown the vertices in a graph, so that they are no longer referenced by this ZXGraph.
 *        This function is used to change ownership of ZXVertices after composing/tensoring ZXGraphs.
 *
 */
void ZXGraph::disownVertices() {
    _inputs.clear();
    _outputs.clear();
    _vertices.clear();
    _topoOrder.clear();
    _inputList.clear();
    _outputList.clear();
}

/**
 * @brief Rearrange `col` index for each spider to enhance the readability of the ZX-graph
 * 
 */
void ZXGraph::normalize() {
    // TODO: Start writing your code!
    // Hint: to trace through all vertices in a ZX-graph, you can try this:
    //          for(auto& v : getVertices()){}
    //
    //       to trace through all edges in a ZX-graph, you can try this:
    //          forEachEdge([](const EdgePair& epair) {
    //              cout << "( " << epair.first.first->getId() << ", " << epair.first.second->getId() << " )\tType:\t" << EdgeType2Str(epair.second) << endl;
    //          });
    //       to get/set the `col` of a ZXVertex, you can use `getCol()`, `setCol()`
    //
    // A Brute-Force Method:
    //  (1) Trace through all vertices and use a hashmap(key: (int) qubit, value: (vector<ZXVertex*>) vec) to classify vertices
    //  (2) For each qubit, reassign the `col` index for each ZXVertex* in vec from 0 to vec.size()-1
    //
    // 👆 This is not a good method, but a very simple method. Try to implement it, optimize it or find out your own solution! 💪💪💪 Good Luck!
    // forEachEdge([](const EdgePair& epair) {
    //      cout << "( " << epair.first.first->getCol() << ", " << epair.first.second->getCol() << " )\tType:\t" << EdgeType2Str(epair.second) << endl;
    // });
    
    int minrow = 0; // minimum index of row(Qubit)
    int vRow = 0; // row for the vertex
    int rowNum = 0; // num of row
    int counter = 0; // count the location for insert
    int vNumMax = 0; // maximum vertex number in a row
    for(auto& v : getVertices()){ // find the minimum index of row
        if(v->getQubit() < minrow) minrow = v->getQubit(); 
    }
    rowNum = getNumInputs() - minrow; // define new rowIndex start from 0
    int vNumList[rowNum] = {0}; // vertex num of each row
    vector<vector<ZXVertex*>> vMatrix(rowNum); // define verticex matrix to store the location

    // =================== build the vertex matrix ===================
    for(auto& v : getVertices()){
        counter = 0; // count the location for insersion (by columns)
        vRow = v->getQubit()-minrow; 
        vNumList[vRow]++;// count the num of vertices for each row
        if(vMatrix[vRow].empty()) vMatrix[vRow].push_back(v); // add vertex if empty
        else {
            for(int i=0; i< int(vMatrix[vRow].size()); i++){
                if(v->getCol()>vMatrix[vRow][counter]->getCol()){
                    counter ++;
                }
            }
            vMatrix[vRow].insert(vMatrix[vRow].begin()+counter, v);
            counter = 0;
        }
    }

    for(int i=0; i<rowNum; i++){ // find the most num of vertices in a row
        if(vNumList[i]>vNumMax) vNumMax=vNumList[i];
    }

    // =================== main algorithm ===================
    // Intro: build a symmetric graph with seperating the first and the last vertices of a qubit, and equally distribute the rest vertices.
    for(int i=0; i<int(vMatrix.size()); i++){
        if(i < -minrow){
            for(int j=0; j<vNumList[i]; j++){
                vMatrix[i][j]->setCol(2+j*float(vNumMax)/float(vNumList[i]));
            }
        }
        else{
            for(int j=0; j<vNumList[i]; j++){
                if(vNumList[i]<4) vMatrix[i][j]->setCol(j*float(vNumMax+3)/float(vNumList[i]-1));
                else{
                    if(j<2) vMatrix[i][j]->setCol(j);
                    else if(j>=vNumList[i]-2) vMatrix[i][j]->setCol(vNumMax+4+j-vNumList[i]);
                    else vMatrix[i][j]->setCol(1+(j-1)*float(vNumMax+1)/float(vNumList[i]-3));
                }
            }
        }
    }
    
    // =================== print the vertices matrix in terminal ===================
    for(int i=0;i<int(vMatrix.size());i++){
        for(int j=0;j<int(vMatrix[i].size());j++){
            cout<<vMatrix[i][j]->getId()<<" " ;
        }
		cout<<endl;
	}
}


// qccread ./benchmark/benchmark_SABRE/small/3_17_13.qasm
// qc2zx
// zxgsimp -fr