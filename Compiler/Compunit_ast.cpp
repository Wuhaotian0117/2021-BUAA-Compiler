//
// Created by wuhaotian on 2021/11/10.
//

#include "Compunit_ast.h"

Compunit_ast::Compunit_ast(SymbolTable &table, Pcode_vm &pcodeVm, Error &error) :
        table(table), pcodeVm(pcodeVm), myerror(error) {
    this->label_int = 1000;
    this->cond_int = 2000;
    this->loop_num = 0;
    this->in_void_func = false;
}

void Compunit_ast::adddecl(struct AstDecl *newdecl) {
    decl_list.push_back(newdecl);
}

void Compunit_ast::addfuncdef(struct AstfuncDef *newfuncdef) {
    func_list.push_back(newfuncdef);
}

void Compunit_ast::setmainfundef(struct Astmaindef *mainfundef) {
    this->astmaindef = mainfundef;
}

void Compunit_ast::printyourself() {
    cout << "the decls: " << decl_list.size() << endl;
    cout << "the funcdef: " << func_list.size() << endl;
    for (int i = 0; i < decl_list.size(); ++i) {
        if (decl_list[i] != nullptr) {
            decl_list[i]->printyourself();
        }
    }

    for (int i = 0; i < func_list.size(); ++i) {
        if (func_list[i] != nullptr) {
            func_list[i]->printyourself();
        }
    }

    this->astmaindef->printyourself();
}

void Compunit_ast::Semantic_ast() {
    //cout << "Semantic begin:" << endl;
    for (int i = 0; i < decl_list.size(); ++i) {
        if (decl_list[i] != nullptr) {
            Semantic_Astdecl(decl_list[i]);
        }
    }

    table.addFunc("main", 0, Intfunction);

    struct expr *maincallfunc = new Callfunc("main", this->astmaindef->line);
    Semantic_expr(maincallfunc, "conrelu");
    Pcode_instr pcodeInstr = Pcode_instr(Exit);
    pcodeVm.addinstr(pcodeInstr);

    //cout << "Semantic decl ok:" << endl;
    for (int i = 0; i < func_list.size(); ++i) {
        if (func_list[i] != nullptr) {
            Semantic_Funcdef(func_list[i]);
        }
    }
    //cout << "Semantic funcdef ok:" << endl;

    //struct AstfuncDef *mainfuncdef = new AstfuncDef(Intfunction, "main");

    Semantic_Maindef(this->astmaindef);
    //cout << "Semantic maindef ok:" << endl;
}

void Compunit_ast::Semantic_Astdecl(struct AstDecl *fdecl) {
    int n = fdecl->sublist.size();
    bool is_const = fdecl->is_const;
    for (int i = 0; i < n; ++i) {
        Semantic_Subdecl(fdecl->sublist[i], is_const);
    }
}

void Compunit_ast::Semantic_Subdecl(struct SubDecl *fsubdecl, bool fis_const) {
    ItemType itemType = fis_const ? ConstType : VarType;

    if (table.check_definition(fsubdecl->identname, itemType) < 0) {
        myerror.add_SemanticError(Redefinition, fsubdecl->line); // TODO
    }
    table.addItem(fsubdecl->identname, itemType, fsubdecl->dimension);
    /*if (fis_const) {
        table.addItem(fsubdecl->identname, ConstType, fsubdecl->dimension);
    } else {
        table.addItem(fsubdecl->identname, VarType, fsubdecl->dimension);
    }*/
    //cout << "fsubdecl " << fsubdecl->identname << fsubdecl->dimension << endl;
    if (fsubdecl->dimension == 1) {
        int fd1 = get_const_value(fsubdecl->dimens[0]);
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, fd1, 0, 1);
        pcodeVm.addinstr(instr);

        if (fis_const) {
            fsubdecl->d1 = fd1;
            table.set_item_length(fsubdecl->identname, fd1, 0); /**确定了一维常量数组的长度*/
            table.set_item_addr(fsubdecl->identname, table.sp); /**分配了一维数组的地址*/
        }

        if (fsubdecl->initval != nullptr) {
            if (fis_const) {
                vector<int> const_res;
                const_res = get_array_value(fsubdecl->initval);
                for (int i = 0; i < const_res.size(); ++i) {
                    table.table_memory.push_back(const_res[i]);  /**将一维常量数组的值，存入memory*/
                    table.sp++;
                }
            } /**如果是一维常量，就将常量求出*/
            vector<struct expr *> res = array_condense(fsubdecl->initval);
            for (int i = 0; i < res.size(); ++i) {
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, fsubdecl->identname);
                pcodeVm.addinstr(pcodeInstr);
                Pcode_instr pcodeInstr1 = Pcode_instr(Load_i, i);
                pcodeVm.addinstr(pcodeInstr1);
                Pcode_instr pcodeInstr2 = Pcode_instr(Gen_address, fsubdecl->identname, 1);
                pcodeVm.addinstr(pcodeInstr2);
                Semantic_expr(res[i], "conrelu");
                Pcode_instr pcodeInstr3 = Pcode_instr(STO);
                pcodeVm.addinstr(pcodeInstr3);
            }
        } else {
            if (fis_const) {
                for (int i = 0; i < fd1; ++i) {
                    table.table_memory.push_back(0);
                    table.sp++;
                }
            }
        }
    } else if (fsubdecl->dimension == 2) {
        int fd1 = get_const_value(fsubdecl->dimens[0]);
        int fd2 = get_const_value(fsubdecl->dimens[1]);
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, fd1, fd2, 2);
        pcodeVm.addinstr(instr);

        if (fis_const) {
            fsubdecl->d1 = fd1;
            fsubdecl->d2 = fd2;
            table.set_item_length(fsubdecl->identname, fd1, fd2); /**确定了二维数组的长度*/
            table.set_item_addr(fsubdecl->identname, table.sp); /**分配了二维常量数组的地址*/
        }
        if (fsubdecl->initval != nullptr) {
            if (fis_const) {
                vector<int> res = get_array_value(fsubdecl->initval);
                for (int i = 0; i < res.size(); ++i) {
                    table.table_memory.push_back(res[i]);
                    table.sp++;
                }
            } /**二维常量，就将常量求出*/
            vector<struct expr *> res = array_condense(fsubdecl->initval);
            int cp = 0;
            for (int i = 0; i < fd1; ++i) {
                for (int j = 0; j < fd2; ++j) {
                    Pcode_instr pcodeInstr = Pcode_instr(Load_address, fsubdecl->identname);
                    pcodeVm.addinstr(pcodeInstr);
                    Pcode_instr pcodeInstr1 = Pcode_instr(Load_i, i);
                    pcodeVm.addinstr(pcodeInstr1);
                    Pcode_instr pcodeInstr2 = Pcode_instr(Load_i, j);
                    pcodeVm.addinstr(pcodeInstr2);
                    Pcode_instr pcodeInstr3 = Pcode_instr(Gen_address, fsubdecl->identname, 2);
                    pcodeVm.addinstr(pcodeInstr3);
                    Semantic_expr(res[cp++], "condrelu");
                    Pcode_instr pcodeInstr4 = Pcode_instr(STO);
                    pcodeVm.addinstr(pcodeInstr4);
                }
            }
        } else {
            if (fis_const) {
                for (int i = 0; i < fd1 * fd2; ++i) {
                    table.table_memory.push_back(0);
                    table.sp++;
                }
            }
        }
    } else {
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, 0, 0, 0);
        pcodeVm.addinstr(instr);

        if (fis_const) {
            if (fsubdecl->initval != nullptr) {
                fsubdecl->d1 = 0;
                fsubdecl->d2 = 0;
                vector<int> res = get_array_value(fsubdecl->initval);
                table.set_item_value(fsubdecl->identname, res[0]);
            } else {
                table.set_item_value(fsubdecl->identname, 0);
            }
        } else {
            if (fsubdecl->initval != nullptr) {
                vector<struct expr *> res = array_condense(fsubdecl->initval);
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, fsubdecl->identname);
                pcodeVm.addinstr(pcodeInstr);
                Semantic_expr(res[0], "condrelu");
                Pcode_instr pcodeInstr2 = Pcode_instr(STO);
                pcodeVm.addinstr(pcodeInstr2);
            }
        }
    }
    // TODO 这里也许需要求出数组各维长度；
}

void Compunit_ast::Subdecl_const(struct SubDecl *fsubdecl) {
    if (fsubdecl->dimension == 1) {
        int fd1 = get_const_value(fsubdecl->dimens[0]);
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, fd1, 0, 1);
        pcodeVm.addinstr(instr);

        fsubdecl->d1 = fd1;
        table.set_item_length(fsubdecl->identname, fd1, 0); /**确定了一维常量数组的长度*/
        table.set_item_addr(fsubdecl->identname, table.sp); /**分配了一维数组的地址*/

        vector<int> res;
        if (fsubdecl->initval != nullptr) {
            res = get_array_value(fsubdecl->initval);
            for (int i = 0; i < res.size(); ++i) {
                //cout << res[i] << " " << endl;
                table.table_memory.push_back(res[i]);  /**将一维常量数组的值，存入memory*/
                table.sp++;
                //cout << "assign ok" << endl;
            }
        } else {
            for (int i = 0; i < fd1; ++i) {
                table.table_memory.push_back(0);
                table.sp++;
            }
        }
    } else if (fsubdecl->dimension == 2) {
        int fd1 = get_const_value(fsubdecl->dimens[0]);
        int fd2 = get_const_value(fsubdecl->dimens[1]);
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, fd1, fd2, 2);
        pcodeVm.addinstr(instr);

        fsubdecl->d1 = fd1;
        fsubdecl->d2 = fd2;
        table.set_item_length(fsubdecl->identname, fd1, fd2); /**确定了二维数组的长度*/
        table.set_item_addr(fsubdecl->identname, table.sp); /**分配了二维常量数组的地址*/

        if (fsubdecl->initval != nullptr) {
            vector<int> res = get_array_value(fsubdecl->initval);
            for (int i = 0; i < res.size(); ++i) {
                table.table_memory.push_back(res[i]);
                table.sp++;
            }
        } else {
            for (int i = 0; i < fd1 * fd2; ++i) {
                table.table_memory.push_back(0);
                table.sp++;
            }
        }
    } else {
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, 0, 0, 0);
        pcodeVm.addinstr(instr);
        fsubdecl->d1 = 0;
        fsubdecl->d2 = 0;
        table.set_item_length(fsubdecl->identname, 0, 0);

        if (fsubdecl->initval != nullptr) {
            vector<int> res = get_array_value(fsubdecl->initval);
            table.set_item_value(fsubdecl->identname, res[0]);
        } else {
            table.set_item_value(fsubdecl->identname, 0);
        }
    }
}

void Compunit_ast::Subdecl_notconst(struct SubDecl *fsubdecl) {
    if (fsubdecl->dimension == 1) {
        int fd1 = get_const_value(fsubdecl->dimens[0]);
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, fd1, 0, 1);
        pcodeVm.addinstr(instr);
        if (fsubdecl->initval != nullptr) {
            vector<struct expr *> res = array_condense(fsubdecl->initval);
            for (int i = 0; i < res.size(); ++i) {
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, fsubdecl->identname);
                pcodeVm.addinstr(pcodeInstr);
                Pcode_instr pcodeInstr1 = Pcode_instr(Load_i, i);
                pcodeVm.addinstr(pcodeInstr1);
                Pcode_instr pcodeInstr2 = Pcode_instr(Gen_address, fsubdecl->identname, 1);
                pcodeVm.addinstr(pcodeInstr2);
                Semantic_expr(res[i], "condrelu");
                Pcode_instr pcodeInstr3 = Pcode_instr(STO);
                pcodeVm.addinstr(pcodeInstr3);
            }
        }
    } else if (fsubdecl->dimension == 2) {
        int fd1 = get_const_value(fsubdecl->dimens[0]);
        int fd2 = get_const_value(fsubdecl->dimens[1]);
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, fd1, fd2, 2);
        pcodeVm.addinstr(instr);
        if (fsubdecl->initval != nullptr) {
            vector<struct expr *> res = array_condense(fsubdecl->initval);
            int cp = 0;
            for (int i = 0; i < fd1; ++i) {
                for (int j = 0; j < fd2; ++j) {
                    Pcode_instr pcodeInstr = Pcode_instr(Load_address, fsubdecl->identname);
                    pcodeVm.addinstr(pcodeInstr);
                    Pcode_instr pcodeInstr1 = Pcode_instr(Load_i, i);
                    pcodeVm.addinstr(pcodeInstr1);
                    Pcode_instr pcodeInstr2 = Pcode_instr(Load_i, j);
                    pcodeVm.addinstr(pcodeInstr2);
                    Pcode_instr pcodeInstr3 = Pcode_instr(Gen_address, fsubdecl->identname, 2);
                    pcodeVm.addinstr(pcodeInstr3);
                    Semantic_expr(res[cp++], "condrelu");
                    Pcode_instr pcodeInstr4 = Pcode_instr(STO);
                    pcodeVm.addinstr(pcodeInstr4);
                }
            }
        }
    } else {
        Pcode_instr instr = Pcode_instr(Var, fsubdecl->identname, 0, 0, 0);
        pcodeVm.addinstr(instr);

        if (fsubdecl->initval != nullptr) {
            vector<struct expr *> res = array_condense(fsubdecl->initval);
            Pcode_instr pcodeInstr = Pcode_instr(Load_address, fsubdecl->identname);
            pcodeVm.addinstr(pcodeInstr);
            Semantic_expr(res[0], "condrelu");
            Pcode_instr pcodeInstr2 = Pcode_instr(STO);
            pcodeVm.addinstr(pcodeInstr2);
        } else {
            Pcode_instr pcodeInstr = Pcode_instr(Load_address, fsubdecl->identname);
            pcodeVm.addinstr(pcodeInstr);
            Pcode_instr pcodeInstr1 = Pcode_instr(Load_i, 0);
            pcodeVm.addinstr(pcodeInstr1);
            Pcode_instr pcodeInstr2 = Pcode_instr(STO);
            pcodeVm.addinstr(pcodeInstr2);
        }
    }
}

void Compunit_ast::Semantic_Funcdef(struct AstfuncDef *ffuncdef) {

    in_void_func = (ffuncdef->functionType == Voidfuncion);

    if (table.check_definition(ffuncdef->funcname, FuncType) < 0) {
        myerror.add_SemanticError(Redefinition, ffuncdef->line); // TODO
    }
    string funlabel = ffuncdef->funcname + "8848";
    Pcode_instr instr = Pcode_instr(Label, funlabel);
    pcodeVm.addinstr(instr);
    pcodeVm.setlabel_to_map(funlabel, pcodeVm.table_sp);

    table.addItem(ffuncdef->funcname, FuncType, 0, ffuncdef->functionType);
    table.upper_index();

    /*Pcode_instr pcodeInstr2 = Pcode_instr(Upper_index);
    pcodeVm.addinstr(pcodeInstr2);*/

    if (ffuncdef->para_list.empty()) {
        table.addFunc(ffuncdef->funcname, 0, ffuncdef->functionType);
    } else {
        vector<int> v1 = ffuncdef->get_param_dimen();
        table.addFunc(ffuncdef->funcname, v1.size(), ffuncdef->functionType, v1);
        for (int i = 0; i < ffuncdef->para_list.size(); ++i) {

            if (table.check_definition(ffuncdef->para_list[i]->paraname, ParaType) < 0) {
                myerror.add_SemanticError(Redefinition, ffuncdef->para_list[i]->line); // TODO
            }

            table.addItem(ffuncdef->para_list[i]->paraname, ParaType, ffuncdef->para_list[i]->dimen);

            Pcode_instr pcodeInstr = Pcode_instr(Var, ffuncdef->para_list[i]->paraname,
                                                 0, 0, ffuncdef->para_list[i]->dimen);
            pcodeVm.addinstr(pcodeInstr);

            int array_length = 0;
            if (ffuncdef->para_list[i]->dimen == 2) {
                array_length = get_const_value(ffuncdef->para_list[i]->exp_list[0]);
            }
            Pcode_instr pcodeInstr1 = Pcode_instr
                    (Para_Handle, ffuncdef->para_list[i]->paraname, array_length);
            pcodeVm.addinstr(pcodeInstr1);
        }
    }
    Semantic_AstBlock(ffuncdef->funcblock, "relu", "relu");
    if (ffuncdef->functionType == Voidfuncion) {
        Pcode_instr pcodeInstr = Pcode_instr(Jump_ra);
        pcodeVm.addinstr(pcodeInstr);
    }
    table.pop_item();

    if (ffuncdef->functionType == Intfunction) {
        if (ffuncdef->funcblock->check_end() < 0) {
            //cout << ffuncdef->funcname <<endl;
            myerror.add_SemanticError(LackReturn, ffuncdef->funcblock->endline); // TODO
        }
    }

    in_void_func = false;
    /*Pcode_instr pcodeInstr3 = Pcode_instr(Pop_item);
    pcodeVm.addinstr(pcodeInstr3);*/
}

void Compunit_ast::Semantic_AstBlock(struct AstBlock *fblock, string label1, string label2) {
    for (int i = 0; i < fblock->blockitem_list.size(); ++i) {
        if (fblock->blockitem_list[i] != nullptr) {
            if (fblock->blockitem_list[i]->block_flag == 1) {
                //cout << "go to decl" << endl;
                struct AstDecl *astDecl = dynamic_cast<AstDecl *>(fblock->blockitem_list[i]);
                Semantic_Astdecl(astDecl);
            } else if (fblock->blockitem_list[i]->block_flag == 2) {
                //cout << "go to stmt" << endl;
                struct AstStmt *astStmt = dynamic_cast<AstStmt *>(fblock->blockitem_list[i]);
                if (astStmt->stmtItem != nullptr) {
                    Semantic_stmtitem(astStmt->stmtItem, label1, label2);
                }
            }
        }
    }
}

void Compunit_ast::Semantic_expr(struct expr *fexpr, string cond_label) {
    int flag = fexpr->exp_flag;
    if (flag == 1) {
        struct Binaryexp *binaryexp = dynamic_cast<Binaryexp *>(fexpr);
        Semantic_binary(binaryexp, cond_label);
    } else if (flag == 2) {
        struct Lval *lval = dynamic_cast<Lval *>(fexpr);
        int opvalue = Semantic_op_list(lval->op_list);
        // TODO

        if (table.check_identifier(lval->identname, ConstType, lval->dimension) < 0) {
            myerror.add_SemanticError(Undefinition, lval->line);
        }
        int lval_dimen = lval->dimension; /**实际使用维度*/
        int check_dimen = table.check_dimension(lval->identname); /**定义维度*/
        //cout << "check_dimen " << check_dimen << lval->identname << endl;
        if (check_dimen == 2) {
            if (lval_dimen == 0) {
                //cout << "real 2, use 0 " << lval->identname << endl;
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
                pcodeVm.addinstr(pcodeInstr);
            } else if (lval_dimen == 1) {
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
                pcodeVm.addinstr(pcodeInstr);
                Semantic_expr(lval->exp_list[0], "condrelu");
                Pcode_instr pcodeInstr1 = Pcode_instr(Gen_address, lval->identname, 1);
                pcodeVm.addinstr(pcodeInstr1);
            } else {
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
                pcodeVm.addinstr(pcodeInstr);
                Semantic_expr(lval->exp_list[0], "condrelu");
                Semantic_expr(lval->exp_list[1], "condrelu");
                Pcode_instr pcodeInstr1 = Pcode_instr(Gen_address, lval->identname, 2);
                pcodeVm.addinstr(pcodeInstr1);
                Pcode_instr pcodeInstr2 = Pcode_instr(Load_value);
                pcodeVm.addinstr(pcodeInstr2);
            }
        } else if (check_dimen == 1) {
            if (lval->dimension == 1) {
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
                pcodeVm.addinstr(pcodeInstr);
                Semantic_expr(lval->exp_list[0], "condrelu");
                Pcode_instr pcodeInstr1 = Pcode_instr(Gen_address, lval->identname, 1);
                pcodeVm.addinstr(pcodeInstr1);
                Pcode_instr pcodeInstr2 = Pcode_instr(Load_value);
                pcodeVm.addinstr(pcodeInstr2);
            } else if (lval->dimension == 0) {
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
                pcodeVm.addinstr(pcodeInstr);
            }
        } else {
            if (table.get_item_isconst(lval->identname) == 1) {
                int value = table.get_item_value(lval->identname);
                Pcode_instr pcodeInstr = Pcode_instr(Load_i, value);
                pcodeVm.addinstr(pcodeInstr);
            } else {
                //cout << "0 dimen:" << lval->identname << endl;
                Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
                pcodeVm.addinstr(pcodeInstr);
                Pcode_instr pcodeInstr1 = Pcode_instr(Load_value);
                pcodeVm.addinstr(pcodeInstr1);
            }
        }
        if (opvalue == -1) {
            Pcode_instr pcodeInstr1 = Pcode_instr(Negtive);
            pcodeVm.addinstr(pcodeInstr1);
        } else if (opvalue == 0) {
            Pcode_instr pcodeInstr1 = Pcode_instr(Not);
            pcodeVm.addinstr(pcodeInstr1);
        }
    } else if (flag == 3) {
        struct AstNumber *astNumber = dynamic_cast<AstNumber *>(fexpr);
        int opvalue = Semantic_op_list(astNumber->op_list);
        Pcode_instr pcodeInstr = Pcode_instr(Load_i, astNumber->count);
        pcodeVm.addinstr(pcodeInstr);
        if (opvalue == -1) {
            Pcode_instr pcodeInstr1 = Pcode_instr(Negtive);
            pcodeVm.addinstr(pcodeInstr1);
        } else if (opvalue == 0) {
            Pcode_instr pcodeInstr1 = Pcode_instr(Not);
            pcodeVm.addinstr(pcodeInstr1);
        }
    } else if (flag == 4) {
        struct Callfunc *callfunc = dynamic_cast<Callfunc *>(fexpr);

        if (table.check_identifier(callfunc->identname, FuncType, 0) < 0) {
            myerror.add_SemanticError(Undefinition, callfunc->line); // TODO
        }

        if (table.check_func_para(callfunc->identname, callfunc->Rparam_list.size()) < 0) {
            myerror.add_SemanticError(NumMismatch, callfunc->line); // TODO
        }

        vector<int> rpara;
        for (int i = 0; i < callfunc->Rparam_list.size(); ++i) {
            int exp_flag = callfunc->Rparam_list[i]->exp_flag;
            if (exp_flag == 1) {
                rpara.push_back(0);
            } else if (exp_flag == 2) {
                struct Lval *lval = dynamic_cast<Lval *>(callfunc->Rparam_list[i]);
                int real_dimen = lval->dimension;
                int defi_dimen = table.check_dimension(lval->identname);
                if (defi_dimen == -1 || defi_dimen < real_dimen) {
                    rpara.push_back(-1);
                } else {
                    rpara.push_back(defi_dimen - real_dimen);
                }
            } else {
                rpara.push_back(0);
            }
        }

        if (table.check_func_para(callfunc->identname, rpara) < 0) {
            myerror.add_SemanticError(TypeMismatch, callfunc->line); // TODO
        }

        int opvalue = Semantic_op_list(callfunc->op_list);
        if (!callfunc->Rparam_list.empty()) {
            int n = callfunc->Rparam_list.size();
            for (int i = 0; i < n; ++i) {
                Semantic_expr(callfunc->Rparam_list[i], "condrelu");
            }
            Pcode_instr pcodeInstr = Pcode_instr(Back, n);  /**将实参倒一下车*/
            pcodeVm.addinstr(pcodeInstr);
            Pcode_instr pcodeInstr1 = Pcode_instr(Jump_jal, callfunc->identname + "8848");
            pcodeVm.addinstr(pcodeInstr1);
        } else {
            Pcode_instr pcodeInstr1 = Pcode_instr(Jump_jal, callfunc->identname + "8848");
            pcodeVm.addinstr(pcodeInstr1);
        }
        if (opvalue == -1) {
            Pcode_instr pcodeInstr1 = Pcode_instr(Negtive);
            pcodeVm.addinstr(pcodeInstr1);
        } else if (opvalue == 0) {
            Pcode_instr pcodeInstr1 = Pcode_instr(Not);
            pcodeVm.addinstr(pcodeInstr1);
        }
    }
}

void Compunit_ast::Semantic_binary(struct Binaryexp *binaryexp, string cond_label) {
    int opvalue = Semantic_op_list(binaryexp->op_list);
    switch (binaryexp->op) {
        case PlusType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Add);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case SubType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Sub);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case MulType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Mult);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case DivType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Div);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case ModType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Mod);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case NegType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Not);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case AndType: {
            Semantic_expr(binaryexp->left, cond_label);
            Pcode_instr pcodeInstr1 = Pcode_instr(Jump_cond_false, cond_label);
            pcodeVm.addinstr(pcodeInstr1);

            Semantic_expr(binaryexp->right, cond_label);
            Pcode_instr pcodeInstr2 = Pcode_instr(Jump_cond_false, cond_label);
            pcodeVm.addinstr(pcodeInstr2);

            /*Pcode_instr pcodeInstr = Pcode_instr(And);
            pcodeVm.addinstr(pcodeInstr);*/
            break;
        }
            /*case OrType: {
                Pcode_instr pcodeInstr = Pcode_instr(Or);
                pcodeVm.addinstr(pcodeInstr);
                break;
            }*/
        case EalType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Eql);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case NeqType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Neq);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case GreType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Gre);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case GeqType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Geq);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case LssType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Lss);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        case LeqType: {
            Semantic_expr(binaryexp->left, "condrelu");
            Semantic_expr(binaryexp->right, "condrelu");
            Pcode_instr pcodeInstr = Pcode_instr(Leq);
            pcodeVm.addinstr(pcodeInstr);
            break;
        }
        default:
            break;
    }
    if (opvalue == -1) {
        Pcode_instr pcodeInstr1 = Pcode_instr(Negtive);
        pcodeVm.addinstr(pcodeInstr1);
    } else if (opvalue == 0) {
        Pcode_instr pcodeInstr1 = Pcode_instr(Not);
        pcodeVm.addinstr(pcodeInstr1);
    }
}

void Compunit_ast::Semantic_Lval(struct Lval *lval) {
    int opvalue = Semantic_op_list(lval->op_list);


}

int Compunit_ast::Semantic_op_list(vector<Optype> v1) {
    int value = 1;
    for (int i = v1.size() - 1; i >= 0; --i) {
        if (v1[i] == SubType) {
            value = -value;
        } else if (v1[i] == NegType) {
            if (value == 0) {
                value = 1;
            } else {
                value = 0;
            }
        }
    }
    return value;
}

int Compunit_ast::get_const_value(struct expr *fexpr) {
    int flag = fexpr->exp_flag;
    if (flag == 1) {
        struct Binaryexp *binaryexp = dynamic_cast<Binaryexp *>(fexpr);
        int opvalue = Semantic_op_list(binaryexp->op_list);

        int numvalue = 0;
        int left = get_const_value(binaryexp->left);
        int right = get_const_value(binaryexp->right);
        if (binaryexp->op == PlusType) {
            numvalue = left + right;
        } else if (binaryexp->op == SubType) {
            numvalue = left - right;
        } else if (binaryexp->op == MulType) {
            numvalue = left * right;
        } else if (binaryexp->op == DivType) {
            numvalue = left / right;
        } else if (binaryexp->op == ModType) {
            numvalue = left % right;
        } else {
            return -11258;
        }
        if (opvalue == -1) {
            numvalue = -numvalue;
        } else if (opvalue == 0) {
            if (numvalue == 1) {
                numvalue = 0;
            } else {
                numvalue = 1;
            }
        }
        return numvalue;
    } else if (flag == 2) {
        struct Lval *lval = dynamic_cast<Lval *>(fexpr);
        int opvalue = Semantic_op_list(lval->op_list);
        int numvalue = 0;
        if (lval->dimension == 0) {
            numvalue = table.get_item_value(lval->identname);
        } else if (lval->dimension == 1) {
            int addr = table.get_item_address(lval->identname) + get_const_value(lval->exp_list[0]);
            numvalue = table.table_memory[addr];
        } else if (lval->dimension == 2) {
            //cout << "dimen2 get value" << endl;
            int a1 = get_const_value(lval->exp_list[0]);
            int a2 = get_const_value(lval->exp_list[1]);
            //cout << "get dimen ok" << endl;
            //cout << "a1:" << a1 << "a2:" << a2 << endl;
            int addr = table.get_item_address(lval->identname, a1, a2);
            //cout << "get addr ok" << endl;
            numvalue = table.table_memory[addr];
        }

        if (opvalue == -1) {
            numvalue = -numvalue;
        } else if (opvalue == 0) {
            if (numvalue == 1) {
                numvalue = 0;
            } else {
                numvalue = 1;
            }
        }
        return numvalue;
    } else if (flag == 3) {
        struct AstNumber *astNumber = dynamic_cast<AstNumber *>(fexpr);
        int opvalue = Semantic_op_list(astNumber->op_list);
        int numvalue = astNumber->count;
        if (opvalue == -1) {
            numvalue = -numvalue;
        } else if (opvalue == 0) {
            if (numvalue == 1) {
                numvalue = 0;
            } else {
                numvalue = 1;
            }
        }
        return numvalue;
    } else {
        return -11258;
    }
}

vector<int> Compunit_ast::get_array_value(struct AstInitval *astInitval) {
    //cout << "get array value:" << endl;
    vector<int> v1;
    v1.push_back(0);
    v1.pop_back();
    if (astInitval->astinitval_flag == 2) {
        //cout << "it is an array" << endl;
        struct Arrayinitval *arrayinitval = dynamic_cast<Arrayinitval *>(astInitval);
        //cout << "cast ok" << endl;
        for (int i = 0; i < arrayinitval->initval_list.size(); ++i) {
            vector<int> ans = get_array_value(arrayinitval->initval_list[i]);
            v1.insert(v1.end(), ans.begin(), ans.end());
        }
        //cout << "array value ok ";
        /*for (int i = 0; i < v1.size(); ++i) {
            cout << v1[i] << " ";
        }
        cout << endl;*/
    } else {
        //cout << "it is an exp:" << endl;
        struct expr *fexpr = dynamic_cast<expr *>(astInitval);
        //cout << "cast ok" << endl;
        v1.push_back(get_const_value(fexpr));
        //cout << "expr get value ok " << v1[0] << endl;
    }
    return v1;
}

vector<struct expr *> Compunit_ast::array_condense(struct AstInitval *astInitval) {
    vector<struct expr *> v1;
    if (astInitval->astinitval_flag == 2) {
        struct Arrayinitval *arrayinitval = dynamic_cast<Arrayinitval *>(astInitval);
        for (int i = 0; i < arrayinitval->initval_list.size(); ++i) {
            vector<expr *> res = array_condense(arrayinitval->initval_list[i]);
            v1.insert(v1.end(), res.begin(), res.end());
        }
    } else {
        struct expr *fexpr = dynamic_cast<expr *>(astInitval);
        v1.push_back(fexpr);
    }
    return v1;
}

void Compunit_ast::Semantic_Maindef(struct Astmaindef *fmaindef) {
    Pcode_instr pcodeInstr = Pcode_instr(Label, "main8848");
    pcodeVm.addinstr(pcodeInstr);
    pcodeVm.setlabel_to_map("main8848", pcodeVm.table_sp);
    //cout << "set main to map" << endl;
    table.upper_index();

    /*Pcode_instr pcodeInstr1 = Pcode_instr(Upper_index);
    pcodeVm.addinstr(pcodeInstr1);*/

    //cout << "upper  index" << endl;
    Semantic_AstBlock(fmaindef->mainblock, "relu", "relu");
    //cout << "mainblock ok" << endl;
    table.pop_item();

    if (fmaindef->mainblock->check_end() < 0) {
        myerror.add_SemanticError(LackReturn, fmaindef->mainblock->endline); // TODO
    }

    /*Pcode_instr pcodeInstr2 = Pcode_instr(Pop_item);
    pcodeVm.addinstr(pcodeInstr2);*/

    //cout << "pop item ok" << endl;
}

void Compunit_ast::Semantic_stmtitem(struct StmtItem *fstmtitem, string label1, string label2) {
    int flag = fstmtitem->stmt_flag;
    switch (flag) {
        case 1: {
            struct Assign *assign = dynamic_cast<Assign *>(fstmtitem);
            Semantic_Assign(assign);
            break;
        }
        case 2: {
            struct Only_expr *onlyexpr = dynamic_cast<Only_expr *>(fstmtitem);
            Semantic_Onlyexpr(onlyexpr);
            break;
        }
        case 3: {
            struct AstBlock *astBlock = dynamic_cast<AstBlock *>(fstmtitem);
            table.upper_index();
            Pcode_instr pcodeInstr = Pcode_instr(Upper_index);
            pcodeVm.addinstr(pcodeInstr);

            Semantic_AstBlock(astBlock, label1, label2);
            table.pop_item();

            Pcode_instr pcodeInstr1 = Pcode_instr(Pop_item);
            pcodeVm.addinstr(pcodeInstr1);
            break;
        }
        case 4: {
            struct Ifstmt *ifstmt = dynamic_cast<Ifstmt *>(fstmtitem);
            Semantic_ifstmt(ifstmt, label1, label2);
            break;
        }
        case 5: {
            struct Whilestmt *whilestmt = dynamic_cast<Whilestmt *>(fstmtitem);
            Semantic_whilestmt(whilestmt);
            break;
        }
        case 6: {
            struct Breakstmt *breakstmt = dynamic_cast<Breakstmt *>(fstmtitem);
            Semantic_break(label2, breakstmt->line);
            break;
        }
        case 7: {
            struct Continuestmt *continuestmt = dynamic_cast<Continuestmt *>(fstmtitem);
            Semantic_continue(label1, continuestmt->line);
            break;
        }
        case 8: {
            struct Returnstmt *returnstmt = dynamic_cast<Returnstmt *>(fstmtitem);
            Semantic_Return(returnstmt);
            break;
        }
        case 9: {
            struct Readstmt *readstmt = dynamic_cast<Readstmt *>(fstmtitem);
            Semantic_Read(readstmt);
            break;
        }
        case 10: {
            struct Printstmt *printstmt = dynamic_cast<Printstmt *>(fstmtitem);
            Semantic_Write(printstmt);
            break;
        }
        default:
            break;
    }
}

void Compunit_ast::Semantic_Assign(struct Assign *fassign) {
    struct Lval *lval = dynamic_cast<Lval *>(fassign->lval);

    if (table.check_identifier(lval->identname, ConstType, lval->dimension) < 0) {
        myerror.add_SemanticError(Undefinition, lval->line); // TODO
    }

    if (table.check_type(lval->identname, ConstType) > 0) { // 如果确实是const常量
        myerror.add_SemanticError(ChangeConst, lval->line); // TODO
    }

    int dimen = lval->dimension;
    if (dimen == 1) {
        Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
        pcodeVm.addinstr(pcodeInstr);
        Semantic_expr(lval->exp_list[0], "condrelu");
        Pcode_instr pcodeInstr1 = Pcode_instr(Gen_address, lval->identname, 1);
        pcodeVm.addinstr(pcodeInstr1);
        Semantic_expr(fassign->valueexp, "condrelu");
        Pcode_instr pcodeInstr2 = Pcode_instr(STO);
        pcodeVm.addinstr(pcodeInstr2);
    } else if (dimen == 2) {
        Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
        pcodeVm.addinstr(pcodeInstr);
        Semantic_expr(lval->exp_list[0], "condrelu");
        Semantic_expr(lval->exp_list[1], "condrelu");
        Pcode_instr pcodeInstr1 = Pcode_instr(Gen_address, lval->identname, 2);
        pcodeVm.addinstr(pcodeInstr1);
        Semantic_expr(fassign->valueexp, "condrelu");
        Pcode_instr pcodeInstr2 = Pcode_instr(STO);
        pcodeVm.addinstr(pcodeInstr2);
    } else {
        Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
        pcodeVm.addinstr(pcodeInstr);
        Semantic_expr(fassign->valueexp, "condrelu");
        Pcode_instr pcodeInstr1 = Pcode_instr(STO);
        pcodeVm.addinstr(pcodeInstr1);
    }
}

void Compunit_ast::Semantic_Onlyexpr(struct Only_expr *fonlyexpr) {
    if (fonlyexpr->only_baby != nullptr) {
        Pcode_instr pcodeInstr = Pcode_instr(alu_moniter);
        pcodeVm.addinstr(pcodeInstr);
        Semantic_expr(fonlyexpr->only_baby, "condrelu");
        Pcode_instr pcodeInstr1 = Pcode_instr(alu_cleaner);
        pcodeVm.addinstr(pcodeInstr1);
    }
}

void Compunit_ast::Semantic_ifstmt(struct Ifstmt *fifstmt, string label1, string label2) {
    label_int++;
    int yourlabel = label_int;
    string cond_label = "cond_label" + to_string(yourlabel);

    Pcode_instr pcodeInstr4 = Pcode_instr(alu_moniter); /**开始计算cond之前monitor*/
    pcodeVm.addinstr(pcodeInstr4);

    Semantic_Cond(fifstmt->astcond, cond_label);

    Pcode_instr pcodeInstr3 = Pcode_instr(Label, cond_label);
    pcodeVm.addinstr(pcodeInstr3);
    pcodeVm.setlabel_to_map(cond_label, pcodeVm.table_sp);

    string elselabel = "else" + to_string(yourlabel);
    Pcode_instr pcodeInstr = Pcode_instr(Jump_false, elselabel);
    pcodeVm.addinstr(pcodeInstr);
    string outlabel = "ifelseout" + to_string(yourlabel);

    if (fifstmt->stmt != nullptr) {
        Pcode_instr pcodeInstr2 = Pcode_instr(alu_cleaner); /**计算完了clean*/
        pcodeVm.addinstr(pcodeInstr2);

        Semantic_stmtitem(fifstmt->stmt->stmtItem, label1, label2);
        Pcode_instr pcodeInstr1 = Pcode_instr(Jump_j, outlabel);
        pcodeVm.addinstr(pcodeInstr1);
    }
    Pcode_instr pcodeInstr1 = Pcode_instr(Label, elselabel);
    pcodeVm.addinstr(pcodeInstr1);
    pcodeVm.setlabel_to_map(elselabel, pcodeVm.table_sp);

    Pcode_instr pcodeInstr6 = Pcode_instr(alu_cleaner);
    pcodeVm.addinstr(pcodeInstr6);

    if (fifstmt->elsestmt != nullptr) {
        Semantic_stmtitem(fifstmt->elsestmt->stmtItem, label1, label2);
    }
    Pcode_instr pcodeInstr2 = Pcode_instr(Label, outlabel);
    pcodeVm.addinstr(pcodeInstr2);
    pcodeVm.setlabel_to_map(outlabel, pcodeVm.table_sp);
}

void Compunit_ast::Semantic_whilestmt(struct Whilestmt *fwhilestmt) {
    label_int++;

    loop_num++;
    int yourlabel = label_int;
    string whilelabel = "while" + to_string(yourlabel);
    Pcode_instr pcodeInstr = Pcode_instr(Label, whilelabel);
    pcodeVm.addinstr(pcodeInstr);
    pcodeVm.setlabel_to_map(whilelabel, pcodeVm.table_sp);

    Pcode_instr pcodeInstr5 = Pcode_instr(alu_moniter); /**开始计算cond之前monito*/
    pcodeVm.addinstr(pcodeInstr5);

    string cond_label = "cond_label" + to_string(yourlabel);
    Semantic_Cond(fwhilestmt->astcond, cond_label);

    Pcode_instr pcodeInstr4 = Pcode_instr(Label, cond_label);
    pcodeVm.addinstr(pcodeInstr4);
    pcodeVm.setlabel_to_map(cond_label, pcodeVm.table_sp);

    string whileout = "whileout" + to_string(yourlabel);
    Pcode_instr pcodeInstr1 = Pcode_instr(Jump_false, whileout);
    pcodeVm.addinstr(pcodeInstr1);

    Pcode_instr pcodeInstr6 = Pcode_instr(alu_cleaner);
    pcodeVm.addinstr(pcodeInstr6);
    Semantic_stmtitem(fwhilestmt->stmt->stmtItem, whilelabel, whileout);

    Pcode_instr pcodeInstr2 = Pcode_instr(Jump_j, whilelabel);
    pcodeVm.addinstr(pcodeInstr2);

    Pcode_instr pcodeInstr3 = Pcode_instr(Label, whileout);
    pcodeVm.addinstr(pcodeInstr3);
    pcodeVm.setlabel_to_map(whileout, pcodeVm.table_sp);
    Pcode_instr pcodeInstr7 = Pcode_instr(alu_cleaner);
    pcodeVm.addinstr(pcodeInstr7);

    loop_num--;
}

void Compunit_ast::Semantic_Cond(struct AstCond *astCond, string cond_label) {
    vector<vector<struct expr *>> condres = get_condense_exp(astCond->condexpr);
    cond_int++;
    //cout << "begin cond : " << condres.size() << endl;
    string orlabel = "or" + to_string(cond_int);

    int n = condres.size() - 1;
    for (int i = 0; i < condres.size() - 1; ++i) {
        Semantic_expr(condres[i][0], orlabel);

        Pcode_instr pcodeInstr1 = Pcode_instr(Label, orlabel);
        pcodeVm.addinstr(pcodeInstr1);
        pcodeVm.setlabel_to_map(orlabel, pcodeVm.table_sp);
        cond_int++;
        orlabel = "or" + to_string(cond_int);

        Pcode_instr pcodeInstr = Pcode_instr(Jump_cond_True, cond_label);
        pcodeVm.addinstr(pcodeInstr);
    }

    Semantic_expr(condres[n][0], orlabel);
    Pcode_instr pcodeInstr = Pcode_instr(Label, orlabel);
    pcodeVm.addinstr(pcodeInstr);
    pcodeVm.setlabel_to_map(orlabel, pcodeVm.table_sp);

    //cout << "cond end :" << endl;
}

vector<vector<struct expr *>> Compunit_ast::get_condense_exp(struct expr *fexpr) {
    vector<vector<struct expr *>> v1;
    if (fexpr->exp_flag == 1) {
        //cout << "here is binary" << endl;
        struct Binaryexp *binaryexp = dynamic_cast<Binaryexp *>(fexpr);
        if (binaryexp->op == OrType) {
            //cout << "here is or" << endl;
            vector<vector<struct expr *>> res1 = get_condense_exp(binaryexp->left);
            //cout << "res1.size " << res1.size() << endl;
            v1.insert(v1.end(), res1.begin(), res1.end());
            vector<vector<struct expr *>> res2 = get_condense_exp(binaryexp->right);
            //cout << "res2.size " << res2.size() << endl;
            v1.insert(v1.end(), res2.begin(), res2.end());
        } else {
            vector<struct expr *> res;
            res.push_back(binaryexp);
            v1.push_back(res);
        }
    } else {
        vector<struct expr *> res;
        res.push_back(fexpr);
        v1.push_back(res);
    }
    return v1;
}

void Compunit_ast::Semantic_break(string label2, int line) {
    if (this->loop_num <= 0) {
        myerror.add_SemanticError(BreakError, line); // TODO
    }
    Pcode_instr pcodeInstr = Pcode_instr(Jump_j, label2);
    pcodeVm.addinstr(pcodeInstr);
}

void Compunit_ast::Semantic_continue(string label1, int line) {
    if (this->loop_num <= 0) {
        myerror.add_SemanticError(BreakError, line); // TODO
    }
    Pcode_instr pcodeInstr = Pcode_instr(Jump_j, label1);
    pcodeVm.addinstr(pcodeInstr);
}

void Compunit_ast::Semantic_Read(struct Readstmt *freadstmt) {
    struct Readstmt *readstmt = freadstmt;
    struct Lval *lval = dynamic_cast<Lval *>(readstmt->lval);

    if (table.check_identifier(lval->identname, ConstType, lval->dimension) < 0) {
        myerror.add_SemanticError(Undefinition, lval->line); // TODO
    }

    if (table.check_type(lval->identname, ConstType) > 0) { // 如果确实是const常量
        myerror.add_SemanticError(ChangeConst, lval->line); // TODO
    }

    int dimen = lval->dimension;
    if (dimen == 1) {
        Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
        pcodeVm.addinstr(pcodeInstr);
        Semantic_expr(lval->exp_list[0], "condrelu");
        Pcode_instr pcodeInstr1 = Pcode_instr(Gen_address, lval->identname, 1);
        pcodeVm.addinstr(pcodeInstr1);
        Pcode_instr pcodeInstr2 = Pcode_instr(Read);
        pcodeVm.addinstr(pcodeInstr2);
        Pcode_instr pcodeInstr3 = Pcode_instr(STO);
        pcodeVm.addinstr(pcodeInstr3);
    } else if (dimen == 2) {
        Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
        pcodeVm.addinstr(pcodeInstr);
        Semantic_expr(lval->exp_list[0], "condrelu");
        Semantic_expr(lval->exp_list[1], "condrelu");
        Pcode_instr pcodeInstr1 = Pcode_instr(Gen_address, lval->identname, 2);
        pcodeVm.addinstr(pcodeInstr1);
        Pcode_instr pcodeInstr2 = Pcode_instr(Read);
        pcodeVm.addinstr(pcodeInstr2);
        Pcode_instr pcodeInstr3 = Pcode_instr(STO);
        pcodeVm.addinstr(pcodeInstr3);
    } else {
        Pcode_instr pcodeInstr = Pcode_instr(Load_address, lval->identname);
        pcodeVm.addinstr(pcodeInstr);
        Pcode_instr pcodeInstr1 = Pcode_instr(Read);
        pcodeVm.addinstr(pcodeInstr1);
        Pcode_instr pcodeInstr2 = Pcode_instr(STO);
        pcodeVm.addinstr(pcodeInstr2);
    }
}

void Compunit_ast::Semantic_Write(struct Printstmt *fprintstmt) {
    for (int i = 0; i < fprintstmt->exp_list.size(); ++i) {
        Semantic_expr(fprintstmt->exp_list[i], "condrelu");
    }
    if (fprintstmt->formatnum != fprintstmt->exp_list.size()) {
        myerror.add_SemanticError(PrintError, fprintstmt->line); // TODO
    }

    Pcode_instr pcodeInstr = Pcode_instr(Back, fprintstmt->exp_list.size());
    pcodeVm.addinstr(pcodeInstr);
    Pcode_instr pcodeInstr1 = Pcode_instr(Write, fprintstmt->formatstring);
    pcodeVm.addinstr(pcodeInstr1);
}

void Compunit_ast::Semantic_Return(struct Returnstmt *freturnstmt) {

    if (in_void_func) { // 如果是void
        if (freturnstmt->returnexp != nullptr) {
            myerror.add_SemanticError(UnexpectedReturn, freturnstmt->line); // TODO
        }
    }
    //cout << "begin Semantic Return:" << endl;
    if (freturnstmt->returnexp != nullptr) {
        Semantic_expr(freturnstmt->returnexp, "condrelu");
        Pcode_instr pcodeInstr = Pcode_instr(Jump_ra);
        pcodeVm.addinstr(pcodeInstr);
    } else {
        Pcode_instr pcodeInstr = Pcode_instr(Jump_ra);
        pcodeVm.addinstr(pcodeInstr);
    }
    //cout << "Semantic_return ok" << endl;
}

