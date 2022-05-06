//
// Created by wuhaotian on 2021/11/10.
//

#ifndef HOMEWORK5_TYPEVALUE_H
#define HOMEWORK5_TYPEVALUE_H

#include <map>
#include <string>

using namespace std;

enum ItemType {
    ConstType,
    VarType,
    FuncType,
    ParaType
};

enum Datatype {
    IntType,
    CharType
};

enum FunctionType {
    Voidfuncion,
    Intfunction
};

enum Optype {
    NullType,
    PlusType, // +
    SubType,  // -
    MulType,  // *
    DivType,  // /
    ModType,  // %
    NegType,  // !
    OrType,   // ||
    AndType,  // &&
    EalType,  // ==
    NeqType,  // !=
    GreType,  // >
    GeqType,  // >=
    LssType,  // <
    LeqType   // <=
};

enum Pcodeinstrtype {
    Nop_Instr,
    Exit,
    Upper_index,
    Pop_item,
    Var,
    Load_address, // 将地址加载到栈
    Load_value,   // 将栈顶的地址的内容加载到栈
    Load_i, // 加载立即数到栈
    Gen_address, // 计算出数组访问的具体地址
    STO,
    Jump_j,
    Jump_false, // 如果为假则跳转
    Jump_ra, // 跳转到ra寄存器中存的地址处
    Jump_jal, // 跳转，并将当前执行指令的下一条存在ra中
    Jump_cond_True,
    Jump_cond_false,
    Call_func,
    Return,
    Label,
    Add,
    Sub,
    Mult,
    Div,
    Mod,
    And,
    Or,
    Not,
    Negtive,
    Eql,
    Neq,
    Gre,
    Geq,
    Lss,
    Leq,
    Read,
    Write,
    Back,
    Para_Handle,
    alu_moniter,
    alu_cleaner
};

static map<Pcodeinstrtype, string> recordmap = {{Exit,         "Exit"},
                                                {Var,          "Var"},
                                                {Upper_index,  "Upper_index"},
                                                {Pop_item,     "Pop_item"},
                                                {Load_address, "Load_address"},
                                                {Load_value,   "Load_value"},
                                                {Load_i,       "Load_i"},
                                                {Gen_address,  "Gen_address"},
                                                {STO,          "STO"},
                                                {Jump_j,       "Jump_j"},
                                                {Jump_false,   "Jump_false"},
                                                {Jump_ra,      "Jump_ra"},
                                                {Jump_jal,     "Jump_jal"},
                                                {Label,        "Label"},
                                                {Add,          "Add"},
                                                {Sub,          "Sub"},
                                                {Mult,         "Mult"},
                                                {Div,          "Div"},
                                                {Mod,          "Mod"},
                                                {And,          "And"},
                                                {Or,           "Or"},
                                                {Not,          "Not"},
                                                {Negtive,      "Negtive"},
                                                {Eql,          "Eql"},
                                                {Neq,          "Neq"},
                                                {Gre,          "Gre"},
                                                {Geq,          "Geq"},
                                                {Lss,          "Lss"},
                                                {Leq,          "Leq"},
                                                {Read,         "Read"},
                                                {Write,        "Write"},
                                                {Back,         "Back"},
                                                {Para_Handle,  "Para_Handle"},
                                                {alu_moniter,  "alu_moniter"},
                                                {alu_cleaner, "alu_cleaner"},
                                                {Jump_cond_True, "Jump_cond_True"},
                                                {Jump_cond_false, "Jump_cond_false"}
};

#endif //HOMEWORK5_TYPEVALUE_H
