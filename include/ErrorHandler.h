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

    unique_ptr<CompUnitAST> &compUnitAst;
    FILE *error;
public:
    ErrorHandler(const char *ERROR, unique_ptr<CompUnitAST> &compUnitAst);

    ~ErrorHandler();

    void program();

    void print();

    //handle error
    void handleDecl(unique_ptr<DeclAST> &decl);

    void handleConstDecl(unique_ptr<AST> &constDecl);

    void handleVarDecl(unique_ptr<AST> &varDecl);

    void handleConstDef(unique_ptr<ConstDefAST> &constDef);

    void handleVarDef(unique_ptr<VarDefAST> &varDef);

    void handleFunc(unique_ptr<FuncDefAST> & funcDef);

    void handleFuncFParam(unique_ptr<FuncFParamAST> & funcFParam);

    void handleBlockItem(unique_ptr<BlockItemAST> & blockItem);
};

#endif //COMPILER_ERRORHANDLER_H
