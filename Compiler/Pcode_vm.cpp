//
// Created by wuhaotian on 2021/11/17.
//

#include "Pcode_vm.h"
#include "TypeValue.h"
#include <algorithm>

Pcode_vm::Pcode_vm() {
    this->display.push_back(0);
    this->indexes.push_back(0);
    this->memory_sp = 0;
    for (int i = 0; i < 100; ++i) {
        memory.push_back(0);
        this->memory_sp++;
    }
    this->table_sp = 0;
    this->Pcode_PC = 0;
    this->floor = -1;
    this->alu_label = 2000;
}

void Pcode_vm::addinstr(Pcode_instr instr) {
    this->Pcode_table.push_back(instr);
    table_sp++;
}

void Pcode_vm::setlabel_to_map(string label, int index) {
    label_map[label] = index;
}

void Pcode_vm::printyourself() {
    for (int i = 0; i < Pcode_table.size(); ++i) {
        Pcode_table[i].printyourself();
    }
}

struct Datastack_item Pcode_vm::get_datastack_item(string fname) {
    //cout << "begin found:" << fname << this->floor << " " << display.back() << endl;
    for (int i = floor; i >= display.back(); --i) {
        if (datastack[i].itemname == fname) {
            return datastack[i];
        }
    }
    //cout << "curdisply not found " << fname << endl;
    //cout << "now display is ";
    /*for (int j = 0; j < display.size(); ++j) {
        cout << display[j] << " ";
    }
    cout << endl;*/
    if (!display.empty()) {
        int temp = min(display[1] - 1, floor);
        for (int i = temp; i >= 0; --i) {
            if (datastack[i].itemname == fname) {
                return datastack[i];
            }
        }
    }
    //cout << fname << " not found" << endl;
    return {};
}

void Pcode_vm::Pcode_cm_run() {
    while (Pcode_PC < Pcode_table.size() && Pcode_table[Pcode_PC].instrtype != Exit) {
        struct Pcode_instr cur = Pcode_table[Pcode_PC];
        //cout << "run " << recordmap.at(cur.instrtype) << endl;
        switch (cur.instrtype) {
            case Nop_Instr: {
                Pcode_PC++;
                break;
            }
            case Var: {
                struct Datastack_item datastackItem =
                        Datastack_item(cur.name, cur.t1, cur.t2, cur.t3, display[display.size() - 1]);

                if (cur.t3 == 0) {
                    datastackItem.addr = this->memory_sp;
                    memory.push_back(0);
                    this->memory_sp++;
                } else if (cur.t3 == 1) {
                    datastackItem.addr = this->memory_sp;
                    for (int i = 0; i < cur.t1; ++i) {
                        memory.push_back(0);
                        memory_sp++;
                    }
                    //this->memory_sp += cur.t1;
                } else {
                    datastackItem.addr = this->memory_sp;
                    for (int i = 0; i < cur.t1 * cur.t2; ++i) {
                        memory.push_back(0);
                        memory_sp++;
                    }
                    //this->memory_sp += cur.t1 * cur.t2;
                }
                //cout << "var " << datastackItem.itemname << " addr is " << datastackItem.addr << endl;
                datastack.push_back(datastackItem);
                floor++;
                Pcode_PC++;
                break;
            }
            case Upper_index: {
                this->indexes.push_back(floor + 1);
                Pcode_PC++;
                break;
            }
            case Pop_item: {
                for (int i = floor; i >= indexes.back(); --i) {
                    datastack.pop_back();
                    floor--;
                }
                indexes.pop_back();
                Pcode_PC++;
                break;
            }
            case Load_address: {
                //cout << "begin load address:" << endl;
                struct Datastack_item datastackItem = get_datastack_item(cur.name);
                //cout << "item name is " << datastackItem.itemname << endl;
                int addr = get_datastack_item(cur.name).addr;
                alu_stack.push_back(addr);
                Pcode_PC++;
                //cout << "Load address ok" << endl;
                break;
            }
            case Load_value: {
                int addr = alu_stack.back();
                alu_stack.pop_back();
                int value = memory[addr];
                //cout << "value in " << addr << " is " << value << endl;
                alu_stack.push_back(value);
                Pcode_PC++;
                break;
            }
            case Load_i: {
                alu_stack.push_back(cur.t1);
                Pcode_PC++;
                break;
            }
            case Gen_address: {
                struct Datastack_item curitem = get_datastack_item(cur.name);
                int addr = curitem.addr;

                if (curitem.dimension == 2) {
                    int v1 = curitem.d2;
                    if (cur.t1 == 2) {
                        int num2 = alu_stack.back();
                        alu_stack.pop_back();
                        int num1 = alu_stack.back();
                        alu_stack.pop_back();
                        int baseaddr = alu_stack.back();
                        alu_stack.pop_back();
                        int offaddr = num1 * v1 + num2 + baseaddr;
                        alu_stack.push_back(offaddr);
                    } else if (cur.t1 == 1) {
                        int num1 = alu_stack.back();
                        alu_stack.pop_back();
                        int baseaddr = alu_stack.back();
                        alu_stack.pop_back();
                        int offaddr = num1 * v1 + baseaddr;
                        alu_stack.push_back(offaddr);
                    }
                } else if (curitem.dimension == 1) {
                    //cout << curitem.itemname << " baseaddr: " << addr << endl;
                    if (cur.t1 == 1) {
                        int num1 = alu_stack.back();
                        alu_stack.pop_back();
                        int baseaddr = alu_stack.back();
                        alu_stack.pop_back();

                        alu_stack.push_back(num1 + baseaddr);
                        //cout << curitem.itemname << " offsetaddr: " << (num1 + baseaddr) << endl;
                    }
                }
                Pcode_PC++;
                break;
            }
            case STO: {
                int value = alu_stack.back();
                alu_stack.pop_back();
                int addr = alu_stack.back();
                alu_stack.pop_back();
                memory[addr] = value;
                //cout << "will sto " << value << " to " << addr << endl;
                Pcode_PC++;
                break;
            }
            case Jump_j: {
                try {
                    int instr_pc = label_map.at(cur.name);
                    Pcode_PC = instr_pc;
                    break;
                } catch (std::out_of_range) {
                    cout << "label is wrong " << cur.name << endl;
                }
            }
            case Jump_ra: {
                Pcode_PC = pc_ra.back();
                pc_ra.pop_back();
                for (int i = floor; i >= display.back(); --i) {
                    datastack.pop_back();
                    floor--;
                }
                display.pop_back();
                break;
            }
            case Jump_jal: {
                int instr_pc = label_map.at(cur.name);
                pc_ra.push_back(Pcode_PC + 1);
                Pcode_PC = instr_pc;
                display.push_back(floor + 1);
                break;
            }
            case Jump_false: {
                int value = alu_stack.back();
                alu_stack.pop_back();
                if (value == 0) {
                    int instr_pc = label_map.at(cur.name);
                    Pcode_PC = instr_pc;
                } else {
                    Pcode_PC++;
                }
                break;
            }
            case Jump_cond_True: {
                int value = alu_stack.back();
                if (value != 0) {
                    int instr_pc = label_map.at(cur.name);
                    Pcode_PC = instr_pc;
                } else {
                    Pcode_PC++;
                }
                break;
            }
            case Jump_cond_false: {
                int value = alu_stack.back();
                if (value == 0) {
                    int instr_pc = label_map.at(cur.name);
                    Pcode_PC = instr_pc;
                } else {
                    Pcode_PC++;
                }
                break;
            }
            case Label: {
                Pcode_PC++;
                break;
            }
            case Add: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 + num2);
                Pcode_PC++;
                break;
            }
            case Sub: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 - num2);
                Pcode_PC++;
                break;
            }
            case Mult: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 * num2);
                Pcode_PC++;
                break;
            }
            case Div: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 / num2);
                Pcode_PC++;
                break;
            }
            case Mod: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                if (num2 != 0) {
                    alu_stack.push_back(num1 % num2);
                } else {
                    alu_stack.push_back(num1);
                }
                Pcode_PC++;
                break;
            }
            case And: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                if (num1 == 0 || num2 == 0) {
                    alu_stack.push_back(0);
                } else {
                    alu_stack.push_back(1);
                }
                Pcode_PC++;
                break;
            }
            case Or: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                if (num1 == 0 && num2 == 0) {
                    alu_stack.push_back(0);
                } else {
                    alu_stack.push_back(1);
                }
                Pcode_PC++;
                break;
            }
            case Not: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                if (num2 == 0) {
                    alu_stack.push_back(1);
                } else {
                    alu_stack.push_back(0);
                }
                Pcode_PC++;
                break;
            }
            case Negtive: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(-num2);
                Pcode_PC++;
                break;
            }
            case Eql: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 == num2);
                Pcode_PC++;
                break;
            }
            case Neq: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 != num2);
                Pcode_PC++;
                break;
            }
            case Gre: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 > num2);
                Pcode_PC++;
                break;
            }
            case Geq: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 >= num2);
                Pcode_PC++;
                break;
            }
            case Lss: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 < num2);
                Pcode_PC++;
                break;
            }
            case Leq: {
                int num2 = alu_stack.back();
                alu_stack.pop_back();
                int num1 = alu_stack.back();
                alu_stack.pop_back();
                alu_stack.push_back(num1 <= num2);
                Pcode_PC++;
                break;
            }
            case Read: {
                int getint;
                cin >> getint;
                alu_stack.push_back(getint);
                Pcode_PC++;
                break;
            }
            case Write: {
                //cout << "format string:" << cur.name << endl;
                string res = "";
                for (int i = 0; i < cur.name.size(); ++i) {
                    if (cur.name[i] == '%' && cur.name[i + 1] == 'd') {
                        res += to_string(param_handle.back());
                        param_handle.pop_back();
                        i++;
                    } else if (cur.name[i] == '\\' && cur.name[i + 1] == 'n') {
                        res += '\n';
                        i++;
                    } else {
                        if (cur.name[i] != '"') {
                            res += cur.name[i];
                        }
                    }
                }
                cout << res;
                Pcode_PC++;
                break;
            }
            case Back: {
                for (int i = 0; i < cur.t1; ++i) {
                    int number = alu_stack.back();
                    alu_stack.pop_back();
                    param_handle.push_back(number);
                }
                Pcode_PC++;
                break;
            }
            case Para_Handle: {
                struct Datastack_item curitem = datastack.back();
                datastack.pop_back();
                if (curitem.dimension == 2) {
                    //cout << "two dimen handle " << curitem.itemname << endl;
                    curitem.addr = param_handle.back();
                    curitem.d2 = cur.t1;
                    //cout << "handle " << curitem.addr << " to " << curitem.itemname << endl;
                    param_handle.pop_back();
                } else if (curitem.dimension == 1) {
                    curitem.addr = param_handle.back();
                    param_handle.pop_back();
                } else {
                    int faddr = curitem.addr;
                    memory[faddr] = param_handle.back();
                    param_handle.pop_back();
                }
                datastack.push_back(curitem);
                Pcode_PC++;
                break;
            }
            case alu_moniter: {
                this->alu_label = alu_stack.size() - 1;
                Pcode_PC++;
                break;
            }
            case alu_cleaner: {
                for (int i = alu_stack.size() - 1; i > alu_label; --i) {
                    alu_stack.pop_back();
                }
                Pcode_PC++;
                break;
            }
            default:
                break;
        }
    }
}
