/****************************************************************************
  FileName     [ qcirMgr.h ]
  PackageName  [ qcir ]
  Synopsis     [ Define QCir manager ]
  Author       [ Chin-Yi Cheng ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef QCIR_MGR_H
#define QCIR_MGR_H

#include <iostream>
#include <vector>

#include "qcir.h"
#include "qcirDef.h"

extern QCirMgr* qcirhMgr;
using namespace std;

//------------------------------------------------------------------------
//  Define types
//------------------------------------------------------------------------
typedef vector<QCir*> QCirList;

//------------------------------------------------------------------------
//  Define classes
//------------------------------------------------------------------------
class QCirMgr {
public:
    QCirMgr() {
        _circuitList.clear();
        _cListItr = _circuitList.begin();
        _nextID = 0;
    }
    ~QCirMgr() {}
    void reset();

    // Test
    bool isID(size_t id) const;

    // Setter and Getter
    size_t getNextID() const { return _nextID; }
    QCir* getQCircuit() const { return _circuitList[_cListItr - _circuitList.begin()]; }
    const QCirList& getQCircuitList() const { return _circuitList; }
    QCirList::iterator getcListItr() const { return _cListItr; }

    void setNextID(size_t id) { _nextID = id; }
    void setQCircuit(QCir* g) {
        delete _circuitList[_cListItr - _circuitList.begin()];
        _circuitList[_cListItr - _circuitList.begin()] = g;
        g->setId(_cListItr - _circuitList.begin());
    }

    // Add and Remove
    QCir* addQCir(size_t id);
    void removeQCir(size_t id);

    // Action
    void checkout2QCir(size_t id);
    void copy(size_t id, bool toNew = true);
    QCir* findQCirByID(size_t id) const;

    // Print
    void printQCirMgr() const;
    void printCListItr() const;
    void printQCircuitListSize() const;

private:
    size_t _nextID;
    QCirList _circuitList;
    QCirList::iterator _cListItr;
};

#endif  // QCIR_MGR_H
