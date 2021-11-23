cmake_minimum_required(VERSION 3.20)
project(Compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(Compiler main.cpp Lexer.cpp include/Lexer.h include/main.h Parser.cpp include/Parser.h include/AST.h include/SymbolTable.h include/type.h ErrorHandler.cpp include/ErrorHandler.h IRcode.cpp include/IRcode.h)
