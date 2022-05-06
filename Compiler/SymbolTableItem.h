//
// Created by wuhaotian on 2021/11/17.
//

#ifndef HOMEWORK5_SYMBOLTABLEITEM_H
#define HOMEWORK5_SYMBOLTABLEITEM_H

#include <iostream>
#include "TypeValue.h"
#include <string>

using namespace std;

class SymbolTableItem {
public:

    int d1;
    int d2;

    SymbolTableItem(string cname, ItemType itemType, int cdimension);

    SymbolTableItem(string funcname, ItemType itemType, int cdimension, FunctionType functionType);

    string get_name();

    ItemType get_type();

    int get_dim();

    FunctionType get_func_type();

    void setdimen(int fd1, int fd2);

    void setaddress(int faddr);

    int getaddr();

    void setvalue(int fvalue);

    int getvalue();

private:
    string name;
    ItemType itemtype;

    int dimension;
    FunctionType functionType;

    int value;
    int address;
};


#endif //HOMEWORK5_SYMBOLTABLEITEM_H
