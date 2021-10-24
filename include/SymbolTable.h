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
    bool isCon; // 0 is var, 1 is con;
    Sym(const string &name, const int type, const bool isCon) : name(name), isCon(isCon), type(type) {}
};

class VarSym : public Sym {
public:
    int level;   // 作用域
    int dimension;

    VarSym(const string &name, const int level, const int dimension, const int type, const bool isCon) : level(level),
                                                                                                         dimension(
                                                                                                                 dimension),
                                                                                                         Sym(name,
                                                                                                             type,
                                                                                                             isCon) {}
};

class ConSym : public Sym {
public:
    int level;   // 作用域
    int dimension; //数组的维度，

    ConSym(const string &name, const int level, const int dimension, const int type, const bool isCon) : level(level),
                                                                                                         dimension(
                                                                                                                 dimension),
                                                                                                         Sym(name,
                                                                                                             type,
                                                                                                             isCon) {}
};


class FuncSym : public Sym {
public:
    vector<VarSym> parameters;
    int parameterNum;

    FuncSym(const string &name, const int parameterNum, const vector<VarSym> &parameters, const int type) : Sym(name,
                                                                                                                type,
                                                                                                                false),
                                                                                                            parameters(
                                                                                                                    parameters),
                                                                                                            parameterNum(
                                                                                                                    parameterNum) {}
};


class SymbolTable {
public:
    vector<ConSym> Con;
    vector<VarSym> Var;
    vector<FuncSym> Func;
};


#endif //COMPILER_SYMBOLTABLE_H
