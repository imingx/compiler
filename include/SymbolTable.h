//
// Created by gmm on 2021/10/18.
//

#ifndef COMPILER_SYMBOLTABLE_H
#define COMPILER_SYMBOLTABLE_H

#include <bits/stdc++.h>

using namespace std;

class Sym {
public:
    string name; //名字
    int type; //变量的类型，或函数的返回值类型
    Sym(const string &name) : name(name) {}
};

class VarSym : public Sym {
public:
    int level;   // 作用域
    int dimension;

    VarSym(const string &name, const int level, const int dimension) : level(level), dimension(dimension), Sym(name) {}
};

class ConSym : public Sym {
public:
    int level;   // 作用域
    int dimension; //数组的维度，

    ConSym(const string &name, const int level, const int dimension) : level(level), dimension(dimension), Sym(name) {}
};


class FuncSym : public Sym {
public:
    vector<VarSym> parameters;
    int parameterNum;
    FuncSym(const string &name, const int parameterNum, const vector<VarSym> &parameters) : Sym(name),
                                                                                            parameters(parameters),
                                                                                            parameterNum(parameterNum) {}
};


class SymbolTable {
public:
//    map<string, ConSym> Con;
//    map<string, VarSym> Var;
//    map<string, FuncSym> Func;
    vector<ConSym> Con;
    vector<VarSym> Var;
    vector<FuncSym> Func;
};


#endif //COMPILER_SYMBOLTABLE_H
