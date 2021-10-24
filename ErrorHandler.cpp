//
// Created by gmm on 2021/10/21.
//

#include "include/ErrorHandler.h"

vector<PIS> errors;
SymbolTable symbolTable;

void ErrorHandler::handleVarDef(unique_ptr<VarDefAST> &varDef) {
    string identName = varDef->name;
    int dimension = varDef->constExps.size();
    int line = varDef->line;
    for (int i = 0; i < symbolTable.Con.size(); ++i) {
        if (symbolTable.Con[i].name == identName && symbolTable.Con[i].level == CurLevel) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, var名字重复定义------\n", line);
        }
    }
    for (int i = 0; i < symbolTable.Var.size(); ++i) {
        if (symbolTable.Var[i].name == identName && symbolTable.Var[i].level == CurLevel) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, var名字重复定义------\n", line);
        }
    }
    VarSym t(identName, CurLevel, dimension);
    symbolTable.Var.push_back(t);
}

void ErrorHandler::handleVarDecl(unique_ptr<AST> &varDecl) {
    VarDeclAST *ptr = dynamic_cast<VarDeclAST *>(varDecl.get());
    CurType.push(ptr->btype->category);

    for (int i = 0; i < ptr->varDefs.size(); ++i) {
        handleVarDef(ptr->varDefs[i]);
    }
    CurType.pop();
}

void ErrorHandler::handleConstDef(unique_ptr<ConstDefAST> &constDef) {
    string identName = constDef->name;
    int dimension = constDef->constExps.size();
    int line = constDef->line;
    //已经定义过了
    for (int i = 0; i < symbolTable.Con.size(); ++i) {
        if (symbolTable.Con[i].name == identName && symbolTable.Con[i].level == CurLevel) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, const名字重复定义------\n", line);
            return;
        }
    }
    for (int i = 0; i < symbolTable.Var.size(); ++i) {
        if (symbolTable.Var[i].name == identName && symbolTable.Var[i].level == CurLevel) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, const名字重复定义------\n", line);
            return;
        }
    }
    ConSym t(identName, CurLevel, dimension);
    symbolTable.Con.push_back(t);
}

void ErrorHandler::handleConstDecl(unique_ptr<AST> &constDecl) {
    ConstDeclAST *ptr = dynamic_cast<ConstDeclAST *>(constDecl.get());
    CurType.push(ptr->btype->category);

    for (int i = 0; i < ptr->constdefs.size(); ++i) {
        handleConstDef(ptr->constdefs[i]);
    }
    CurType.pop();
}

void ErrorHandler::handleDecl(unique_ptr<DeclAST> &decl) {
    if (decl->type == 0) // var
    {
        handleVarDecl(decl->decl);
    } else { // const
        handleConstDecl(decl->decl);
    }
}

void ErrorHandler::handleFuncFParam(unique_ptr<FuncFParamAST> & funcFParam) {
    CurType.push(funcFParam->bType->category);
    string paramName = funcFParam->name;
    int line = funcFParam->line;
    int dimension = funcFParam->constExps.size();

    for (int j = 0; j < symbolTable.Var.size(); ++j) {
        if (symbolTable.Var[j].name == paramName && symbolTable.Var[j].level == CurLevel){
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, param名字重复定义------\n", line);
            CurType.pop();
            return;
        }
    }
    VarSym t(paramName, CurLevel, dimension);
    symbolTable.Var.push_back(t);
}

void ErrorHandler::handleBlockItem(unique_ptr<BlockItemAST> & blockItem) {
    if (blockItem->type == 0) { //decl
        handleDecl();
    } else { //stmt

    }
}

void ErrorHandler::handleFunc(unique_ptr<FuncDefAST> &funcDef) {
    CurType.push(funcDef->funcType->category);
    string funcName = funcDef->name;
    int line = funcDef->line;
    for (int i = 0; i < symbolTable.Func.size(); ++i) {
        if (symbolTable.Func[i].name == funcName) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, func名字重复定义------\n", line);
            CurType.pop();
            break;
        }
    }
    CurLevel ++;

    unique_ptr<FuncFParamsAST> &funcFParams = funcDef->funcFParams;
    for (int i = 0; i < funcFParams->funcFParams.size(); ++i) {
        handleFuncFParam(funcFParams->funcFParams[i]);
    }

    vector<unique_ptr<BlockItemAST>> &blockItems = funcDef->block->blockItems;
    for (int i = 0; i < blockItems.size(); ++i) {
        handleBlockItem(blockItems[i]);
    }

    CurType.pop();
    CurLevel --;
    symbolTable.Func.push_back(?);
}

void ErrorHandler::program() {
    vector<unique_ptr<DeclAST>> &decls = (compUnitAst->decls);
    for (int i = 0; i < decls.size(); ++i) {
        handleDecl(decls[i]);
    }
    vector<unique_ptr<FuncDefAST>> &funcs = compUnitAst->funcs;
    for (int i = 0; i < funcs.size(); ++i) {
        handleFunc(funcs[i]);
    }
}


ErrorHandler::ErrorHandler(const char *ERROR, unique_ptr<CompUnitAST> &compUnitAst) : compUnitAst(compUnitAst) {
    this->error = fopen(ERROR, "w");
    this->CurLevel = 0; // 全局默认为零层
    this->isloop = false;
    this->CurFunc = "";
}

ErrorHandler::~ErrorHandler() {
    fclose(this->error);
}

void ErrorHandler::print() {
    sort(errors.begin(), errors.end());
    errors.erase(unique(errors.begin(), errors.end()), errors.end());
    for (auto i: errors) {
        fprintf(error, "%d %s\n", i.first, i.second.c_str());
    }
}