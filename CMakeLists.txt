cmake_minimum_required(VERSION 3.20)
project(Compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(Compiler main.cpp Lexer.cpp include/Lexer.h include/main.h Parser.cpp include/Parser.h AST.cpp include/AST.h SymbolTable.cpp include/SymbolTable.h include/TYPE.h)
