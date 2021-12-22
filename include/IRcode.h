//
// Created by gmm on 2021/11/22.
//

#ifndef COMPILER_IRCODE_H
#define COMPILER_IRCODE_H

#include<bits/stdc++.h>
#include "AST.h"
#include "type.h"

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
    int offset;

    void setOffset(int offset) {
        this->offset = offset;
    }

    FuncSym(string name, int paraNum, vector<shared_ptr<VarSym>> &exps, SymbolType returnType) {
        this->name = name;
        this->paraNum = paraNum;
        this->exps = exps;
        this->returnType = returnType;
//        this->offset = offset;
    }

    FuncSym(string name, SymbolType type) {
        this->name = name;
        this->returnType = type;
        this->paraNum = 0;
//        this->offset = offset;
    }
};

class VarSym {
public:
    string name;        //变量名
    int dim;            //变量维度
    SymbolType type;    //变量类型
    int level;          //所处层级
    int originLevel;  //我由于设置非常量数组都是全局，而符号表需要删去一些东西，所以需要记一下。
    vector<int> exps;   //每一层的值   int a[2][3];
    //    vector<shared_ptr<Obj>> exps; //每一个表达式。int a[exp][exp]
    shared_ptr<Obj> value;      //int a = 10;
    //int a = b; 这个需要到程序运行才知道。
    //如果是数组：
    vector<shared_ptr<Obj>> values; // int a[10] = {1,2,3,4};
    int offset;
    int needSpace;
    bool isConst;  //是否是const，但是无用，因为其index不一定是const

    bool isArrParam; //是否是数组参数

    int count; //引用次数
    bool whetherDefiniteValue;

    void setValue(int value) {
        this->value = make_shared<Obj>(value);
    }

    void setValue(shared_ptr<Obj> value) {
        this->value = value;
    }

    void setOffsetAndNeedSpace(int offset, int needSpace) {
        this->offset = offset;
        this->needSpace = needSpace;
    }

    //赋值名字，维度，表达式，类型，层级
    VarSym(bool isConst, string name, int dim, vector<int> &exps, SymbolType type, int level) {
        this->isConst = isConst;
        this->name = name;
        this->dim = dim;
        this->type = type;
        this->exps = exps;
        this->originLevel = this->level = level;
        int len = 1;
        for (int i = 0; i < exps.size(); ++i) {
            len *= exps[i];
        }
        isArrParam = false;
        values.reserve(len);
    }

    VarSym(bool isConst, string name, int dim, SymbolType type, int level, shared_ptr<Obj> &value) {
        this->isConst = isConst;
        this->name = name;
        this->dim = dim;
        this->type = type;
        this->originLevel = this->level = level;
        this->value = value;
        isArrParam = false;
    }

    VarSym(bool isConst, string name, int dim, SymbolType type, int level) {
        this->isConst = isConst;
        this->name = name;
        this->dim = dim;
        this->type = type;
        this->originLevel = this->level = level;
        this->value = make_shared<Obj>(0);
        isArrParam = false;
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
    //1 is type:int, void
    //2 is name
    //3 is name[index]
    //4 is name[1]  name[num_index]
    //5 is num
    string name;

    shared_ptr<Obj> index;

    int num_index;

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

    Obj(string name) {
        this->branch = 2;
        this->name = name;
    }

    Obj(string name, shared_ptr<VarSym> &var) {
        this->branch = 2;
        this->name = name;
        this->var = var;
    }

    Obj(string name, shared_ptr<FuncSym> &func) {
        this->branch = 2;
        this->name = name;
        this->func = func;
    }

    Obj(string name, shared_ptr<Obj> &index) {
        this->branch = 3;
        this->name = name;
        this->index = index;
    }

    Obj(string name, int num_index) {
        this->branch = 4;
        this->name = name;
        this->num_index = num_index;
    }

    Obj(string name, shared_ptr<Obj> &obj, shared_ptr<VarSym> &var) {
        this->var = var;
        if (obj->branch == 2 || obj->branch == 3 || obj->branch == 4) {
            this->branch = 3;
            this->index = obj;
        } else if (obj->branch == 5) {
            this->branch = 4;
            this->num_index = obj->num;
        }
        this->name = name;
    }

    Obj(string name, int num_index, shared_ptr<VarSym> &var) {
        this->branch = 4;
        this->name = name;
        this->num_index = num_index;
        this->var = var;
    }

    Obj(int num) {
        this->branch = 5;
        this->num = num;
    }

    Obj() : branch(0) {}

    Obj(Obj &t) {
        this->num_index = t.num_index;
        this->name = t.name;
        this->branch = t.branch;
        this->index = t.index;
        this->type = t.type;
        this->num = t.num;
        this->var = t.var;
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

    string newLabel(SymbolType type);

    shared_ptr<Obj> newValue();

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

    shared_ptr<Obj> programLVal(shared_ptr<LValAST> &lVal, bool isAssign);

    void programPrintf(const string &formatString, vector<shared_ptr<ExpAST>> exps);

    shared_ptr<Obj> programExp(shared_ptr<ExpAST> &exp);

    shared_ptr<Obj> programConstExp(shared_ptr<ConstExpAST> &constExp);

    void programConstInitVal(shared_ptr<ConstInitValAST> &constInitVal, shared_ptr<Obj> obj[3]);

    void programInitVal(shared_ptr<InitValAST> &initVal, shared_ptr<Obj> arr[3]);

    shared_ptr<Obj> programAddExp(shared_ptr<AddExpAST> &addExp);

    shared_ptr<Obj> programMulExp(shared_ptr<MulExpAST> &mulExp);

    shared_ptr<Obj> programUnaryExp(shared_ptr<UnaryExpAST> &unaryExp);

    shared_ptr<Obj> programPrimaryExp(shared_ptr<PrimaryExpAST> &primaryExp);

    shared_ptr<Obj> programRelExp(shared_ptr<RelExpAST> &relExp, string &label, string &Else, bool haveEq);

    shared_ptr<Obj> programEqExp(shared_ptr<EqExpAST> &eqExp, string &label, string &Else);

    void programLAndExp(shared_ptr<LAndExpAST> &lAndExp, string &label, string &Else);

    void programLOrExp(shared_ptr<LOrExpAST> &lOrExp, string &label, string &Else);

    void programCond(shared_ptr<CondAST> &cond, string &label, string &Else);

    //    void programFuncRParams(shared_ptr<FuncRParamsAST> &funcRParams, vector<int> &dimension);
};


#endif //COMPILER_IRCODE_H
