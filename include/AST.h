//
// Created by gmm on 2021/10/18.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <bits/stdc++.h>
#include "Lexer.h"

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
    int category;
    string type;
public:
    FuncTypeAST(const int category, const string &type) : category(category), type(type) {}
};

class FuncFParamAST : public AST {
    unique_ptr<BTypeAST> bType;
    string name;
    vector<unique_ptr<ConstExpAST>> constExps;
public:
    FuncFParamAST(unique_ptr<BTypeAST> bType,
                  const string &name,
                  vector<unique_ptr<ConstExpAST>> constExps) : bType(move(bType)),
                                                               name(name), constExps(move(constExps)) {}
};

class FuncFParamsAST : public AST {
    vector<unique_ptr<FuncFParamAST>> funcFParams;
public:
    FuncFParamsAST(vector<unique_ptr<FuncFParamAST>> funcFParams) : funcFParams(move(funcFParams)) {}
};


class LOrExpAST : public AST {
    vector<unique_ptr<LAndExpAST>> lAndExps;
public:
    LOrExpAST(vector<unique_ptr<LAndExpAST>> lAndExps) : lAndExps(move(lAndExps)) {}
};

class LAndExpAST : public AST {
    vector<unique_ptr<EqExpAST>> eqExps;
public:
    LAndExpAST(vector<unique_ptr<EqExpAST>> eqExps) : eqExps(move(eqExps)) {}
};

class EqExpAST : public AST {
    vector<int> symbols; // ==  !=
    vector<unique_ptr<RelExpAST>> relExps;
public:
    EqExpAST(vector<int> symbols,
             vector<unique_ptr<RelExpAST>> relExps) : symbols(symbols), relExps(move(relExps)) {}
};

class RelExpAST : public AST {
    vector<int> symbols; // < > <= >=
    vector<unique_ptr<AddExpAST>> addExps;
public:
    RelExpAST(vector<int> symbols,
              vector<unique_ptr<AddExpAST>> addExps) : symbols(symbols),
                                                       addExps(move(addExps)) {}
};

class CondAST : public AST {
    unique_ptr<LOrExpAST> lOrExp;
public:
    CondAST(unique_ptr<LOrExpAST> lOrExp) : lOrExp(move(lOrExp)) {}
};

class StmtAST : public AST {
    int type;

    //1 lval = exp
    unique_ptr<LValAST> lVal;
    unique_ptr<ExpAST> exp;

    //2 [exp];
    unique_ptr<ExpAST> expSingle;

    //3 block
    unique_ptr<BlockAST> block;

    //4 if
    unique_ptr<CondAST> condIf;
    unique_ptr<StmtAST> stmtIf;
    unique_ptr<StmtAST> stmtElse;

    //5 while
    unique_ptr<CondAST> condWhile;
    unique_ptr<StmtAST> stmtWhile;

    //6 break or continue;
    int category;

    //7 return;
    unique_ptr<ExpAST> expReturn;

    //8 lval = getint();
    unique_ptr<LValAST> lValGetint;

    //9 printf
    string formatString;
    vector<unique_ptr<ExpAST>> expsPrintf;

public:
    StmtAST(unique_ptr<LValAST> lVal,
            unique_ptr<ExpAST> exp) : lVal(move(lVal)), exp(move(exp)), type(1) {}

    StmtAST(unique_ptr<ExpAST> expSingle) : expSingle(move(expSingle)), type(2) {}

    StmtAST(unique_ptr<BlockAST> block) : block(move(block)), type(3) {}

    StmtAST(unique_ptr<CondAST> condIf,
            unique_ptr<StmtAST> stmtIf,
            unique_ptr<StmtAST> stmtElse) : condIf(move(condIf)), stmtIf(move(stmtIf)),
                                            stmtElse(move(stmtElse)), type(4) {}

    StmtAST(unique_ptr<CondAST> condWhile,
            unique_ptr<StmtAST> stmtWhile) : condWhile(move(condWhile)), stmtWhile(move(stmtWhile)), type(5) {}

    StmtAST(const int category) : category(category), type(6) {}

    StmtAST(unique_ptr<ExpAST> expReturn, string _return) : expReturn(move(expReturn)), type(7) {}

    StmtAST(unique_ptr<LValAST> lValGetint) : lValGetint(move(lValGetint)), type(8) {}

    StmtAST(const string &formatString, vector<unique_ptr<ExpAST>> expsPrintf) :
            formatString(move(formatString)), expsPrintf(move(expsPrintf)), type(9) {}
};

class BlockItemAST : public AST {
    unique_ptr<AST> item;
public:
    BlockItemAST(unique_ptr<AST> item) : item(move(item)) {}
};

class BlockAST : public AST {
    vector<unique_ptr<BlockItemAST>> blockItems;
public:
    BlockAST(vector<unique_ptr<BlockItemAST>> blockItems) : blockItems(move(blockItems)) {}
};

class FuncDefAST : public AST {
    unique_ptr<FuncTypeAST> funcType;
    string name;
    unique_ptr<FuncFParamsAST> funcFParams;
    unique_ptr<BlockAST> block;
public:
    FuncDefAST(unique_ptr<FuncTypeAST> funcType,
               const string &name,
               unique_ptr<FuncFParamsAST> funcFParams,
               unique_ptr<BlockAST> block) : funcType(move(funcType)),
                                             name(name), funcFParams(move(funcFParams)), block(move(block)) {}
};

class DeclAST : public AST {
    unique_ptr<AST> decl;
public:
    DeclAST(unique_ptr<AST> decl) : decl(move(decl)) {}
};


class ConstInitValAST : public AST {
    unique_ptr<ConstExpAST> constExp;
    vector<unique_ptr<ConstInitValAST>> constInitVals;
public:
    ConstInitValAST(unique_ptr<ConstExpAST> constExp) : constExp(move(constExp)) {}

    ConstInitValAST(vector<unique_ptr<ConstInitValAST>> constInitVals) : constInitVals(move(constInitVals)),
                                                                         constExp(nullptr) {}
};

class PrimaryExpAST : public AST {
    unique_ptr<AST> item;
public:
    PrimaryExpAST(unique_ptr<AST> item) : item(move(item)) {}
};

class LValAST : public AST {
    string name;
    vector<unique_ptr<ExpAST>> exps;
public:
    LValAST(const string &name, vector<unique_ptr<ExpAST>> exps) : name(name), exps(move(exps)) {}
};

class NumberAST : public AST {
    long long number;
    string raw;
public:
    NumberAST(const long long number, const string &raw) : number(number),
                                                           raw(raw) {}
};

class UnaryOpAST : public AST {
    int symbol; // + - !
public:
    UnaryOpAST(int symbol) : symbol(symbol) {}
};


class FuncRParamsAST : public AST {
    vector<unique_ptr<ExpAST>> exps;
public:
    FuncRParamsAST(vector<unique_ptr<ExpAST>> exps) : exps(move(exps)) {}
};

class UnaryExpAST : public AST {
    int type; // 1 is primaryExp, 2 is ident(funcRparams), 3 is UnaryOp UnaryExp
    unique_ptr<PrimaryExpAST> primaryExp;
    string name;
    unique_ptr<FuncRParamsAST> funcRParams;
    unique_ptr<UnaryOpAST> unaryOp;
    unique_ptr<UnaryExpAST> unaryExp;
public:
    int getType() { return type; }

    UnaryExpAST(unique_ptr<PrimaryExpAST> primaryExp) : primaryExp(move(primaryExp)), type(1) {}

    UnaryExpAST(const string &name, unique_ptr<FuncRParamsAST> funcRParams) :
            name(name), funcRParams(move(funcRParams)), type(2) {}

    UnaryExpAST(unique_ptr<UnaryOpAST> unaryOp, unique_ptr<UnaryExpAST> unaryExp) :
            unaryOp(move(unaryOp)), unaryExp(move(unaryExp)), type(3) {}
};

class MulExpAST : public AST {
    vector<unique_ptr<UnaryExpAST>> unaryExps;
    vector<int> symbol; // * % /
public:
    MulExpAST(vector<int> symbol, vector<unique_ptr<UnaryExpAST>> unaryExps) :
            symbol(symbol), unaryExps(move(unaryExps)) {}
};

class AddExpAST : public AST {
    vector<int> symbol; // + 或 -
    vector<unique_ptr<MulExpAST>> mulExps;
public:
    AddExpAST(vector<int> symbol, vector<unique_ptr<MulExpAST>> mulExps) :
            symbol(symbol), mulExps(move(mulExps)) {}
};

class ExpAST : public AST {
    unique_ptr<AddExpAST> addExp;
public:
    ExpAST(unique_ptr<AddExpAST> addExp) : addExp(move(addExp)) {}
};

class ConstExpAST : public AST {
    unique_ptr<AddExpAST> addExp;
public:
    ConstExpAST(unique_ptr<AddExpAST> addExp) : addExp(move(addExp)) {}
};

class ConstDefAST : public AST {
    string name;
    vector<unique_ptr<ConstExpAST>> constExps;
    unique_ptr<ConstInitValAST> constInitVal;
public:
    ConstDefAST(const string &name, vector<unique_ptr<ConstExpAST>> constExps,
                unique_ptr<ConstInitValAST> constInitVal) : name(name), constExps(move(constExps)),
                                                            constInitVal(move(constInitVal)) {}
};

class BTypeAST : public AST {
    int category;
    string type;
public:
    BTypeAST(const int category, const string &type) : category(category), type(type) {}
};

class ConstDeclAST : public AST {
    string symbol; // const
    unique_ptr<BTypeAST> btype;
    vector<unique_ptr<ConstDefAST>> constdefs;
public:
    ConstDeclAST(const string &symbol, unique_ptr<BTypeAST> btype,
                 vector<unique_ptr<ConstDefAST>> constdefs) : symbol(symbol), btype(move(btype)),
                                                              constdefs(move(constdefs)) {}
};

class InitValAST : public AST {
    unique_ptr<ExpAST> exp;
    vector<unique_ptr<InitValAST>> initVals;
public:
    InitValAST(unique_ptr<ExpAST> exp) : exp(move(exp)) {}

    InitValAST(vector<unique_ptr<InitValAST>> initVals) : initVals(move(initVals)), exp(nullptr) {}
};

class VarDefAST : public AST {
    string name;
    vector<unique_ptr<ConstExpAST>> constExps;
    unique_ptr<InitValAST> initVal;
public:
    VarDefAST(const string &name, vector<unique_ptr<ConstExpAST>> constExps,
              unique_ptr<InitValAST> initVal) : name(name),
                                                constExps(move(constExps)), initVal(move(initVal)) {}
};

class VarDeclAST : public AST {
    unique_ptr<BTypeAST> btype;
    vector<unique_ptr<VarDefAST>> varDefs;
public:
    VarDeclAST(unique_ptr<BTypeAST> btype,
               vector<unique_ptr<VarDefAST>> varDefs) : btype(move(btype)), varDefs(move(varDefs)) {}
};

class MainFuncDefAST : public AST {
    string funcType;
    string name;
    unique_ptr<BlockAST> block;
public:
    MainFuncDefAST(unique_ptr<BlockAST> block) : block(move(block)), name("main"), funcType("int") {}
};

class CompUnitAST : public AST {
    vector<unique_ptr<DeclAST>> decls;
    vector<unique_ptr<FuncDefAST>> funcs;
    unique_ptr<MainFuncDefAST> main;
public:
    CompUnitAST(vector<unique_ptr<DeclAST>> decls, vector<unique_ptr<FuncDefAST>> funcs,
                unique_ptr<MainFuncDefAST> main) : decls(move(decls)), funcs(move(funcs)), main(move(main)) {}
};


#endif //COMPILER_AST_H
