/****************************************************************************
  FileName     [ qcirAction.cpp ]
  PackageName  [ qcir ]
  Synopsis     [ Define class QCir Action functions ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>  // for assert
#include <cstddef>  // for NULL, size_t
#include <iomanip>

#include "qcir.h"        // for QCir
#include "qcirGate.h"    // for QCirGate
#include "textFormat.h"  // for TextFormat
#include "zxGraph.h"

using namespace std;
namespace TF = TextFormat;
extern size_t verbose;

/**
 * @brief
 *
 * @param start
 * @param end
 */
void QCir::analyze(size_t start, size_t end) {
    updateGateTime();
    // TODO - t3

    // Hint: Useful cmd: QCSET, setting the delay of gates.
    //       Useful functions:
    //          gate->getDelay(): getting the delay of the QCirGate.
    //          gate->getTime(): getting the "ending" time of the QCirGate, i.e. starting time = gate->getTime()-gate->getDelay().
    //       You can get first gate of the qubit by class QCirQubit. Please refer to class QCirQubit and QCirGate for more information.

    size_t clifford = 0;
    size_t cxcnt = 0;
    size_t tfamily = 0;
    size_t nct = 0;
    size_t total_gates = 0;

    auto analysisMCR = [&clifford, &tfamily, &nct, &cxcnt](QCirGate* g) -> void {
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

    // lifted from QCir::countGate
    auto countGates = [&](QCirQubit* qb) {
        clifford = 0;
        cxcnt = 0;
        tfamily = 0;
        nct = 0;
        total_gates = 0;
        QCirGate* g = qb->getFirst();
        while (g != NULL) {
            GateType type = g->getType();
            total_gates++;
            switch (type) {
                case GateType::H:
                    clifford++;
                    break;
                case GateType::P:
                    if (g->getPhase().denominator() <= 2)
                        clifford++;
                    else if (g->getPhase().denominator() == 4)
                        tfamily++;
                    else
                        nct++;
                    break;
                case GateType::RZ:
                    if (g->getPhase().denominator() <= 2)
                        clifford++;
                    else if (g->getPhase().denominator() == 4)
                        tfamily++;
                    else
                        nct++;
                    break;
                case GateType::Z:
                    clifford++;
                    break;
                case GateType::S:
                    clifford++;
                    break;
                case GateType::SDG:
                    clifford++;
                    break;
                case GateType::T:
                    tfamily++;
                    break;
                case GateType::TDG:
                    tfamily++;
                    break;
                case GateType::RX:
                    if (g->getPhase().denominator() <= 2)
                        clifford++;
                    else if (g->getPhase().denominator() == 4)
                        tfamily++;
                    else
                        nct++;
                    break;
                case GateType::X:
                    clifford++;
                    break;
                case GateType::SX:
                    clifford++;
                    break;
                case GateType::RY:
                    if (g->getPhase().denominator() <= 2)
                        clifford++;
                    else if (g->getPhase().denominator() == 4)
                        tfamily++;
                    else
                        nct++;
                    break;
                case GateType::Y:
                    clifford++;
                    break;
                case GateType::SY:
                    clifford++;
                    break;
                case GateType::MCP:
                    analysisMCR(g);
                    break;
                case GateType::CZ:
                    clifford += 3;  // H-X-H
                    cxcnt++;
                    break;
                case GateType::CCZ:
                    tfamily += 7;
                    clifford += 10;
                    cxcnt += 6;
                    break;
                case GateType::MCRX:
                    analysisMCR(g);
                    break;
                case GateType::CX:
                    clifford++;
                    cxcnt++;
                    break;
                case GateType::CCX:
                    tfamily += 7;
                    clifford += 8;
                    cxcnt += 6;
                    break;
                case GateType::MCRY:
                    analysisMCR(g);
                    break;
                default:
                    cerr << "Error: the gate type is ERRORTYPE" << endl;
                    break;
            }

            g = g->getQubit(qb->getId())._child;
        }
    };

    size_t total_time = 0;
    vector<size_t> qubit_empty_time = vector<size_t>(_qubits.size(), 0);
    vector<size_t> qubit_end_time = vector<size_t>(_qubits.size(), 0);
    for (auto qb : _qubits) {
        QCirGate* gate = qb->getFirst();
        QCirGate* next = NULL;
        size_t empty_time = gate->getTime() - gate->getDelay();
        while (gate != NULL) {
            next = gate->getQubit(qb->getId())._child;
            if (next == NULL) {
                break;
            }
            empty_time += (next->getTime() - next->getDelay()) - gate->getTime();
            gate = next;
        }
        size_t end_time = qb->getLast()->getTime();
        qubit_end_time[qb->getId()] = end_time;
        total_time = max(total_time, end_time);
        qubit_empty_time[qb->getId()] = empty_time;
    }

    for (auto qb : _qubits) {
        std::cout << TF::BOLD("Q " + to_string(qb->getId()) + ":\t");

        countGates(qb);
        cout << TF::GREEN("Total: " + to_string(total_gates)) << ", ";
        cout << TF::GREEN("Clifford: " + to_string(clifford)) << ", ";
        cout << TF::RED("2-qubit: " + to_string(cxcnt)) << ", ";
        cout << TF::RED("T-family: " + to_string(tfamily)) << ", ";
        if (nct > 0)
            cout << TF::BOLD(TF::RED("Others: " + to_string(nct)));
        else
            cout << TF::BOLD(TF::GREEN("Others: " + to_string(nct)));

        std::cout << endl;

        qubit_empty_time[qb->getId()] += total_time - qubit_end_time[qb->getId()];
        float usage_rate = 100.0 - ((qubit_empty_time[qb->getId()] * 100.0) / total_time);
        ostringstream oss;
        oss << "usage rate: " << setfill(' ') << setw(5) << setprecision(2) << fixed
            << usage_rate << "%";
        cout << '\t' << (usage_rate >= 80 ? TF::BOLD(TF::GREEN(oss.str())) : TF::BOLD(TF::RED(oss.str())))
             << '\n'
             << endl;
    }
}

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