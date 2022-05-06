//
// Created by wuhaotian on 2021/11/17.
//

#include "SymbolTableItem.h"

SymbolTableItem::SymbolTableItem(string cname, ItemType itemType, int cdimension) : name(cname),
                                                                                    itemtype(itemType),
                                                                                    dimension(cdimension) {}

SymbolTableItem::SymbolTableItem(string funcname, ItemType itemType, int cdimension, FunctionType functionType) : name(
        funcname), itemtype(itemType), dimension(cdimension), functionType(functionType) {}


void SymbolTableItem::setaddress(int faddr) {
    this->address = faddr;
}

int SymbolTableItem::getaddr() {
    return this->address;
}

void SymbolTableItem::setdimen(int fd1, int fd2) {
    this->d1 = fd1;
    this->d2 = fd2;
}

void SymbolTableItem::setvalue(int fvalue) {
    this->value = fvalue;
}

int SymbolTableItem::getvalue() {
    return this->value;
}


string SymbolTableItem::get_name() {
    return name;
}

ItemType SymbolTableItem::get_type() {
    return itemtype;
}

int SymbolTableItem::get_dim() {
    return dimension;
}

FunctionType SymbolTableItem::get_func_type() {
    return functionType;
}
