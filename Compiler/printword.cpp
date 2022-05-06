//
// Created by wuhaotian on 2021/10/15.
//

#include <iostream>
#include <map>
#include <string>

using namespace std;

const map<int, string> record = {{1,  "IDENFR"},
                                 {2,  "INTCON"},
                                 {3,  "STRCON"},
                                 {4,  "MAINTK"},
                                 {5,  "CONSTTK"},
                                 {6,  "INTTK"},
                                 {7,  "BREAKTK"},
                                 {8,  "CONTINUETK"},
                                 {9,  "IFTK"},
                                 {10, "ELSETK"},
                                 {11, "WHILETK"},
                                 {12, "GETINTTK"},
                                 {13, "PRINTFTK"},
                                 {14, "RETURNTK"},
                                 {15, "VOIDTK"},
                                 {16, "NOT"},
                                 {17, "AND"},
                                 {18, "OR"},
                                 {19, "PLUS"},
                                 {20, "MINU"},
                                 {21, "MULT"},
                                 {22, "DIV"},
                                 {23, "MOD"},
                                 {24, "LSS"},
                                 {25, "LEQ"},
                                 {26, "GRE"},
                                 {27, "GEQ"},
                                 {28, "EQL"},
                                 {29, "NEQ"},
                                 {30, "ASSIGN"},
                                 {31, "SEMICN"},
                                 {32, "COMMA"},
                                 {33, "LPARENT"},
                                 {34, "RPARENT"},
                                 {35, "LBRACK"},
                                 {36, "RBRACK"},
                                 {37, "LBRACE"},
                                 {38, "RBRACE"}};

void printword(int fsymbol, string fword, int fline) {
    if (fsymbol != 0) {
        //cout << record.at(fsymbol) << " " << fword << endl;
    }
}

string symstring(int fsymbol, string fword, int fline) {
    if (fsymbol != 0) {
        return record.at(fsymbol) + " " + fword;
    } else {
        return nullptr;
    }
}

