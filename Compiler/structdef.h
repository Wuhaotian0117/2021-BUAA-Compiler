//
// Created by wuhaotian on 2021/11/10.
//

#ifndef HOMEWORK5_STRUCTDEF_H
#define HOMEWORK5_STRUCTDEF_H

#include <iostream>
#include <vector>
#include "TypeValue.h"
#include "symbol.h"

using namespace std;

/**Decl → ConstDecl | VarDecl
 * ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
VarDecl → BType VarDef { ',' VarDef }**/


/**BlockItem → Decl | Stmt*/
struct AstBlockItem {
    int block_flag;

    virtual void printyourself() {
        cout << "super class astBlockitem" << endl;
    }

};

struct AstInitval {

    int astinitval_flag;
    int line;

    virtual void printyourself() {
        cout << "super class:astInitval" << endl;
    }

};

struct expr : AstInitval {
    int exp_flag;

    virtual void setoplist(vector<Optype> op_list) {

    }

    virtual void printyourself() {
        cout << "super class:exp" << endl;
    }

};

struct Binaryexp : expr {
    //int exp_flag;
    vector<Optype> op_list;
    Optype op;
    struct expr *left;
    struct expr *right;

    Binaryexp(Optype midop, struct expr *leftnode, struct expr *rightnode, int fline) {
        this->exp_flag = 1;
        this->op = midop;
        this->left = leftnode;
        this->right = rightnode;
        this->line = fline;
    }

    void setoplist(vector<Optype> fop_list) override {
        if (this->op_list.empty()) {
            this->op_list = fop_list;
        } else {
            this->op_list.insert(this->op_list.end(), fop_list.begin(), fop_list.end());
        }
    }

    void printyourself() {
        cout << "(";
        if (left != nullptr) {
            left->printyourself();
        }
        cout << "(" << this->op << ") ";
        if (right != nullptr) {
            right->printyourself();
        }
        cout << ")";
    }
};

/**LVal → Ident {'[' Exp ']'}*/
struct Lval : expr {
    //int exp_flag;
    vector<Optype> op_list;
    string identname;
    int d1;
    int d2;
    int dimension;
    vector<expr *> exp_list;

    Lval(string fname, int fline) {
        this->exp_flag = 2;
        this->identname = fname;
        this->dimension = 0;
        this->line = fline;
    }

    void addexpr(struct expr *fexpr) {
        this->exp_list.push_back(fexpr);
        this->dimension++;
    }

    void setoplist(vector<Optype> fop_list) override {
        if (this->op_list.empty()) {
            this->op_list = fop_list;
        } else {
            this->op_list.insert(this->op_list.end(), fop_list.begin(), fop_list.end());
        }
    }

    void printyourself() {
        if (!op_list.empty()) {
            cout << "opnum: " << this->op_list.size() << " ";
        }
        //cout << identname << " [diemn: " << dimension << "] ";
        cout << identname << " ";
    }
};

struct AstNumber : expr {
    //int exp_flag;
    vector<Optype> op_list;
    int count;

    AstNumber(int fcount, int fline) {
        this->exp_flag = 3;
        this->count = fcount;
        this->line = fline;
    }

    void setoplist(vector<Optype> fop_list) override {
        if (this->op_list.empty()) {
            this->op_list = fop_list;
        } else {
            this->op_list.insert(this->op_list.end(), fop_list.begin(), fop_list.end());
        }
    }

    void printyourself() {
        if (!op_list.empty()) {
            cout << "opnum: " << this->op_list.size() << " ";
        }
        cout << count;
    }
};

struct Callfunc : expr {
    //int exp_flag;
    vector<Optype> op_list;
    string identname;
    vector<expr *> Rparam_list;

    Callfunc(string fname, int fline) {
        this->exp_flag = 4;
        this->identname = fname;
        this->line = fline;
    }

    void setoplist(vector<Optype> fop_list) override {
        if (this->op_list.empty()) {
            this->op_list = fop_list;
        } else {
            this->op_list.insert(this->op_list.end(), fop_list.begin(), fop_list.end());
        }
    }

    void setRparam_list(vector<expr *> rparam_list) {
        this->Rparam_list = rparam_list;
    }

    void printyourself() {
        if (!op_list.empty()) {
            cout << "opnum: " << this->op_list.size() << " ";
        }
        cout << identname << " (para: " << Rparam_list.size() << ") ";
    }

};


/**ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
 * InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'**/

struct Arrayinitval : AstInitval {
    vector<AstInitval *> initval_list;

    Arrayinitval() {
        this->astinitval_flag = 2;
    }

    void addinitval(struct AstInitval *finitval) {
        initval_list.push_back(finitval);
    }

    void printyourself() {
        for (int i = 0; i < initval_list.size(); ++i) {
            if (initval_list[i] != nullptr) {
                cout << '{';
                initval_list[i]->printyourself();
                cout << "},";
            }
        }
    }
};


/**ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
 * VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal**/
struct SubDecl {
    string identname;
    int d1;
    int d2;
    int dimension;
    vector<expr *> dimens;
    struct AstInitval *initval;
    int line;

    SubDecl(string fname, int fline) {
        this->identname = fname;
        this->initval = nullptr;
        this->dimension = 0;
        this->line = fline;
    }

    void setdimen(struct expr *dimen_exp) {
        dimens.push_back(dimen_exp);
        this->dimension++;
    }

    void setinitval(struct AstInitval *finitval) {
        this->initval = finitval;
    }

    void printyourself() {
        cout << "int " << identname;
        cout << "%" << this->dimension << "%";
        if (initval != nullptr) {
            cout << " = ";
            initval->printyourself();
            cout << ";";
        }
        cout << endl;
    }
};

/**Decl → ConstDecl | VarDecl*/
struct AstDecl : AstBlockItem {
    //int block_flag;
    bool is_const;
    Datatype declType;
    vector<SubDecl *> sublist;

    AstDecl(bool fis_const, Datatype fdatatype) {
        block_flag = 1;
        this->is_const = fis_const;
        this->declType = fdatatype;
    }

    void addsubdecl(struct SubDecl *fsubdecl) {
        sublist.push_back(fsubdecl);
    }

    void printyourself() {
        for (int i = 0; i < sublist.size(); ++i) {
            if (sublist[i] != nullptr) {
                sublist[i]->printyourself();
            }
        }
    }
};

/**FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]*/
struct funcparam {
    Datatype paratype;
    string paraname;
    int dimen;
    vector<expr *> exp_list;

    int line;

    funcparam(Datatype datatype, string fname, int fline) {
        this->paratype = datatype;
        this->paraname = fname;
        this->dimen = 0;
        this->line = fline;
    }

    void addexpr(struct expr *fexpr) {
        exp_list.push_back(fexpr);
    }

    void printyourself() {
        cout << paraname << " ";
    }
};


struct AstCond {
    struct expr *condexpr;

    AstCond(struct expr *condexp) {
        this->condexpr = condexp;
    }

    void printyourself() {
        if (condexpr != nullptr) {
            condexpr->printyourself();
        }
    }
};

struct StmtItem {
    int stmt_flag;
    int line;

    virtual ~StmtItem() {};

    virtual void printyourself() {
        cout << "super class StmtItem" << endl;
    }
};

struct AstStmt : AstBlockItem {
    //int block_flag;
    struct StmtItem *stmtItem;

    /** here the quote is compelled to the error: **/

    AstStmt(struct StmtItem *stmtItem1) {
        this->block_flag = 2;
        this->stmtItem = stmtItem1;
    }

    void printyourself() {
        if (stmtItem != nullptr) {
            stmtItem->printyourself();
        }
    }
};

/**LVal '=' Exp ';'*/
struct Assign : StmtItem {
    //int stmt_flag;
    struct expr *lval;
    struct expr *valueexp;

    Assign(struct expr *leftexpr, struct expr *fvalueexpr, int fline) {
        this->stmt_flag = 1;
        this->lval = leftexpr;
        this->valueexp = fvalueexpr;
        this->line = fline;
    }

    void printyourself() {
        if (lval != nullptr) {
            lval->printyourself();
        }
        cout << " = ";
        if (valueexp != nullptr) {
            valueexp->printyourself();
            cout << ";";
        }
    }
};

/** [Exp] ';'*/
struct Only_expr : StmtItem {
    //int stmt_flag;
    struct expr *only_baby;

    Only_expr() {
        this->stmt_flag = 2;
        this->only_baby = nullptr;
    }

    Only_expr(struct expr *fexpr) {
        this->stmt_flag = 2;
        this->only_baby = fexpr;
    }

    void printyourself() {
        if (only_baby != nullptr) {
            only_baby->printyourself();
            cout << ";";
        }
    }
};

/**if' '(' Cond ')' Stmt [ 'else' Stmt ]*/
struct Ifstmt : StmtItem {
    //int stmt_flag;
    struct AstCond *astcond;
    struct AstStmt *stmt;
    struct AstStmt *elsestmt;

    Ifstmt() {
        this->stmt_flag = 4;
        this->astcond = nullptr;
        this->stmt = nullptr;
        this->elsestmt = nullptr;
    }

    void setcond(struct AstCond *cond) {
        this->astcond = cond;
    }

    void setstmt(struct AstStmt *astStmt) {
        this->stmt = astStmt;
    }

    void setelsestmt(struct AstStmt *elseastStmt) {
        this->elsestmt = elseastStmt;
    }

    void printyourself() {
        cout << "if";
        cout << "(";
        if (astcond != nullptr) {
            astcond->printyourself();
        }
        cout << ")";
        if (stmt != nullptr) {
            cout << "{\n";
            stmt->printyourself();
            cout << "}";
        }
        if (elsestmt != nullptr) {
            cout << "else";
            cout << "{\n";
            if (stmt == elsestmt) {
                cout << "why you are equal?" << endl;
            }
            elsestmt->printyourself();
            cout << "}";
        }
    }
};

/**'while' '(' Cond ')' Stmt*/
struct Whilestmt : StmtItem {
    //int stmt_flag;
    struct AstCond *astcond;
    struct AstStmt *stmt;

    Whilestmt() {
        this->stmt_flag = 5;
        this->astcond = nullptr;
        this->stmt = nullptr;
    }

    void setcond(struct AstCond *cond) {
        this->astcond = cond;
    }

    void setstmt(struct AstStmt *astStmt) {
        this->stmt = astStmt;
    }

    void printyourself() {
        cout << "while(";
        if (astcond != nullptr) {
            astcond->printyourself();
        }
        cout << ")";
        if (stmt != nullptr) {
            cout << "{\n";
            stmt->printyourself();
            cout << "}";
        }
    }
};

/**'break' ';' | 'continue' ';'*/
struct Breakstmt : StmtItem {
    //int stmt_flag;

    Breakstmt(int fline) {
        this->stmt_flag = 6;
        this->line = fline;
    }

    void printyourself() {
        cout << "break;";
    }

};

struct Continuestmt : StmtItem {
    //int stmt_flag;

    Continuestmt(int fline) {
        this->stmt_flag = 7;
        this->line = fline;
    }

    void printyourself() {
        cout << "continue;";
    }
};

/**'return' [Exp] ';'*/
struct Returnstmt : StmtItem {
    //int stmt_flag;
    struct expr *returnexp;

    Returnstmt(int fline) {
        this->stmt_flag = 8;
        this->returnexp = nullptr;
        this->line = fline;
    }

    void setreturnexp(struct expr *fexp) {
        this->returnexp = fexp;
    }

    void printyourself() {
        cout << "return ";
        if (returnexp != nullptr) {
            returnexp->printyourself();
        }
        cout << ";";
    }
};

/**LVal '=' 'getint''('')'';'*/
struct Readstmt : StmtItem {
    //int stmt_flag;
    struct expr *lval;

    Readstmt(struct expr *fexpr, int fline) {
        this->stmt_flag = 9;
        this->lval = fexpr;
        this->line = fline;
    }

    void printyourself() {
        lval->printyourself();
        cout << "=getint;";
    }
};

/**'printf''('FormatString{','Exp}')'';'*/
struct Printstmt : StmtItem {
    //int stmt_flag;
    string formatstring;
    vector<expr *> exp_list;

    int formatnum;

    Printstmt(int fline) {
        this->stmt_flag = 10;
        this->line = fline;
    }

    void setstring(string fstring, int fnum) {
        this->formatstring = fstring;
        this->formatnum = fnum;
    }

    void addexpr(struct expr *fexpr) {
        this->exp_list.push_back(fexpr);
    }

    void printyourself() {
        cout << "printf: " + formatstring << " ";
        for (int i = 0; i < exp_list.size(); ++i) {
            if (exp_list[i] != nullptr) {
                exp_list[i]->printyourself();
            }
        }
    }

};

/**Block → '{' { BlockItem } '}'*/
struct AstBlock : StmtItem {
    //int stmt_flag;
    vector<AstBlockItem *> blockitem_list;

    int endline;

    AstBlock() {
        this->stmt_flag = 3;
    }

    void addblock_item(struct AstBlockItem *item) {
        blockitem_list.push_back(item);
    }

    void setendline(int fline) {
        this->endline = fline;
    }

    int check_end() {
        if (!blockitem_list.empty()) {
            if (blockitem_list.back()->block_flag != 2) {
                return -1;
            } else {
                struct AstStmt *astStmt = dynamic_cast<AstStmt *>(blockitem_list.back());
                if (astStmt->stmtItem->stmt_flag != 8) {
                    return -1;
                } else {
                    return 1;
                }
            }
        } else {
            return -1;
        }
    }

    void printyourself() {
        cout << "{" << endl;
        for (int i = 0; i < blockitem_list.size(); ++i) {
            if (blockitem_list[i] != nullptr) {
                blockitem_list[i]->printyourself();
                cout << endl;
            }
        }
        cout << "\n}" << endl;
    }

};


/**MainFuncDef → 'int' 'main' '(' ')' Block**/
struct Astmaindef {
    AstBlock *mainblock;
    int line;

    Astmaindef(struct AstBlock *fmainblock, int fline) {
        this->mainblock = fmainblock;
        this->line = fline;
    }

    void printyourself() {
        if (mainblock != nullptr) {
            cout << "int main()";
            mainblock->printyourself();
        }
    }
};

/**FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
 * FuncFParams → FuncFParam { ',' FuncFParam }**/
struct AstfuncDef {
    FunctionType functionType;
    string funcname;
    vector<funcparam *> para_list;
    struct AstBlock *funcblock;

    int line;


    AstfuncDef(FunctionType functype, string fname, int fline) {
        this->functionType = functype;
        this->funcname = fname;
        this->line = fline;
    }

    void addparam(vector<funcparam *> fpara_list) {
        this->para_list = fpara_list;
    }

    void setblock(struct AstBlock *astBlock) {
        this->funcblock = astBlock;
    }

    void printyourself() {
        cout << "funcdef " << funcname;
        cout << '(';
        for (int i = 0; i < para_list.size(); ++i) {
            if (para_list[i] != nullptr) {
                para_list[i]->printyourself();
            }
        }
        cout << ')';
        if (funcblock != nullptr) {
            funcblock->printyourself();
        }
    }

    vector<int> get_param_dimen() {
        vector<int> v1;
        for (int i = 0; i < this->para_list.size(); ++i) {
            if (para_list[i] != nullptr) {
                v1.push_back(para_list[i]->dimen);
            }
        }
        return v1;
    }
};

/*union StmtItem {
    struct Assign assign;
    struct Only_expr onlyExpr;
    struct Block block;
    struct ifstmt ifstmt;
    struct whilestmt whilestmt;
    struct breakstmt breakstmt;
    struct continuestmt continuestmt;
    struct returnstmt returnstmt;
    struct readstmt readstmt;
    struct printstmt printstmt;
};*/


#endif //HOMEWORK5_STRUCTDEF_H
