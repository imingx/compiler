//
// Created by gmm on 2021/10/21.
//

#include "include/ErrorHandler.h"

vector<PIS> errors;
OldSymbolTable symbolTable;

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

    vector<shared_ptr<ConstExpAST>> &constExps = varDef->constExps;
    for (int i = 0; i < constExps.size(); ++i) {
        handleConstExp(constExps[i]);
    }

    if (varDef->initVal != nullptr) {
        handleInitVal(varDef->initVal);
    }

    OldVarSym t(identName, CurLevel, dimension, CurType.top(), false);
    symbolTable.Var.push_back(t);
}

void ErrorHandler::handleVarDecl(shared_ptr<VarDeclAST> &varDecl) {
    CurType.push(varDecl->btype->category);

    for (int i = 0; i < varDecl->varDefs.size(); ++i) {
        handleVarDef(varDecl->varDefs[i]);
    }
    CurType.pop();
}

int ErrorHandler::handleAddExp(shared_ptr<AddExpAST> &addExp) {
    vector<shared_ptr<MulExpAST>> &mulExps = addExp->mulExps;
    int d = 0; //维度
    for (int i = 0; i < mulExps.size(); ++i) {
        d = handleMulExp(mulExps[i]);
    }
    return d;
}

int ErrorHandler::handleMulExp(shared_ptr<MulExpAST> &mulExp) {
    vector<shared_ptr<UnaryExpAST>> &unaryExps = mulExp->unaryExps;
    int d = 0;
    for (int i = 0; i < unaryExps.size(); ++i) {
        d = handleUnaryExp(unaryExps[i]);
    }
    return d;
}

void ErrorHandler::handleFuncRParams(shared_ptr<FuncRParamsAST> &funcRParams, vector<int> &dimension) {
    for (int i = 0; i < funcRParams->exps.size(); ++i) {
        int d = handleExp(funcRParams->exps[i]);
        dimension.push_back(d);
    }
    return;
}


int ErrorHandler::handleUnaryExp(shared_ptr<UnaryExpAST> &unaryExp) {
    int d = 0;
    switch (unaryExp->type) {
        case 1: {
            d = handlePrimaryExp(unaryExp->primaryExp);
        }
            break;
        case 2: {
            string name = unaryExp->name;
            bool flag = false;
            int index = 0;
            for (index = 0; index < symbolTable.Func.size(); ++index) {
                if (symbolTable.Func[index].name == name) {
                    flag = true;
                    break;
                }
            }
            if (flag) {
                OldFuncSym t = symbolTable.Func[index];
                int parameterNum = t.parameterNum;
                int nowParameterNum = 0;
                if (unaryExp->funcRParams != nullptr) {
                    nowParameterNum = unaryExp->funcRParams->exps.size();
                }
                if (parameterNum != nowParameterNum) {
                    errors.push_back({unaryExp->line, "d"});
                    printf("%d d, 参数不匹配------\n", unaryExp->line);
                    break;
                }

                if (nowParameterNum != 0) {
                    vector<int> dimension;
                    handleFuncRParams(unaryExp->funcRParams, dimension);
                    for (int i = 0; i < dimension.size(); ++i) {
                        if (dimension[i] != t.parameters[i].dimension) {
                            errors.push_back({unaryExp->line, "e"});
                            printf("%d e, 参数类型不匹配------\n", unaryExp->line);
                            break;
                        }
                    }
                }
                if (t.type == INTTK) {
                    d = 0;
                } else {
                    d = -1;
                }
            } else {
                //没有
                errors.push_back({unaryExp->line, "c"});
                printf("%d c, 调用未定义函数-------\n", unaryExp->line);
            }
        }
            break;
        case 3: {
            d = handleUnaryExp(unaryExp->unaryExp);
        }
            break;
    }
    return d;
}

int ErrorHandler::handlePrimaryExp(shared_ptr<PrimaryExpAST> &primaryExp) {
    int d = 0;
    switch (primaryExp->type) {
        case 1: {
            shared_ptr<ExpAST> t = dynamic_pointer_cast<ExpAST>(primaryExp->item);
            d = handleExp(t);
        }
            break;
        case 2: {
            shared_ptr<LValAST> t = dynamic_pointer_cast<LValAST>(primaryExp->item);
            bool null;
            d = handleLVal(t, null);
        }
            break;
        case 3: {
            d = 0;
        }
            break;
    }
    return d;
}

void ErrorHandler::handleRelExp(shared_ptr<RelExpAST> &relExp) {
    vector<shared_ptr<AddExpAST>> &addExps = relExp->addExps;
    for (int i = 0; i < addExps.size(); ++i) {
        handleAddExp(addExps[i]);
    }
}

void ErrorHandler::handleEqExp(shared_ptr<EqExpAST> &eqExp) {
    vector<shared_ptr<RelExpAST>> &relExps = eqExp->relExps;
    for (int i = 0; i < relExps.size(); ++i) {
        handleRelExp(relExps[i]);
    }
}

void ErrorHandler::handleLAndExp(shared_ptr<LAndExpAST> &lAndExp) {
    vector<shared_ptr<EqExpAST>> &eqExps = lAndExp->eqExps;
    for (int i = 0; i < eqExps.size(); ++i) {
        handleEqExp(eqExps[i]);
    }
}

void ErrorHandler::handleLOrExp(shared_ptr<LOrExpAST> &lOrExp) {
    vector<shared_ptr<LAndExpAST>> &lAndExps = lOrExp->lAndExps;
    for (int i = 0; i < lAndExps.size(); ++i) {
        handleLAndExp(lAndExps[i]);
    }
}

void ErrorHandler::handleConstExp(shared_ptr<ConstExpAST> &constExp) {
    handleAddExp(constExp->addExp);
}

void ErrorHandler::handleCond(shared_ptr<CondAST> &cond) {
    handleLOrExp(cond->lOrExp);
}

void ErrorHandler::handleInitVal(shared_ptr<InitValAST> &initVal) {
    if (initVal->exp != nullptr) {
        handleExp(initVal->exp);
    } else {
        vector<shared_ptr<InitValAST>> &initVals = initVal->initVals;
        for (int i = 0; i < initVals.size(); ++i) {
            handleInitVal(initVals[i]);
        }
    }
}

void ErrorHandler::handleConstINitVal(shared_ptr<ConstInitValAST> &constInitVal) {
    if (constInitVal->constExp != nullptr) {
        handleConstExp(constInitVal->constExp);
    } else {
        vector<shared_ptr<ConstInitValAST>> &constInitVals = constInitVal->constInitVals;
        for (int i = 0; i < constInitVals.size(); ++i) {
            handleConstINitVal(constInitVals[i]);
        }
    }
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

    vector<shared_ptr<ConstExpAST>> &constExps = constDef->constExps;
    for (int i = 0; i < constExps.size(); ++i) {
        handleConstExp(constExps[i]);
    }

    handleConstINitVal(constDef->constInitVal);

    OldConSym t(identName, CurLevel, dimension, CurType.top(), true);
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

void ErrorHandler::handleFuncFParam(shared_ptr<FuncFParamAST> &funcFParam, vector<OldVarSym> &parameters) {
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

    vector<shared_ptr<ConstExpAST>> &constExps = funcFParam->constExps;
    for (int i = 0; i < constExps.size(); ++i) {
        if (i != 0) {
            handleConstExp(constExps[i]);
        }
    }

    OldVarSym t(paramName, CurLevel, dimension, CurType.top(), false);
    symbolTable.Var.push_back(t);
    parameters.push_back(t);
    CurType.pop();
}

int ErrorHandler::handleLVal(shared_ptr<LValAST> &lVal, bool &ans) {

    int d = 0;

    string name = lVal->name;
    int line = lVal->line;
    bool flag = false;
    bool isCon = false;

    int i, j;
    int level = -1;
    for (i = symbolTable.Var.size() - 1; i >= 0; --i) {
        if (symbolTable.Var[i].name == name) {
            flag = true;
            level = symbolTable.Var[i].level;
            break;
        }
    }
    for (j = symbolTable.Con.size() - 1; j >= 0; --j) {
        if (symbolTable.Con[j].name == name) {
            if (symbolTable.Con[j].level > level) {
                flag = true;
                isCon = true;
                level = symbolTable.Con[j].level;
            }
            break;
        }
    }
//    for (i = 0; i < symbolTable.Var.size(); ++i) {
//        if (symbolTable.Var[i].name == name) {
//            flag = true;
//            break;
//        }
//    }
//    for (j = 0; j < symbolTable.Con.size(); ++j) {
//        if (symbolTable.Con[j].name == name) {
//            flag = true;
//            isCon = true;
//            break;
//        }
//    }
    if (flag) {
        if (isCon) {
            ans = true;
        } else {
            ans = false;
        }
    } else {
        errors.push_back({line, "c"});
        printf("%d c, 未定义名字-------\n", line);
    }

    vector<shared_ptr<ExpAST>> exps = lVal->exps;
    for (int k = 0; k < exps.size(); ++k) {
        handleExp(exps[k]);
    }

    int originDim = 0;
    int lValDim = lVal->exps.size();
    if (flag) {
        if (isCon) {
            originDim = symbolTable.Con[j].dimension;
        } else {
            originDim = symbolTable.Var[i].dimension;
        }
        return originDim - lValDim;
    }
    return -2;
}

bool ErrorHandler::handlePrintf(const string &formatString, const int num) {
    int total = 0;
    for (int i = 0; i < formatString.size(); ++i) {
        if (formatString[i] == '%') {
            if (formatString[i + 1] == 'd') {
                total++;
                i++;
            }
        }
    }
    if (total != num) {
        return false;
    } else {
        return true;
    }
}


int ErrorHandler::handleExp(shared_ptr<ExpAST> &exp) {
    return handleAddExp(exp->addExp);
}

void ErrorHandler::handleStmt(shared_ptr<StmtAST> &stmt) {

    switch (stmt->type) {
        case 1: {     //1 lval = exp
            //line is 0, represent it is right.
            //line is not 0, represent the line number.
            bool ans = false;
            handleLVal(stmt->lVal, ans);
            if (ans) {
                errors.push_back({stmt->line, "h"});
                printf("%d h, 改变了常量的值-------\n", stmt->line);
            }
            handleExp(stmt->exp);
        }
            break;
        case 2: {     //2 [exp];
            if (stmt->expSingle != nullptr)
                handleExp(stmt->expSingle);
        }
            break;
        case 3: {     //3 block
            CurLevel++;
            handleBLock(stmt->block, false);
            CurLevel--;
        }
            break;
        case 4: {     //4 if
            handleCond(stmt->condIf);
            handleStmt(stmt->stmtIf);
            if (stmt->stmtElse != nullptr)
                handleStmt(stmt->stmtElse);
        }
            break;
        case 5: { //while
            isloop.push(true);
            handleCond(stmt->condWhile);
            handleStmt(stmt->stmtWhile);
            isloop.pop();
        }
            break;
        case 6: { //6 break or continue;
            if (!isloop.top()) {
                errors.push_back({stmt->line, "m"});
                printf("%d m, 非循环块用了break,continue---------\n", stmt->line);
            }
        }
            break;
        case 7: { // return
            bool isNull = false;
            if (stmt->expReturn == nullptr)
                isNull = true;
            if (returnType == VOIDTK && isNull == false) {
                errors.push_back({stmt->line, "f"});
                printf("%d f, void函数有返回值---------\n", stmt->line);
            }
        }
            break;
        case 8: {   //8 lval = getint();
            bool ans = false;
            handleLVal(stmt->lValGetint, ans);
            if (ans) {
                errors.push_back({stmt->line, "h"});
                printf("%d h, 改变了常量的值-------\n", stmt->line);
            }
        }
            break;
        case 9: { //printf
            //是否相等
            bool equal = handlePrintf(stmt->formatString, stmt->expsPrintf.size());
            if (!equal) {
                errors.push_back({stmt->line, "l"});
                printf("%d l, printf参数不匹配-----\n", stmt->line);
            }
            for (int i = 0; i < stmt->expsPrintf.size(); ++i) {
                handleExp(stmt->expsPrintf[i]);
            }
        }
            break;
    }
}

void ErrorHandler::handleBlockItem(shared_ptr<BlockItemAST> &blockItem) {
    if (blockItem->type == 0) { //decl
        shared_ptr<DeclAST> decl = dynamic_pointer_cast<DeclAST>(blockItem->item);
        handleDecl(decl);
    } else { //stmt
        shared_ptr<StmtAST> stmt = dynamic_pointer_cast<StmtAST>(blockItem->item);
        handleStmt(stmt);
    }
}

void ErrorHandler::handleBLock(shared_ptr<BlockAST> &block, bool isfunc) {
    vector<shared_ptr<BlockItemAST>> &blockItems = block->blockItems;
    for (int i = 0; i < blockItems.size(); ++i) {
        handleBlockItem(blockItems[i]);
    }
    //符号表
    for (auto i = symbolTable.Var.begin(); i != symbolTable.Var.end(); ++i) {
        if (i->level == CurLevel) {
            symbolTable.Var.erase(i);
            --i;
        }
    }
    for (auto i = symbolTable.Con.begin(); i != symbolTable.Con.end(); ++i) {
        if (i->level == CurLevel) {
            symbolTable.Con.erase(i);
            --i;
        }
    }
    //判断return
    if (blockItems.empty()) {
        if (returnType == INTTK && isfunc) {
            errors.push_back({block->line, "g"});
            printf("%d g, intFunc没有return---\n", block->line);
        }
        return;
    }
    shared_ptr<BlockItemAST> end = blockItems.back();

    if (end->type == 0) {
        if (returnType == INTTK && isfunc) {
            //没有return.
            errors.push_back({block->line, "g"});
            printf("%d g, intFunc没有return---\n", block->line);
        }
    } else {
        shared_ptr<StmtAST> stmt = dynamic_pointer_cast<StmtAST>(end->item);

        if (returnType == INTTK && stmt->type != 7 && isfunc) {
            //没有return.
            errors.push_back({block->line, "g"});
            printf("%d g, intFunc没有return---\n", block->line);
        }
    }

}

void ErrorHandler::handleFunc(shared_ptr<FuncDefAST> &funcDef) {
    CurType.push(funcDef->funcType->category);
    returnType = funcDef->funcType->category;

    isloop.push(false);
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

    vector<OldVarSym> parameters;
    shared_ptr<FuncFParamsAST> &funcFParams = funcDef->funcFParams;
    int parameterNum = 0;
    if (funcFParams != nullptr) {
        parameterNum = funcFParams->funcFParams.size();
        for (int i = 0; i < parameterNum; ++i) {
            handleFuncFParam(funcFParams->funcFParams[i], parameters);
        }
    }


    OldFuncSym t(funcName, parameterNum, parameters, CurType.top());
    symbolTable.Func.push_back(t);

    shared_ptr<BlockAST> &block = funcDef->block;
    handleBLock(block, true);

    CurType.pop();
    isloop.pop();
    CurLevel--;
}

void ErrorHandler::handleMainDef(shared_ptr<MainFuncDefAST> &mainFunc) {
    string name = mainFunc->name;
    CurType.push(mainFunc->type);
    returnType = mainFunc->type;

    isloop.push(false);
    int line = mainFunc->line;
    for (int i = 0; i < symbolTable.Func.size(); ++i) {
        if (symbolTable.Func[i].name == name) {
            errors.push_back({line, "b"}); //名字重复定义
            printf("%d b, mainfunc名字重复定义------\n", line);
            break;
        }
    }
    CurLevel++;

    shared_ptr<BlockAST> &block = mainFunc->block;
    handleBLock(block, true);

    vector<OldVarSym> parameters;
    OldFuncSym t(name, 0, parameters, CurType.top());
    symbolTable.Func.push_back(t);
    CurLevel--;
    CurType.pop();
    isloop.pop();
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
    if (compUnitAst->main != nullptr) {
        shared_ptr<MainFuncDefAST> &mainFunc = compUnitAst->main;
        handleMainDef(mainFunc);
    }
}


ErrorHandler::ErrorHandler(const char *ERROR, shared_ptr<CompUnitAST> &compUnitAst) : compUnitAst(compUnitAst) {
    this->error = fopen(ERROR, "w");
    this->CurLevel = 0; // 全局默认为零层
    this->CurFunc = "";
    this->isloop.push(false);
}

ErrorHandler::~ErrorHandler() {
    fclose(this->error);
    isloop.pop();
}

void ErrorHandler::print() {
    sort(errors.begin(), errors.end());
    errors.erase(unique(errors.begin(), errors.end()), errors.end());
    for (auto i: errors) {
        fprintf(error, "%d %s\n", i.first, i.second.c_str());
    }
}