//
// Created by wuhaotian on 2021/11/17.
//

#include "Pcode_instr.h"
#include <iostream>

Pcode_instr::Pcode_instr(Pcodeinstrtype pcodeinstrtype) {
    this->instrtype = pcodeinstrtype;

    this->t1 = -10;
    this->t2 = -10;
    this->t3 = -10;
    this->name = "relu";
}

Pcode_instr::Pcode_instr(Pcodeinstrtype pcodeinstrtype, string fname) {
    this->instrtype = pcodeinstrtype;
    this->name = fname;

    this->t1 = -10;
    this->t2 = -10;
    this->t3 = -10;
}

Pcode_instr::Pcode_instr(Pcodeinstrtype pcodeinstrtype, string fname, int d1) {
    this->instrtype = pcodeinstrtype;
    this->name = fname;
    this->t1 = d1;

    this->t2 = -10;
    this->t3 = -10;
}

Pcode_instr::Pcode_instr(Pcodeinstrtype pcodeinstrtype, string fname, int d1, int d2, int dimen) {
    this->instrtype = pcodeinstrtype;
    this->name = fname;
    this->t1 = d1;
    this->t2 = d2;
    this->t3 = dimen;
}

Pcode_instr::Pcode_instr(Pcodeinstrtype pcodeinstrtype, int d1) {
    this->instrtype = pcodeinstrtype;
    this->t1 = d1;

    this->name = "relu";
    this->t2 = -10;
    this->t3 = -10;
}

void Pcode_instr::printyourself() {
    cout << recordmap.at(this->instrtype) << " " <<
         this->name << " " << this->t1 << " " << this->t2 << " " << this->t3 << endl;
}