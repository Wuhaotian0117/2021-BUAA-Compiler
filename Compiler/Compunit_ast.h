//
// Created by wuhaotian on 2021/11/10.
//

#ifndef HOMEWORK5_COMPUNIT_AST_H
#define HOMEWORK5_COMPUNIT_AST_H

#include <iostream>
#include "structdef.h"
#include "SymbolTable.h"
#include "Pcode_vm.h"
#include "Error.h"

using namespace std;

class Compunit_ast {
public:

    int label_int;
    int cond_int;

    Compunit_ast(SymbolTable &table, Pcode_vm &pcodeVm, Error &error);

    void adddecl(struct AstDecl *newdecl);

    void addfuncdef(struct AstfuncDef *newfuncdef);

    void setmainfundef(struct Astmaindef *mainfundef);

    void Semantic_ast();

    void Semantic_Astdecl(struct AstDecl *fdecl);

    void Semantic_Subdecl(struct SubDecl *fsubdecl, bool fis_const);

    void Subdecl_const(struct SubDecl *fsubdecl);

    void Subdecl_notconst(struct SubDecl *fsubdecl);

    void Semantic_Funcdef(struct AstfuncDef *ffuncdef);

    void Semantic_expr(struct expr *fexpr, string cond_label);

    void Semantic_binary(struct Binaryexp *binaryexp, string cond_label);

    void Semantic_Lval(struct Lval *lval);

    void Semantic_Cond(struct AstCond *astCond, string cond_label);

    vector<vector<struct expr *>> get_condense_exp(struct expr* fexpr);

    int Semantic_op_list(vector<Optype> v1);

    void Semantic_AstBlock(struct AstBlock *fblock, string label1, string label2);

    int get_const_value(struct expr *fexpr);

    vector<int> get_array_value(struct AstInitval *astInitval);

    vector<struct expr *> array_condense(struct AstInitval *astInitval);

    void Semantic_stmtitem(struct StmtItem *fstmtitem, string label1, string label2);

    void Semantic_Assign(struct Assign *fassign);

    void Semantic_Onlyexpr(struct Only_expr *fonlyexpr);

    void Semantic_ifstmt(struct Ifstmt *fifstmt, string label1, string label2);

    void Semantic_whilestmt(struct Whilestmt *fwhilestmt);

    void Semantic_break(string label2, int line);

    void Semantic_continue(string label1, int line);

    void Semantic_Return(struct Returnstmt *freturnstmt);

    void Semantic_Read(struct Readstmt *freadstmt);

    void Semantic_Write(struct Printstmt *fprintstmt);

    void Semantic_Maindef(struct Astmaindef *fmaindef);

    void printyourself();

private:
    SymbolTable &table;
    Pcode_vm &pcodeVm;
    vector<AstDecl *> decl_list;
    vector<AstfuncDef *> func_list;
    Astmaindef *astmaindef;
    Error &myerror;

    int loop_num;
    bool in_void_func;
};


#endif //HOMEWORK5_COMPUNIT_AST_H
