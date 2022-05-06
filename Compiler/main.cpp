#include <iostream>
#include <string>
#include "LexicalAnalyzer.h"
#include "ParserAnalyzer.h"
#include "Compunit_ast.h"
#include "SymbolTable.h"
#include "Pcode_vm.h"
#include "Error.h"

using namespace std;

int main() {
    FILE *fpp = fopen("testfile.txt", "r");
    //freopen("pcoderesult.txt", "w", stdout);

    Error myError;

    SymbolTable table;

    Pcode_vm pcodeVm;
    //cout << "open file ok" << endl;
    LexicalAnalyzer laxer(fpp, myError);
    Compunit_ast asthahaha(table, pcodeVm, myError);
    ParserAnalyzer parser(fpp, laxer, asthahaha, myError);


    parser.Program();
    /*cout << "ast is ok" << endl;
    asthahaha.printyourself();*/

    asthahaha.Semantic_ast();

    if (myError.check_error()) {
        freopen("error.txt", "w", stdout);
        myError.print_error();
    } else {
        /*cout << "\n\n\nHere is the Pcode:" << endl;
        pcodeVm.printyourself();

        cout << "\nhere is output:" << endl;*/
        freopen("pcoderesult.txt", "w", stdout);
        pcodeVm.Pcode_cm_run();
    }
    return 0;
}
