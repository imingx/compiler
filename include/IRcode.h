//
// Created by gmm on 2021/11/22.
//

#ifndef COMPILER_IRCODE_H
#define COMPILER_IRCODE_H

#include<bits/stdc++.h>
#include "AST.h"

using namespace std;


class IRcode;

class VarSym;

class Obj;

class SymTable;

struct StringData;

extern vector<shared_ptr<IRcode>> IRCodeList;
extern SymTable symTable;
extern vector<StringData> stringData;

struct StringData {
    string label;
    string content;
};

class FuncSym {
public:
    string name;
    int paraNum;
    SymbolType returnType;
    vector<shared_ptr<VarSym>> exps;

    FuncSym(string name, int paraNum, vector<shared_ptr<VarSym>> &exps, SymbolType returnType) {
        this->name = name;
        this->paraNum = paraNum;
        this->exps = exps;
        this->returnType = returnType;
    }

    FuncSym(string name, SymbolType type) {
        this->name = name;
        this->returnType = type;
        this->paraNum = 0;
    }
};

class VarSym {
public:
    string name;
    int dim;
    SymbolType type;
    int level;
    vector<shared_ptr<Obj>> exps; //int a[exp][exp]
    shared_ptr<Obj> value;   //int a = 10;
    int offset;
    int needSpace;

    void setValue(shared_ptr<Obj> value) {
        this->value = value;
    }

    void setOffsetAndNeedSpace(int offset, int needSpace) {
        this->offset = offset;
        this->needSpace = needSpace;
    }

    VarSym(string name, int dim, vector<shared_ptr<Obj>> &exps, SymbolType type, int level) {
        this->name = name;
        this->dim = dim;
        this->type = type;
        this->exps = exps;
        this->level = level;
    }

    VarSym(string name, int dim, vector<shared_ptr<Obj>> &exps, SymbolType type, int level, shared_ptr<Obj> &value) {
        this->name = name;
        this->dim = dim;
        this->type = type;
        this->exps = exps;
        this->level = level;
        this->value = value;
    }

    VarSym(string name, int dim, SymbolType type, int level, shared_ptr<Obj> &value) {
        this->name = name;
        this->dim = dim;
        this->type = type;
        this->level = level;
        this->value = value;
    }

    VarSym(string name, int dim, SymbolType type, int level) {
        this->name = name;
        this->dim = dim;
        this->type = type;
        this->level = level;
    }
};

class SymTable {
public:
    vector<shared_ptr<VarSym>> Var;
    vector<shared_ptr<FuncSym>> Func;
};

class Obj {
public:
    int branch;
    //0 is nothing
    //1 is type(int), void, arr
    //if arr, str[][tx]
    //2 is str
    //3 is str[tx]
    //4 is str[1]  str[num]
    //5 is num
    string str;
    string index;
    SymbolType type;
    int num;
    shared_ptr<VarSym> var;
    shared_ptr<FuncSym> func;

    string p();

    void setVar(shared_ptr<VarSym> &var) {
        this->var = var;
    }

    void setFunc(shared_ptr<FuncSym> &func) {
        this->func = func;
    }

    Obj(SymbolType type) {
        this->branch = 1;
        this->type = type;
    }

    Obj(string str) {
        this->branch = 2;
        this->str = str;
    }

    Obj(string str, shared_ptr<VarSym> &var) {
        this->branch = 2;
        this->str = str;
        this->var = var;
    }

    Obj(string str, shared_ptr<FuncSym> &func) {
        this->branch = 2;
        this->str = str;
        this->func = func;
    }

    Obj(string str, string index) {
        this->branch = 3;
        this->str = str;
        this->index = index;
    }

    Obj(string str, int num) {
        this->branch = 4;
        this->str = str;
        this->num = num;
    }

    Obj(string str, shared_ptr<Obj> &obj) {
        if (obj->branch == 2) {
            this->branch = 3;
            this->index = obj->str;
        } else if (obj->branch == 5) {
            this->branch = 4;
            this->num = obj->num;
        }
        this->str = str;
        if (obj->type == ARR) {
            this->type = ARR;
        }
    }

    Obj(string str, int num, shared_ptr<VarSym> &var) {
        //para int a[1]
        this->var = var;
        this->branch = 4;
        this->num = num;
        this->str = str;
        this->type = ARR;
    }

    Obj(string str, shared_ptr<Obj> &obj, shared_ptr<VarSym> &var) {
        this->var = var;
        if (obj->branch == 2) {
            this->branch = 3;
            this->index = obj->str;
        } else if (obj->branch == 5) {
            this->branch = 4;
            this->num = obj->num;
        }
        this->str = str;
        if (obj->type == ARR) {
            this->type = ARR;
        }
    }

    Obj(int num) {
        this->branch = 5;
        this->num = num;
    }

    Obj() : branch(0) {}

    Obj(Obj &t) {
        this->branch = t.branch;
        this->str = t.str;
        this->index = t.index;
        this->type = t.type;
        this->num = t.num;
    }
};

class IRcode {
public:
    OperatorType op;
    shared_ptr<Obj> obj[3];
//    shared_ptr<IRcode *> next = nullptr;
public:
    void print(); //输出
    void PRINTOBJ(int num) { //输出几个obj
        for (int i = 0; i < num; ++i) {
            if (obj[i] != nullptr)
                cout << obj[i]->p() << " ";
        }
        cout << endl;
    }

    IRcode(OperatorType op, shared_ptr<Obj> obj[3]) {
        this->op = op;
        this->obj[0] = obj[0];
        this->obj[1] = obj[1];
        this->obj[2] = obj[2];
    }
};

class IRcodeMaker {
private:
    shared_ptr<CompUnitAST> &compUnitAst;
    int NowLevel;
public:
    shared_ptr<Obj> newtemp();

    void print();

    IRcodeMaker(shared_ptr<CompUnitAST> &compUnitAst);

    void program();

    void programDecl(shared_ptr<DeclAST> &decl);

    void programConstDecl(shared_ptr<ConstDeclAST> &constDecl);

    void programVarDecl(shared_ptr<VarDeclAST> &varDecl);

    void programConstDef(shared_ptr<ConstDefAST> &constDef);

    void programVarDef(shared_ptr<VarDefAST> &varDef);

    void programFunc(shared_ptr<FuncDefAST> &funcDef);

    void programFuncFParam(shared_ptr<FuncFParamAST> &funcFParam, vector<shared_ptr<VarSym>> &exps);

    void programBlockItem(shared_ptr<BlockItemAST> &blockItem);

    void programStmt(shared_ptr<StmtAST> &stmt);

    void programMainDef(shared_ptr<MainFuncDefAST> &mainFunc);

    void programBLock(shared_ptr<BlockAST> &block, bool isfunc);

    shared_ptr<Obj> programLVal(shared_ptr<LValAST> &lVal);

    void programPrintf(const string &formatString, vector<shared_ptr<ExpAST>> exps);

    void programCond(shared_ptr<CondAST> &cond);

    shared_ptr<Obj> programExp(shared_ptr<ExpAST> &exp);

    shared_ptr<Obj> programConstExp(shared_ptr<ConstExpAST> &constExp);

    void programConstInitVal(shared_ptr<ConstInitValAST> &constInitVal, shared_ptr<Obj> obj[3], int *count);

    void programInitVal(shared_ptr<InitValAST> &initVal, shared_ptr<Obj> obj[3], int *count);

    shared_ptr<Obj> programAddExp(shared_ptr<AddExpAST> &addExp);

    shared_ptr<Obj> programMulExp(shared_ptr<MulExpAST> &mulExp);

    shared_ptr<Obj> programUnaryExp(shared_ptr<UnaryExpAST> &unaryExp);

    shared_ptr<Obj> programPrimaryExp(shared_ptr<PrimaryExpAST> &primaryExp);

    void programRelExp(shared_ptr<RelExpAST> &relExp);

    void programEqExp(shared_ptr<EqExpAST> &eqExp);

    void programLAndExp(shared_ptr<LAndExpAST> &lAndExp);

    void programLOrExp(shared_ptr<LOrExpAST> &lOrExp);

    void programFuncRParams(shared_ptr<FuncRParamsAST> &funcRParams, vector<int> &dimension);
};


#endif //COMPILER_IRCODE_H
