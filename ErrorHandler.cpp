//
// Created by gmm on 2021/10/21.
//

#include "include/ErrorHandler.h"

vector<PIS> errors;
SymbolTable symbolTable;

void ErrorHandler::handleVarDef(shared_ptr<VarDefAST> &varDef) {
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

void ErrorHandler::handleVarDecl(shared_ptr<VarDeclAST> &varDecl) {
    CurType.push(varDecl->btype->category);

    for (int i = 0; i < varDecl->varDefs.size(); ++i) {
        handleVarDef(varDecl->varDefs[i]);
    }
    CurType.pop();
}

void ErrorHandler::handleConstDef(shared_ptr<ConstDefAST> &constDef) {
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

void ErrorHandler::handleConstDecl(shared_ptr<ConstDeclAST> &constDecl) {

    CurType.push(constDecl->btype->category);

    for (int i = 0; i < constDecl->constdefs.size(); ++i) {
        handleConstDef(constDecl->constdefs[i]);
    }
    CurType.pop();
}

void ErrorHandler::handleDecl(shared_ptr<DeclAST> &decl) {
    if (decl->type == 0) // var
    {
        shared_ptr<VarDeclAST> p = dynamic_pointer_cast<VarDeclAST>(decl->decl);
        //dynamic_pointer_cast 可以转换shared_ptr为派生类的shared_ptr
        //dynamic_cast 可以转换指针为派生类的指针。
        //static_* 相较于dynamic，可以实现动态类型检查，看是否是该基类的派生类。
        handleVarDecl(p);
    } else { // const
        shared_ptr<ConstDeclAST> p = dynamic_pointer_cast<ConstDeclAST>(decl->decl);
        //dynamic_pointer_cast 可以转换shared_ptr为派生类的shared_ptr
        //dynamic_cast 可以转换指针为派生类的指针。
        //static_* 相较于dynamic，可以实现动态类型检查，看是否是该基类的派生类。
        handleConstDecl(p);
    }
}

void ErrorHandler::handleFuncFParam(shared_ptr<FuncFParamAST> &funcFParam, vector<VarSym> &parameters) {
    CurType.push(funcFParam->bType->category);
    string paramName = funcFParam->name;
    int line = funcFParam->line;
    int dimension = funcFParam->constExps.size();

    for (int j = 0; j < symbolTable.Var.size(); ++j) {
        if (symbolTable.Var[j].name == paramName && symbolTable.Var[j].level == CurLevel) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, param名字重复定义------\n", line);
            CurType.pop();
            return;
        }
    }
    CurType.pop();
    VarSym t(paramName, CurLevel, dimension);
    symbolTable.Var.push_back(t);
    parameters.push_back(t);
}

void ErrorHandler::handleStmt(shared_ptr<StmtAST> & stmt) {

}

void ErrorHandler::handleBlockItem(shared_ptr<BlockItemAST> &blockItem) {
    if (blockItem->type == 0) { //decl
        shared_ptr<DeclAST> decl = dynamic_pointer_cast<DeclAST>(blockItem);
        handleDecl(decl);
    } else { //stmt
        shared_ptr<StmtAST> stmt = dynamic_pointer_cast<StmtAST>(blockItem);
        handleStmt(stmt);
    }
}

void ErrorHandler::handleFunc(shared_ptr<FuncDefAST> &funcDef) {
    CurType.push(funcDef->funcType->category);
    string funcName = funcDef->name;
    int line = funcDef->line;
    for (int i = 0; i < symbolTable.Func.size(); ++i) {
        if (symbolTable.Func[i].name == funcName) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, func名字重复定义------\n", line);
            break;
        }
    }
    CurLevel++;

    vector<VarSym>parameters;
    shared_ptr<FuncFParamsAST> &funcFParams = funcDef->funcFParams;
    int parameterNum = funcFParams->funcFParams.size();
    for (int i = 0; i < parameterNum; ++i) {
        handleFuncFParam(funcFParams->funcFParams[i], parameters);
    }

    vector<shared_ptr<BlockItemAST>> &blockItems = funcDef->block->blockItems;
    for (int i = 0; i < blockItems.size(); ++i) {
        handleBlockItem(blockItems[i]);
    }

    CurType.pop();
    CurLevel--;
    FuncSym t(funcName, parameterNum, parameters);
    symbolTable.Func.push_back(t);
}

void ErrorHandler::handleMainDef(shared_ptr<MainFuncDefAST> &mainFunc) {
    string name = mainFunc->name;
    CurType.push(mainFunc->type);
    int line = mainFunc->line;
    for (int i = 0; i < symbolTable.Func.size(); ++i) {
        if (symbolTable.Func[i].name == name) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, mainfunc名字重复定义------\n", line);
            break;
        }
    }
    CurLevel ++;

    vector<shared_ptr<BlockItemAST>> &blockItems = mainFunc->block->blockItems;
    for (int i = 0; i < blockItems.size(); ++i) {
        handleBlockItem(blockItems[i]);
    }

    CurLevel --;
    CurType.pop();
    vector<VarSym>parameters;
    FuncSym t(name, 0, parameters);
    symbolTable.Func.push_back(t);
}

void ErrorHandler::program() {
    vector<shared_ptr<DeclAST>> &decls = (compUnitAst->decls);
    for (int i = 0; i < decls.size(); ++i) {
        handleDecl(decls[i]);
    }
    vector<shared_ptr<FuncDefAST>> &funcs = compUnitAst->funcs;
    for (int i = 0; i < funcs.size(); ++i) {
        handleFunc(funcs[i]);
    }
    shared_ptr<MainFuncDefAST> &mainFunc = compUnitAst->main;
    handleMainDef(mainFunc);
}


ErrorHandler::ErrorHandler(const char *ERROR, shared_ptr<CompUnitAST> &compUnitAst) : compUnitAst(compUnitAst) {
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