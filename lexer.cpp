//
// Created by gmm on 2021/9/28.
//
#include "include/lexer.h"

using namespace std;

bool isSingle(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == ':' || c == ';' || c == ','
        || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '%')
        return true;
    else return false;
}

Lexer::Lexer(const char *FILE_IN, const char *FILE_OUT) {
    this->in = fopen(FILE_IN, "r");
    this->out = fopen(FILE_OUT, "w");
    this->line = 1;
    keyWord["main"] = MAINTK;
    keyWord["const"] = CONSTTK;
    keyWord["int"] = INTTK;
    keyWord["break"] = BREAKTK;
    keyWord["continue"] = CONTINUETK;
    keyWord["if"] = IFTK;
    keyWord["else"] = ELSETK;
    keyWord["while"] = WHILETK;
    keyWord["getint"] = GETINTTK;
    keyWord["printf"] = PRINTFTK;
    keyWord["return"] = RETURNTK;
    keyWord["void"] = VOIDTK;
    keyWord["!"] = NOT;
    keyWord["!="] = NEQ;
    keyWord["<"] = LSS;
    keyWord[">"] = GRE;
    keyWord[">="] = GEQ;
    keyWord["<="] = LEQ;
    keyWord["=="] = EQL;
    keyWord["="] = ASSIGN;
    keyWord["&&"] = AND;
    keyWord["||"] = OR;
    keyWord["+"] = PLUS;
    keyWord["-"] = MINU;
    keyWord["*"] = MULT;
    keyWord["/"] = DIV;
    keyWord["%"] = MOD;
    keyWord[";"] = SEMICN;
    keyWord[","] = COMMA;
    keyWord["("] = LPARENT;
    keyWord[")"] = RPARENT;
    keyWord["["] = LBRACK;
    keyWord["]"] = RBRACK;
    keyWord["{"] = LBRACE;
    keyWord["}"] = RBRACE;
}

void Lexer::analyse() {
    char c;
    while ((c = fgetc(in)) != EOF) {
        string str;
        Word word;
        while (isspace(c)) {
            if (c == '\n') ++line;
            c = fgetc(in);
        }
        if (c == EOF) {
            break;
        }
        word.line = line;
        if (isalpha(c) || c == '_') {
            do {
                str += c;
                c = fgetc(in);
            } while (isalnum(c) || c == '_');
            ungetc(c, in);

            map<string, CATEGORY>::iterator iterator = keyWord.find(str);
            if (iterator == keyWord.end()) {
                word.category = IDENFR;
            } else {
                word.category = iterator->second;
            }
            word.raw = str;
        } else if (isSingle(c)) {
            if (c == '/') {
                c = fgetc(in);
                if (c == '/') {
                    while ((c = fgetc(in)) != '\n');
                    continue;
                } else if (c == '*') {
                    int d = fgetc(in), e = fgetc(in);
                    while ( !(d == '*' && e == '/') ){
                        d = e;
                        e = fgetc(in);
                    }
                    continue;
                } else {
                    ungetc(c, in);
                    c = '/';
                }
            }
            str += c;
            word.raw = str;
            word.category = keyWord[str];
        } else if (isdigit(c)) {
            do {
                str += c;
                c = fgetc(in);
            } while (isdigit(c));

            ungetc(c, in);
            word.category = INTCON;
            word.raw = str;
        } else if (c == '\"') {
            do {
                str += c;
                c = fgetc(in);
            } while (c != '\"');
            str += c;
            word.category = STRCON;

            word.raw = str;
        } else if (c == '&' || c == '|') {
            str += c;
            c = fgetc(in);
            if (c == '&') {
                str += c;
                word.category = AND;
            } else if (c == '|') {
                str += c;
                word.category = OR;
            }
            word.raw = str;
        } else if (c == '<' || c == '>' || c == '=' || c == '!') {
            str += c;
            c = fgetc(in);

            if (c == '=') {
                str += c;
            } else {
                ungetc(c, in);
            }
            word.category = keyWord[str];
        }
        cout << token[word.category] << " " << str << endl;
        fprintf(out, "%s %s\n", token[word.category], str.c_str());
        words.push_back(word);
    }
}