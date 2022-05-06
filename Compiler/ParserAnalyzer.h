//
// Created by wuhaotian on 2021/10/15.
//

#ifndef HOMEWORK3P_PARSERANALYZER_H
#define HOMEWORK3P_PARSERANALYZER_H

#include <iostream>
#include <cstdio>
#include <deque>
#include "LexicalAnalyzer.h"
#include "Compunit_ast.h"
#include "symbol.h"
#include "structdef.h"
#include "Error.h"

class ParserAnalyzer {
public:
    ParserAnalyzer(FILE *f, LexicalAnalyzer &laxer, Compunit_ast &compunit_ast, Error &myerror);

    void Program();

private:
    FILE *fp;
    int symbol;
    int nextsym;
    LexicalAnalyzer &laxer;
    Compunit_ast &compunit_ast;

    Error &myerror;

    deque<string> bufferqueue;

    int okgetsym();

    void printgrammar(string s);

    void pop_all();

    void pop_select(string s);

    void buffer_clear();

    void error();

    void Compunit();

    struct AstDecl *Decl();

    struct AstDecl *ConstDecl();

    struct SubDecl *ConstDef();

    struct AstInitval *ConstInitVal();

    struct AstDecl *VarDecl();

    struct SubDecl *VarDef();

    struct AstInitval *InitVal();

    struct AstfuncDef *FuncDef(FunctionType functionType);

    vector<struct funcparam *> FuncFParams(); // 函数形参表

    struct funcparam *FuncFParam();

    void MainFuncDef();

    struct StmtItem *Block();

    struct AstBlockItem *BlockItem();

    struct StmtItem *Stmt();

    struct AstCond *Cond();

    struct expr *LOrExp();

    struct expr *LAndExp();

    struct expr *EqExp();

    struct expr *RelExp();

    struct expr *ConstExp();

    struct expr *AddExp();

    struct expr *MulExp();

    struct expr *UnaryExp();

    Optype UnaryOp();

    struct expr *PrimaryExp();

    struct expr *Exp();

    struct expr *LVal();

    struct expr *Number();

    int IntConst();

    vector<expr *> FuncRParams(); // 函数实参表

};


#endif //HOMEWORK3P_PARSERANALYZER_H
