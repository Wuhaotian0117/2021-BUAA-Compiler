cmake_minimum_required(VERSION 3.20)
project(Compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(Compiler main.cpp LexicalAnalyzer.cpp LexicalAnalyzer.h symbol.h printword.h printword.cpp ParserAnalyzer.cpp ParserAnalyzer.h structdef.h TypeValue.h Compunit_ast.cpp Compunit_ast.h Pcode_vm.cpp Pcode_vm.h Datastack_item.cpp Datastack_item.h Pcode_instr.cpp Pcode_instr.h SymbolTable.cpp SymbolTable.h SymbolTableItem.cpp SymbolTableItem.h Error.cpp Error.h)
