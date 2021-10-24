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

typedef pair<int, string> PIS;

//template<typename T, typename... Ts>
//std::shared_ptr<T> make_unique(Ts &&... params) {
//    return std::shared_ptr<T>(new T(std::forward<Ts>(params)...));
//}

//rule name
enum RULE_CATEGORY {
    PROGRAM, COMPUNIT, DECL, CONSTDECL, BTYPE, CONSTDEF, CONSTINITVAL,
    VARDECL, VARDEF, INITVAL, FUNCDEF, FUNCTYPE, FUNCFPARAMS,
    FUNCFPARAM, BLOCK, BLOCKITEM, STMT, ASSIGNSTMT, EXPSTMT,
    CONDITIONSTMT, WHILESTMT, BREAKSTMT, CONTINUESTMT, RETURNSTMT,
    EXP, COND, LVAL, PRIMARYEXP, NUMBER, INTCONST, UNARYEXP,
    CALLEE, UNARYOP, FUNCRPARAMS, PARAM, MULEXP, MULOP, ADDEXP,
    ADDOP, RELEXP, RELOP, EQEXP, EQOP, LANDEXP, LOREXP, CONSTEXP
};

static const int RULE_NUM = 100;

static const char *ruleNames[RULE_NUM] = {"<Program>", "<CompUnit>", "<Decl>", "<ConstDecl>", "<BType>", "<ConstDef>",
                                          "<ConstInitVal>",
                                          "<VarDecl>", "<VarDef>", "<InitVal>", "<FuncDef>", "<FuncType>",
                                          "<FuncFParamsAST>",
                                          "<FuncFParam>", "<BlockAST>", "<BlockItem>", "<Stmt>", "<AssignStmt>",
                                          "<ExpStmt>",
                                          "<ConditionStmt>", "<WhileStmt>", "<BreakStmt>", "<ContinueStmt>",
                                          "<ReturnStmt>",
                                          "<Exp>", "<Cond>", "<LVal>", "<PrimaryExp>", "<Number>", "<IntConst>",
                                          "<UnaryExp>",
                                          "<Callee>", "<UnaryOp>", "<FuncRParams>", "<Param>", "<MulExp>", "<MulOp>",
                                          "<AddExp>",
                                          "<AddOp>", "<RelExp>", "<RelOp>", "<EqExp>", "<EqOp>", "<LAndExp>",
                                          "<LOrExp>", "<ConstExp>"};


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


#endif //COMPILER_TYPE_H
