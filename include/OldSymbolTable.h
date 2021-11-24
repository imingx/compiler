//
// Created by gmm on 2021/10/18.
//

#ifndef COMPILER_OLDSYMBOLTABLE_H
#define COMPILER_OLDSYMBOLTABLE_H

#include <bits/stdc++.h>

using namespace std;




// 上面是中间代码生成的符号表
//--------------------------------------------------------------------
// 下面是错误处理使用的符号表

class OldSym {
public:
    string name; //名字
    int type; //变量的类型，或函数的返回值类型
    bool isCon; // 0 is var, 1 is con;
    OldSym(const string &name, const int type, const bool isCon) : name(name), isCon(isCon), type(type) {}
};

// 变量
class OldVarSym : public OldSym {
public:
    int level;   // 作用域
    int dimension;

    OldVarSym(const string &name, const int level, const int dimension, const int type, const bool isCon) :
            level(level), dimension(dimension), OldSym(name, type, isCon) {}
};

// 常量
class OldConSym : public OldSym {
public:
    int level;   // 作用域
    int dimension; //数组的维度，

    OldConSym(const string &name, const int level, const int dimension, const int type, const bool isCon) :
            level(level), dimension(dimension), OldSym(name, type, isCon) {}
};

// 函数
class OldFuncSym : public OldSym {
public:
    vector<OldVarSym> parameters;
    int parameterNum;

    OldFuncSym(const string &name, const int parameterNum, const vector<OldVarSym> &parameters, const int type) :
            OldSym(name, type, false), parameters(parameters), parameterNum(parameterNum) {}
};


class OldSymbolTable {
public:
    vector<OldConSym> Con;
    vector<OldVarSym> Var;
    vector<OldFuncSym> Func;
};


#endif //COMPILER_OLDSYMBOLTABLE_H
