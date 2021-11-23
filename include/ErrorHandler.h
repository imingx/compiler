//
// Created by gmm on 2021/10/21.
//

#ifndef COMPILER_ERRORHANDLER_H
#define COMPILER_ERRORHANDLER_H

#include <bits/stdc++.h>
#include "type.h"
#include "AST.h"
#include "SymbolTable.h"

using namespace std;

extern vector<PIS> errors;
extern SymbolTable symbolTable;

class ErrorHandler {
private:
    int CurLevel;

    stack<int> CurType; //voidtk, inttk

    string CurFunc; //目前所处的函数
    int returnType;

    stack<bool> isloop;

    shared_ptr<CompUnitAST> &compUnitAst;
    FILE *error;
public:
    ErrorHandler(const char *ERROR, shared_ptr<CompUnitAST> &compUnitAst);

    ~ErrorHandler();

    void program();

    void print();

    //handle error
    void handleDecl(shared_ptr<DeclAST> &decl);

    void handleConstDecl(shared_ptr<ConstDeclAST> &constDecl);

    void handleVarDecl(shared_ptr<VarDeclAST> &varDecl);

    void handleConstDef(shared_ptr<ConstDefAST> &constDef);

    void handleVarDef(shared_ptr<VarDefAST> &varDef);

    void handleFunc(shared_ptr<FuncDefAST> &funcDef);

    void handleFuncFParam(shared_ptr<FuncFParamAST> &funcFParam, vector<VarSym> &parameters);

    void handleBlockItem(shared_ptr<BlockItemAST> &blockItem);

    void handleStmt(shared_ptr<StmtAST> &stmt);

    void handleMainDef(shared_ptr<MainFuncDefAST> &mainFunc);

    void handleBLock(shared_ptr<BlockAST> &block, bool isfunc);

    int handleLVal(shared_ptr<LValAST> &lVal, bool &ans);

    bool handlePrintf(const string &formatString, const int num);

    void handleCond(shared_ptr<CondAST> &cond);

    int handleExp(shared_ptr<ExpAST> &exp);

    void handleConstExp(shared_ptr<ConstExpAST> &constExp);

    void handleConstINitVal(shared_ptr<ConstInitValAST> &constInitVal);

    void handleInitVal(shared_ptr<InitValAST> &initVal);

    int handleAddExp(shared_ptr<AddExpAST> &addExp);

    int handleMulExp(shared_ptr<MulExpAST> &mulExp);

    int handleUnaryExp(shared_ptr<UnaryExpAST> &unaryExp);

    int handlePrimaryExp(shared_ptr<PrimaryExpAST> &primaryExp);

    void handleRelExp(shared_ptr<RelExpAST> & relExp);

    void handleEqExp( shared_ptr<EqExpAST> & eqExp);

    void handleLAndExp( shared_ptr<LAndExpAST> &lAndExp);

    void handleLOrExp( shared_ptr<LOrExpAST> & lOrExp);

    void handleFuncRParams( shared_ptr<FuncRParamsAST> & funcRParams, vector<int> & dimension);

};

#endif //COMPILER_ERRORHANDLER_H
