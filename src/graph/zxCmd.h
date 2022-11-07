/****************************************************************************
  FileName     [ zxCmd.h ]
  PackageName  [ graph ]
  Synopsis     [ Define basic zx package commands ]
  Author       [ Cheng-Hua Lu ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/


#ifndef ZX_CMD_H
#define ZX_CMD_H

#include "cmdParser.h"

CmdClass(ZXModeCmd);
CmdClass(ZXNewCmd);
CmdClass(ZXRemoveCmd);
CmdClass(ZXCHeckoutCmd);
CmdClass(ZXPrintCmd);
CmdClass(ZXCOPyCmd);
CmdClass(ZXCOMposeCmd);
CmdClass(ZXTensorCmd);
CmdClass(ZXGPrintCmd);
CmdClass(ZXGTestCmd);
CmdClass(ZXGEditCmd);
CmdClass(ZXGTraverseCmd);
CmdClass(ZXGTSMappingCmd);
CmdClass(ZXGReadCmd);
CmdClass(ZXGWriteCmd);
CmdClass(ZXGAdjointCmd);
CmdClass(ZXGAssignCmd);

#define ZX_CMD_QUBIT_ID_VALID_OR_RETURN(option, qid) {\
if (!myStr2Int((option), (qid))) { \
    cerr << "Error: invalid qubit number!!" << endl; \
    return errorOption(CMD_OPT_ILLEGAL, (option)); \
}}

#define ZX_CMD_VERTEX_TYPE_VALID_OR_RETURN(option, vt) {\
(vt) = VertexType::ERRORTYPE; \
if (myStrNCmp("Z", (option), 1) == 0) { \
    (vt) = VertexType::Z; \
} else if (myStrNCmp("X", (option), 1) == 0) { \
    (vt) = VertexType::X; \
} else if (myStrNCmp("H_BOX", (option), 1) == 0 || myStrNCmp("HBOX", (option), 1) == 0) { \
    (vt) = VertexType::H_BOX; \
} else { \
    cerr << "Error: invalid vertex type!!" << endl; \
    return errorOption(CMD_OPT_ILLEGAL, (option)); \
}}

#define ZX_CMD_EDGE_TYPE_VALID_OR_RETURN(option, et) {\
(et) = EdgeType::ERRORTYPE; \
if (myStrNCmp("SIMPLE", (option), 1) == 0) {\
    (et) = EdgeType::SIMPLE; \
} else if (myStrNCmp("HADAMARD", (option), 1) == 0){\
    (et) = EdgeType::HADAMARD; \
} else { \
    cerr << "Error: invalid edge type!!" << endl; \
    return errorOption(CMD_OPT_ILLEGAL, (option)); \
}}

#define ZX_CMD_VERTEX_ID_VALID_OR_RETURN(option, id) {\
if (!myStr2Uns((option), (id))) { \
    cerr << "Error: invalid vertex ID!!" << endl; \
    return errorOption(CMD_OPT_ILLEGAL, (option)); \
}}

#define ZX_CMD_PHASE_VALID_OR_RETURN(option, phase) {\
(phase) = 0;\
if (!(phase).fromString(option)) {\
    cerr << "Error: not a legal phase!!" << endl;\
    return CmdExec::errorOption(CMD_OPT_ILLEGAL, option);\
}}

#endif // ZX_CMD_H