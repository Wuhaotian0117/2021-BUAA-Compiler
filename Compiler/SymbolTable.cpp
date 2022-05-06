//
// Created by wuhaotian on 2021/11/17.
//

#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    floor = -1;
    indexes.push_back(0);
    this->table_memory.push_back(0);
    this->sp = 1;
}

void SymbolTable::addItem(string name, ItemType itemType, int dimension) {
    SymbolTableItem newitem(name, itemType, dimension);
    table.push_back(newitem);
    floor++;
}

void SymbolTable::addItem(string name, ItemType itemType, int dimension, FunctionType functionType) {
    SymbolTableItem newitem(name, itemType, dimension, functionType);
    table.push_back(newitem);
    floor++;
}

void SymbolTable::addFunc(string funcname, int para_num, FunctionType functionType) {
    vector<int> para_list;
    Func_def newfunc = {funcname, para_num, functionType, para_list};
    func_list.push_back(newfunc);
}

void SymbolTable::addFunc(string funcname, int para_num, FunctionType functionType, vector<int> &para_list) {
    Func_def newfunc = {funcname, para_num, functionType, para_list};
    func_list.push_back(newfunc);
}

int SymbolTable::check_definition(string name, ItemType itemType) {
    if (itemType == FuncType) {
        for (int i = indexes.back(); i <= floor; ++i) { // 函数不能跟函数重名
            if (table[i].get_name() == name && table[i].get_type() == itemType) {
                return -1;
            }
        }
    } else {
        for (int i = indexes.back(); i <= floor; ++i) { // 其他标识符不能重名,变量名可以和函数名相同
            if (table[i].get_name() == name && table[i].get_type() != FuncType) {
                return -1;
            }
        }
    }
    return 0;
}

int SymbolTable::check_identifier(string name, ItemType itemType, int dimension) {
    if (itemType == FuncType) {
        for (int i = 0; i < func_list.size(); ++i) {
            if (func_list[i].func_name == name) {
                return 1;
            }
        }
        return -1;
    } else {
        for (int i = floor; i >= 0; --i) {
            if (table[i].get_name() == name && table[i].get_type() != FuncType) {
                return 1;
            }
        }
        return -1;
    }
}

int SymbolTable::check_dimension(string name) {
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == name && table[i].get_type() != FuncType) {
            /*if (table[i].get_dim() >= dimension) {
                return table[i].get_dim() - dimension;
            } else {
                return -1;
            }*/
            return table[i].get_dim();
        }
    }
    return -1;
}

int SymbolTable::check_func_para(string funcname, int rek) {
    for (int i = 0; i < func_list.size(); ++i) {
        if (func_list[i].func_name == funcname) {
            if (rek == func_list[i].para_num) {
                return 1;
            } else {
                return -1;
            }
        }
    }
    return -1;
}

int SymbolTable::check_func_para(string funcname, vector<int> rpara) {
    for (int i = 0; i < func_list.size(); ++i) {
        if (func_list[i].func_name == funcname) {
            vector<int> para_list = func_list[i].para_list;
            int j = 0, k = 0;
            for (j = 0, k = 0; j < rpara.size() && k < para_list.size(); ++j, ++k) {
                if (rpara[j] != para_list[k]) {
                    return -1;
                }
            }
            if (j != k) {
                return -1;
            }
            return 1;
        }
    }
    return -1;
}

int SymbolTable::check_type(string name, ItemType itemType) {
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == name) {
            if (table[i].get_type() == itemType) {
                return 1;
            } else {
                return -1;
            }
        }
    }
    return -1;
}

void SymbolTable::assign_memory(int num) {
    this->sp += num;
}

void SymbolTable::set_item_length(string fname, int fd1, int fd2) {
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == fname) {
            //table[i].d1 = fd1;
            //table[i].d2 = fd2;
            table[i].setdimen(fd1, fd2);
            break;
        }
    }
}

void SymbolTable::set_item_addr(string fname, int faddr) {
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == fname) {
            table[i].setaddress(faddr);
            break;
        }
    }
}

void SymbolTable::set_item_value(string fname, int fvalue) {
    for (int i = floor; i >= indexes.back(); --i) {
        if (table[i].get_name() == fname) {
            table[i].setvalue(fvalue);
            break;
        }
    }
}

int SymbolTable::get_item_value(string fname) {
    /*cout << "here is floor when " << fname << endl;
    for (int i = floor; i >= 0; --i) {
        cout << i << " " << table[i].get_name() << endl;
    }
    cout << "end" << endl;*/
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == fname) {
            return table[i].getvalue();
        }
    }
    //cout << "not found " << fname << endl;
    return 0;
}

int SymbolTable::get_item_isconst(string fname) {
    /*cout << "here is floor:" << endl;
    for (int i = floor; i >= 0; --i) {
        cout << i << " " << table[i].get_name() << endl;
    }
    cout << "end" << endl;*/
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == fname) {
            if (table[i].get_type() == ConstType) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 0;
}

int SymbolTable::get_item_address(string fname) { // 变量
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == fname) {
            return table[i].getaddr();
        }
    }
    //cout << "not found the addr " << fname << endl;
    return 0;
}

int SymbolTable::get_item_address(string fname, int fd1, int fd2) { // 二维数组
    for (int i = floor; i >= 0; --i) {
        if (table[i].get_name() == fname) {
            return table[i].getaddr() + fd1 * table[i].d2 + fd2;
        }
    }
    //cout << "not found the addr " << fname << endl;
    return 0;
}

void SymbolTable::upper_index() {
    indexes.push_back(floor + 1);
    /*cout << "indexes is ";
    for (int i = 0; i < indexes.size(); ++i) {
        cout << indexes[i] << " ";
    }
    cout << "and floor is " << floor << endl;*/
}

void SymbolTable::pop_item() {
    for (int i = floor; i >= indexes.back(); --i) {
        table.pop_back();
        floor--;
    }
    indexes.pop_back();
}
