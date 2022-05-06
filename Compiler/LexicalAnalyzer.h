//
// Created by wuhaotian on 2021/10/15.
//

#ifndef HOMEWORK3P_LEXICALANALYZER_H
#define HOMEWORK3P_LEXICALANALYZER_H


#include <cstdio>
#include <string>
#include "Error.h"

using namespace std;

class LexicalAnalyzer {
public:
    LexicalAnalyzer(FILE *f, Error & error);

    int getsym();

    int getlookup();

    string getword();

    bool getflag();

    int getline();

    int getformatnum();

private:
    FILE *fp;
    char curchar;
    bool flag;
    int symbol;
    string word;
    int line;

    Error &myerror;
    bool stringflag;
    int formatnum;

    void nextline(char c);

    bool isSpace(char c);

    bool isLetter(char c);

    int isReserver(string fword);

    int checkstring();

    bool isLegalchar(char c);


};


#endif //HOMEWORK3P_LEXICALANALYZER_H
