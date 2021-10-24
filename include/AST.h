//
// Created by gmm on 2021/10/18.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <bits/stdc++.h>
#include "Lexer.h"
#include "TYPE.h"

using namespace std;

class AST;

class ExpAST;

class CompUnitAST;

class DeclAST;

class FuncDefAST;

class MainFuncDefAST;

class ConstDeclAST;

class VarDeclAST;

class BTypeAST;

class ConstDefAST;

class FuncDefAST;

class LValAST;

class NumberAST;

class VarDefAST;

class InitValAST;

class FuncTypeAST;

class FuncFParamsAST;

class FuncFParamAST;

class BlockAST;

class ConstExpAST;

class BlockItemAST;

class CondAST;

class StmtAST;

class LOrExpAST;

class LAndExpAST;

class EqExpAST;

class RelExpAST;

class AddExpAST;

class AST {
public:
    virtual ~AST() = default;
};

class FuncTypeAST : public AST {
public:
    int category;
    string type;
public:
    FuncTypeAST(const int category, const string &type) : category(category), type(type) {}
};

class FuncFParamAST : public AST {
public:
    shared_ptr<BTypeAST> bType;
    string name;
    int line;
    vector<shared_ptr<ConstExpAST>> constExps;
public:
    FuncFParamAST(shared_ptr<BTypeAST> bType,
                  const string &name,
                  vector<shared_ptr<ConstExpAST>> constExps, const int line) : bType(move(bType)),
                                                                               name(name), constExps(move(constExps)),
                                                                               line(line) {}
};

class FuncFParamsAST : public AST {
public:
    vector<shared_ptr<FuncFParamAST>> funcFParams;
public:
    FuncFParamsAST(vector<shared_ptr<FuncFParamAST>> funcFParams) : funcFParams(move(funcFParams)) {}
};


class LOrExpAST : public AST {
public:
    vector<shared_ptr<LAndExpAST>> lAndExps;
public:
    LOrExpAST(vector<shared_ptr<LAndExpAST>> lAndExps) : lAndExps(move(lAndExps)) {}
};

class LAndExpAST : public AST {
public:
    vector<shared_ptr<EqExpAST>> eqExps;
public:
    LAndExpAST(vector<shared_ptr<EqExpAST>> eqExps) : eqExps(move(eqExps)) {}
};

class EqExpAST : public AST {
public:
    vector<int> symbols; // ==  !=
    vector<shared_ptr<RelExpAST>> relExps;
public:
    EqExpAST(vector<int> symbols,
             vector<shared_ptr<RelExpAST>> relExps) : symbols(symbols), relExps(move(relExps)) {}
};

class RelExpAST : public AST {
public:
    vector<int> symbols; // < > <= >=
    vector<shared_ptr<AddExpAST>> addExps;
public:
    RelExpAST(vector<int> symbols,
              vector<shared_ptr<AddExpAST>> addExps) : symbols(symbols),
                                                       addExps(move(addExps)) {}
};

class CondAST : public AST {
public:
    shared_ptr<LOrExpAST> lOrExp;
public:
    CondAST(shared_ptr<LOrExpAST> lOrExp) : lOrExp(move(lOrExp)) {}
};

class StmtAST : public AST {
public:
    int type;

    //1 lval = exp
    shared_ptr<LValAST> lVal;
    shared_ptr<ExpAST> exp;

    //2 [exp];
    shared_ptr<ExpAST> expSingle;

    //3 block
    shared_ptr<BlockAST> block;

    //4 if
    shared_ptr<CondAST> condIf;
    shared_ptr<StmtAST> stmtIf;
    shared_ptr<StmtAST> stmtElse;

    //5 while
    shared_ptr<CondAST> condWhile;
    shared_ptr<StmtAST> stmtWhile;

    //6 break or continue;
    int category;

    //7 return;
    shared_ptr<ExpAST> expReturn;
    int line;

    //8 lval = getint();
    shared_ptr<LValAST> lValGetint;

    //9 printf
    string formatString;
    vector<shared_ptr<ExpAST>> expsPrintf;

public:
    StmtAST(shared_ptr<LValAST> lVal,
            shared_ptr<ExpAST> exp, const int line) : lVal(move(lVal)), exp(move(exp)), type(1), line(line) {}

    StmtAST(shared_ptr<ExpAST> expSingle) : expSingle(move(expSingle)), type(2) {}

    StmtAST(shared_ptr<BlockAST> block) : block(move(block)), type(3) {}

    StmtAST(shared_ptr<CondAST> condIf,
            shared_ptr<StmtAST> stmtIf,
            shared_ptr<StmtAST> stmtElse) : condIf(move(condIf)), stmtIf(move(stmtIf)),
                                            stmtElse(move(stmtElse)), type(4) {}

    StmtAST(shared_ptr<CondAST> condWhile,
            shared_ptr<StmtAST> stmtWhile) : condWhile(move(condWhile)), stmtWhile(move(stmtWhile)), type(5) {}

    StmtAST(const int category, const int line) : category(category), type(6), line(line) {}

    StmtAST(shared_ptr<ExpAST> expReturn, string _return, const int line) : expReturn(move(expReturn)), line(line),
                                                                            type(7) {}

    StmtAST(shared_ptr<LValAST> lValGetint, const int line) : lValGetint(move(lValGetint)), type(8), line(line) {}

    StmtAST(const string &formatString, vector<shared_ptr<ExpAST>> expsPrintf, const int line) :
            formatString(move(formatString)), expsPrintf(move(expsPrintf)), type(9), line(line) {}
};

class BlockItemAST : public AST {
public:
    shared_ptr<AST> item;
    int type; //0 is decl, 1 is stmt
public:
    BlockItemAST(shared_ptr<AST> item, const int type) : item(move(item)), type(type) {}
};

class BlockAST : public AST {
public:
    vector<shared_ptr<BlockItemAST>> blockItems;
    int line;
public:
    BlockAST(vector<shared_ptr<BlockItemAST>> blockItems, const int line) : blockItems(move(blockItems)), line(line) {}
};

class FuncDefAST : public AST {
public:
    shared_ptr<FuncTypeAST> funcType;
    string name;
    int line;
    shared_ptr<FuncFParamsAST> funcFParams;
    shared_ptr<BlockAST> block;
public:
    FuncDefAST(shared_ptr<FuncTypeAST> funcType,
               const string &name,
               shared_ptr<FuncFParamsAST> funcFParams,
               shared_ptr<BlockAST> block, const int line) : funcType(move(funcType)),
                                                             name(name), funcFParams(move(funcFParams)),
                                                             block(move(block)), line(line) {}
};

class DeclAST : public AST {
public:
    shared_ptr<AST> decl;
    int type; // 0 is var, 1 is const
public:
    DeclAST(shared_ptr<AST> decl, const int type) : decl(move(decl)), type(type) {}
};


class ConstInitValAST : public AST {
public:
    shared_ptr<ConstExpAST> constExp;
    vector<shared_ptr<ConstInitValAST>> constInitVals;
public:
    ConstInitValAST(shared_ptr<ConstExpAST> constExp) : constExp(move(constExp)) {}

    ConstInitValAST(vector<shared_ptr<ConstInitValAST>> constInitVals) : constInitVals(move(constInitVals)),
                                                                         constExp(nullptr) {}
};

class PrimaryExpAST : public AST {
public:
    shared_ptr<AST> item;
public:
    PrimaryExpAST(shared_ptr<AST> item) : item(move(item)) {}
};

class LValAST : public AST {
public:
    string name;
    int line;
    vector<shared_ptr<ExpAST>> exps;
public:
    LValAST(const string &name, vector<shared_ptr<ExpAST>> exps, const int line) : name(name), exps(move(exps)),
                                                                                   line(line) {}
};

class NumberAST : public AST {
public:
    long long number;
    string raw;
public:
    NumberAST(const long long number, const string &raw) : number(number),
                                                           raw(raw) {}
};

class UnaryOpAST : public AST {
public:
    int symbol; // + - !
public:
    UnaryOpAST(int symbol) : symbol(symbol) {}
};


class FuncRParamsAST : public AST {
public:
    vector<shared_ptr<ExpAST>> exps;
public:
    FuncRParamsAST(vector<shared_ptr<ExpAST>> exps) : exps(move(exps)) {}
};

class UnaryExpAST : public AST {
public:
    int type; // 1 is primaryExp, 2 is ident(funcRparams), 3 is UnaryOp UnaryExp
    shared_ptr<PrimaryExpAST> primaryExp;
    string name;
    int line;
    shared_ptr<FuncRParamsAST> funcRParams;
    shared_ptr<UnaryOpAST> unaryOp;
    shared_ptr<UnaryExpAST> unaryExp;
public:
    int getType() { return type; }

    UnaryExpAST(shared_ptr<PrimaryExpAST> primaryExp) : primaryExp(move(primaryExp)), type(1) {}

    UnaryExpAST(const string &name, shared_ptr<FuncRParamsAST> funcRParams, const int line) :
            name(name), funcRParams(move(funcRParams)), type(2), line(line) {}

    UnaryExpAST(shared_ptr<UnaryOpAST> unaryOp, shared_ptr<UnaryExpAST> unaryExp) :
            unaryOp(move(unaryOp)), unaryExp(move(unaryExp)), type(3) {}
};

class MulExpAST : public AST {
public:
    vector<shared_ptr<UnaryExpAST>> unaryExps;
    vector<int> symbol; // * % /
public:
    MulExpAST(vector<int> symbol, vector<shared_ptr<UnaryExpAST>> unaryExps) :
            symbol(symbol), unaryExps(move(unaryExps)) {}
};

class AddExpAST : public AST {
public:
    vector<int> symbol; // + 或 -
    vector<shared_ptr<MulExpAST>> mulExps;
public:
    AddExpAST(vector<int> symbol, vector<shared_ptr<MulExpAST>> mulExps) :
            symbol(symbol), mulExps(move(mulExps)) {}
};

class ExpAST : public AST {
public:
    shared_ptr<AddExpAST> addExp;
public:
    ExpAST(shared_ptr<AddExpAST> addExp) : addExp(move(addExp)) {}
};

class ConstExpAST : public AST {
public:
    shared_ptr<AddExpAST> addExp;
public:
    ConstExpAST(shared_ptr<AddExpAST> addExp) : addExp(move(addExp)) {}
};

class ConstDefAST : public AST {
public:
    string name;
    int line;
    vector<shared_ptr<ConstExpAST>> constExps;
    shared_ptr<ConstInitValAST> constInitVal;
public:
    ConstDefAST(const string &name, vector<shared_ptr<ConstExpAST>> constExps,
                shared_ptr<ConstInitValAST> constInitVal, const int line) : name(name), constExps(move(constExps)),
                                                                            constInitVal(move(constInitVal)),
                                                                            line(line) {}
};

class BTypeAST : public AST {
public:
    int category;
    string type;
public:
    BTypeAST(const int category, const string &type) : category(category), type(type) {}
};

class ConstDeclAST : public AST {
public:
    string symbol; // const
    shared_ptr<BTypeAST> btype;
    vector<shared_ptr<ConstDefAST>> constdefs;
public:
    ConstDeclAST(const string &symbol, shared_ptr<BTypeAST> btype,
                 vector<shared_ptr<ConstDefAST>> constdefs) : symbol(symbol), btype(move(btype)),
                                                              constdefs(move(constdefs)) {}
};

class InitValAST : public AST {
public:
    shared_ptr<ExpAST> exp;
    vector<shared_ptr<InitValAST>> initVals;
public:
    InitValAST(shared_ptr<ExpAST> exp) : exp(move(exp)) {}

    InitValAST(vector<shared_ptr<InitValAST>> initVals) : initVals(move(initVals)), exp(nullptr) {}
};

class VarDefAST : public AST {
public:
    string name;
    int line;
    vector<shared_ptr<ConstExpAST>> constExps;
    shared_ptr<InitValAST> initVal;
public:
    VarDefAST(const string &name, vector<shared_ptr<ConstExpAST>> constExps,
              shared_ptr<InitValAST> initVal, const int line) : name(name),
                                                                constExps(move(constExps)), initVal(move(initVal)),
                                                                line(line) {}
};

class VarDeclAST : public AST {
public:
    shared_ptr<BTypeAST> btype;
    vector<shared_ptr<VarDefAST>> varDefs;
public:
    VarDeclAST(shared_ptr<BTypeAST> btype,
               vector<shared_ptr<VarDefAST>> varDefs) : btype(move(btype)), varDefs(move(varDefs)) {}
};

class MainFuncDefAST : public AST {
public:
    string funcType;
    int type;
    string name;
    shared_ptr<BlockAST> block;
    int line;
public:
    MainFuncDefAST(shared_ptr<BlockAST> block, const int line) : block(move(block)), name("main"), funcType("int"),
                                                                 type(INTTK), line(line) {}
};

class CompUnitAST : public AST {
public:
    vector<shared_ptr<DeclAST>> decls;
    vector<shared_ptr<FuncDefAST>> funcs;
    shared_ptr<MainFuncDefAST> main;
public:
    CompUnitAST(vector<shared_ptr<DeclAST>> decls, vector<shared_ptr<FuncDefAST>> funcs,
                shared_ptr<MainFuncDefAST> main) : decls(move(decls)), funcs(move(funcs)), main(move(main)) {}
};

#endif //COMPILER_AST_H
