//
// Created by wuhaotian on 2021/11/17.
//

#ifndef HOMEWORK5_PCODE_VM_H
#define HOMEWORK5_PCODE_VM_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "Datastack_item.h"
#include "Pcode_instr.h"

using namespace std;


class Pcode_vm {
public:

    Pcode_vm();

    void addinstr(Pcode_instr instr);

    void setlabel_to_map(string label, int index);

    void printyourself();

    void Pcode_cm_run();

    struct Datastack_item get_datastack_item(string fname);

    vector<Pcode_instr> Pcode_table;
    vector<int> alu_stack; /**计算栈*/
    map<string, int> label_map;  /**label地址对应map*/
    vector<Datastack_item> datastack;  /**运行数据栈*/
    vector<int> pc_ra;
    vector<int> param_handle;

    vector<int> memory;  /**内存*/

    vector<int> indexes;
    vector<int> display;

    int Pcode_PC;
    int table_sp;
    int memory_sp;
    int floor;
    int alu_label;

};


#endif //HOMEWORK5_PCODE_VM_H
