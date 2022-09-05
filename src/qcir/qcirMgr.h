/****************************************************************************
  FileName     [ qcirMgr.h ]
  PackageName  [ qcir ]
  Synopsis     [ Define quantum circuit manager ]
  Author       [ Chin-Yi Cheng ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef QCIR_MGR_H
#define QCIR_MGR_H

#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <iostream>
#include <functional>
#include "qcirGate.h"
#include "qcirQubit.h"
#include "qcirDef.h"
#include "phase.h"

extern QCirMgr *qCirMgr;
using namespace std;
// template<typename F>
class QCirMgr
{
public:
  QCirMgr()
  {
    _gateId = 0;
    _qubitId = 0;
    _globalDFScounter = 1;
    _dirty = true;
    _qgate.clear();
    _qubits.clear();
    _topoOrder.clear();
  }
  ~QCirMgr() {}

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  QCirGate *getGate(size_t gid) const;
  QCirQubit *getQubit(size_t qid) const;
  size_t getNQubit() const { return _qubits.size(); }
  
  // Member functions about circuit construction
  void addQubit(size_t num);
  bool removeQubit(size_t q);
  void addGate(string type, vector<size_t> bits, Phase phase, bool append);
  void prependGate(string type, vector<size_t> bits){ cout << "Prepend Gate not support now."; }
  bool removeGate(size_t id);
  bool parseQASM(string qasm_file);
  
  void updateGateTime();
  void printZXTopoOrder();
  // DFS functions
  template<typename F>
  void topoTraverse(F lambda){
      if (_dirty){
        updateTopoOrder();
        _dirty = false;
      }
      for_each(_topoOrder.begin(),_topoOrder.end(),lambda);
  }
  bool printTopoOrder();
  // pass a function F (public functions) into for_each 
  // lambdaFn such as mappingToZX / updateGateTime
 
  void updateTopoOrder();

  // Member functions about circuit reporting
  void printGates();
  bool printGateInfo(size_t,bool);
  void printSummary();
  void printQubits();
  
private:
  void DFS(QCirGate*);
  bool _dirty;
  unsigned _globalDFScounter;
  size_t _gateId;
  size_t _qubitId;
  vector<QCirGate *> _qgate;
  vector<QCirQubit*> _qubits;
  vector<QCirGate *> _topoOrder;
};

#endif // QCIR_MGR_H