//
// Created by wuhaotian on 2021/10/15.
//

#include "ParserAnalyzer.h"
#include "printword.h"
#include "structdef.h"
#include <string>

using namespace std;

ParserAnalyzer::ParserAnalyzer(FILE *f, LexicalAnalyzer &laxer, Compunit_ast &compunit_ast, Error &myerror) :
        laxer(laxer), compunit_ast(compunit_ast), myerror(myerror) {
    fp = f;
}

int ParserAnalyzer::okgetsym() {
    int res = laxer.getsym();
    while (res <= 0 && laxer.getflag()) {
        res = laxer.getsym();
    }
    return res;
}

void ParserAnalyzer::printgrammar(string s) {
    //cout << s << endl;
}

void ParserAnalyzer::pop_all() {
    while (!bufferqueue.empty()) {
        //cout << bufferqueue.front() << endl;
        bufferqueue.pop_front();
    }
}

void ParserAnalyzer::pop_select(string s) {
    while (!bufferqueue.empty()) {
        if (bufferqueue.back() != s) {
            bufferqueue.pop_back();
        } else {
            break;
        }
    }
    while (!bufferqueue.empty()) {
        //cout << bufferqueue.front() << endl;
        bufferqueue.pop_front();
    }
}

void ParserAnalyzer::buffer_clear() {
    while (!bufferqueue.empty()) {
        bufferqueue.pop_front();
    }
}

void ParserAnalyzer::error() {
    //cout << "syntax error!" << endl;
}

void ParserAnalyzer::Program() {
    symbol = okgetsym();
    Compunit();
    //cout << "Compunit ok" << endl;
}

void ParserAnalyzer::Compunit() {
    bool funcdefop = false;
    bool mainfuncop = false;
    FunctionType functionType;
    while (true) {
        if (symbol == CONSTTK) {
            struct AstDecl *astDecl = ConstDecl();
            compunit_ast.adddecl(astDecl);
        } else if (symbol == INTTK) {
            nextsym = laxer.getlookup(); // ident or main
            /*cout << "here next sym is ";
            printword(nextsym, fword, line);*/
            if (nextsym == MAINTK) {
                mainfuncop = true;
                //cout << "here is the main func" << endl;
                break;
            }
            symbol = okgetsym(); // ident
            nextsym = laxer.getlookup(); // op
            if (nextsym == COMMA || nextsym == ASSIGN || nextsym == SEMICN || nextsym == LBRACK) { // vardef
                printword(INTTK, "int", laxer.getline()); // int
                struct AstDecl *astDecl = VarDecl();
                compunit_ast.adddecl(astDecl);
            } else if (nextsym == LPARENT) { // funcdef here symbol is ident
                printword(INTTK, "int", laxer.getline()); // int
                printgrammar("<FuncType>");
                functionType = Intfunction;
                //printword(symbol, laxer.getword(), laxer.getline());; // ident
                funcdefop = true;
                break;
            }
        } else if (symbol == VOIDTK) {
            nextsym = laxer.getlookup();
            functionType = Voidfuncion;
            funcdefop = true;
            break;
        } else {
            error();
        }
    }

    if (funcdefop) {
        if (symbol == IDENFR) {
            struct AstfuncDef *newfunc = FuncDef(functionType);
            compunit_ast.addfuncdef(newfunc);
            nextsym = laxer.getlookup();
        }
        while (true) { // here symbol ident
            if (nextsym == MAINTK) { // here symbol is int
                mainfuncop = true;
                break;
            } else {
                printword(symbol, laxer.getword(), laxer.getline());
                //printword(INTTK, "int", line); // here symbol is ident
                functionType = (laxer.getword() == "int") ? Intfunction : Voidfuncion;
                printgrammar("<FuncType>");
                symbol = okgetsym();
                struct AstfuncDef *newfunc = FuncDef(functionType);
                compunit_ast.addfuncdef(newfunc);
                nextsym = laxer.getlookup();
            }
        }
    }
    if (mainfuncop) { // here symbol is int
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == MAINTK) {
            printword(symbol, laxer.getword(), laxer.getline()); // main
            symbol = okgetsym();
            MainFuncDef();
        } else {
            error();
        }
    }
    printgrammar("<CompUnit>");
}

struct AstDecl *ParserAnalyzer::Decl() {
    //if (symbol == INTTK || symbol == CONSTTK) {
    //while (symbol == INTTK || symbol == CONSTTK) {
    if (symbol == INTTK) {
        printword(symbol, laxer.getword(), laxer.getline()); // int
        symbol = okgetsym();
        struct AstDecl *vardecl = VarDecl();
        return vardecl;
    } else {
        struct AstDecl *constdecl = ConstDecl();
        return constdecl;
    }
    //}
    /*} else {
        error();
    }*/
}

struct AstDecl *ParserAnalyzer::ConstDecl() {
    printword(symbol, laxer.getword(), laxer.getline());
    symbol = okgetsym(); // int
    printword(symbol, laxer.getword(), laxer.getline());
    symbol = okgetsym(); // ident

    int cur_line = laxer.getline();

    struct AstDecl *newdecl = new AstDecl(true, IntType);

    struct SubDecl *newsubdecl = ConstDef();
    newdecl->addsubdecl(newsubdecl);

    while (symbol == COMMA) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        cur_line = laxer.getline();
        struct SubDecl *constdecl = ConstDef();
        newdecl->addsubdecl(constdecl);
    }
    if (symbol == SEMICN) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
    } else {
        myerror.add_errorItem(LackSemicn, cur_line); // TODO
    }
    printgrammar("<ConstDecl>");
    return newdecl;
}

struct SubDecl *ParserAnalyzer::ConstDef() {
    printword(symbol, laxer.getword(), laxer.getline()); // ident

    struct SubDecl *newsubdecl = new SubDecl(laxer.getword(), laxer.getline());
    symbol = okgetsym();
    while (symbol == LBRACK) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        int cur_line = laxer.getline();
        struct expr *constexp = ConstExp();
        newsubdecl->setdimen(constexp);
        pop_all();
        if (symbol == RBRACK) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
        } else {
            myerror.add_errorItem(LackRbrack, cur_line); // TODO
        }
    }
    if (symbol == ASSIGN) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        struct AstInitval *constinitval = ConstInitVal();
        newsubdecl->setinitval(constinitval);
    } else {
        error();
    }
    printgrammar("<ConstDef>");
    return newsubdecl;
}

struct AstDecl *ParserAnalyzer::VarDecl() {

    //if (symbol == IDENFR) {
    int cur_line = laxer.getline();
    struct AstDecl *newdecl = new AstDecl(false, IntType);
    struct SubDecl *newsubdecl = VarDef();
    newdecl->addsubdecl(newsubdecl);

    while (symbol == COMMA) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        cur_line = laxer.getline();
        struct SubDecl *vardecl = VarDef();
        newdecl->addsubdecl(vardecl);
    }
    if (symbol == SEMICN) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
    } else {
        myerror.add_errorItem(LackSemicn, cur_line);
    }
    /*} else {
        error();
    }*/
    printgrammar("<VarDecl>");
    return newdecl;
}

struct AstInitval *ParserAnalyzer::ConstInitVal() {
    if (symbol == LBRACE) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == RBRACE) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            return nullptr;
        } else {
            Arrayinitval *arrayinitval = new Arrayinitval();
            struct AstInitval *constinitval = ConstInitVal();
            arrayinitval->addinitval(constinitval);
            while (symbol == COMMA) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                struct AstInitval *follow = ConstInitVal();
                arrayinitval->addinitval(follow);
            }
            if (symbol == RBRACE) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
            }
            return arrayinitval;
        }
    } else {
        struct AstInitval *initval = ConstExp();
        pop_all();
        return initval;
    }
    printgrammar("<ConstInitVal>");
}

struct SubDecl *ParserAnalyzer::VarDef() {
    //if (symbol == IDENFR) {
    printword(symbol, laxer.getword(), laxer.getline());
    struct SubDecl *newsubdecl = new SubDecl(laxer.getword(), laxer.getline());
    symbol = okgetsym();
    if (symbol == LBRACK) {
        while (symbol == LBRACK) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            int cur_line = laxer.getline();
            struct expr *varexp = ConstExp();
            newsubdecl->setdimen(varexp);
            pop_all();
            if (symbol == RBRACK) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
            } else {
                myerror.add_errorItem(LackRbrack, cur_line); // TODO
            }
        }
        if (symbol == ASSIGN) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            struct AstInitval *varinitval = InitVal();
            newsubdecl->setinitval(varinitval);
        }
    } else if (symbol == ASSIGN) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        struct AstInitval *varinitval = InitVal();
        newsubdecl->setinitval(varinitval);
    }
    /*} else {
        error();
    }*/
    printgrammar("<VarDef>");
    return newsubdecl;
}

struct AstInitval *ParserAnalyzer::InitVal() {
    if (symbol == LBRACE) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == RBRACE) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            return nullptr;
        } else {
            Arrayinitval *arrayinitval = new Arrayinitval();
            struct AstInitval *varinitval = InitVal();
            arrayinitval->addinitval(varinitval);

            while (symbol == COMMA) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                struct AstInitval *follow = InitVal();
                arrayinitval->addinitval(follow);
            }
            if (symbol == RBRACE) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
            }
            return arrayinitval;
        }
    } else {
        struct AstInitval *astInitval = Exp();
        pop_all();
        return astInitval;
    }
    printgrammar("<InitVal>");
}

struct AstfuncDef *ParserAnalyzer::FuncDef(FunctionType functionType) {
    printword(symbol, laxer.getword(), laxer.getline());

    AstfuncDef *newfunc = new AstfuncDef(functionType, laxer.getword(), laxer.getline());
    int cur_line = laxer.getline();
    symbol = okgetsym();

    if (symbol == LPARENT) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == RPARENT) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            struct AstBlock *funcblock = dynamic_cast<AstBlock *>(Block());
            newfunc->setblock(funcblock);

        } else if (symbol == INTTK) {
            cur_line = laxer.getline();
            vector<funcparam *> fparam_list = FuncFParams();
            newfunc->addparam(fparam_list);
            if (symbol == RPARENT) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                struct AstBlock *funcblock = dynamic_cast<AstBlock *>(Block());
                newfunc->setblock(funcblock);
            } else {
                myerror.add_errorItem(LackRparent, cur_line); // TODO
                struct AstBlock *funcblock = dynamic_cast<AstBlock *>(Block());
                newfunc->setblock(funcblock);
            }
        } else {
            myerror.add_errorItem(LackRparent, cur_line); // TODO
            struct AstBlock *funcblock = dynamic_cast<AstBlock *>(Block());
            newfunc->setblock(funcblock);
        }
    } else {
        error();
    }
    printgrammar("<FuncDef>");
    return newfunc;
}

vector<struct funcparam *> ParserAnalyzer::FuncFParams() {
    vector<struct funcparam *> fparam_list;
    struct funcparam *newparam = FuncFParam();
    fparam_list.push_back(newparam);
    while (symbol == COMMA) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        struct funcparam *follow = FuncFParam();
        fparam_list.push_back(follow);
    }
    printgrammar("<FuncFParams>");
    return fparam_list;
}

struct funcparam *ParserAnalyzer::FuncFParam() {
    printword(symbol, laxer.getword(), laxer.getline());
    Datatype datatype = (laxer.getword() == "int") ? IntType : CharType;
    symbol = okgetsym(); // ident
    printword(symbol, laxer.getword(), laxer.getline());

    int cur_line = laxer.getline();

    struct funcparam *newparam = new funcparam(datatype, laxer.getword(), cur_line);

    int dimen = 0;
    symbol = okgetsym();
    if (symbol == LBRACK) {
        dimen++;
        cur_line = laxer.getline();
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == RBRACK) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            while (symbol == LBRACK) {
                dimen++;
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                cur_line = laxer.getline();
                struct expr *constexp = ConstExp();
                newparam->addexpr(constexp);

                pop_all();
                if (symbol == RBRACK) {
                    printword(symbol, laxer.getword(), laxer.getline());
                    symbol = okgetsym();
                } else {
                    myerror.add_errorItem(LackRbrack, cur_line);
                }
            }
        } else {
            myerror.add_errorItem(LackRbrack, cur_line);
        }
    }
    printgrammar("<FuncFParam>");
    newparam->dimen = dimen;
    return newparam;
}

void ParserAnalyzer::MainFuncDef() {
    int cur_line = laxer.getline();
    if (symbol == LPARENT) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
    } else {
        myerror.add_errorItem(LackRparent, cur_line); // TODO
    }
    if (symbol == RPARENT) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
    } else {
        myerror.add_errorItem(LackRparent, cur_line); // TODO
    }
    if (symbol == LBRACE) {
        struct AstBlock *mainblock = dynamic_cast<AstBlock *>(Block());
        struct Astmaindef *astmaindef = new Astmaindef(mainblock, cur_line);
        compunit_ast.setmainfundef(astmaindef);
    } else {
        error();
    }
    printgrammar("<MainFuncDef>");
}

struct expr *ParserAnalyzer::ConstExp() {
    struct expr *constexp = AddExp();
    // TODO
    //printgrammar("<ConstExp>");
    bufferqueue.emplace_back("<ConstExp>");
    return constexp;
}

struct expr *ParserAnalyzer::AddExp() {
    struct expr *resexp = MulExp();
    /*while (symbol == PLUS || symbol == MINUS) {
        //printgrammar("<AddExp>");
        bufferqueue.emplace_back("<AddExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        MulExp();
    }*/
    // TODO
    while (symbol == PLUS || symbol == MINUS) {
        //printgrammar("<AddExp>");
        bufferqueue.emplace_back("<AddExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        Optype op = (symbol == PLUS) ? PlusType : SubType;
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        int cur_line = laxer.getline();
        struct expr *follow = MulExp();
        struct Binaryexp *binaryexp = new Binaryexp(op, resexp, follow, cur_line);
        bufferqueue.emplace_back("<AddExp>");
        resexp = binaryexp;
    }
    //printgrammar("<AddExp>");
    bufferqueue.emplace_back("<AddExp>");
    return resexp;
}

struct expr *ParserAnalyzer::MulExp() {
    struct expr *resexp = UnaryExp();
    /*while (symbol == MULT || symbol == DIV || symbol == MOD) {
        //printgrammar("<MulExp>");
        bufferqueue.emplace_back("<MulExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        UnaryExp();
    }*/
    // TODO
    while (symbol == MULT || symbol == DIV || symbol == MOD) {
        //printgrammar("<MulExp>");
        bufferqueue.emplace_back("<MulExp>");
        Optype op;
        if (symbol == MULT) {
            op = MulType;
        } else if (symbol == DIV) {
            op = DivType;
        } else {
            op = ModType;
        }
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        int cur_line = laxer.getline();
        struct expr *follow = UnaryExp();
        struct Binaryexp *binaryexp = new Binaryexp(op, resexp, follow, cur_line);
        bufferqueue.emplace_back("<MulExp>");
        resexp = binaryexp;
    }
    //printgrammar("<MulExp>");
    bufferqueue.emplace_back("<MulExp>");
    return resexp;
}

struct expr *ParserAnalyzer::UnaryExp() {
    if (symbol == IDENFR) {
        nextsym = laxer.getlookup();
        if (nextsym == LPARENT) {
            int cur_line = laxer.getline();
            struct Callfunc *callfunc = new Callfunc(laxer.getword(), cur_line);

            //printword(symbol, laxer.getword(), laxer.getline()); // symbol here is ident
            bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
            symbol = okgetsym(); // (
            //printword(symbol, laxer.getword(), laxer.getline()); // symbol here is (
            bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
            symbol = okgetsym();
            if (symbol == RPARENT) {
                //printword(symbol, laxer.getword(), laxer.getline());
                bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
                symbol = okgetsym();
                bufferqueue.emplace_back("<UnaryExp>");
                return callfunc;
            } else if (symbol == LPARENT || symbol == IDENFR || symbol == INTCON || symbol == PLUS || symbol == MINUS) {
                cur_line = laxer.getline();
                vector<expr *> exp_list = FuncRParams();
                callfunc->setRparam_list(exp_list);
                if (symbol == RPARENT) {
                    //printword(symbol, laxer.getword(), laxer.getline());
                    bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
                    symbol = okgetsym();
                } else {
                    myerror.add_errorItem(LackRparent, cur_line); // TODO
                }
                bufferqueue.emplace_back("<UnaryExp>");
                return callfunc;
            } else {
                myerror.add_errorItem(LackRparent, cur_line); // TODO
                return callfunc;
            }
        } else { // PrimaryExp->LVal
            struct expr *resexp = PrimaryExp();
            bufferqueue.emplace_back("<UnaryExp>");
            return resexp;
        }
    } else if (symbol == PLUS || symbol == MINUS || symbol == NOT) {
        vector<Optype> op_list;
        while (symbol == PLUS || symbol == MINUS || symbol == NOT) {
            Optype unaryop = UnaryOp();
            op_list.push_back(unaryop);
        }
        struct expr *resexp = UnaryExp();
        resexp->setoplist(op_list);
        bufferqueue.emplace_back("<UnaryExp>");
        return resexp;
    } else {
        struct expr *resexp = PrimaryExp();
        bufferqueue.emplace_back("<UnaryExp>");
        return resexp;
    }
    //printgrammar("<UnaryExp>");
    bufferqueue.emplace_back("<UnaryExp>");
}

Optype ParserAnalyzer::UnaryOp() {
    Optype op = NullType;
    if (symbol == PLUS || symbol == MINUS || symbol == NOT) {
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));

        if (symbol == PLUS) {
            op = PlusType;
        } else if (symbol == MINUS) {
            op = SubType;
        } else {
            op = NegType;
        }
        symbol = okgetsym();
    } else {
        error();
    }
    //printgrammar("<UnaryOp>");
    bufferqueue.emplace_back("<UnaryOp>");
    return op;
}

struct expr *ParserAnalyzer::PrimaryExp() {
    struct expr *resexpr = nullptr;

    if (symbol == LPARENT) {
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        int cur_line = laxer.getline();
        resexpr = Exp();
        if (symbol == RPARENT) {
            //printword(symbol, laxer.getword(), laxer.getline());
            bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
            symbol = okgetsym();
        } else {
            myerror.add_errorItem(LackRparent, cur_line); // TODO
        }
    } else if (symbol == IDENFR) {
        resexpr = LVal();
    } else if (symbol == INTCON) {
        resexpr = Number();
    } else {
        error();
    }
    //printgrammar("<PrimaryExp>");
    bufferqueue.emplace_back("<PrimaryExp>");
    return resexpr;
}

struct expr *ParserAnalyzer::Exp() {
    struct expr *addexp = AddExp();
    //printgrammar("<Exp>");
    bufferqueue.emplace_back("<Exp>");
    return addexp;
}

struct expr *ParserAnalyzer::LVal() {

    //if (symbol == IDENFR) {
    //printword(symbol, laxer.getword(), laxer.getline());
    bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));

    int cur_line = laxer.getline();
    struct Lval *newlval = new Lval(laxer.getword(), cur_line);
    symbol = okgetsym();
    while (symbol == LBRACK) {
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        cur_line = laxer.getline();
        struct expr *newexp = Exp();
        newlval->addexpr(newexp);
        if (symbol == RBRACK) {
            //printword(symbol, laxer.getword(), laxer.getline());
            bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
            symbol = okgetsym();
        } else {
            myerror.add_errorItem(LackRbrack, cur_line);
        }
    }
    /*} else {
        error();
    }*/
    //printgrammar("<LVal>");
    bufferqueue.emplace_back("<LVal>");
    return newlval;
}

struct expr *ParserAnalyzer::Number() {
    int cur_line = laxer.getline();
    int constvalue = IntConst();
    //printgrammar("<Number>");
    bufferqueue.emplace_back("<Number>");
    struct AstNumber *resexp = new AstNumber(constvalue, cur_line);
    return resexp;
}

int ParserAnalyzer::IntConst() {

    //if (symbol == INTCON) {
    //printword(symbol, laxer.getword(), laxer.getline());
    int res = 0;
    string strnum = laxer.getword();
    for (int i = 0; i < strnum.size(); ++i) {
        res = res * 10 + (strnum[i] - '0');
    }
    bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
    symbol = okgetsym();
    /*} else {
        error();
    }*/
    //printgrammar("<IntConst>");
    return res;
}

vector<expr *> ParserAnalyzer::FuncRParams() {
    vector<expr *> exp_list;
    struct expr *newexpr = Exp();
    exp_list.push_back(newexpr);

    while (symbol == COMMA) {
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        struct expr *follow = Exp();
        exp_list.push_back(follow);
    }
    //printgrammar("<FuncRParams>");
    bufferqueue.emplace_back("<FuncRParams>");
    return exp_list;
}

struct StmtItem *ParserAnalyzer::Block() {
    struct AstBlock *astBlock = new AstBlock();
    if (symbol == LBRACE) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        while (symbol != RBRACE) {
            struct AstBlockItem *newitem = BlockItem();
            astBlock->addblock_item(newitem);
        }
        if (symbol == RBRACE) {
            astBlock->setendline(laxer.getline()); // TODO
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
        }
    } else {
        error();
    }
    printgrammar("<Block>");
    return astBlock;
}

struct AstBlockItem *ParserAnalyzer::BlockItem() {
    if (symbol == CONSTTK || symbol == INTTK) {
        struct AstBlockItem *declitem = Decl();
        return declitem;
    } else {
        struct StmtItem *stmtitem = Stmt();
        struct AstStmt *astStmt = new AstStmt(stmtitem);
        return astStmt;
    }
}

struct StmtItem *ParserAnalyzer::Stmt() {
    if (symbol == LBRACE) {
        struct StmtItem *resitem = Block();
        printgrammar("<Stmt>");
        return resitem;
    } else if (symbol == IFTK) {
        struct Ifstmt *ifstmt = new Ifstmt();
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == LPARENT) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            int cur_line = laxer.getline();
            struct AstCond *astCond = Cond();
            ifstmt->setcond(astCond);
            pop_all();
            if (symbol == RPARENT) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                struct StmtItem *stmtItem = Stmt();
                struct AstStmt *astStmt = new AstStmt(stmtItem);
                ifstmt->setstmt(astStmt);
            } else {
                myerror.add_errorItem(LackRparent, cur_line); // TODO;
                struct StmtItem *stmtItem = Stmt();
                struct AstStmt *astStmt = new AstStmt(stmtItem);
                ifstmt->setstmt(astStmt);
            }
            if (symbol == ELSETK) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                struct StmtItem *elseitem = Stmt();
                struct AstStmt *elsestmt = new AstStmt(elseitem);
                ifstmt->setelsestmt(elsestmt);
            }
        }
        printgrammar("<Stmt>");
        return ifstmt;
    } else if (symbol == WHILETK) {
        struct Whilestmt *whilestmt = new Whilestmt();
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == LPARENT) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            int cur_line = laxer.getline();
            struct AstCond *astCond = Cond();
            whilestmt->setcond(astCond);
            pop_all();
            if (symbol == RPARENT) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                struct StmtItem *stmtItem = Stmt();
                struct AstStmt *astStmt = new AstStmt(stmtItem);
                whilestmt->setstmt(astStmt);
            } else {
                myerror.add_errorItem(LackRparent, cur_line); // TODO
                struct StmtItem *stmtItem = Stmt();
                struct AstStmt *astStmt = new AstStmt(stmtItem);
                whilestmt->setstmt(astStmt);
            }
        }
        printgrammar("<Stmt>");
        return whilestmt;
    } else if (symbol == BREAKTK || symbol == CONTINUETK) {
        int loopstmt = symbol;
        printword(symbol, laxer.getword(), laxer.getline());
        int cur_line = laxer.getline();
        symbol = okgetsym();
        if (symbol == SEMICN) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
        } else {
            myerror.add_errorItem(LackSemicn, cur_line);
        }
        if (loopstmt == BREAKTK) {
            struct Breakstmt *breakstmt = new Breakstmt(cur_line);
            printgrammar("<Stmt>");
            return breakstmt;
        } else {
            struct Continuestmt *continuestmt = new Continuestmt(cur_line);
            printgrammar("<Stmt>");
            return continuestmt;
        }
    } else if (symbol == RETURNTK) {
        int cur_line = laxer.getline();
        struct Returnstmt *returnstmt = new Returnstmt(cur_line);
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == SEMICN) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
        } else {
            if (symbol == LPARENT || symbol == IDENFR || symbol == INTCON || symbol == PLUS || symbol == MINUS) {
                struct expr *returnexp = Exp();
                returnstmt->setreturnexp(returnexp);
                pop_all();
                if (symbol == SEMICN) {
                    printword(symbol, laxer.getword(), laxer.getline());
                    symbol = okgetsym();
                } else {
                    myerror.add_errorItem(LackSemicn, cur_line); // TODO
                }
            } else {
                myerror.add_errorItem(LackSemicn, cur_line); // TODO
            }
        }
        printgrammar("<Stmt>");
        return returnstmt;
    } else if (symbol == PRINTFTK) {
        int cur_line = laxer.getline();
        int num = 0; // print num
        struct Printstmt *printstmt = new Printstmt(cur_line);
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        if (symbol == LPARENT) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            if (symbol == STRCON) {
                num = laxer.getformatnum();
                printstmt->setstring(laxer.getword(), num);
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                while (symbol == COMMA) {
                    printword(symbol, laxer.getword(), laxer.getline());
                    symbol = okgetsym();
                    struct expr *fexpr = Exp();
                    printstmt->addexpr(fexpr);
                    pop_all();
                }
                if (symbol == RPARENT) {
                    cur_line = laxer.getline();
                    printword(symbol, laxer.getword(), laxer.getline());
                    symbol = okgetsym();
                    if (symbol == SEMICN) {
                        printword(symbol, laxer.getword(), laxer.getline());
                        symbol = okgetsym();
                    } else {
                        myerror.add_errorItem(LackSemicn, cur_line); // TODO
                    }
                } else if (symbol == SEMICN) {
                    myerror.add_errorItem(LackRparent, laxer.getline()); // TODO
                    symbol = okgetsym();
                }
            }
        }
        printgrammar("<Stmt>");
        return printstmt;
    } else if (symbol == SEMICN) {
        printword(symbol, laxer.getword(), laxer.getline());
        symbol = okgetsym();
        struct Only_expr *onlyExpr = new Only_expr();
        printgrammar("<Stmt>");
        return onlyExpr;
    } else if (symbol == IDENFR) {
        int cur_line1 = laxer.getline();
        struct expr *leftexpr = Exp();
        if (symbol == ASSIGN) {
            pop_select("<LVal>");
            buffer_clear();
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
            if (symbol == GETINTTK) {
                int cur_line = laxer.getline();
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
                if (symbol == LPARENT) {
                    printword(symbol, laxer.getword(), laxer.getline());
                    symbol = okgetsym();
                    if (symbol == RPARENT) {
                        printword(symbol, laxer.getword(), laxer.getline());
                        symbol = okgetsym();
                        if (symbol == SEMICN) {
                            printword(symbol, laxer.getword(), laxer.getline());
                            symbol = okgetsym();
                        } else {
                            myerror.add_errorItem(LackSemicn, cur_line); // TODO
                        }
                    } else {
                        myerror.add_errorItem(LackRparent, cur_line); // TODO
                        symbol = okgetsym();
                    }
                }
                struct Readstmt *readstmt = new Readstmt(leftexpr, cur_line1);
                printgrammar("<Stmt>");
                return readstmt;
            } else {
                int cur_line = laxer.getline();
                struct expr *valueexpr = Exp();
                struct Assign *assign = new Assign(leftexpr, valueexpr, cur_line1);
                pop_all();
                if (symbol == SEMICN) {
                    printword(symbol, laxer.getword(), laxer.getline());
                    symbol = okgetsym();
                } else {
                    myerror.add_errorItem(LackSemicn, cur_line); // TODO
                }
                printgrammar("<Stmt>");
                return assign;
            }
        } else {
            pop_all();
            if (symbol == SEMICN) {
                printword(symbol, laxer.getword(), laxer.getline());
                symbol = okgetsym();
            } else {
                myerror.add_errorItem(LackSemicn, cur_line1); // TODO
            }
            struct Only_expr *onlyExpr = new Only_expr(leftexpr);
            printgrammar("<Stmt>");
            return onlyExpr;
        }
    } else {
        int cur_line = laxer.getline();
        struct expr *leftexpr = Exp();
        struct Only_expr *onlyExpr = new Only_expr(leftexpr);
        pop_all();
        if (symbol == SEMICN) {
            printword(symbol, laxer.getword(), laxer.getline());
            symbol = okgetsym();
        } else {
            myerror.add_errorItem(LackSemicn, cur_line);
        }
        printgrammar("<Stmt>");
        return onlyExpr;
    }
    printgrammar("<Stmt>");
}

struct AstCond *ParserAnalyzer::Cond() {
    struct expr *condexp = LOrExp();
    //printgrammar("<Cond>");
    bufferqueue.emplace_back("<Cond>");
    struct AstCond *astCond = new AstCond(condexp);
    return astCond;
}

struct expr *ParserAnalyzer::LOrExp() {
    struct expr *resexp = LAndExp();
    /*while (symbol == OR) {
        //printgrammar("<LOrExp>");
        bufferqueue.emplace_back("<LOrExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        LAndExp();
    }*/
    while (symbol == OR) {
        //printgrammar("<LOrExp>");
        bufferqueue.emplace_back("<LOrExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        Optype op = OrType;
        int cur_line = laxer.getline();
        struct expr *follow = LAndExp();
        struct Binaryexp *binaryexp = new Binaryexp(op, resexp, follow, cur_line);
        bufferqueue.emplace_back("<LOrExp>");
        resexp = binaryexp;
    }
    //printgrammar("<LOrExp>");
    bufferqueue.emplace_back("<LOrExp>");
    return resexp;
}

struct expr *ParserAnalyzer::LAndExp() {
    struct expr *resexp = EqExp();
    /*while (symbol == AND) {
        //printgrammar("<LAndExp>");
        bufferqueue.emplace_back("<LAndExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        EqExp();
    }*/
    while (symbol == AND) {
        //printgrammar("<LAndExp>");
        bufferqueue.emplace_back("<LAndExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        Optype op = AndType;
        int cur_line = laxer.getline();
        struct expr *follow = EqExp();
        struct Binaryexp *binaryexp = new Binaryexp(op, resexp, follow, cur_line);
        bufferqueue.emplace_back("<LAndExp>");
        resexp = binaryexp;
    }
    //printgrammar("<LAndExp>");
    bufferqueue.emplace_back("<LAndExp>");
    return resexp;
}

struct expr *ParserAnalyzer::EqExp() {
    struct expr *resexp = RelExp();
    /*while (symbol == EQL || symbol == NEQ) {
        //printgrammar("<EqExp>");
        bufferqueue.emplace_back("<EqExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        RelExp();
    }*/
    while (symbol == EQL || symbol == NEQ) {
        //printgrammar("<EqExp>");
        bufferqueue.emplace_back("<EqExp>");
        Optype op = (symbol == EQL) ? EalType : NeqType;
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        int cur_line = laxer.getline();
        struct expr *follow = RelExp();
        struct Binaryexp *binaryexp = new Binaryexp(op, resexp, follow, cur_line);
        bufferqueue.emplace_back("<EqExp>");
        resexp = binaryexp;
    }
    //printgrammar("<EqExp>");
    bufferqueue.emplace_back("<EqExp>");
    return resexp;
}

struct expr *ParserAnalyzer::RelExp() {
    struct expr *resexp = AddExp();
    /*while (symbol == LSS || symbol == GRE || symbol == LEQ || symbol == GEQ) {
        //printgrammar("<RelExp>");
        bufferqueue.emplace_back("<RelExp>");
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        AddExp();
    }*/

    while (symbol == LSS || symbol == GRE || symbol == LEQ || symbol == GEQ) {
        //printgrammar("<RelExp>");
        bufferqueue.emplace_back("<RelExp>");
        Optype op;
        if (symbol == LSS) {
            op = LssType;
        } else if (symbol == GRE) {
            op = GreType;
        } else if (symbol == LEQ) {
            op = LeqType;
        } else {
            op = GeqType;
        }
        //printword(symbol, laxer.getword(), laxer.getline());
        bufferqueue.emplace_back(symstring(symbol, laxer.getword(), laxer.getline()));
        symbol = okgetsym();
        int cur_line = laxer.getline();
        struct expr *follow = AddExp();
        struct Binaryexp *binaryexp = new Binaryexp(op, resexp, follow, cur_line);
        bufferqueue.emplace_back("<RelExp>");
        resexp = binaryexp;
    }
    //printgrammar("<RelExp>");
    bufferqueue.emplace_back("<RelExp>");
    return resexp;
}