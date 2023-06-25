/****************************************************************************
  FileName     [ qcirAction.cpp ]
  PackageName  [ qcir ]
  Synopsis     [ Define class QCir Action functions ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>  // for assert
#include <cstddef>  // for NULL, size_t

#include "qcir.h"      // for QCir
#include "qcirGate.h"  // for QCirGate
#include "zxGraph.h"
#include "textFormat.h"  // for TextFormat


namespace TF = TextFormat;

using namespace std;
extern size_t verbose;

/**
 * @brief Copy a circuit
 *
 * @return QCir*
 */
QCir* QCir::copy() {
    updateTopoOrder();
    QCir* newCircuit = new QCir(0);
    unordered_map<QCirQubit*, QCirQubit*> oldQ2newQ;
    unordered_map<QCirGate*, QCirGate*> oldG2newG;
    newCircuit->addQubit(_qubits.size());

    size_t biggestQubit = 0;
    size_t biggestGate = 0;

    for (size_t i = 0; i < _qubits.size(); i++) {
        oldQ2newQ[_qubits[i]] = newCircuit->getQubits()[i];
        // NOTE - Update Id
        if (_qubits[i]->getId() > biggestQubit) biggestQubit = _qubits[i]->getId();
        oldQ2newQ[_qubits[i]]->setId(_qubits[i]->getId());
    }

    for (const auto& gate : _topoOrder) {
        string type = gate->getTypeStr();
        vector<size_t> bits;
        for (const auto& b : gate->getQubits()) {
            bits.push_back(b._qubit);
        }
        oldG2newG[gate] = newCircuit->addGate(gate->getTypeStr(), bits, gate->getPhase(), true);
    }

    // NOTE - Update Id
    for (const auto& [oldG, newG] : oldG2newG) {
        if (oldG->getId() > biggestGate) biggestGate = oldG->getId();
        newG->setId(oldG->getId());
    }
    newCircuit->setNextGateId(biggestGate + 1);
    newCircuit->setNextQubitId(biggestQubit + 1);
    return newCircuit;
}

/**
 * @brief Append the target to current QCir
 *
 * @param target
 * @return QCir*
 */
QCir* QCir::compose(QCir* target) {
    QCir* copiedQCir = target->copy();
    vector<QCirQubit*> targQubits = copiedQCir->getQubits();
    for (auto& qubit : targQubits) {
        if (getQubit(qubit->getId()) == NULL)
            insertSingleQubit(qubit->getId());
    }
    copiedQCir->updateTopoOrder();
    for (auto& targGate : copiedQCir->getTopoOrderdGates()) {
        vector<size_t> bits;
        for (const auto& b : targGate->getQubits()) {
            bits.push_back(b._qubit);
        }
        addGate(targGate->getTypeStr(), bits, targGate->getPhase(), true);
    }
    return this;
}

/**
 * @brief Tensor the target to current tensor of QCir
 *
 * @param target
 * @return QCir*
 */
QCir* QCir::tensorProduct(QCir* target) {
    QCir* copiedQCir = target->copy();

    unordered_map<size_t, QCirQubit*> oldQ2NewQ;
    vector<QCirQubit*> targQubits = copiedQCir->getQubits();
    for (auto& qubit : targQubits) {
        oldQ2NewQ[qubit->getId()] = addSingleQubit();
    }
    copiedQCir->updateTopoOrder();
    for (auto& targGate : copiedQCir->getTopoOrderdGates()) {
        vector<size_t> bits;
        for (const auto& b : targGate->getQubits()) {
            bits.push_back(oldQ2NewQ[b._qubit]->getId());
        }
        addGate(targGate->getTypeStr(), bits, targGate->getPhase(), true);
    }
    return this;
}

/**
 * @brief Perform DFS from currentGate
 *
 * @param currentGate the gate to start DFS
 */
void QCir::DFS(QCirGate* currentGate) {
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
 * @brief Update topological order
 *
 * @return const vector<QCirGate*>&
 */
const vector<QCirGate*>& QCir::updateTopoOrder() {
    _topoOrder.clear();
    _globalDFScounter++;
    QCirGate* dummy = new HGate(-1);

    for (size_t i = 0; i < _qubits.size(); i++) {
        dummy->addDummyChild(_qubits[i]->getFirst());
    }
    DFS(dummy);
    _topoOrder.pop_back();  // pop dummy
    reverse(_topoOrder.begin(), _topoOrder.end());
    assert(_topoOrder.size() == _qgates.size());
    delete dummy;

    return _topoOrder;
}

/**
 * @brief Print topological order
 */
bool QCir::printTopoOrder() {
    auto testLambda = [](QCirGate* G) { cout << G->getId() << endl; };
    topoTraverse(testLambda);
    return true;
}

/**
 * @brief Update execution time of gates
 */
void QCir::updateGateTime() {
    updateTopoOrder();
    auto Lambda = [](QCirGate* currentGate) {
        vector<BitInfo> Info = currentGate->getQubits();
        size_t max_time = 0;
        for (size_t i = 0; i < Info.size(); i++) {
            if (Info[i]._parent == NULL)
                continue;
            if (Info[i]._parent->getTime() > max_time)
                max_time = Info[i]._parent->getTime();
        }
        currentGate->setTime(max_time + currentGate->getDelay());
    };
    topoTraverse(Lambda);
}

/**
 * @brief Print ZX-graph of gate following the topological order
 */
void QCir::printZXTopoOrder() {
    auto Lambda = [](QCirGate* G) {
        cout << "Gate " << G->getId() << " (" << G->getTypeStr() << ")" << endl;
        ZXGraph* tmp = G->getZXform();
        tmp->printVertices();
    };
    topoTraverse(Lambda);
}

/**
 * @brief Reset QCir
 *
 */
void QCir::reset() {
    _qgates.clear();
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


void QCir::analyze(size_t start, size_t end) {

    size_t total_depth = 0;
    for (size_t i = 0; i < _qgates.size(); i++) {
        if (_qgates[i]->getTime() > total_depth) total_depth = _qgates[i]->getTime();
    }
    total_depth = (total_depth > end) ? end : total_depth;
    cout << TF::BOLD(TF::BLUE("detecting from time : " + to_string(start) + " to " + to_string(total_depth) )) << endl;

    updateGateTime();
    size_t total = 0;
    size_t clifford = 0;
    size_t tfamily = 0;
    size_t cxcnt = 0;
    size_t nct = 0;
    size_t h = 0;
    size_t rz = 0;
    size_t z = 0;
    size_t s = 0;
    size_t sdg = 0;
    size_t t = 0;
    size_t tdg = 0;
    size_t rx = 0;
    size_t x = 0;
    size_t sx = 0;
    size_t ry = 0;
    size_t y = 0;
    size_t sy = 0;

    size_t mcp = 0;
    size_t cz = 0;
    // size_t ccz = 0;
    // size_t crz = 0;
    size_t mcrx = 0;
    size_t cx = 0;
    size_t ccx = 0;
    size_t mcry = 0;

    auto analysisMCR = [&clifford, &tfamily, &nct, &cxcnt](QCirGate *g) -> void {
        if (g->getQubits().size() == 2) {
            if (g->getPhase().denominator() == 1) {
                clifford++;
                if (g->getType() != GateType::MCPX || g->getType() != GateType::MCRX) clifford += 2;
                cxcnt++;
            } else if (g->getPhase().denominator() == 2) {
                clifford += 2;
                cxcnt += 2;
                tfamily += 3;
            } else
                nct++;
        } else if (g->getQubits().size() == 1) {
            if (g->getPhase().denominator() <= 2)
                clifford++;
            else if (g->getPhase().denominator() == 4)
                tfamily++;
            else
                nct++;
        } else
            nct++;
    };
    for (auto qb : _qubits) {
        for (auto &g : _qgates) {
            bool ok = false;
            if((g->getTime()-g->getDelay()) >=start && (g->getTime()) <= end){
                for (const auto& b : g->getQubits()) {
                    if(b._qubit == qb->getId()) { ok = true;}
                }
                if(ok){
                    total++ ;
                    GateType type = g->getType();
                    switch (type) {
                        case GateType::H:
                            h++;
                            clifford++;
                            break;
                        case GateType::P:
                            rz++;
                            if (g->getPhase().denominator() <= 2)
                                clifford++;
                            else if (g->getPhase().denominator() == 4)
                                tfamily++;
                            else
                                nct++;
                            break;
                        case GateType::RZ:
                            rz++;
                            if (g->getPhase().denominator() <= 2)
                                clifford++;
                            else if (g->getPhase().denominator() == 4)
                                tfamily++;
                            else
                                nct++;
                            break;
                        case GateType::Z:
                            z++;
                            clifford++;
                            break;
                        case GateType::S:
                            s++;
                            clifford++;
                            break;
                        case GateType::SDG:
                            sdg++;
                            clifford++;
                            break;
                        case GateType::T:
                            t++;
                            tfamily++;
                            break;
                        case GateType::TDG:
                            tdg++;
                            tfamily++;
                            break;
                        case GateType::RX:
                            rx++;
                            if (g->getPhase().denominator() <= 2)
                                clifford++;
                            else if (g->getPhase().denominator() == 4)
                                tfamily++;
                            else
                                nct++;
                            break;
                        case GateType::X:
                            x++;
                            clifford++;
                            break;
                        case GateType::SX:
                            sx++;
                            clifford++;
                            break;
                        case GateType::RY:
                            ry++;
                            if (g->getPhase().denominator() <= 2)
                                clifford++;
                            else if (g->getPhase().denominator() == 4)
                                tfamily++;
                            else
                                nct++;
                            break;
                        case GateType::Y:
                            y++;
                            clifford++;
                            break;
                        case GateType::SY:
                            sy++;
                            clifford++;
                            break;
                        case GateType::MCP:
                            mcp++;
                            analysisMCR(g);
                            break;
                        case GateType::CZ:
                            cz++;           // --C--
                            clifford += 3;  // H-X-H
                            cxcnt++;
                            break;
                        case GateType::CCZ:
                            cz++;
                            tfamily += 7;
                            clifford += 10;
                            cxcnt += 6;
                            break;
                        case GateType::MCRX:
                            mcrx++;
                            analysisMCR(g);
                            break;
                        case GateType::CX:
                            cx++;
                            clifford++;
                            cxcnt++;
                            break;
                        case GateType::CCX:
                            ccx++;
                            tfamily += 7;
                            clifford += 8;
                            cxcnt += 6;
                            break;
                        case GateType::MCRY:
                            mcry++;
                            analysisMCR(g);
                            break;
                        default:
                            cerr << "Error: the gate type is ERRORTYPE" << endl;
                            break;
                    }
                }
            }
        }

        size_t depth = 0;
        size_t current_max_depth = 0;
        for (size_t i = 0; i < _qgates.size(); i++) {
            if (_qgates[i]->getTime() > current_max_depth && (_qgates[i]->getTime()-_qgates[i]->getDelay()) >=start && (_qgates[i]->getTime()) <= end) {
                current_max_depth = _qgates[i]->getTime();
                depth ++;
            }
        }
        cout << TF::BOLD(TF::MAGENTA("Qubit " + to_string(qb->getId()) + " : ")) << endl;
        cout << TF::BOLD(TF::MAGENTA("Total       : " + to_string(total))) << endl;
        cout << TF::BOLD(TF::GREEN("Clifford    : " + to_string(clifford))) << endl;
        cout << "└── " << TF::BOLD(TF::RED("2-qubit : " + to_string(cxcnt))) << endl;
        cout << TF::BOLD(TF::RED("T-family    : " + to_string(tfamily))) << endl;
        if (nct > 0)
            cout << TF::BOLD(TF::RED("Others      : " + to_string(nct))) << endl;
        else
            cout << TF::BOLD(TF::GREEN("Others      : " + to_string(nct))) << endl;
        float usage = float(total) * 100 /float(depth);
         ostringstream oss;
         oss << setprecision(6) << usage;
         cout << TF::BOLD(TF::GREEN("Usage rate  : " + oss.str() + "%")) << endl;
         cout << "────────────────────────────────────" << endl;
        total = 0;
        clifford = 0;
        tfamily = 0;
        cxcnt = 0;
        nct = 0;
        h = 0;
        rz = 0;
        z = 0;
        s = 0;
        sdg = 0;
        t = 0;
        tdg = 0;
        rx = 0;
        x = 0;
        sx = 0;
        ry = 0;
        y = 0;
        sy = 0;
        mcp = 0;
        cz = 0;
        // ccz = 0;
        // crz = 0;
        mcrx = 0;
        cx = 0;
        ccx = 0;
        mcry = 0;
    }
}

