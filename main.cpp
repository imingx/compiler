#include "include/main.h"

using namespace std;

/*
    cmake_minimum_required(VERSION 3.20)
    project(Compiler)

    set(CMAKE_CXX_STANDARD 11)

    add_executable(Compiler main.cpp Lexer.cpp include/Lexer.h include/main.h Parser.cpp include/Parser.h include/AST.h include/OldSymbolTable.h include/type.h ErrorHandler.cpp include/ErrorHandler.h IRcode.cpp include/IRcode.h)
 */

int main() {
    //Lexer & Parser的输出控制在TYPE.h

    // lexer
    Lexer lexer(FILE_IN, FILE_OUT);
    lexer.program();

    // parser
    Parser parser(FILE_OUT);
    parser.program();
    shared_ptr<CompUnitAST> &AST = parser.getAST();

    // error handling
    ErrorHandler errorHandler(ERROR, AST);
    errorHandler.program();
//    errorHandler.print(); // error print

    // Above is frontend
    // --------------------------------------------
    // Below is backend

    IRcodeMaker iRcodeMaker(AST);
    iRcodeMaker.program();
    iRcodeMaker.print();

    Mips mips(MIPS);
    mips.program();

    return 0;
}
