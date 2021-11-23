//
// Created by gmm on 2021/9/28.
//

#ifndef COMPILER_MAIN_H
#define COMPILER_MAIN_H

# include "Lexer.h"
# include "Parser.h"
# include "AST.h"
# include "type.h"
# include "ErrorHandler.h"
# include "IRcode.h"

// FILE IO define
static const char *FILE_IN = "testfile.txt";
static const char *FILE_OUT = "output.txt";
static const char *ERROR = "error.txt";
static const char *MIPS = "mips.txt";
static const char *PCODE = "pcoderesult.txt";

#endif //COMPILER_MAIN_H
