cmake_minimum_required(VERSION 3.13)
project(new)

set(CMAKE_CXX_STANDARD 11)

add_executable(new main.cpp Lexer.cpp include/Lexer.h include/main.h Parser.cpp include/Parser.h AST.cpp include/AST.h SymbolTable.cpp include/SymbolTable.h include/TYPE.h)
