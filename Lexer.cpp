//
// Created by gmm on 2021/9/28.
//
#include "include/Lexer.h"

using namespace std;

vector<Word> words;
map<string, TOKEN_CATEGORY> keyWord;

Lexer::Lexer(const char *FILE_IN, const char *FILE_OUT) {
    this->in = fopen(FILE_IN, "r");
    this->out = fopen(FILE_OUT, "w");
    if (!in) {
        printf("Cannot open input file!");
        exit(1);
    }
    this->line = 1;
    this->initKeyWord();
}

Lexer::~Lexer() {
    fclose(in);
    fclose(out);
}

int Lexer::getNext(Word &w) {
    bool flag = true;
    Word word;
    while (flag) {
        flag = false;
        char c = fgetc(in);
        string str;
        while (isspace(c)) {
            if (c == '\n') ++(this->line);
            c = fgetc(in);
        }
        if (c == EOF) {
            return -1;
        }
        if (isalpha(c) || c == '_') {
            do {
                str += c;
                c = fgetc(in);
            } while (isalnum(c) || c == '_');
            ungetc(c, in);

            map<string, TOKEN_CATEGORY>::iterator iterator = keyWord.find(str);
            if (iterator == keyWord.end()) {
                word.category = IDENFR;
            } else {
                word.category = iterator->second;
            }
        } else if (this->isSingle(c)) {
            if (c == '/') {
                c = fgetc(in);
                if (c == '/') {
                    while ((c = fgetc(in)) != '\n');
                    this->line++;
                    flag = true;
                    continue;
                } else if (c == '*') {
                    int d = fgetc(in), e = fgetc(in);
                    while (!(d == '*' && e == '/')) {
                        if (d == '\n') this->line++;
                        d = e;
                        e = fgetc(in);
                    }
                    flag = true;
                    continue;
                } else {
                    ungetc(c, in);
                    c = '/';
                }
            }
            str += c;
            word.category = keyWord[str];
        } else if (isdigit(c)) {
            do {
                str += c;
                c = fgetc(in);
            } while (isdigit(c));

            ungetc(c, in);
            word.category = INTCON;
        } else if (c == '\"') {
            do {
                str += c;
                c = fgetc(in);
            } while (c != '\"');
            str += c;
            word.category = STRCON;
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
        word.line = line;
        word.raw = str;
    }
#ifdef LexerPrint
    cout << tokenName[word.category] << " " << word.raw << " " << word.line << endl;
//    fprintf(out, "%s %s\n", tokenName[word.category], word.raw.c_str());
#endif
    words.push_back(word);
    w = word;
    return w.category;
}

void Lexer::program() {
    Word word;
    while (getNext(word) != -1);
    /*
    char c;
    while ((c = fgetc(in)) != EOF) {
        string str;
        Word word;
        while (isspace(c)) {
            if (c == '\n') ++(this->line);
            c = fgetc(in);
        }
        if (c == EOF) {
            break;
        }
        if (isalpha(c) || c == '_') {
            do {
                str += c;
                c = fgetc(in);
            } while (isalnum(c) || c == '_');
            ungetc(c, in);

            map<string, TOKEN_CATEGORY>::iterator iterator = keyWord.find(str);
            if (iterator == keyWord.end()) {
                word.category = IDENFR;
            } else {
                word.category = iterator->second;
            }
        } else if (isSingle(c)) {
            if (c == '/') {
                c = fgetc(in);
                if (c == '/') {
                    while ((c = fgetc(in)) != '\n');
                    this->line++;
                    continue;
                } else if (c == '*') {
                    int d = fgetc(in), e = fgetc(in);
                    while (!(d == '*' && e == '/')) {
                        if (d == '\n') this->line++;
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
            word.category = keyWord[str];
        } else if (isdigit(c)) {
            do {
                str += c;
                c = fgetc(in);
            } while (isdigit(c));

            ungetc(c, in);
            word.category = INTCON;
        } else if (c == '\"') {
            do {
                str += c;
                c = fgetc(in);
            } while (c != '\"');
            str += c;
            word.category = STRCON;
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
        word.line = line;
        word.raw = str;
//        cout << tokenName[word.category] << " " << str << endl;
//        fprintf(out, "%s %s\n", tokenName[word.category], str.c_str());
        words.push_back(word);
    }
     */
}

void Lexer::initKeyWord() {
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

bool Lexer::isSingle(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == ':' || c == ';' || c == ','
        || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '%')
        return true;
    else return false;
}