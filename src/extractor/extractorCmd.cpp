/****************************************************************************
  FileName     [ m2Cmd.cpp ]
  PackageName  [ m2 ]
  Synopsis     [ Define basic m2 package commands ]
  Author       [ Chin-Yi Cheng ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <iostream>
#include <iomanip>
#include "util.h"
#include "extractorCmd.h"
#include "extract.h"
#include "zxGraphMgr.h"

using namespace std;
extern size_t verbose;
extern int effLimit;
extern ZXGraphMgr *zxGraphMgr;

bool initExtractCmd()
{
   if (!(cmdMgr->regCmd("EXTRact", 4, new ExtractCmd) && 
         cmdMgr->regCmd("EXTStep", 4, new ExtractStepCmd)
         ))
   {
      cerr << "Registering \"extract\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}

CmdExecStatus
ExtractCmd::exec(const string &option) {    
    if (!CmdExec::lexNoOption(option)) return CMD_EXEC_ERROR;

    if(zxGraphMgr->getgListItr() == zxGraphMgr->getGraphList().end()){
        cerr << "Error: ZX-graph list is empty now. Please ZXNew before EXTRact." << endl;
        return CMD_EXEC_ERROR;
    }
    else{
        zxGraphMgr->copy(zxGraphMgr->getNextID());
        Extractor ext(zxGraphMgr->getGraph());
        ext.extract();
    }
    return CMD_EXEC_DONE;
}

void ExtractCmd::usage(ostream &os) const {
    os << "Usage: EXTRact" << endl;
}

void ExtractCmd::help() const {
    cout << setw(15) << left << "EXTRact: "
         << "extract the circuit from the ZX-graph" << endl;
}

CmdExecStatus
ExtractStepCmd::exec(const string &option) {    
    string token;
    if (!CmdExec::lexSingleOption(option, token)) return CMD_EXEC_ERROR;

    if(zxGraphMgr->getgListItr() == zxGraphMgr->getGraphList().end()){
        cerr << "Error: ZX-graph list is empty now. Please ZXNew before ZXPrint." << endl;
        return CMD_EXEC_ERROR;
    }
    
    if (myStrNCmp("-CLFrontier", token, 4) == 0){
        zxGraphMgr->copy(zxGraphMgr->getNextID());
        Extractor ext(zxGraphMgr->getGraph());
        ext.cleanFrontier();
    }   
    else if (myStrNCmp("-RMGadget", token, 4) == 0){
        zxGraphMgr->copy(zxGraphMgr->getNextID());
        Extractor ext(zxGraphMgr->getGraph());
        ext.removeGadget();
    }
    else {
        cout << "Error: unsupported option " << token << " !!"  << endl;
        return CMD_EXEC_ERROR;
    }
    return CMD_EXEC_DONE;
}

void ExtractStepCmd::usage(ostream &os) const {
    os << "Usage: EXTStep <-CLFrontier | -RMGadget >" << endl;
}

void ExtractStepCmd::help() const {
    cout << setw(15) << left << "EXTStep: "
         << "perform a step in extraction" << endl;
}