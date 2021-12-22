//
// Created by gmm on 2021/10/18.
//

#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include <bits/stdc++.h>

using namespace std;

#ifndef LexerPrint
//#define LexerPrint      //控制01词法分析输出
#endif

#ifndef ParserPrint
//#define ParserPrint   //控制02语法分析输出
#endif

#ifndef OPTIMIZE
#define OPTIMIZE
#endif

typedef pair<int, string> PIS;

//template<typename T, typename... Ts>
//std::shared_ptr<T> make_unique(Ts &&... params) {
//    return std::shared_ptr<T>(new T(std::forward<Ts>(params)...));
//}

//rule name
//enum RULE_CATEGORY {
//    PROGRAM, COMPUNIT, DECL, CONSTDECL, BTYPE, CONSTDEF, CONSTINITVAL,
//    VARDECL, VARDEF, INITVAL, FUNCDEF, FUNCTYPE, FUNCFPARAMS,
//    FUNCFPARAM, BLOCK, BLOCKITEM, STMT, ASSIGNSTMT, EXPSTMT,
//    CONDITIONSTMT, WHILESTMT, BREAKSTMT, CONTINUESTMT, RETURNSTMT,
//    EXP, COND, LVAL, PRIMARYEXP, NUMBER, INTCONST, UNARYEXP,
//    CALLEE, UNARYOP, FUNCRPARAMS, PARAM, MULEXP, MULOP, ADDEXP,
//    ADDOP, RELEXP, RELOP, EQEXP, EQOP, LANDEXP, LOREXP, CONSTEXP
//};
//
//static const int RULE_NUM = 100;
//
//static const char *ruleNames[RULE_NUM] = {"<Program>", "<CompUnit>", "<Decl>", "<ConstDecl>", "<BType>", "<ConstDef>",
//                                          "<ConstInitVal>",
//                                          "<VarDecl>", "<VarDef>", "<InitVal>", "<FuncDef>", "<FuncType>",
//                                          "<FuncFParamsAST>",
//                                          "<FuncFParam>", "<BlockAST>", "<BlockItem>", "<Stmt>", "<AssignStmt>",
//                                          "<ExpStmt>",
//                                          "<ConditionStmt>", "<WhileStmt>", "<BreakStmt>", "<ContinueStmt>",
//                                          "<ReturnStmt>",
//                                          "<Exp>", "<Cond>", "<LVal>", "<PrimaryExp>", "<Number>", "<IntConst>",
//                                          "<UnaryExp>",
//                                          "<Callee>", "<UnaryOp>", "<FuncRParams>", "<Param>", "<MulExp>", "<MulOp>",
//                                          "<AddExp>",
//                                          "<AddOp>", "<RelExp>", "<RelOp>", "<EqExp>", "<EqOp>", "<LAndExp>",
//                                          "<LOrExp>", "<ConstExp>"};


//token name
enum TOKEN_CATEGORY {
    IDENFR, INTCON, STRCON, MAINTK,
    CONSTTK, INTTK, BREAKTK, CONTINUETK, IFTK,
    ELSETK, NOT, AND, OR, WHILETK, GETINTTK,
    PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK,
    MULT, DIV, MOD, LSS, LEQ, GRE, GEQ, EQL,
    NEQ, ASSIGN, SEMICN, COMMA, LPARENT,
    RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
};

static const int TOKEN_NUM = 100;

static const char *tokenName[TOKEN_NUM] = {"IDENFR", "INTCON", "STRCON", "MAINTK",
                                           "CONSTTK", "INTTK", "BREAKTK", "CONTINUETK", "IFTK",
                                           "ELSETK", "NOT", "AND", "OR", "WHILETK", "GETINTTK",
                                           "PRINTFTK", "RETURNTK", "PLUS", "MINU", "VOIDTK",
                                           "MULT", "DIV", "MOD", "LSS", "LEQ", "GRE", "GEQ", "EQL",
                                           "NEQ", "ASSIGN", "SEMICN", "COMMA", "LPARENT",
                                           "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};

enum OperatorType {
    OpArray,
    OpVar,   //变量
    OpConst, //常量
    OpPLUS,
    OpMINU,
    OpMULT,
    OpDIV,
    OpAssign,
    OpPush,
    OpCall,
    OpNot,
    OpMOD,
    OpScanf,
    OpPrint,
    OpFunc,
    OpJMain,
    OpExit,
    OpPara,
    OpReturn,
    OpPrintInt,
    OpLSS,
    OpLEQ,
    OpGRE,
    OpGEQ,
    OpEQL,
    OpNEQ,
    OpJmp,
    OpLabel,
    OpSeq,
    OpSne,
    OpSlt,
    OpSle,
    OpSgt,
    OpSge,
    OpSll,
    OpSrl,
    OpAnd,
    OpOr,
    OpSrlv,
    OpSllv,
};


static const char *const operatorString[] = {
        "Array",
        "Var",   //变量
        "Const", //常量
        "Plus",
        "Minu",
        "Mult",
        "Div",
        "Assign",
        "Push",
        "Call",
        "Not",
        "Mod",
        "Getint",
        "Print",
        "Func",
        "JMain",
        "Exit",
        "Para",
        "Ret",
        "PrintInt",
        "LSS", // <
        "LEQ", // <=
        "GRE", // >
        "GEQ", // >=
        "EQL", // ==
        "NEQ", // !=
        "Jmp",
        "Label",
        "seq",
        "sne",
        "slt",
        "sle",
        "sgt",
        "sge",
        "sll",
        "srl",
        "and",
        "or",
        "srlv",
        "sllv",
};

enum SymbolType {
    INT,
    VOID,
    IF,
    WHILE
};

static const char *const symbolTypeString[] = {
        "int",
        "void",
        "if_",
        "while_"
};

#endif //COMPILER_TYPE_H
