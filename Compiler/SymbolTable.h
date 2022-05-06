//
// Created by wuhaotian on 2021/11/17.
//

#ifndef HOMEWORK5_SYMBOLTABLE_H
#define HOMEWORK5_SYMBOLTABLE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "TypeValue.h"
#include "SymbolTableItem.h"

using namespace std;

typedef struct Function {
    string func_name;
    int para_num;
    FunctionType functionType;
    vector<int> para_list;
} Func_def;

class SymbolTable {
public:

    vector<SymbolTableItem> table;

    vector<int> table_memory;
    int sp;

    SymbolTable();

    void addItem(string name, ItemType itemType, int dimension);

    void addItem(string name, ItemType itemType, int dimension, FunctionType functionType);

    void addFunc(string funcname, int para_num, FunctionType functionType);

    void addFunc(string funcname, int para_num, FunctionType functionType, vector<int> &para_list);

    int check_definition(string name, ItemType itemType); // 定义时检查是否已经定义

    int check_identifier(string name, ItemType itemType, int dimension); // 使用时检查是否定义

    int check_dimension(string name);

    int check_func_para(string funcname, int rek);

    int check_func_para(string funcname, vector<int> rpara);

    int check_type(string name, ItemType itemType);

    void assign_memory(int num);

    void set_item_length(string fname, int fd1, int fd2);

    void set_item_addr(string fname, int faddr);

    void set_item_value(string fname, int fvalue);

    int get_item_value(string fname);

    int get_item_isconst(string fname);

    int get_item_address(string fname);

    int get_item_address(string fname, int fd1, int fd2);

    void upper_index();

    void pop_item();

private:

    vector<int> indexes;
    vector<Func_def> func_list;

    int floor;


};


#endif //HOMEWORK5_SYMBOLTABLE_H
