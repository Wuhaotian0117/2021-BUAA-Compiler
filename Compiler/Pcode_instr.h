//
// Created by wuhaotian on 2021/11/17.
//

#ifndef HOMEWORK5_PCODE_INSTR_H
#define HOMEWORK5_PCODE_INSTR_H

#include <string>
#include <vector>
#include "TypeValue.h"

using namespace std;

class Pcode_instr {
public:
    Pcodeinstrtype instrtype;
    int t1;
    int t2;
    int t3;
    string name;

    Pcode_instr(Pcodeinstrtype pcodeinstrtype);

    Pcode_instr(Pcodeinstrtype pcodeinstrtype, string fname);

    Pcode_instr(Pcodeinstrtype pcodeinstrtype, string fname, int d1);

    Pcode_instr(Pcodeinstrtype pcodeinstrtype, int d1);

    Pcode_instr(Pcodeinstrtype pcodeinstrtype, string fname, int d1, int d2, int dimen);

    void printyourself();

};


#endif //HOMEWORK5_PCODE_INSTR_H
