/****************************************************************************
  FileName     [ tensorMgr.h ]
  PackageName  [ tensor ]
  Synopsis     [ Define tensor manager ]
  Author       [ Mu-Te (Joshua) Lau ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef TENSOR_MGR_H
#define TENSOR_MGR_H

#include "tensorDef.h"
#include "qtensor.h"
#include <unordered_map>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>

extern TensorMgr* tensorMgr;

struct TensorInfo {
    QTensor<double>* tensor;
    std::string info;
    
    friend std::ostream& operator<< (std::ostream& os, const TensorInfo& tsInfo) {
        return os << "#Dim: " 
                  << setw(4) << right << tsInfo.tensor->dimension()
                  << "\tInfo: " << left << tsInfo.info;
    }
};

using TensorList = std::map<size_t, TensorInfo>;

class TensorMgr {
public:
    TensorMgr() {}

    void reset();

    const TensorList& getTensorList() const                        { return _tensorList; }
    QTensor<double>* getTensor (const size_t& id) const            { return _tensorList.at(id).tensor; }
    const string& getInfo (const size_t& id) const                 { return _tensorList.at(id).info; }
    
    void setTensor(const size_t& id, QTensor<double>* tensor)      { _tensorList[id].tensor = tensor; }
    void setInfo (const size_t& id, const std::string& str)        { _tensorList[id].info = str; }
    
    QTensor<double>* addTensor(const size_t& id, const std::string& str);
    void removeTensor(const size_t& id);

    void printTensorMgr() const;
    void printTensor(const size_t& id) const;

    size_t nextID();
    
private:
    TensorList _tensorList;
};

#endif // TENSOR_MGR_H 