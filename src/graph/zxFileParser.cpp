/****************************************************************************
  FileName     [ zxFileParser.cpp ]
  PackageName  [ graph ]
  Synopsis     [ Define ZX format parser implementation ]
  Author       [ Mu-Te (Joshua) Lau ]
  Copyright    [ Copyleft(c) 2022-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include "zxFileParser.h"

#include <fstream>   // for ifstream
#include <iostream>  // for ifstream

#include "phase.h"  // for Phase
#include "util.h"   // for myStr2Float, myStr2Int

using namespace std;

bool ZXFileParser::parse(const string& filename) {
    _storage.clear();
    _takenInputQubits.clear();
    _takenOutputQubits.clear();

    ifstream zxFile(filename);
    if (!zxFile.is_open()) {
        cerr << "Cannot open the file \"" << filename << "\"!!" << endl;
        return false;
    }

    return parseInternal(zxFile);
}

bool ZXFileParser::parseInternal(ifstream& f) {
    // each line should be in the format of
    // <VertexString> [(<Qubit, Column>)] [NeighborString...] [Phase phase]
    _lineNumber = 1;
    for (string line; getline(f, line); _lineNumber++) {
        line = stripLeadingSpacesAndComments(line);
        if (line.empty()) continue;

        vector<string> tokens;
        if (!tokenize(line, tokens)) return false;

        unsigned id;
        VertexInfo info;

        if (!parseTypeAndId(tokens[0], info.type, id)) return false;

        if (info.type == 'I' || info.type == 'O') {
            if (!validTokensForBoundaryVertex(tokens)) return false;
        }

        if (!parseQubit(tokens[1], info.type, info.qubit)) return false;
        if (!parseColumn(tokens[2], info.column)) return false;

        if (info.phase.fromString(tokens.back())) {
            tokens.pop_back();
        }

        pair<char, size_t> neighbor;
        for (size_t i = 3; i < tokens.size(); ++i) {
            if (!parseNeighbor(tokens[i], neighbor)) return false;
            info.neighbors.push_back(neighbor);
        }

        _storage[id] = info;
    }

    return true;
}

string ZXFileParser::stripLeadingSpacesAndComments(string& line) {
    size_t firstNonSpace = line.find_first_not_of(" ");
    size_t commentStart = line.find("//");
    if (firstNonSpace == string::npos) return "";
    if (firstNonSpace == commentStart) return "";

    return line.substr(firstNonSpace, commentStart - firstNonSpace);
}

bool ZXFileParser::tokenize(const string& line, vector<string>& tokens) {
    string token;

    // parse first token
    size_t pos = myStrGetTok(line, token);
    tokens.push_back(token);

    // parsing parenthesis
    size_t leftParenPos = line.find_first_of("(", pos);
    size_t rightParenPos = line.find_first_of(")", leftParenPos == string::npos ? 0 : leftParenPos);
    bool hasLeftParenthesis = (leftParenPos != string::npos);
    bool hasRightParenthesis = (rightParenPos != string::npos);

    if (hasLeftParenthesis) {
        if (hasRightParenthesis) {
            // coordinate string is given
            pos = myStrGetTok(line, token, leftParenPos + 1, ',');

            if (pos == string::npos) {
                printFailedAtLineNum();
                cerr << "missing comma between declaration of qubit and column!!" << endl;
                return false;
            }

            token = stripWhitespaces(token);
            if (token == "") {
                printFailedAtLineNum();
                cerr << "missing argument before comma!!" << endl;
                return false;
            }
            tokens.push_back(token);

            pos = myStrGetTok(line, token, pos + 1, ')');

            token = stripWhitespaces(token);
            if (token == "") {
                printFailedAtLineNum();
                cerr << "missing argument before right parenthesis!!" << endl;
                return false;
            }
            tokens.push_back(token);

            pos = rightParenPos + 1;
        } else {
            printFailedAtLineNum();
            cerr << "missing closing parenthesis!!" << endl;
            return false;
        }
    } else {  // if no left parenthesis
        if (hasRightParenthesis) {
            printFailedAtLineNum();
            cerr << "missing opening parenthesis!!" << endl;
            return false;
        } else {
            // coordinate info is left out
            tokens.push_back("-");
            tokens.push_back("-");
        }
    }

    // parse remaining
    pos = myStrGetTok(line, token, pos);

    while (token.size()) {
        tokens.push_back(token);
        pos = myStrGetTok(line, token, pos);
    }

    return true;
}

bool ZXFileParser::parseTypeAndId(const string& token, char& type, unsigned& id) {
    type = toupper(token[0]);

    if (type == 'G') {
        printFailedAtLineNum();
        cerr << "ground vertices are not supported yet!!" << endl;
        return false;
    }

    if (string("IOZXH").find(type) == string::npos) {
        printFailedAtLineNum();
        cerr << "unsupported vertex type (" << type << ")!!" << endl;
        return false;
    }

    string idString = token.substr(1);

    if (idString.empty()) {
        printFailedAtLineNum();
        cerr << "Missing vertex ID after vertex type declaration (" << type << ")!!" << endl;
        return false;
    }

    if (!myStr2Uns(idString, id)) {
        printFailedAtLineNum();
        cerr << "vertex ID (" << idString << ") is not an unsigned integer!!" << endl;
        return false;
    }

    if (_storage.contains(id)) {
        printFailedAtLineNum();
        cerr << "duplicated vertex ID (" << id << ")!!" << endl;
        return false;
    }

    return true;
}

bool ZXFileParser::validTokensForBoundaryVertex(const vector<string>& tokens) {
    if (tokens[1] == "-") {
        printFailedAtLineNum();
        cerr << "please specify the qubit ID to boundary vertex!!" << endl;
        return false;
    }

    if (tokens.size() <= 3) return true;

    Phase tmp;
    if (tmp.fromString(tokens.back())) {
        printFailedAtLineNum();
        cerr << "cannot assign phase to boundary vertex!!" << endl;
        return false;
    }
    return true;
}

bool ZXFileParser::parseQubit(const string& token, const char& type, int& qubit) {
    if (token == "-") {
        qubit = 0;
        return true;
    }

    if (!myStr2Int(token, qubit)) {
        printFailedAtLineNum();
        cerr << "qubit ID (" << token << ") is not an integer in line " << _lineNumber << "!!" << endl;
        return false;
    }

    if (type == 'I') {
        if (_takenInputQubits.contains(qubit)) {
            printFailedAtLineNum();
            cerr << "duplicated input qubit ID (" << qubit << ")!!" << endl;
            return false;
        }
        _takenInputQubits.insert(qubit);
    }

    if (type == 'O') {
        if (_takenOutputQubits.contains(qubit)) {
            printFailedAtLineNum();
            cerr << "duplicated output qubit ID (" << qubit << ")!!" << endl;
            return false;
        }
        _takenOutputQubits.insert(qubit);
    }

    return true;
}

bool ZXFileParser::parseColumn(const string& token, float& column) {
    if (token == "-") {
        column = 0;
        return true;
    }

    if (!myStr2Float(token, column)) {
        printFailedAtLineNum();
        cerr << "column ID (" << token << ") is not an unsigned integer!!" << endl;
        return false;
    }

    return true;
}

bool ZXFileParser::parseNeighbor(const string& token, pair<char, size_t>& neighbor) {
    char type = toupper(token[0]);
    unsigned id;
    if (string("SH").find(type) == string::npos) {
        printFailedAtLineNum();
        cerr << "unsupported edge type (" << type << ")!!" << endl;
        return false;
    }

    string neighborString = token.substr(1);

    if (neighborString.empty()) {
        printFailedAtLineNum();
        cerr << "Missing neighbor vertex ID after edge type declaration (" << type << ")!!" << endl;
        return false;
    }

    if (!myStr2Uns(neighborString, id)) {
        printFailedAtLineNum();
        cerr << "neighbor vertex ID (" << neighborString << ") is not an unsigned integer!!" << endl;
        return false;
    }

    neighbor = {type, id};
    return true;
}

void ZXFileParser::printFailedAtLineNum() const {
    cerr << "Error: failed to read line " << _lineNumber << ": ";
}