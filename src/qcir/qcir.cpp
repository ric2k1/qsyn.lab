/****************************************************************************
  FileName     [ qcir.cpp ]
  PackageName  [ qcir ]
  Synopsis     [ Define qcir manager functions ]
  Author       [ Chin-Yi Cheng ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include "qcir.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

#include "tensorMgr.h"
#include "zxGraphMgr.h"

using namespace std;
QCir *qCir = 0;
extern ZXGraphMgr *zxGraphMgr;
extern TensorMgr *tensorMgr;
extern size_t verbose;

/**
 * @brief Get Gate.
 *
 * @param id
 * @return QCirGate*
 */
QCirGate *QCir::getGate(size_t id) const {
    for (size_t i = 0; i < _qgate.size(); i++) {
        if (_qgate[i]->getId() == id)
            return _qgate[i];
    }
    return NULL;
}

/**
 * @brief Get Qubit.
 *
 * @param id
 * @return QCirQubit
 */
QCirQubit *QCir::getQubit(size_t id) const {
    for (size_t i = 0; i < _qubits.size(); i++) {
        if (_qubits[i]->getId() == id)
            return _qubits[i];
    }
    return NULL;
}

/**
 * @brief Print QCir Summary
 */
void QCir::printSummary() {
    if (_dirty)
        updateGateTime();
    cout << "Listed by gate ID" << endl;
    for (size_t i = 0; i < _qgate.size(); i++) {
        _qgate[i]->printGate();
    }
}

/**
 * @brief Print Qubits
 */
void QCir::printQubits() {
    if (_dirty)
        updateGateTime();

    for (size_t i = 0; i < _qubits.size(); i++)
        _qubits[i]->printBitLine();
}

/**
 * @brief Print Gate information
 *
 * @param id
 * @param showTime
 */
bool QCir::printGateInfo(size_t id, bool showTime) {
    if (getGate(id) != NULL) {
        if (showTime && _dirty)
            updateGateTime();
        getGate(id)->printGateInfo(showTime);
        return true;
    } else {
        cerr << "Error: id " << id << " not found!!" << endl;
        return false;
    }
}

/**
 * @brief Add Qubit.
 *
 * @param num
 */
void QCir::addQubit(size_t num) {
    for (size_t i = 0; i < num; i++) {
        QCirQubit *temp = new QCirQubit(_qubitId);
        _qubits.push_back(temp);
        _qubitId++;
        clearMapping();
    }
}

/**
 * @brief Perform DFS from currentGate
 *
 * @param currentGate
 */
void QCir::DFS(QCirGate *currentGate) {
    currentGate->setVisited(_globalDFScounter);

    vector<BitInfo> Info = currentGate->getQubits();
    for (size_t i = 0; i < Info.size(); i++) {
        if ((Info[i])._child != NULL) {
            if (!((Info[i])._child->isVisited(_globalDFScounter))) {
                DFS((Info[i])._child);
            }
        }
    }
    _topoOrder.push_back(currentGate);
}

/**
 * @brief Update Topological Order
 */
void QCir::updateTopoOrder() {
    _topoOrder.clear();
    _globalDFScounter++;
    QCirGate *dummy = new HGate(-1);

    for (size_t i = 0; i < _qubits.size(); i++) {
        dummy->addDummyChild(_qubits[i]->getFirst());
    }
    DFS(dummy);
    _topoOrder.pop_back();  // pop dummy
    reverse(_topoOrder.begin(), _topoOrder.end());
    assert(_topoOrder.size() == _qgate.size());
    delete dummy;
}

/**
 * @brief Print topological order
 */
bool QCir::printTopoOrder() {
    auto testLambda = [](QCirGate *G) { cout << G->getId() << endl; };
    topoTraverse(testLambda);
    return true;
}

/**
 * @brief Update execution time of gates
 */
void QCir::updateGateTime() {
    auto Lambda = [](QCirGate *currentGate) {
        vector<BitInfo> Info = currentGate->getQubits();
        size_t max_time = 0;
        for (size_t i = 0; i < Info.size(); i++) {
            if (Info[i]._parent == NULL)
                continue;
            if (Info[i]._parent->getTime() + 1 > max_time)
                max_time = Info[i]._parent->getTime() + 1;
        }
        currentGate->setTime(max_time);
    };
    topoTraverse(Lambda);
}

/**
 * @brief Print ZX-graph of gate following the topological order
 */
void QCir::printZXTopoOrder() {
    auto Lambda = [this](QCirGate *G) {
        cout << "Gate " << G->getId() << " (" << G->getTypeStr() << ")" << endl;
        ZXGraph *tmp = G->getZXform();
        tmp->printVertices();
    };
    topoTraverse(Lambda);
}

/**
 * @brief Clear mapping
 */
void QCir::clearMapping() {
    for (size_t i = 0; i < _ZXGraphList.size(); i++) {
        cerr << "Note: Graph " << _ZXGraphList[i]->getId() << " is deleted due to modification(s) !!" << endl;
        _ZXGraphList[i]->reset();
        zxGraphMgr->removeZXGraph(_ZXGraphList[i]->getId());
    }
    _ZXGraphList.clear();
}

/**
 * @brief Mapping QCir to ZX-graph
 */
void QCir::ZXMapping() {
    if (zxGraphMgr == 0) {
        cerr << "Error: ZXMODE is OFF, please turn on before mapping" << endl;
        return;
    }
    if (verbose >= 3) cout << "Traverse and build the graph... " << endl;
    updateTopoOrder();

    ZXGraph *_ZXG = zxGraphMgr->addZXGraph(zxGraphMgr->getNextID());
    _ZXG->setRef((void **)_ZXG);

    if (verbose >= 5) cout << "> Add boundaries" << endl;
    for (size_t i = 0; i < _qubits.size(); i++) {
        ZXVertex *input = _ZXG->addInput(_qubits[i]->getId());
        ZXVertex *output = _ZXG->addOutput(_qubits[i]->getId());
        _ZXG->addEdge(input, output, EdgeType(EdgeType::SIMPLE));
    }

    
    topoTraverse([this, _ZXG](QCirGate *G) {
        if (verbose >= 5) cout << "> Gate " << G->getId() << " (" << G->getTypeStr() << ")" << endl;
        //REVIEW - Check mem leak problem
        ZXGraph *tmp = G->getZXform();
        if (tmp == NULL) {
            //TODO cleanup when conversion fails
            cerr << "Gate " << G->getId() << " (type: " << G->getTypeStr() << ") is not implemented, the conversion result is wrong!!" << endl;
            return;
        }

        _ZXG->concatenate(tmp, false);
    });

    _ZXGraphList.push_back(_ZXG);
}

/**
 * @brief Convert QCir to tensor
 */
void QCir::tensorMapping() {
    if (verbose >= 3) cout << "Traverse and build the tensor... " << endl;
    updateTopoOrder();
    if (verbose >= 5) cout << "> Add boundary" << endl;
    if (!tensorMgr) tensorMgr = new TensorMgr();
    size_t id = tensorMgr->nextID();
    _tensor = tensorMgr->addTensor(id, "QC");
    *_tensor = tensordot(*_tensor, QTensor<double>::identity(_qubits.size()));
    _qubit2pin.clear();
    for (size_t i = 0; i < _qubits.size(); i++) {
        _qubit2pin[_qubits[i]->getId()] = make_pair(2 * i, 2 * i + 1);
        if (verbose >= 8) cout << "  - Add Qubit " << _qubits[i]->getId() << " output port: " << 2 * i + 1 << endl;
    }

    topoTraverse([this](QCirGate *G) {
        if (verbose >= 5) cout << "> Gate " << G->getId() << " (" << G->getTypeStr() << ")" << endl;
        QTensor<double> tmp = G->getTSform();
        vector<size_t> ori_pin;
        vector<size_t> new_pin;
        ori_pin.clear();
        new_pin.clear();
        for (size_t np = 0; np < G->getQubits().size(); np++) {
            new_pin.push_back(2 * np);
            BitInfo info = G->getQubits()[np];
            ori_pin.push_back(_qubit2pin[info._qubit].second);
        }
        *_tensor = tensordot(*_tensor, tmp, ori_pin, new_pin);
        updateTensorPin(G->getQubits(), tmp);
    });

    vector<size_t> input_pin, output_pin;
    for (size_t i = 0; i < _qubits.size(); i++) {
        input_pin.push_back(_qubit2pin[_qubits[i]->getId()].first);
        output_pin.push_back(_qubit2pin[_qubits[i]->getId()].second);
    }
    *_tensor = _tensor->toMatrix(input_pin, output_pin);
    cout << "Stored the resulting tensor as tensor id " << id << endl;
}

/**
 * @brief Update tensor pin
 *
 * @param pins
 * @param tmp
 */
void QCir::updateTensorPin(vector<BitInfo> pins, QTensor<double> tmp) {
    // size_t count_pin_used = 0;
    // unordered_map<size_t, size_t> table; // qid to pin (pin0 = ctrl 0 pin1 = ctrl 1)
    if (verbose >= 8) cout << "> Pin Permutation" << endl;
    for (auto it = _qubit2pin.begin(); it != _qubit2pin.end(); ++it) {
        if (verbose >= 8) {
            // NOTE print old input axis id
            cout << "  - Qubit: " << it->first << " input : " << it->second.first << " -> ";
        }
        it->second.first = _tensor->getNewAxisId(it->second.first);
        if (verbose >= 8) {
            // NOTE print new input axis id
            cout << it->second.first << " | ";
            // NOTE print new output axis id
            cout << " output: " << it->second.second << " -> ";
        }

        bool connected = false;
        bool target = false;
        size_t ithCtrl = 0;
        for (size_t i = 0; i < pins.size(); i++) {
            if (pins[i]._qubit == it->first) {
                connected = true;
                if (pins[i]._isTarget)
                    target = true;
                else
                    ithCtrl = i;
                break;
            }
        }
        if (connected) {
            if (target)
                it->second.second = _tensor->getNewAxisId(_tensor->dimension() + tmp.dimension() - 1);

            else
                it->second.second = _tensor->getNewAxisId(_tensor->dimension() + 2 * ithCtrl + 1);
        } else
            it->second.second = _tensor->getNewAxisId(it->second.second);

        if (verbose >= 8) {
            // NOTE print new axis id
            cout << it->second.second << endl;
        }
    }
}

/**
 * @brief Remove Qubit with specific id
 *
 * @param id
 * @return true if succcessfully removed
 * @return false if not found or the qubit is not empty
 */
bool QCir::removeQubit(size_t id) {
    // Delete the ancilla only if whole line is empty
    QCirQubit *target = getQubit(id);
    if (target == NULL) {
        cerr << "Error: id " << id << " not found!!" << endl;
        return false;
    } else {
        if (target->getLast() != NULL || target->getFirst() != NULL) {
            cerr << "Error: id " << id << " is not an empty qubit!!" << endl;
            return false;
        } else {
            _qubits.erase(remove(_qubits.begin(), _qubits.end(), target), _qubits.end());
            clearMapping();
            return true;
        }
    }
}

/**
 * @brief Add Gate
 *
 * @param type
 * @param bits
 * @param phase
 * @param append
 */
void QCir::addGate(string type, vector<size_t> bits, Phase phase, bool append) {
    QCirGate *temp = NULL;
    for_each(type.begin(), type.end(), [](char &c) { c = ::tolower(c); });
    if (type == "h")
        temp = new HGate(_gateId);
    else if (type == "z")
        temp = new ZGate(_gateId);
    else if (type == "s")
        temp = new SGate(_gateId);
    else if (type == "s*" || type == "sdg" || type == "sd")
        temp = new SDGGate(_gateId);
    else if (type == "t")
        temp = new TGate(_gateId);
    else if (type == "tdg" || type == "td" || type == "t*")
        temp = new TDGGate(_gateId);
    else if (type == "p")
        temp = new RZGate(_gateId);
    else if (type == "cz")
        temp = new CZGate(_gateId);
    else if (type == "x" || type == "not")
        temp = new XGate(_gateId);
    else if (type == "y")
        temp = new YGate(_gateId);
    else if (type == "sx" || type == "x_1_2")
        temp = new SXGate(_gateId);
    else if (type == "sy" || type == "y_1_2")
        temp = new SYGate(_gateId);
    else if (type == "cx" || type == "cnot")
        temp = new CXGate(_gateId);
    else if (type == "ccx" || type == "ccnot")
        temp = new CCXGate(_gateId);
    else if (type == "ccz")
        temp = new CCZGate(_gateId);
    // Note: rz and p has a little difference
    else if (type == "rz") {
        temp = new RZGate(_gateId);
        temp->setRotatePhase(phase);
    } else if (type == "rx") {
        temp = new RXGate(_gateId);
        temp->setRotatePhase(phase);
    } else {
        cerr << "Error: The gate " << type << " is not implemented!!" << endl;
        abort();
        return;
    }
    if (append) {
        size_t max_time = 0;
        for (size_t k = 0; k < bits.size(); k++) {
            size_t q = bits[k];
            temp->addQubit(q, k == bits.size() - 1);  // target is the last one
            QCirQubit *target = getQubit(q);
            if (target->getLast() != NULL) {
                temp->setParent(q, target->getLast());
                target->getLast()->setChild(q, temp);
                if ((target->getLast()->getTime() + 1) > max_time)
                    max_time = target->getLast()->getTime() + 1;
            } else
                target->setFirst(temp);
            target->setLast(temp);
        }
        temp->setTime(max_time);
    } else {
        for (size_t k = 0; k < bits.size(); k++) {
            size_t q = bits[k];
            temp->addQubit(q, k == bits.size() - 1);  // target is the last one
            QCirQubit *target = getQubit(q);
            if (target->getFirst() != NULL) {
                temp->setChild(q, target->getFirst());
                target->getFirst()->setParent(q, temp);
            } else
                target->setLast(temp);
            target->setFirst(temp);
        }
        _dirty = true;
    }
    _qgate.push_back(temp);
    _gateId++;
    clearMapping();
}

bool QCir::removeGate(size_t id) {
    QCirGate *target = getGate(id);
    if (target == NULL) {
        cerr << "Error: id " << id << " not found!!" << endl;
        return false;
    } else {
        vector<BitInfo> Info = target->getQubits();
        for (size_t i = 0; i < Info.size(); i++) {
            if (Info[i]._parent != NULL)
                Info[i]._parent->setChild(Info[i]._qubit, Info[i]._child);
            else
                getQubit(Info[i]._qubit)->setFirst(Info[i]._child);
            if (Info[i]._child != NULL)
                Info[i]._child->setParent(Info[i]._qubit, Info[i]._parent);
            else
                getQubit(Info[i]._qubit)->setLast(Info[i]._parent);
            Info[i]._parent = NULL;
            Info[i]._child = NULL;
        }
        _qgate.erase(remove(_qgate.begin(), _qgate.end(), target), _qgate.end());
        _dirty = true;
        clearMapping();
        return true;
    }
}

void QCir::reset(){
    _qgate.clear();
    _qubits.clear();
    _topoOrder.clear();
    _ZXGraphList.clear();
    _qubit2pin.clear();
    
    _gateId = 0;
    _ZXNodeId = 0;
    _qubitId = 0;
    _dirty = true;
    _globalDFScounter = 1;
    _tensor = nullptr;
}