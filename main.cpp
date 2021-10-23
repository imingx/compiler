#include "include/main.h"

using namespace std;

/*
    cmake_minimum_required(VERSION 3.20)
    project(Compiler)

    set(CMAKE_CXX_STANDARD 11)

    add_executable(Compiler main.cpp Lexer.cpp include/Lexer.h include/main.h Parser.cpp include/Parser.h AST.cpp include/AST.h SymbolTable.cpp include/SymbolTable.h include/TYPE.h ErrorHandle.cpp include/ErrorHandle.h)
 */

int main() {
    //Lexer和Parser的输出控制在TYPE.h

    // lexer
    Lexer lexer(FILE_IN, FILE_OUT);
    lexer.program();

    // parser
    ErrorHandle errorHandle(ERROR);

    Parser parser(FILE_OUT);
    parser.program();

//    unique_ptr<CompUnitAST> AST = parser.getAST();

    return 0;
}
