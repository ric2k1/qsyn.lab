/****************************************************************************
  FileName     [ apArgType.cpp ]
  PackageName  [ argparser ]
  Synopsis     [ Define parser argument types ]
  Author       [ Design Verification Lab ]
  Copyright    [ Copyright(c) 2023 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include "apArgType.h"

namespace ArgParse {

namespace detail {

std::string getTypeString(bool) { return "bool"; }
std::string getTypeString(int) { return "int"; }
std::string getTypeString(unsigned) { return "unsigned"; }
std::string getTypeString(std::string const& val) { return "string"; }

bool parseFromString(bool& val, std::string const& token) {
    if (myStrNCmp("true", token, 1) == 0) {
        val = true;
        return true;
    } else if (myStrNCmp("false", token, 1) == 0) {
        val = false;
        return true;
    }
    return false;
}

bool parseFromString(int& val, std::string const& token) {
    if (!myStr2Int(token, val)) {
        return false;
    }
    return true;
}

bool parseFromString(unsigned& val, std::string const& token) {
    if (!myStr2Uns(token, val)) {
        return false;
    }
    return true;
}

bool parseFromString(std::string& val, std::string const& token) {
    val = token;
    return true;
}

}

ArgType<bool>::ActionType storeTrue(ArgType<bool>& arg) {
    arg.defaultValue(false);
    arg.constValue(true);
    return [&arg]() -> bool {
        arg.setValueToConst();
        return true;
    };
}

ArgType<bool>::ActionType storeFalse(ArgType<bool>& arg) {
    arg.defaultValue(true);
    arg.constValue(false);
    return [&arg]() -> bool {
        arg.setValueToConst();
        return true;
    };
}

}