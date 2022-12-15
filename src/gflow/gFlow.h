/****************************************************************************
  FileName     [ gFlow.h ]
  PackageName  [ gflow ]
  Synopsis     [ Define class GFlow structure ]
  Author       [ Mu-Te (Joshua) Lau ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef GFLOW_H
#define GFLOW_H

#include "zxGraph.h"
#include "m2.h"
#include <vector>
#include <unordered_map>

class GFlow {
public:
    using Levels = std::vector<ZXVertexList>;
    using CorrectionSets = std::unordered_map<ZXVertex*, ZXVertexList>;
    
    GFlow(ZXGraph* g): _zxgraph(g), _dirty(true) {}

    void reset();
    bool calculate();

    const Levels& getLevels() { return _levels; }
    bool isDirty() const { return _dirty; }

    void print() const;
    void printLevels() const;

private:
    ZXGraph* _zxgraph;
    Levels _levels;
    CorrectionSets _correctionSets;

    bool _valid;
    bool _dirty;

    // helper members
    ZXVertexList _frontier;
    ZXVertexList _neighbors;
    unordered_set<ZXVertex*> _taken;
    M2           _coefficientMatrix;

    // gflow calculation subroutines
    void clearTemporaryStorage();
    void calculateZerothLayer();
    void updateNeighborsByFrontier();
    void setCorrectionSetFromMatrix(ZXVertex* v, const M2& matrix);
    void updateFrontier();

    void printFlagInfo() const;
};

#endif // GFLOW_H