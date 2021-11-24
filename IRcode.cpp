//
// Created by gmm on 2021/11/22.
//

#include "include/IRcode.h"

int var_offset; //偏移量，.data
stack<SymbolType> defineType;
vector<shared_ptr<IRcode>> IRCodeList;
SymTable symTable;
vector<StringData> stringData;

void IRcodeMaker::print() {
    for (int i = 0; i < IRCodeList.size(); ++i) {
        IRCodeList[i]->print();
    }
}

void IRcode::print() {
    switch (op) {
        case OpVar: //变量定义
            cout << operatorString[op] << " ";
            PRINTOBJ(3);
            // var int b， var int b = 1;
            break;
        case OpArray: //数组定义
            cout << operatorString[op] << " ";
            PRINTOBJ(2);
            break;
        case OpConst: //常数
            cout << operatorString[op] << " ";
            PRINTOBJ(3);
            break;
        case OpPLUS: //加
            cout << obj[0]->p() << " = " << obj[1]->p() << " + " << obj[2]->p() << endl;
            break;
        case OpMINU: // -
            cout << obj[0]->p() << " = " << obj[1]->p() << " - " << obj[2]->p() << endl;
            break;
        case OpMULT: //*
            cout << obj[0]->p() << " = " << obj[1]->p() << " * " << obj[2]->p() << endl;
            break;
        case OpDIV: // /
            cout << obj[0]->p() << " = " << obj[1]->p() << " / " << obj[2]->p() << endl;
            break;
        case OpMOD: // %
            cout << obj[0]->p() << " = " << obj[1]->p() << " % " << obj[2]->p() << endl;
            break;
        case OpAssign: // =
            cout << obj[0]->p() << " = " << obj[1]->p() << endl;
            break;
        case OpPush: // push
            cout << operatorString[op] << " " << obj[0]->p() << endl;
            break;
        case OpCall:
            cout << operatorString[op] << " " << obj[0]->p() << endl;
            break;
        case OpFunc:
            cout << obj[0]->p() << " " << obj[1]->p() << "()" << endl;
            break;
        case OpNot:
            cout << obj[0]->p() << " = !" << obj[1]->p() << endl;
            break;
        case OpPara:
            cout << operatorString[op] << " " << obj[0]->p() << " " << obj[1]->p() << endl;
            break;
        case OpParaArr:
            cout << operatorString[op] << " " << obj[0]->p() << " " << obj[1]->p() << " " <<
                 obj[2]->p() << endl;
            break;
        case OpReturn:
            cout << operatorString[op] << " ";
            PRINTOBJ(3);
            break;
        case OpScanf:
            cout << operatorString[op] << " " << obj[0]->p() << endl;
            break;
        case OpPrint:
            cout << operatorString[op] << " " << obj[0]->p() << endl;
            break;
        case OpPrintInt:
            cout << operatorString[op] << " " << obj[0]->p() << endl;
            break;
        case OpExit:
            cout << operatorString[op] << endl;
            break;
    }
}

string Obj::p() {
    switch (branch) {
        case 0:
            return "";
        case 1:
            return symbolTypeString[type];
        case 2:
            return str;
        case 3:
            if (type == ARR)
                return str + "[][" + index + "]";
            else
                return str + "[" + index + "]";
        case 4:
            if (num == 0x3f3f3f3f)
                return str + "[]";
            else
                return str + "[" + to_string(num) + "]";
        case 5:
            return to_string(num);
    }
    return "";
}

IRcodeMaker::IRcodeMaker(shared_ptr<CompUnitAST> &compUnitAst) : compUnitAst(compUnitAst) {
    var_offset = 0;
    NowLevel = 0;
}

void IRcodeMaker::programConstInitVal(shared_ptr<ConstInitValAST> &constInitVal, shared_ptr<Obj> obj[3], int *count) {
    if (obj[1]->branch == 2) {
        //var int a = 10;
        obj[2] = programConstExp(constInitVal->constExp);
        shared_ptr<IRcode> t = make_shared<IRcode>(OpConst, obj);
        IRCodeList.push_back(t);
    } else {
        if (constInitVal->constExp != nullptr) {
            shared_ptr<Obj> myObj[3] = {make_shared<Obj>(obj[1]->str, *count)};
            myObj[1] = programConstExp(constInitVal->constExp);
            shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, myObj);
            IRCodeList.push_back(t);
            (*count)++;
        } else {
            vector<shared_ptr<ConstInitValAST>> &constInitVals = constInitVal->constInitVals;
            for (int i = 0; i < constInitVals.size(); ++i) {
                programConstInitVal(constInitVals[i], obj, count);
            }
        }
    }

}

void IRcodeMaker::programConstDef(shared_ptr<ConstDefAST> &constDef) {
    string identName = constDef->name; // 名字
    int dimension = constDef->constExps.size(); //维度
    vector<shared_ptr<ConstExpAST>> &constExps = constDef->constExps;
    //插入符号表
    shared_ptr<Obj> before;
    shared_ptr<Obj> now;

    vector<shared_ptr<Obj>> exps;

    for (int i = 0; i < constExps.size(); ++i) {
        before = now;
        now = programConstExp(constExps[i]);

        exps.push_back(now);

        if (i != 0) {
            shared_ptr<Obj> obj[3];
            obj[0] = newtemp();
            obj[1] = before;
            obj[2] = now;
            now = obj[0];
            shared_ptr<IRcode> t = make_shared<IRcode>(OpMULT, obj);
            IRCodeList.push_back(t);
        }
    }
    int count = 0;
    if (constExps.empty()) {
        //a
        shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                  make_shared<Obj>(identName)};
        programConstInitVal(constDef->constInitVal, obj, &count);

    } else {
        //a[];
        shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                  make_shared<Obj>(identName, now)};
        shared_ptr<IRcode> t = make_shared<IRcode>(OpArray, obj);
        IRCodeList.push_back(t);
        programConstInitVal(constDef->constInitVal, obj, &count);
    }

// 插入符号表？
    shared_ptr<VarSym> var;
    if (constExps.empty()) {
        var = make_shared<VarSym>(identName, dimension, defineType.top(), NowLevel);
    } else {
        var = make_shared<VarSym>(identName, dimension, exps, defineType.top(), NowLevel);
    }

    symTable.Var.push_back(var);
}

void IRcodeMaker::programConstDecl(shared_ptr<ConstDeclAST> &constDecl) {
    if (constDecl->btype->category == INTTK) {
        defineType.push(INT);
    }
    for (int i = 0; i < constDecl->constdefs.size(); ++i) {
        programConstDef(constDecl->constdefs[i]);
    }
    defineType.pop();
}

shared_ptr<Obj> IRcodeMaker::programLVal(shared_ptr<LValAST> &lVal) {
    string varName = lVal->name;
    vector<shared_ptr<ExpAST>> &exps = lVal->exps;

    shared_ptr<VarSym> var;

    for (int i = symTable.Var.size() - 1; i >= 0; --i) {
        if (symTable.Var[i]->name.compare(varName) == 0) {
            var = symTable.Var[i];
            break;
        }
    }


    vector<shared_ptr<Obj>> &defineExps = var->exps;

    vector<shared_ptr<Obj>> nowExps;
    for (int i = 0; i < exps.size(); ++i) {
        shared_ptr<Obj> now = programExp(exps[i]);
        nowExps.push_back(now);
    }

    if (exps.empty()) {
        shared_ptr<Obj> obj = make_shared<Obj>(varName);
        return obj;
    } else {

        shared_ptr<Obj> old;
        shared_ptr<Obj> now;

        if (nowExps.size() == 1) {
            now = nowExps[0];
            shared_ptr<Obj> obj = make_shared<Obj>(varName, now);
            return obj;
        }

        for (int i = 0; i < nowExps.size(); ++i) {
            if (i != nowExps.size() - 1) {
                shared_ptr<Obj> mult = nowExps[i];
                for (int j = i + 1; j < nowExps.size(); j++) {
                    shared_ptr<Obj> obj[3] = {newtemp(), mult, defineExps[j]};
                    shared_ptr<IRcode> t = make_shared<IRcode>(OpMULT, obj);
                    IRCodeList.push_back(t);
                    mult = obj[0];
                }
                old = now;
                now = mult;
            } else {
                old = now;
                now = nowExps[i];
            }

            if (i != 0) {
                shared_ptr<Obj> obj[3] = {newtemp(), old, now};
                shared_ptr<IRcode> t = make_shared<IRcode>(OpPLUS, obj);
                IRCodeList.push_back(t);
                old = obj[0];
            }
        }

        shared_ptr<Obj> obj = make_shared<Obj>(varName, old);
        return obj;
    }
}

shared_ptr<Obj> IRcodeMaker::programPrimaryExp(shared_ptr<PrimaryExpAST> &primaryExp) {
    switch (primaryExp->type) {
        case 1: { //(exp)
            shared_ptr<ExpAST> exp = dynamic_pointer_cast<ExpAST>(primaryExp->item);
            return programExp(exp);
        }
        case 2: {//lval
            shared_ptr<LValAST> lVal = dynamic_pointer_cast<LValAST>(primaryExp->item);
            return programLVal(lVal);
        }
        case 3: {//number
            shared_ptr<NumberAST> number = dynamic_pointer_cast<NumberAST>(primaryExp->item);
            int num = number->number;
            shared_ptr<Obj> obj = make_shared<Obj>(num);
            return obj;
        }
    }
    return nullptr;
}

shared_ptr<Obj> IRcodeMaker::programExp(shared_ptr<ExpAST> &exp) {
    return programAddExp(exp->addExp);
}

shared_ptr<Obj> IRcodeMaker::programUnaryExp(shared_ptr<UnaryExpAST> &unaryExp) {
    switch (unaryExp->type) {
        case 1: {
            return programPrimaryExp(unaryExp->primaryExp);
        }
        case 2: {//函数调用
            string funcName = unaryExp->name; //函数名
            shared_ptr<FuncRParamsAST> funcRParams = unaryExp->funcRParams;
            for (int i = 0; i < funcRParams->exps.size(); ++i) {
                shared_ptr<Obj> ans = programExp(funcRParams->exps[i]);
                shared_ptr<Obj> obj[3];
                obj[0] = ans;
                shared_ptr<IRcode> t = make_shared<IRcode>(OpPush, obj);
                IRCodeList.push_back(t);
            }
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(funcName)};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpCall, obj);
            IRCodeList.push_back(t);

            shared_ptr<Obj> ret[3];
            ret[0] = newtemp();
            ret[1] = make_shared<Obj>("RET");
            t = make_shared<IRcode>(OpAssign, ret);
            IRCodeList.push_back(t);
            return ret[0];
        }
        case 3: {// + - unaryExp
            shared_ptr<UnaryOpAST> unaryOp = unaryExp->unaryOp;
            shared_ptr<Obj> ans = programUnaryExp(unaryExp->unaryExp);

            shared_ptr<Obj> obj[3];
            obj[0] = newtemp();
            obj[1] = make_shared<Obj>(0);
            obj[2] = ans;

            shared_ptr<IRcode> t;
            if (unaryOp->symbol == PLUS) {
                t = make_shared<IRcode>(OpPLUS, obj);
            } else if (unaryOp->symbol == MINU) {
                t = make_shared<IRcode>(OpMINU, obj);
            } else {
                t = make_shared<IRcode>(OpNot, obj);
            }
            IRCodeList.push_back(t);
            return obj[0];
        }
    }
    return nullptr;
}

shared_ptr<Obj> IRcodeMaker::programMulExp(shared_ptr<MulExpAST> &mulExp) {
    vector<shared_ptr<UnaryExpAST>> &unaryExps = mulExp->unaryExps;
    vector<int> &symbol = mulExp->symbol;

    shared_ptr<Obj> before;
    shared_ptr<Obj> now;
    for (int i = 0; i < unaryExps.size(); ++i) {
        before = now;
        now = programUnaryExp(unaryExps[i]);
        if (i != 0) {
            shared_ptr<Obj> obj[3];
            obj[0] = newtemp();
            obj[1] = before;
            obj[2] = now;
            now = obj[0];
            shared_ptr<IRcode> t;
            if (symbol[i - 1] == MULT)
                t = make_shared<IRcode>(OpMULT, obj);
            else if (symbol[i - 1] == DIV)
                t = make_shared<IRcode>(OpDIV, obj);
            else if (symbol[i - 1] == MOD)
                t = make_shared<IRcode>(OpMOD, obj);
            IRCodeList.push_back(t);
        }
    }
    return now;
}

shared_ptr<Obj> IRcodeMaker::programAddExp(shared_ptr<AddExpAST> &addExp) {
    vector<shared_ptr<MulExpAST>> &mulExps = addExp->mulExps;
    vector<int> &symbol = addExp->symbol;
    shared_ptr<Obj> before;
    shared_ptr<Obj> now;
    for (int i = 0; i < mulExps.size(); ++i) {
        before = now;
        now = programMulExp(mulExps[i]);
        if (i != 0) {
            shared_ptr<Obj> obj[3];
            obj[0] = newtemp();
            obj[1] = before;
            obj[2] = now;
            now = obj[0];
            shared_ptr<IRcode> t;
            if (symbol[i - 1] == PLUS)
                t = make_shared<IRcode>(OpPLUS, obj);
            else if (symbol[i - 1] == MINU)
                t = make_shared<IRcode>(OpMINU, obj);
            IRCodeList.push_back(t);
        }
    }
    return now;
}

shared_ptr<Obj> IRcodeMaker::programConstExp(shared_ptr<ConstExpAST> &constExp) {
    return programAddExp(constExp->addExp);
}

void IRcodeMaker::programInitVal(shared_ptr<InitValAST> &initVal, shared_ptr<Obj> obj[3], int *count) {
    if (obj[1]->branch == 2) {
        //var int a = 10;
        obj[2] = programExp(initVal->exp);
        shared_ptr<IRcode> t = make_shared<IRcode>(OpVar, obj);
        IRCodeList.push_back(t);
    } else {
        if (initVal->exp != nullptr) {
            shared_ptr<Obj> myObj[3] = {make_shared<Obj>(obj[1]->str, *count)};
            myObj[1] = programExp(initVal->exp);
            shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, myObj);
            IRCodeList.push_back(t);
            (*count)++;
        } else {
            vector<shared_ptr<InitValAST>> &initVals = initVal->initVals;
            for (int i = 0; i < initVals.size(); ++i) {
                programInitVal(initVals[i], obj, count);
            }
        }
    }
}

void IRcodeMaker::programVarDef(shared_ptr<VarDefAST> &varDef) {
    string identName = varDef->name; //标识符的名字
    int dimension = varDef->constExps.size(); //标识符的维度
    vector<shared_ptr<ConstExpAST>> &constExps = varDef->constExps;

    vector<shared_ptr<Obj>> exps;

    shared_ptr<Obj> before;
    shared_ptr<Obj> now;
    for (int i = 0; i < constExps.size(); ++i) {
        before = now;
        now = programConstExp(constExps[i]);

        exps.push_back(now);

        if (i != 0) {
            shared_ptr<Obj> obj[3];
            obj[0] = newtemp();
            obj[1] = before;
            obj[2] = now;
            now = obj[0];
            shared_ptr<IRcode> t = make_shared<IRcode>(OpMULT, obj);
            IRCodeList.push_back(t);
        }
    }


    if (varDef->initVal != nullptr) {
        int count = 0;
        if (constExps.empty()) {
            //var int a = 1;
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName)};
            programInitVal(varDef->initVal, obj, &count);

        } else {
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName, now)};

            shared_ptr<IRcode> t = make_shared<IRcode>(OpArray, obj);
            IRCodeList.push_back(t);
            //var int a[2];
            //a[0] = 1;
            //a[1] = 2;
            programInitVal(varDef->initVal, obj, &count);
        }
    } else {
        //没有初始值，只需要定义即可
        //var int i;
        //var int j[1][2];
        if (constExps.empty()) {
            //不是数组
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName)};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpVar, obj);
            IRCodeList.push_back(t);
        } else {
            //是数组
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName, now)};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpArray, obj);
            IRCodeList.push_back(t);
        }
    }
    //插入符号表

    shared_ptr<VarSym> var;
    if (constExps.empty()) {
        var = make_shared<VarSym>(identName, dimension, defineType.top(), NowLevel);
    } else {
        var = make_shared<VarSym>(identName, dimension, exps, defineType.top(), NowLevel);
    }

    symTable.Var.push_back(var);
}

void IRcodeMaker::programVarDecl(shared_ptr<VarDeclAST> &varDecl) {
    if (varDecl->btype->category == INTTK) {
        defineType.push(INT);
    }
    for (int i = 0; i < varDecl->varDefs.size(); ++i) {
        programVarDef(varDecl->varDefs[i]);
    }
    defineType.pop();
}

void IRcodeMaker::programDecl(shared_ptr<DeclAST> &decl) {
    if (decl->type == 0) // VarDecl
    {
        shared_ptr<VarDeclAST> p = dynamic_pointer_cast<VarDeclAST>(decl->decl);
        programVarDecl(p);
    } else { //ConstDecl
        shared_ptr<ConstDeclAST> p = dynamic_pointer_cast<ConstDeclAST>(decl->decl);
        programConstDecl(p);
    }
}

void IRcodeMaker::programFuncFParam(shared_ptr<FuncFParamAST> &funcFParam, vector<shared_ptr<VarSym>> &exps) {
    string name = funcFParam->name;
    int dimension = funcFParam->constExps.size();

    if (funcFParam->bType->category == INTTK)
        defineType.push(INT);
    vector<shared_ptr<ConstExpAST>> &constExps = funcFParam->constExps;

    if (constExps.empty()) {
        //para int a
        shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(name)};
        shared_ptr<IRcode> t = make_shared<IRcode>(OpPara, obj);
        IRCodeList.push_back(t);

        shared_ptr<VarSym> var = make_shared<VarSym>(name, dimension, defineType.top(), NowLevel);
        exps.push_back(var);
    } else {
        //para int b[][12];
        vector<shared_ptr<Obj>> arrExps; //这是数组的每层的大小

        shared_ptr<Obj> obj[3] = {make_shared<Obj>(ARR), make_shared<Obj>(defineType.top())};
        if (constExps.size() == 1) {
            obj[2] = make_shared<Obj>(name, 0x3f3f3f3f);
            shared_ptr<IRcode> t = make_shared<IRcode>(OpParaArr, obj);
            IRCodeList.push_back(t);

            arrExps.push_back(make_shared<Obj>(0x3f3f3f3f));
            shared_ptr<VarSym> var = make_shared<VarSym>(name, dimension, arrExps, defineType.top(), NowLevel);
            exps.push_back(var);
            return;
        }
        arrExps.push_back(make_shared<Obj>(0x3f3f3f3f));

        shared_ptr<Obj> before;
        shared_ptr<Obj> now;
        for (int i = 1; i < constExps.size(); ++i) {
            before = now;
            now = programConstExp(constExps[i]);
            arrExps.push_back(now);
            if (i != 1) {
                shared_ptr<Obj> obj[3];
                obj[0] = newtemp();
                obj[1] = before;
                obj[2] = now;
                now = obj[0];
                shared_ptr<IRcode> t = make_shared<IRcode>(OpMULT, obj);
                IRCodeList.push_back(t);
            }
        }
        now->type = ARR;
        shared_ptr<Obj> ooo[3] = {make_shared<Obj>(ARR), make_shared<Obj>(defineType.top()),
                                  make_shared<Obj>(name, now)};
        shared_ptr<IRcode> t = make_shared<IRcode>(OpParaArr, ooo);
        IRCodeList.push_back(t);

        shared_ptr<VarSym> var = make_shared<VarSym>(name, dimension, arrExps, defineType.top(), NowLevel);
    }

    defineType.pop();
}

void IRcodeMaker::programStmt(shared_ptr<StmtAST> &stmt) {
    switch (stmt->type) {
        case 1: {//1 lval = exp,--------
            shared_ptr<Obj> lval = programLVal(stmt->lVal);
            shared_ptr<Obj> exp = programExp(stmt->exp);
            shared_ptr<Obj> obj[3] = {lval, exp};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
            IRCodeList.push_back(t);
        }
            break;
        case 2: {//2 [exp];------------
            if (stmt->expSingle != nullptr)
                programExp(stmt->expSingle);
        }
            break;
        case 3: {  //3 block -----------
            NowLevel++;
            programBLock(stmt->block, false);
            NowLevel--;
        }
            break;
        case 4: { //4 if
        }
            break;
        case 5: { //5 while
        }
            break;
        case 6: { //6 break or continue
        }
            break;
        case 7: { //7 return ----------------
            if (stmt->expReturn == nullptr) {
                shared_ptr<Obj> obj[3];
                shared_ptr<IRcode> t = make_shared<IRcode>(OpReturn, obj);
                IRCodeList.push_back(t);
            } else {
                shared_ptr<Obj> obj[3];
                obj[0] = programExp(stmt->expReturn);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpReturn, obj);
                IRCodeList.push_back(t);
            }
        }
            break;
        case 8: { //8 lval = getint();--------------
            shared_ptr<Obj> scan_obj[3] = {newtemp()};
            shared_ptr<IRcode> scan_t = make_shared<IRcode>(OpScanf, scan_obj);
            IRCodeList.push_back(scan_t);

            shared_ptr<Obj> lval = programLVal(stmt->lValGetint);
            shared_ptr<Obj> obj[3] = {lval, scan_obj[0]};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
            IRCodeList.push_back(t);
        }
            break;
        case 9: { //9 printf----------------
            programPrintf(stmt->formatString, stmt->expsPrintf);
        }
            break;
    }
}

void IRcodeMaker::programPrintf(const string &formatString, vector<shared_ptr<ExpAST>> exps) {
    static int printLabel = -1;

    vector<shared_ptr<Obj>> out;
    for (int i = 0; i < exps.size(); ++i) {
        out.push_back(programExp(exps[i]));
    }
    vector<string> divide;
    int left = 1, right;
    //去除 两边的双引号，并且防止越界
    for (int i = 1; i < formatString.size() - 2; ++i) {
        if (formatString[i] == '%')
            if (formatString[i + 1] == 'd') {
                divide.push_back(formatString.substr(left, i - left));
                left = i + 2;
            }
    }
    divide.push_back(formatString.substr(left, formatString.size() - 1 - left));
//    for (int i = 0; i < divide.size(); ++i) {
//        cout << divide[i] << endl;
//    }
    for (int i = 0; i < divide.size(); ++i) {
        //输出字符串
        if (divide[i].compare("")) {
            //不为零说明不相等，不是空串。
            StringData data;
            data.content = divide[i];
            data.label = "strlabel" + to_string(++printLabel);

            stringData.push_back(data);

            shared_ptr<Obj> obj[3] = {make_shared<Obj>(data.label)};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpPrint, obj);
            IRCodeList.push_back(t);
        }
        if (i != divide.size() - 1) {
            //输出int
            shared_ptr<Obj> obj[3] = {out[i]};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpPrintInt, obj);
            IRCodeList.push_back(t);
        }
    }
}

void IRcodeMaker::programBlockItem(shared_ptr<BlockItemAST> &blockItem) {
    if (blockItem->type == 0) { //decl
        shared_ptr<DeclAST> decl = dynamic_pointer_cast<DeclAST>(blockItem->item);
        programDecl(decl);
    } else { //stmt
        shared_ptr<StmtAST> stmt = dynamic_pointer_cast<StmtAST>(blockItem->item);
        programStmt(stmt);
    }
}

void IRcodeMaker::programBLock(shared_ptr<BlockAST> &block, bool isfunc) {
    vector<shared_ptr<BlockItemAST>> &blockItems = block->blockItems;

    for (int i = 0; i < blockItems.size(); ++i) {
        programBlockItem(blockItems[i]);
    }

    for (auto i = symTable.Var.begin(); i != symTable.Var.end(); ++i) {
        if ((*i)->level == NowLevel) {
            symTable.Var.erase(i);
            --i;
        }
    }
}


void IRcodeMaker::programFunc(shared_ptr<FuncDefAST> &funcDef) {
    string funcName = funcDef->name;
    if (funcDef->funcType->category == INTTK) {
        defineType.push(INT);
    } else if (funcDef->funcType->category == VOIDTK) {
        defineType.push(VOID);
    }
    NowLevel++;
    shared_ptr<FuncFParamsAST> &funcFParams = funcDef->funcFParams;

    shared_ptr<Obj> decl[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(funcName)};
    shared_ptr<IRcode> t = make_shared<IRcode>(OpFunc, decl);
    IRCodeList.push_back(t); //int func()


    //param:
    if (funcFParams != nullptr) {
        int paraNum = funcFParams->funcFParams.size();
        vector<shared_ptr<VarSym>> exps;

        for (int i = 0; i < funcFParams->funcFParams.size(); ++i) {
            programFuncFParam(funcFParams->funcFParams[i], exps);
        }

        shared_ptr<FuncSym> func = make_shared<FuncSym>(funcName, paraNum, exps, defineType.top());
        symTable.Func.push_back(func);
    } else {
        shared_ptr<FuncSym> func = make_shared<FuncSym>(funcName, defineType.top());
        symTable.Func.push_back(func);
    }

    shared_ptr<BlockAST> &block = funcDef->block;
    programBLock(block, true);

    NowLevel--;
    defineType.pop();
}

void IRcodeMaker::programMainDef(shared_ptr<MainFuncDefAST> &mainFunc) {
    string name = mainFunc->name;
    defineType.push(INT);
    NowLevel++;

    shared_ptr<Obj> decl[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(name)};
    shared_ptr<IRcode> t = make_shared<IRcode>(OpFunc, decl);
    IRCodeList.push_back(t); //int func()

    shared_ptr<BlockAST> &block = mainFunc->block;
    programBLock(block, true);

    NowLevel--;
    defineType.pop();
}

void IRcodeMaker::program() {
    vector<shared_ptr<DeclAST>> &decls = (compUnitAst->decls);
    for (int i = 0; i < decls.size(); ++i) {
        this->programDecl(decls[i]);
    }
    vector<shared_ptr<FuncDefAST>> &funcs = compUnitAst->funcs;
    for (int i = 0; i < funcs.size(); ++i) {
        programFunc(funcs[i]);
    }

    if (compUnitAst->main != nullptr) {
        shared_ptr<MainFuncDefAST> &mainFunc = compUnitAst->main;
        programMainDef(mainFunc);
    }

    shared_ptr<Obj> decl[3];
    shared_ptr<IRcode> t = make_shared<IRcode>(OpExit, decl);
    IRCodeList.push_back(t);
    return;
}


shared_ptr<Obj> IRcodeMaker::newtemp() {
    static int count = -1;
    count++;
    string temp = "mi" + to_string(count);
    shared_ptr<Obj> obj = make_shared<Obj>(temp);
    return obj;
}
