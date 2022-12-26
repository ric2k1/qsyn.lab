/****************************************************************************
  FileName     [ qcirGate.cpp ]
  PackageName  [ qcir ]
  Synopsis     [ Define qcir gate functions ]
  Author       [ Chin-Yi Cheng ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include "qcirGate.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "qcir.h"

using namespace std;

extern size_t verbose;

/**
 * @brief Get Qubit.
 *
 * @param qubit
 * @return BitInfo
 */
const BitInfo QCirGate::getQubit(size_t qubit) const {
    for (size_t i = 0; i < _qubits.size(); i++) {
        if (_qubits[i]._qubit == qubit)
            return _qubits[i];
    }
    cerr << "Not Found" << endl;
    return _qubits[0];
}

/**
 * @brief Adc qubit to a gate
 *
 * @param qubit
 * @param isTarget
 */
void QCirGate::addQubit(size_t qubit, bool isTarget) {
    BitInfo temp = {._qubit = qubit, ._parent = NULL, ._child = NULL, ._isTarget = isTarget};
    _qubits.push_back(temp);
}

/**
 * @brief Set parent to the gate on qubit.
 *
 * @param qubit
 * @param p
 */
void QCirGate::setParent(size_t qubit, QCirGate *p) {
    for (size_t i = 0; i < _qubits.size(); i++) {
        if (_qubits[i]._qubit == qubit) {
            _qubits[i]._parent = p;
            break;
        }
    }
}

/**
 * @brief Add dummy child c to gate
 *
 * @param c
 */
void QCirGate::addDummyChild(QCirGate *c) {
    BitInfo temp = {._qubit = 0, ._parent = NULL, ._child = c, ._isTarget = false};
    _qubits.push_back(temp);
}

/**
 * @brief Set child to gate on qubit.
 *
 * @param qubit
 * @param c
 */
void QCirGate::setChild(size_t qubit, QCirGate *c) {
    for (size_t i = 0; i < _qubits.size(); i++) {
        if (_qubits[i]._qubit == qubit) {
            _qubits[i]._child = c;
            break;
        }
    }
}

/**
 * @brief Print Gate brief information
 */
void QCirGate::printGate() const {
    cout << "ID:" << right << setw(4) << _id;
    cout << " (" << right << setw(3) << getTypeStr() << ") ";
    // cout << "Gate " << _id << ": " << getTypeStr() << "   \t"
    cout << "     Time: " << right << setw(4) << _time << "     Qubit: ";
    for (size_t i = 0; i < _qubits.size(); i++) {
        cout << right << setw(3) << _qubits[i]._qubit << " ";
    }
    cout << endl;
}

/**
 * @brief Print single-qubit gate.
 *
 * @param gtype
 * @param showTime
 */
void QCirGate::printSingleQubitGate(string gtype, bool showTime) const {
    BitInfo Info = getQubits()[0];
    string qubitInfo = "Q" + to_string(Info._qubit);
    string parentInfo = "";
    if (Info._parent == NULL)
        parentInfo = "Start";
    else
        parentInfo = ("G" + to_string(Info._parent->getId()));
    string childInfo = "";
    if (Info._child == NULL)
        childInfo = "End";
    else
        childInfo = ("G" + to_string(Info._child->getId()));
    for (size_t i = 0; i < parentInfo.size() + qubitInfo.size() + 2; i++)
        cout << " ";
    cout << " ┌─";
    for (size_t i = 0; i < gtype.size(); i++) cout << "─";
    cout << "─┐ " << endl;
    cout << qubitInfo << " " << parentInfo << " ─┤ " << gtype << " ├─ " << childInfo << endl;
    for (size_t i = 0; i < parentInfo.size() + qubitInfo.size() + 2; i++)
        cout << " ";
    cout << " └─";
    for (size_t i = 0; i < gtype.size(); i++) cout << "─";
    cout << "─┘ " << endl;
    if (gtype == "RX" || gtype == "RY" || gtype == "RZ")
        cout << "Rotate Phase: " << _rotatePhase << endl;
    if (showTime)
        cout << "Execute at t= " << getTime() << endl;
}

/**
 * @brief Print multiple-qubit gate.
 *
 * @param gtype
 * @param showRotate
 * @param showTime
 */
void QCirGate::printMultipleQubitsGate(string gtype, bool showRotate, bool showTime) const {
    assert(gtype.size() % 2 == 1);
    size_t paddingSize = (gtype.size() - 1) / 2;
    string max_qubit = to_string(max_element(_qubits.begin(), _qubits.end(), [](BitInfo const a, BitInfo const b) {
                                     return a._qubit < b._qubit;
                                 })->_qubit);

    vector<string> parents;
    for (size_t i = 0; i < _qubits.size(); i++) {
        if (getQubits()[i]._parent == NULL)
            parents.push_back("Start");
        else
            parents.push_back("G" + to_string(getQubits()[i]._parent->getId()));
    }
    string max_parent = *max_element(parents.begin(), parents.end(), [](string const a, string const b) {
        return a.size() < b.size();
    });

    for (size_t i = 0; i < _qubits.size(); i++) {
        BitInfo Info = getQubits()[i];
        string qubitInfo = "Q";
        for (size_t j = 0; j < max_qubit.size() - to_string(Info._qubit).size(); j++)
            qubitInfo += " ";
        qubitInfo += to_string(Info._qubit);
        string parentInfo = "";
        if (Info._parent == NULL)
            parentInfo = "Start";
        else
            parentInfo = ("G" + to_string(Info._parent->getId()));
        for (size_t k = 0; k < max_parent.size() - (parents[i].size()); k++)
            parentInfo += " ";
        string childInfo = "";
        if (Info._child == NULL)
            childInfo = "End";
        else
            childInfo = ("G" + to_string(Info._child->getId()));
        if (Info._isTarget) {
            for (size_t j = 0; j < max_qubit.size() + max_parent.size() + 3; j++)
                cout << " ";
            cout << " ┌─";
            for (size_t j = 0; j < paddingSize; j++) cout << "─";
            cout << "┴";
            for (size_t j = 0; j < paddingSize; j++) cout << "─";
            cout << "─┐ " << endl;
            cout << qubitInfo << " " << parentInfo << " ─┤ " << gtype << " ├─ " << childInfo << endl;
            for (size_t j = 0; j < max_qubit.size() + max_parent.size() + 3; j++)
                cout << " ";
            cout << " └─";
            for (size_t j = 0; j < 2 * paddingSize; j++) cout << "─";
            cout << "──┘ " << endl;
        } else {
            cout << qubitInfo << " " << parentInfo << " ──";
            for (size_t j = 0; j < paddingSize; j++) cout << "─";
            cout << "─●─";
            for (size_t j = 0; j < paddingSize; j++) cout << "─";
            cout << "── " << childInfo << endl;
        }
    }
    if (showRotate)
        cout << "Rotate Phase: " << _rotatePhase << endl;
    if (showTime)
        cout << "Execute at t= " << getTime() << endl;
}