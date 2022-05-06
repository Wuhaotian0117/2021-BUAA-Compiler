//
// Created by wuhaotian on 2021/12/12.
//

#ifndef HOMEWORK6_ERROR_H
#define HOMEWORK6_ERROR_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;
enum ErrorType {
    Illegalstring,
    LackSemicn,
    LackRparent,
    LackRbrack,
    Redefinition,
    Undefinition,
    NumMismatch,
    TypeMismatch,
    UnexpectedReturn,
    LackReturn,
    ChangeConst,
    PrintError, //
    BreakError
};

struct ErrorItem {
    ErrorType errorType;
    int error_line;

    ErrorItem(ErrorType type, int line) {
        this->errorType = type;
        this->error_line = line;
    }
};

class Error {
private:
    static bool cmp(struct ErrorItem a, struct ErrorItem b);

public:
    vector<ErrorItem> error_stack;

    void add_errorItem(ErrorType type, int line);

    void add_SemanticError(ErrorType type, int line);

    void print_error();

    bool check_error();


};


#endif //HOMEWORK6_ERROR_H
