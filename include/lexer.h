//
// Created by gmm on 2021/9/28.
//
#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H
#include <bits/stdc++.h>
using namespace std;

enum CATEGORY {
    IDENFR, INTCON, STRCON, MAINTK,
    CONSTTK, INTTK, BREAKTK, CONTINUETK, IFTK,
    ELSETK, NOT, AND, OR, WHILETK, GETINTTK,
    PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK,
    MULT, DIV, MOD, LSS, LEQ, GRE, GEQ, EQL,
    NEQ, ASSIGN, SEMICN, COMMA, LPARENT,
    RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
};

//typedef  pair<string, CATEGORY> PSC;

static const int CATEGORY_NUM = 38;


struct Word {
    enum CATEGORY category;
    string raw;
    int line;
};
class Lexer {
private:
    FILE *in, *out;
    int line;
    map<string, CATEGORY> keyWord;
public:
    Lexer(const char *in, const char *out);
    void analyse();
};

static char const *token[CATEGORY_NUM] = {"IDENFR", "INTCON", "STRCON", "MAINTK",
                                   "CONSTTK", "INTTK", "BREAKTK", "CONTINUETK", "IFTK",
                                   "ELSETK", "NOT", "AND", "OR", "WHILETK", "GETINTTK",
                                   "PRINTFTK", "RETURNTK", "PLUS", "MINU", "VOIDTK",
                                   "MULT", "DIV", "MOD", "LSS", "LEQ", "GRE", "GEQ", "EQL",
                                   "NEQ", "ASSIGN", "SEMICN", "COMMA", "LPARENT",
                                   "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};

static vector<Word> words;

bool isSingle(char c);
#endif //COMPILER_LEXER_H
