//
// Created by wuhaotian on 2021/10/15.
//

#include "LexicalAnalyzer.h"
#include "symbol.h"
#include <string>

const string reservers[12] = {"main", "const", "int", "break", "continue", "if", "else", "while", "getint", "printf",
                              "return", "void"};

LexicalAnalyzer::LexicalAnalyzer(FILE *f, Error &error) : fp(f), myerror(error) {
    flag = true;
    word = "";
    line = 1;
    stringflag = true;
    formatnum = 0;
}

void LexicalAnalyzer::nextline(char c) {
    if (c == '\n') {
        line++;
    }
}

bool LexicalAnalyzer::isSpace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

bool LexicalAnalyzer::isLetter(char c) {
    return isalpha(c) || c == '_';
}

int LexicalAnalyzer::isReserver(string fword) {
    for (int i = 0; i < 12; ++i) {
        if (fword == reservers[i]) {
            return i + 4;
        }
    }
    return 0;
}

int LexicalAnalyzer::getsym() {
    word.clear();
    curchar = fgetc(fp);
    if (curchar == EOF || feof(fp)) {
        flag = false;
        return 0;
    }
    while (isSpace(curchar)) {
        nextline(curchar);
        curchar = fgetc(fp);
    }
    if (curchar == EOF || feof(fp)) {
        flag = false;
        return 0;
    } else if (isLetter(curchar)) { // 标识符或保留字
        while (curchar && (isalnum(curchar) || curchar == '_')) {
            word += curchar;
            curchar = fgetc(fp);
        }
        if (curchar == EOF || feof(fp)) {
            flag = false;
            return IDENFR;
        }
        fseek(fp, -1, SEEK_CUR);
        int resultValue = isReserver(word);
        if (resultValue == 0) {
            return IDENFR;
        } else {
            return resultValue;
        }
    } else if (isdigit(curchar)) { // 数值常量
        while (curchar && isdigit(curchar)) {
            word += curchar;
            curchar = fgetc(fp);
        }
        if (curchar == EOF || feof(fp)) {
            flag = false;
            return INTCON;
        }
        fseek(fp, -1, SEEK_CUR);
        return INTCON;
    } else if (curchar == '"') { // Double quotation mark - string
        stringflag = true; // whether the string is legal
        formatnum = 0; // the num of %d
        word += curchar;
        curchar = fgetc(fp);
        while (curchar && curchar != '"') {
            word += curchar;
            curchar = fgetc(fp);
        }
        if (curchar == '"') {
            word += curchar;
            formatnum = checkstring(); // check whether it has illegal char, and count the %d
            if (!stringflag) {
                //myerror.LexicalAnalysisError(Illegalstring, line);
                myerror.add_errorItem(Illegalstring, line);
                stringflag = true;
            }
            return STRCON;
        } else {
            flag = false;
            return -1;
        }
    } else if (curchar == '!') { // not or not equal
        word += curchar;
        curchar = fgetc(fp);
        if (curchar != '=') {
            fseek(fp, -1, SEEK_CUR);
            return NOT;
        } else {
            word += curchar;
            return NEQ;
        }
    } else if (curchar == '&') { // and
        word += curchar;
        curchar = fgetc(fp);
        if (curchar == '&') {
            word += curchar;
            return AND;
        } else {
            flag = false;
            return -1;
        }
    } else if (curchar == '|') { // or
        word += curchar;
        curchar = fgetc(fp);
        if (curchar == '|') {
            word += curchar;
            return OR;
        } else {
            flag = false;
            return -1;
        }
    } else if (curchar == '+') { // plus
        word += curchar;
        return PLUS;
    } else if (curchar == '-') { // minus
        word += curchar;
        return MINUS;
    } else if (curchar == '*') { // star - multiply
        word += curchar;
        return MULT;
    } else if (curchar == '/') { // div or notes
        curchar = fgetc(fp);
        if (curchar == '*') { // multiline notes
            do {
                do {
                    curchar = fgetc(fp);
                    nextline(curchar);
                } while (curchar && curchar != '*');
                do {
                    curchar = fgetc(fp);
                    nextline(curchar);
                    if (curchar == '/') return 0;
                } while (curchar == '*');
            } while (curchar && curchar != '*');
            return 0;
        } else if (curchar == '/') { // double div - single line notes
            while (curchar && curchar != '\n' && !feof(fp)) {
                curchar = fgetc(fp);
            }
            if (curchar == EOF || feof(fp)) {
                flag = false;
                return 0;
            } else {
                nextline(curchar);
                //fseek(fp, -1, SEEK_CUR);
                return 0;
            }
        } else { // it is a single div
            fseek(fp, -1, SEEK_CUR);
            word += '/';
            return DIV;
        }
    } else if (curchar == '%') { // mod
        word += curchar;
        return MOD;
    } else if (curchar == '<') {
        word += curchar;
        curchar = fgetc(fp);
        if (curchar != '=') { // less
            fseek(fp, -1, SEEK_CUR);
            return LSS;
        } else { // less equal
            word += curchar;
            return LEQ;
        }
    } else if (curchar == '>') {
        word += curchar;
        curchar = fgetc(fp);
        if (curchar != '=') { // greater
            fseek(fp, -1, SEEK_CUR);
            return GRE;
        } else { // greater equal
            word += curchar;
            return GEQ;
        }
    } else if (curchar == '=') {
        word += curchar;
        curchar = fgetc(fp);
        if (curchar != '=') { // assign
            fseek(fp, -1, SEEK_CUR);
            return ASSIGN;
        } else { // equal
            word += curchar;
            return EQL;
        }
    } else if (curchar == ';') { // semicolon
        word += curchar;
        return SEMICN;
    } else if (curchar == ',') { // comma
        word += curchar;
        return COMMA;
    } else if (curchar == '(') {
        word += curchar;
        return LPARENT;
    } else if (curchar == ')') {
        word += curchar;
        return RPARENT;
    } else if (curchar == '[') {
        word += curchar;
        return LBRACK;
    } else if (curchar == ']') {
        word += curchar;
        return RBRACK;
    } else if (curchar == '{') {
        word += curchar;
        return LBRACE;
    } else if (curchar == '}') {
        word += curchar;
        return RBRACE;
    } else {
        flag = false;
        return -1;
    }
}

int LexicalAnalyzer::getlookup() {
    char fcurchar;
    string fword = "";

    int ccount = 0;
    int fres;
    fcurchar = fgetc(fp);
    ccount++;
    if (fcurchar == EOF || feof(fp)) {
        flag = false;
        fres = 0;
    }
    while (isSpace(fcurchar)) {
        fcurchar = fgetc(fp);
        ccount++;
    }
    if (fcurchar == EOF || feof(fp)) {
        flag = false;
        fres = 0;
    } else if (isLetter(fcurchar)) { // 标识符或保留字
        while (fcurchar && (isalnum(fcurchar) || fcurchar == '_')) {
            fword += fcurchar;
            fcurchar = fgetc(fp);
            ccount++;
        }
        if (fcurchar == EOF || feof(fp)) {
            flag = false;
            fres = IDENFR;
        }
        fseek(fp, -1, SEEK_CUR);
        ccount--;
        int fresultValue = isReserver(fword);
        if (fresultValue == 0) {
            fres = IDENFR;
        } else {
            fres = fresultValue;
        }
    } else if (isdigit(fcurchar)) { // 数值常量
        while (fcurchar && isdigit(fcurchar)) {
            fword += fcurchar;
            fcurchar = fgetc(fp);
            ccount++;
        }
        if (fcurchar == EOF || feof(fp)) {
            flag = false;
            fres = INTCON;
        }
        fseek(fp, -1, SEEK_CUR);
        ccount--;
        fres = INTCON;
    } else if (fcurchar == '"') { // Double quotation mark - string
        fword += fcurchar;
        fcurchar = fgetc(fp);
        ccount++;
        while (fcurchar && fcurchar != '"') {
            fword += fcurchar;
            fcurchar = fgetc(fp);
            ccount++;
        }
        if (fcurchar == '"') {
            fword += fcurchar;
            fres = STRCON;
        } else {
            flag = false;
            fres = -1;
        }
    } else if (fcurchar == '!') { // not or not equal
        fword += fcurchar;
        fcurchar = fgetc(fp);
        ccount++;
        if (fcurchar != '=') {
            fseek(fp, -1, SEEK_CUR);
            ccount--;
            fres = NOT;
        } else {
            fword += fcurchar;
            fres = NEQ;
        }
    } else if (fcurchar == '&') { // and
        fword += fcurchar;
        fcurchar = fgetc(fp);
        ccount++;
        if (fcurchar == '&') {
            fword += fcurchar;
            fres = AND;
        } else {
            flag = false;
            fres = -1;
        }
    } else if (fcurchar == '|') { // or
        fword += fcurchar;
        fcurchar = fgetc(fp);
        ccount++;
        if (fcurchar == '|') {
            fword += fcurchar;
            fres = OR;
        } else {
            flag = false;
            fres = -1;
        }
    } else if (fcurchar == '+') { // plus
        fword += fcurchar;
        fres = PLUS;
    } else if (fcurchar == '-') { // minus
        fword += fcurchar;
        fres = MINUS;
    } else if (fcurchar == '*') { // star - multiply
        fword += fcurchar;
        fres = MULT;
    } else if (fcurchar == '/') { // div or notes
        fcurchar = fgetc(fp);
        ccount++;
        if (fcurchar == '*') { // multiline notes
            do {
                do {
                    fcurchar = fgetc(fp);
                    ccount++;
                } while (fcurchar && fcurchar != '*');
                do {
                    fcurchar = fgetc(fp);
                    ccount++;
                    if (fcurchar == '/') {
                        fres = 0;
                        fseek(fp, -ccount, SEEK_CUR);
                        return fres;
                    }
                } while (fcurchar == '*');
            } while (fcurchar && fcurchar != '*');
            fres = 0;
        } else if (fcurchar == '/') { // double div - single line notes
            while (fcurchar && fcurchar != '\n' && !feof(fp)) {
                fcurchar = fgetc(fp);
                ccount++;
            }
            if (fcurchar == EOF || feof(fp)) {
                flag = false;
                fres = 0;
            } else {
                //fseek(fp, -1, SEEK_CUR);
                fres = 0;
                fseek(fp, -ccount, SEEK_CUR);
            }
        } else { // it is a single div
            fseek(fp, -1, SEEK_CUR);
            ccount--;
            fword += '/';
            fres = DIV;
        }
    } else if (fcurchar == '%') { // mod
        fword += fcurchar;
        fres = MOD;
    } else if (fcurchar == '<') {
        fword += fcurchar;
        fcurchar = fgetc(fp);
        ccount++;
        if (fcurchar != '=') { // less
            fseek(fp, -1, SEEK_CUR);
            ccount--;
            fres = LSS;
        } else { // less equal
            fword += fcurchar;
            fres = LEQ;
        }
    } else if (fcurchar == '>') {
        fword += fcurchar;
        fcurchar = fgetc(fp);
        ccount++;
        if (fcurchar != '=') { // greater
            fseek(fp, -1, SEEK_CUR);
            ccount--;
            fres = GRE;
        } else { // greater equal
            fword += fcurchar;
            fres = GEQ;
        }
    } else if (fcurchar == '=') {
        fword += fcurchar;
        fcurchar = fgetc(fp);
        ccount++;
        if (fcurchar != '=') { // assign
            fseek(fp, -1, SEEK_CUR);
            ccount--;
            fres = ASSIGN;
        } else { // equal
            fword += fcurchar;
            fres = EQL;
        }
    } else if (fcurchar == ';') { // semicolon
        fword += fcurchar;
        fres = SEMICN;
    } else if (fcurchar == ',') { // comma
        fword += fcurchar;
        fres = COMMA;
    } else if (fcurchar == '(') {
        fword += fcurchar;
        fres = LPARENT;
    } else if (fcurchar == ')') {
        fword += fcurchar;
        fres = RPARENT;
    } else if (fcurchar == '[') {
        fword += fcurchar;
        fres = LBRACK;
    } else if (fcurchar == ']') {
        fword += fcurchar;
        fres = RBRACK;
    } else if (fcurchar == '{') {
        fword += fcurchar;
        fres = LBRACE;
    } else if (fcurchar == '}') {
        fword += fcurchar;
        fres = RBRACE;
    } else {
        flag = false;
        fres = -1;
    }

    if (fres > 0) {
        fseek(fp, -ccount, SEEK_CUR);
    }
    return fres;
}

string LexicalAnalyzer::getword() {
    return word;
}

int LexicalAnalyzer::checkstring() {
    int res = 0;
    for (int i = 1; i < word.size() - 1; ++i) {
        if (!isLegalchar(word[i])) {
            if (word[i] == '%' && word[i + 1] == 'd') {
                res++;
            } else {
                stringflag = false;
            }
        }
        if (word[i] == '\\' && word[i + 1] != 'n') {
            stringflag = false;
        }
    }
    return res;
}

bool LexicalAnalyzer::getflag() {
    return flag;
}

int LexicalAnalyzer::getline() {
    return line;
}

int LexicalAnalyzer::getformatnum() {
    return formatnum;
}

bool LexicalAnalyzer::isLegalchar(char c) {
    if (c == 32 || c == 33 || (c >= 40 && c <= 126)) {
        return true;
    } else {
        return false;
    }
}
