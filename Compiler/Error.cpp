//
// Created by wuhaotian on 2021/12/12.
//

#include <iostream>
#include "Error.h"
#include <algorithm>

void Error::add_errorItem(ErrorType type, int line) {
    struct ErrorItem item = ErrorItem(type, line);
    error_stack.push_back(item);
}

void Error::add_SemanticError(ErrorType type, int line) {
    struct ErrorItem item = ErrorItem(type, line);
    error_stack.push_back(item);
}

bool Error::check_error() {
    return !this->error_stack.empty();
}

void Error::print_error() {
    sort(error_stack.begin(), error_stack.end(), cmp);
    for (int i = 0; i < error_stack.size(); ++i) {
        ErrorType errorType = error_stack[i].errorType;
        int line = error_stack[i].error_line;
        switch (errorType) {
            case Illegalstring:
                cout << line << " a" << endl;
                break;
            case LackSemicn:
                cout << line << " i" << endl;
                break;
            case LackRparent:
                cout << line << " j" << endl;
                break;
            case LackRbrack:
                cout << line << " k" << endl;
                break;
            case Redefinition:
                cout << line << " b" << endl;
                break;
            case Undefinition:
                cout << line << " c" << endl;
                break;
            case NumMismatch:
                cout << line << " d" << endl;
                break;
            case TypeMismatch:
                cout << line << " e" << endl;
                break;
            case UnexpectedReturn:
                cout << line << " f" << endl;
                break;
            case LackReturn:
                cout << line << " g" << endl;
                break;
            case ChangeConst:
                cout << line << " h" << endl;
                break;
            case PrintError:
                cout << line << " l" << endl;
                break;
            case BreakError:
                cout << line << " m" << endl;
                break;
            default:
                break;
        }
    }
}

bool Error::cmp(struct ErrorItem a, struct ErrorItem b) {
    return a.error_line < b.error_line;
}
