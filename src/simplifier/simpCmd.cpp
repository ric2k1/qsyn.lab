/****************************************************************************
  FileName     [ simpCmd.cpp ]
  PackageName  [ simplifier ]
  Synopsis     [ Define simplifier package commands ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cstddef>  // for size_t
#include <iomanip>
#include <iostream>
#include <string>

#include "apCmd.h"
#include "cmdMacros.h"  // for CMD_N_OPTS_AT_MOST_OR_RETURN
#include "phase.h"      // for Phase
#include "simplify.h"
#include "zxCmd.h"
#include "zxGraphMgr.h"

using namespace std;
using namespace ArgParse;
extern size_t verbose;

unique_ptr<ArgParseCmdType> ZXGSimpCmd();

bool initSimpCmd() {
    if (!(
            cmdMgr->regCmd("ZXGSimp", 4, ZXGSimpCmd()))) {
        cerr << "Registering \"zx\" commands fails... exiting" << endl;
        return false;
    }
    return true;
}

ArgType<int>::ConstraintType validPreduceParam = {
    [](ArgType<int> &arg) {
        return [&arg]() {
            return (arg.getValue() <= 10 && arg.getValue() >= 2);
        };
    },
    [](ArgType<int> const &arg) {
        return [&arg]() {
            cerr << "The parameter in partition reduce should be in the range of [2, 10]" << endl;
        };
    }};

//------------------------------------------------------------------------------------------------------------------
//    ZXGSimp [-TOGraph | -TORGraph | -HRule | -SPIderfusion | -BIAlgebra | -IDRemoval | -STCOpy | -HFusion |
//             -HOPF | -PIVOT | -LComp | -INTERClifford | -PIVOTGadget | -PIVOTBoundary | -CLIFford | -FReduce | -SReduce | -PReduce]
//------------------------------------------------------------------------------------------------------------------
unique_ptr<ArgParseCmdType> ZXGSimpCmd() {
    auto cmd = make_unique<ArgParseCmdType>("ZXGSimp");

    cmd->parserDefinition = [](ArgumentParser &parser) {
        parser.help("perform simplification strategies for ZX-graph");
        parser.addArgument<int>("d")
            .constraint(validPreduceParam)
             .required(false)
            .help("parameter for preduce");
            

        auto mutex = parser.addMutuallyExclusiveGroup();
        mutex.addArgument<bool>("-freduce")
            .action(storeTrue)
            .help("perform full reduce");
        mutex.addArgument<bool>("-sreduce")
            .action(storeTrue)
            .help("perform symbolic reduce");
        mutex.addArgument<bool>("-preduce")
            .action(storeTrue)
            .help("perform partitioning reduce");


        mutex.addArgument<bool>("-interclifford")
            .action(storeTrue)
            .help("perform inter-clifford");
        mutex.addArgument<bool>("-clifford")
            .action(storeTrue)
            .help("perform clifford");

        mutex.addArgument<bool>("-bialgebra")
            .action(storeTrue)
            .help("perform bialgebra");
        mutex.addArgument<bool>("-gadgetfusion")
            .action(storeTrue)
            .help("perform gadget fusion");
        mutex.addArgument<bool>("-hfusion")
            .action(storeTrue)
            .help("perform hadamard fusion");
        mutex.addArgument<bool>("-hrule")
            .action(storeTrue)
            .help("perform hadamard rule");
        mutex.addArgument<bool>("-idremoval")
            .action(storeTrue)
            .help("perform identity removal");
        mutex.addArgument<bool>("-lcomp")
            .action(storeTrue)
            .help("perform local complementation");
        mutex.addArgument<bool>("-pivotrule")
            .action(storeTrue)
            .help("perform pivot");
        mutex.addArgument<bool>("-pivotboundary")
            .action(storeTrue)
            .help("perform pivot boundary");
        mutex.addArgument<bool>("-pivotgadget")
            .action(storeTrue)
            .help("perform pivot gadget");
        mutex.addArgument<bool>("-spiderfusion")
            .action(storeTrue)
            .help("perform spider fusion");
        mutex.addArgument<bool>("-stcopy")
            .action(storeTrue)
            .help("perform state copy");

        mutex.addArgument<bool>("-tograph")
            .action(storeTrue)
            .help("convert to green (Z) graph");
        mutex.addArgument<bool>("-torgraph")
            .action(storeTrue)
            .help("convert to red (X) graph");
    };

    cmd->onParseSuccess = [](ArgumentParser const &parser) {
        ZX_CMD_GRAPHMGR_NOT_EMPTY_OR_RETURN("ZXGSimp");
        Simplifier s(zxGraphMgr->getGraph());
        if (parser["-sreduce"].isParsed())
            s.symbolicReduce();
        else if (parser["-preduce"].isParsed()){
            if(!parser["d"].isParsed()) 
                cerr << "Error: Partition reduce in ZXGSimp should provide a integer `d` after the argument!" << endl;
            else s.partitionReduce(parser["d"]);
        }
        else if (parser["-interclifford"].isParsed())
            s.interiorCliffordSimp();
        else if (parser["-clifford"].isParsed())
            s.cliffordSimp();
        else if (parser["-bialgebra"].isParsed())
            s.bialgSimp();
        else if (parser["-gadgetfusion"].isParsed())
            s.gadgetSimp();
        else if (parser["-hfusion"].isParsed())
            s.hfusionSimp();
        else if (parser["-hrule"].isParsed())
            s.hruleSimp();
        else if (parser["-idremoval"].isParsed())
            s.idSimp();
        else if (parser["-lcomp"].isParsed())
            s.lcompSimp();
        else if (parser["-pivotrule"].isParsed())
            s.pivotSimp();
        else if (parser["-pivotboundary"].isParsed())
            s.pivotBoundarySimp();
        else if (parser["-pivotgadget"].isParsed())
            s.pivotGadgetSimp();
        else if (parser["-spiderfusion"].isParsed())
            s.sfusionSimp();
        else if (parser["-stcopy"].isParsed())
            s.copySimp();
        else if (parser["-tograph"].isParsed())
            s.toGraph();
        else if (parser["-torgraph"].isParsed())
            s.toRGraph();
        else
            s.fullReduce();
        return CMD_EXEC_DONE;
    };

    return cmd;
}