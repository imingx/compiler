//
// Created by gmm on 2021/10/21.
//

#ifndef COMPILER_ERRORHANDLER_H
#define COMPILER_ERRORHANDLER_H

#include <bits/stdc++.h>
#include "TYPE.h"
#include "AST.h"
#include "SymbolTable.h"

using namespace std;

extern vector<PIS> errors;
extern SymbolTable symbolTable;

class ErrorHandler {
private:
    int CurLevel;

    stack<int> CurType; //voidtk inttk

    string CurFunc; //目前所处的函数

    bool isloop;

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
};

#endif //COMPILER_ERRORHANDLER_H
