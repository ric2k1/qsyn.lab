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
            .help("perform interior clifford simplification");
        mutex.addArgument<bool>("-clifford")
            .action(storeTrue)
            .help("perform clifford simplification");

        mutex.addArgument<bool>("-bialgebra")
            .action(storeTrue)
            .help("apply bialgebra rules");
        mutex.addArgument<bool>("-gadgetfusion")
            .action(storeTrue)
            .help("fuse phase gadgets connected to the same set of vertices");
        mutex.addArgument<bool>("-hfusion")
            .action(storeTrue)
            .help("remove adjacent H-boxes or H-edges");
        mutex.addArgument<bool>("-hrule")
            .action(storeTrue)
            .help("convert H-boxes to H-edges");
        mutex.addArgument<bool>("-idremoval")
            .action(storeTrue)
            .help("remove Z/X-spiders with no phase");
        mutex.addArgument<bool>("-lcomp")
            .action(storeTrue)
            .help("apply local complementations to vertices with phase ±π/2");
        mutex.addArgument<bool>("-pivotrule")
            .action(storeTrue)
            .help("apply pivot rules to vertex pairs with phase 0 or π.");
        mutex.addArgument<bool>("-pivotboundary")
            .action(storeTrue)
            .help("apply pivot rules to vertex pairs connected to the boundary");
        mutex.addArgument<bool>("-pivotgadget")
            .action(storeTrue)
            .help("unfuse the phase and apply pivot rules to form gadgets");
        // mutex.addArgument<bool>("-degadgetize")
        //     .action(storeTrue)
        //     .help("[UNSTABLE!] apply unfusions and pivot rules so that the resulting graph has no gadgets");
        mutex.addArgument<bool>("-spiderfusion")
            .action(storeTrue)
            .help("fuse spiders of the same color");
        mutex.addArgument<bool>("-stcopy")
            .action(storeTrue)
            .help("apply state copy rules");

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
        // else if (parser["-degadgetize"].isParsed())
        //     s.degadgetizeSimp();
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