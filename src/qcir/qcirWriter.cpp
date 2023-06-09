/****************************************************************************
  FileName     [ qcirWriter.cpp ]
  PackageName  [ qcir ]
  Synopsis     [ Define class QCir Writer functions ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>  // for fstream
#include <string>   // for string

#include "qcir.h"  // for QCir

using namespace std;

/**
 * @brief Write QASM
 *
 * @param filename
 * @return true if successfully write
 * @return false if path or file not found
 */
bool QCir::writeQASM(string filename) {
    updateTopoOrder();
    fstream file;
    file.open(filename, std::fstream::out);
    if (!file)
        return false;
    file << "OPENQASM 2.0;\n";
    file << "include \"qelib1.inc\";\n";
    file << "qreg q[" << _qubits.size() << "];\n";
    for (size_t i = 0; i < _topoOrder.size(); i++) {
        QCirGate* curGate = _topoOrder[i];
        file << curGate->getTypeStr();
        if (curGate->getType() == GateType::MCP || curGate->getType() == GateType::MCPX || curGate->getType() == GateType::MCPY || curGate->getType() == GateType::MCRZ || curGate->getType() == GateType::MCRX || curGate->getType() == GateType::MCRY || curGate->getType() == GateType::P || curGate->getType() == GateType::PX || curGate->getType() == GateType::PY || curGate->getType() == GateType::RZ || curGate->getType() == GateType::RX || curGate->getType() == GateType::RY) {
            file << "(" << curGate->getPhase().getAsciiString() << ")"
                 << " ";
        } else {
            file << " ";
        }
        vector<BitInfo> pins = curGate->getQubits();
        for (size_t qb = 0; qb < pins.size(); qb++) {
            file << "q[" << pins[qb]._qubit << "]";
            if (qb == pins.size() - 1)
                file << ";\n";
            else
                file << ",";
        }
    }
    return true;
}