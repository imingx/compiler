//
// Created by gmm on 2021/11/22.
//

#include "include/IRcode.h"

int var_offset; //偏移量，.data
int global_offset;

stack<SymbolType> defineType;

stack<shared_ptr<Obj>> while_stack_head, while_stack_end;

vector<shared_ptr<IRcode>> IRCodeList;

SymTable symTable;

vector<StringData> stringData;

bool isMain;

IRcodeMaker::IRcodeMaker(shared_ptr<CompUnitAST> &compUnitAst) : compUnitAst(compUnitAst) {
    var_offset = 0;
    NowLevel = 0;
    isMain = false;
    global_offset = 0;
}

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
            cout << obj[0]->p() << " = !" << obj[2]->p() << endl;
            break;
        case OpPara:
            cout << operatorString[op] << " " << obj[0]->p() << " " << obj[1]->p() << endl;
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
        case OpJMain:
            cout << operatorString[op] << endl;
            break;
        case OpJmp:
            cout << operatorString[op] << " " << obj[0]->p() << endl;
            break;
        case OpLabel:
            cout << obj[0]->p() << ":" << endl;
            break;
        case OpLSS:
            cout << "blt " << obj[0]->p() << " " << obj[1]->p() << " " << obj[2]->p() << endl;
            break;
        case OpLEQ:
            cout << "ble " << obj[0]->p() << " " << obj[1]->p() << " " << obj[2]->p() << endl;
            break;
        case OpGRE:
            cout << "bgt " << obj[0]->p() << " " << obj[1]->p() << " " << obj[2]->p() << endl;
            break;
        case OpGEQ:
            cout << "bge " << obj[0]->p() << " " << obj[1]->p() << " " << obj[2]->p() << endl;
            break;
        case OpEQL:
            cout << "beq " << obj[0]->p() << " " << obj[1]->p() << " " << obj[2]->p() << endl;
            break;
        case OpNEQ:
            cout << "bne " << obj[0]->p() << " " << obj[1]->p() << " " << obj[2]->p() << endl;
            break;
        case OpSlt:
        case OpSle:
        case OpSgt:
        case OpSge:
            cout << operatorString[op] << " " << obj[0]->p() << " " << obj[1]->p() << " " << obj[2]->p() << endl;
            break;
        default:
            cout << "忘记输出了\n";
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
            return name;
        case 3:
            return name + "[" + index->p() + "]";
        case 4:
            return name + "[" + to_string(num_index) + "]";
        case 5:
            return to_string(num);
    }
    return "";
}


void IRcodeMaker::programConstInitVal(shared_ptr<ConstInitValAST> &constInitVal, shared_ptr<Obj> arr[3]) {
    int dim = arr[1]->var->dim;//数组的维度
    if (dim == 0) {
        //const var int a = 10;
        arr[2] = programConstExp(constInitVal->constExp);
        arr[1]->var->setValue(arr[2]);
        shared_ptr<IRcode> t = make_shared<IRcode>(OpVar, arr);
        IRCodeList.push_back(t);
    } else if (dim == 1) {
        //const var int a[10] = {1,b,3};
        int i;
        for (i = 0; i < constInitVal->constInitVals.size(); ++i) {
            shared_ptr<Obj> obj[3];
            obj[0] = make_shared<Obj>(arr[1]->name, i, arr[1]->var);
            obj[1] = programConstExp(constInitVal->constInitVals[i]->constExp);
            obj[0]->var->values.push_back(obj[1]);
            shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
            IRCodeList.push_back(t);
        }
        if (NowLevel == 0) {
            for (; i < arr[1]->num_index; ++i) {
                shared_ptr<Obj> obj[3];
                obj[0] = make_shared<Obj>(arr[1]->name, i, arr[1]->var);
                obj[1] = make_shared<Obj>(0);
                obj[0]->var->values.push_back(obj[1]);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                IRCodeList.push_back(t);
            }
        }
    } else if (dim == 2) {
        int first = arr[1]->var->exps[0];
        int second = arr[1]->var->exps[1];
        int i;
        for (i = 0; i < constInitVal->constInitVals.size(); ++i) {
            int j;
            for (j = 0; j < constInitVal->constInitVals[i]->constInitVals.size(); ++j) {
                shared_ptr<Obj> obj[3];
                obj[0] = make_shared<Obj>(arr[1]->name, i * second + j, arr[1]->var);
                obj[1] = programConstExp(constInitVal->constInitVals[i]->constInitVals[j]->constExp);
                obj[0]->var->values.push_back(obj[1]);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                IRCodeList.push_back(t);
            }
            if (NowLevel == 0) {
                for (; j < second; ++j) {
                    shared_ptr<Obj> obj[3];
                    obj[0] = make_shared<Obj>(arr[1]->name, i * second + j, arr[1]->var);
                    obj[1] = make_shared<Obj>(0);
                    obj[0]->var->values.push_back(obj[1]);
                    shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                    IRCodeList.push_back(t);
                }
            }
        }
        if (NowLevel == 0) {
            for (int j = i * second; j < first * second; ++j) {
                shared_ptr<Obj> obj[3];
                obj[0] = make_shared<Obj>(arr[1]->name, j, arr[1]->var);
                obj[1] = make_shared<Obj>(0);
                obj[0]->var->values.push_back(obj[1]);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                IRCodeList.push_back(t);
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

    vector<int> exps;
    int needSpace = 1;

    for (int i = 0; i < constExps.size(); ++i) {
        before = now;
        now = programConstExp(constExps[i]);
        needSpace *= now->num;
        exps.push_back(now->num);
//        if (i != 0) {
//            shared_ptr<Obj> obj[3];
//            obj[0] = newValue();
//            obj[1] = before;
//            obj[2] = now;
//            now = obj[0];
//
//            shared_ptr<VarSym> var = make_shared<VarSym>(obj[0]->name, 0, INT, NowLevel);
//            var_offset += 1;
//            var->setOffsetAndNeedSpace(var_offset, 1);
//
//            obj[0]->setVar(var);
//
//            shared_ptr<IRcode> t = make_shared<IRcode>(OpMULT, obj);
//            IRCodeList.push_back(t);
//        }
    }

//    for (int i = 0; i < exps.size(); ++i) {
//        needSpace *= exps[i];
//    }

    shared_ptr<VarSym> var;
    if (constExps.empty()) {
        var = make_shared<VarSym>(true, identName, dimension, defineType.top(), NowLevel);
    } else {
        var = make_shared<VarSym>(true, identName, dimension, exps, defineType.top(), NowLevel);
    }

    var_offset += needSpace;
    if (NowLevel == 0)
        global_offset += needSpace;
    var->setOffsetAndNeedSpace(var_offset, needSpace);

//    cout << "needspace is " << needSpace << endl;
//    cout << "offset is " << var_offset << endl;

    symTable.Var.push_back(var);

    if (constDef->constInitVal->constInitVals.empty()) {
        //a
        shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                  make_shared<Obj>(identName, var)};
        programConstInitVal(constDef->constInitVal, obj);
    } else {
        //a[];
        shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                  make_shared<Obj>(identName, needSpace, var)};
        shared_ptr<IRcode> t = make_shared<IRcode>(OpArray, obj);
        IRCodeList.push_back(t);
        programConstInitVal(constDef->constInitVal, obj);
    }
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

    vector<shared_ptr<Obj>> objExp;

    for (int i = 0; i < exps.size(); ++i) {
        shared_ptr<Obj> now = programExp(exps[i]);
        objExp.push_back(now);
    }

    if (var->dim != exps.size()) {
        //这里作传参，数组。
        //一维数组a[3]传a,二维数组b[3][3]传b, b[0]
        if (exps.size() == 0) {
            shared_ptr<Obj> obj = make_shared<Obj>(varName, var);
            return obj;
        } else if (exps.size() == 1) {
            shared_ptr<Obj> obj = make_shared<Obj>(varName, objExp[0], var);
            return obj;
        }
    } else if (var->dim == 0) {
        shared_ptr<Obj> obj = make_shared<Obj>(varName, var);
//        if (var->value != nullptr)
        obj->num = var->value->num;
        if (var->isConst)
            obj->branch = 5;//num
        return obj;
    } else if (var->dim == 1) {
        //一维数组取值。
        shared_ptr<Obj> obj = make_shared<Obj>(varName, objExp[0], var);
        if (objExp[0]->branch == 5) {
            int index = objExp[0]->num;
            if (var->isConst) {
                obj->num = var->values[index]->num;
                obj->branch = 5;//变为num
            }
        }
        return obj;
    } else if (var->dim == 2) {
        //二维数组
        int first = var->exps[0], second = var->exps[1];

        if ((objExp[0]->branch == 5 && objExp[1]->branch == 5)) {
//        || (objExp[0]->branch != 5 && objExp[1]->branch == 5 && objExp[0]->var->isConst)
//        || (objExp[1]->branch != 5 && objExp[0]->branch == 5 && objExp[1]->var->isConst)
//       || (objExp[1]->branch != 5 && objExp[0]->branch != 5 && objExp[1]->var->isConst && objExp[0]->var->isConst)) {
            int index = objExp[0]->num * second + objExp[1]->num;
            shared_ptr<Obj> tmp = make_shared<Obj>(index);
            shared_ptr<Obj> obj = make_shared<Obj>(varName, tmp, var);
            if (var->isConst) {
                obj->num = var->values[index]->num;
                obj->branch = 5;
            }
            return obj;
        }
        //双方都不是const值，需要计算。
        shared_ptr<Obj> obj1[3] = {newValue(), objExp[0], make_shared<Obj>(second)};
        shared_ptr<VarSym> var1 = make_shared<VarSym>(false, obj1[0]->name, 0, INT, NowLevel);
        var_offset += 1;
        if (NowLevel == 0)
            global_offset += 1;
        var1->setOffsetAndNeedSpace(var_offset, 1);
        obj1[0]->setVar(var1);
        shared_ptr<IRcode> t1 = make_shared<IRcode>(OpMULT, obj1);
        IRCodeList.push_back(t1);

        shared_ptr<Obj> obj2[3] = {newValue(), obj1[0], objExp[1]};
        shared_ptr<VarSym> var2 = make_shared<VarSym>(false, obj2[0]->name, 0, INT, NowLevel);
        var_offset += 1;
        if (NowLevel == 0)
            global_offset += 1;
        var2->setOffsetAndNeedSpace(var_offset, 1);
        obj2[0]->setVar(var2);
        shared_ptr<IRcode> t2 = make_shared<IRcode>(OpPLUS, obj2);
        IRCodeList.push_back(t2);

        shared_ptr<Obj> obj = make_shared<Obj>(varName, obj2[0], var);
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
    shared_ptr<Obj> obj = programAddExp(exp->addExp);
//    if (obj->var && obj->var->isConst && (obj->branch == 2 || obj->branch == 4))
//        obj->branch = 5;
    return obj;
}

shared_ptr<Obj> IRcodeMaker::programUnaryExp(shared_ptr<UnaryExpAST> &unaryExp) {
    switch (unaryExp->type) {
        case 1: {
            return programPrimaryExp(unaryExp->primaryExp);
        }
        case 2: {//函数调用
            string funcName = unaryExp->name; //函数名
            shared_ptr<FuncRParamsAST> funcRParams = unaryExp->funcRParams;

            if (funcRParams != nullptr) {
                for (int i = 0; i < funcRParams->exps.size(); ++i) {
                    shared_ptr<Obj> ans = programExp(funcRParams->exps[i]);
                    shared_ptr<Obj> obj[3];
                    obj[0] = ans;
                    shared_ptr<IRcode> t = make_shared<IRcode>(OpPush, obj);
                    IRCodeList.push_back(t);
                }
            }

            shared_ptr<FuncSym> func;

            for (int i = 0; i < symTable.Func.size(); ++i) {
                if (symTable.Func[i]->name == funcName) {
                    func = symTable.Func[i];
                }
            }

            shared_ptr<Obj> obj[3] = {make_shared<Obj>(funcName, func)};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpCall, obj);
            IRCodeList.push_back(t);

            if (func->returnType != VOID) {
                shared_ptr<Obj> ret[3];
                ret[0] = newValue();
                shared_ptr<VarSym> var = make_shared<VarSym>(false, obj[0]->name, 0, INT, NowLevel);
                var_offset += 1;
                if (NowLevel == 0)
                    global_offset += 1;
                var->setOffsetAndNeedSpace(var_offset, 1);
                ret[0]->setVar(var);
                ret[1] = make_shared<Obj>("RET");
                t = make_shared<IRcode>(OpAssign, ret);
                IRCodeList.push_back(t);
                return ret[0];
            } else {
                //返回值为零
                return make_shared<Obj>(0);
            }
        }
        case 3: {// + - ! unaryExp
            //最后增加！
            shared_ptr<UnaryOpAST> unaryOp = unaryExp->unaryOp;
            shared_ptr<Obj> ans = programUnaryExp(unaryExp->unaryExp);

            shared_ptr<Obj> obj[3];
            obj[0] = newValue();
            obj[1] = make_shared<Obj>(0);
            obj[2] = ans;

            shared_ptr<VarSym> var = make_shared<VarSym>(false, obj[0]->name, 0, INT, NowLevel);

            var_offset += 1;
            if (NowLevel == 0)
                global_offset += 1;
            var->setOffsetAndNeedSpace(var_offset, 1);

            obj[0]->setVar(var);

            shared_ptr<IRcode> t;
            if (unaryOp->symbol == PLUS) { // +
                obj[0]->num = 0 + obj[2]->num;
                t = make_shared<IRcode>(OpPLUS, obj);
            } else if (unaryOp->symbol == MINU) {// -
                obj[0]->num = 0 - obj[2]->num;
                t = make_shared<IRcode>(OpMINU, obj);
            } else if (unaryOp->symbol == NOT) { // !
                if (ans->branch == 5) {
                    obj[0]->num = ans->num == 0 ? 1 : 0;
                }
                t = make_shared<IRcode>(OpNot, obj);
            }
            if (ans->branch == 5) {
                obj[0]->branch = 5;
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

    int num = 1;

    bool isConst = true;

    for (int i = 0; i < unaryExps.size(); ++i) {
        before = now;
        now = programUnaryExp(unaryExps[i]);

        if (i == 0)
            num *= now->num;
        else {
            if (symbol[i - 1] == MULT)
                num *= now->num;
            else if (symbol[i - 1] == DIV && now->num != 0)
                num /= now->num;
            else if (symbol[i - 1] == MOD && now->num != 0) {
                num %= now->num;
            }
        }

        if (now->branch != 5)
            isConst = false;

        if (i != 0) {
            shared_ptr<Obj> obj[3];
            obj[0] = newValue();
            obj[1] = before;
            obj[2] = now;
            now = obj[0];

            shared_ptr<VarSym> var = make_shared<VarSym>(isConst, obj[0]->name, 0, INT, NowLevel);

            var_offset += 1;
            if (NowLevel == 0)
                global_offset += 1;
            var->setOffsetAndNeedSpace(var_offset, 1);

            obj[0]->setVar(var);
            if (isConst) {
                obj[0]->branch = 5;
                obj[0]->num = num;
            }

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
    now->num = num;
    return now;
}

shared_ptr<Obj> IRcodeMaker::programAddExp(shared_ptr<AddExpAST> &addExp) {
    vector<shared_ptr<MulExpAST>> &mulExps = addExp->mulExps;
    vector<int> &symbol = addExp->symbol;
    shared_ptr<Obj> before;
    shared_ptr<Obj> now;

    int num = 0;

    bool isConst = true;

    for (int i = 0; i < mulExps.size(); ++i) {
        before = now;
        now = programMulExp(mulExps[i]);

        if (i == 0)
            num += now->num;
        else {
            if (symbol[i - 1] == PLUS)
                num += now->num;
            else
                num -= now->num;
        }
        if (now->branch != 5)
            isConst = false;
        if (i != 0) {
            shared_ptr<Obj> obj[3];
            obj[0] = newValue();
            obj[1] = before;
            obj[2] = now;
            now = obj[0];

            shared_ptr<VarSym> var = make_shared<VarSym>(isConst, obj[0]->name, 0, INT, NowLevel);

            var_offset += 1;
            if (NowLevel == 0)
                global_offset += 1;
            var->setOffsetAndNeedSpace(var_offset, 1);

            obj[0]->setVar(var);
            if (isConst) {
                obj[0]->branch = 5;
                obj[0]->num = num;
            }
            shared_ptr<IRcode> t;
            if (symbol[i - 1] == PLUS)
                t = make_shared<IRcode>(OpPLUS, obj);
            else if (symbol[i - 1] == MINU)
                t = make_shared<IRcode>(OpMINU, obj);
            IRCodeList.push_back(t);
        }
    }
    now->num = num;
    return now;
}

shared_ptr<Obj> IRcodeMaker::programConstExp(shared_ptr<ConstExpAST> &constExp) {
    shared_ptr<Obj> obj = programAddExp(constExp->addExp);
    obj->branch = 5;
    return obj;
}

void IRcodeMaker::programInitVal(shared_ptr<InitValAST> &initVal, shared_ptr<Obj> arr[3]) {
    int dim = arr[1]->var->dim;//数组的维度
    if (dim == 0) {
        //var int a = 10;
        arr[2] = programExp(initVal->exp);
        arr[1]->var->setValue(arr[2]);
        shared_ptr<IRcode> t = make_shared<IRcode>(OpVar, arr);
        IRCodeList.push_back(t);
    } else if (dim == 1) {
        //var int a[10] = {1,b,3};
        int i;
        for (i = 0; i < initVal->initVals.size(); ++i) {
            shared_ptr<Obj> obj[3];
            obj[0] = make_shared<Obj>(arr[1]->name, i, arr[1]->var);
            obj[1] = programExp(initVal->initVals[i]->exp);
            obj[0]->var->values.push_back(obj[1]);
            shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
            IRCodeList.push_back(t);
        }
        if (NowLevel == 0) {
            for (; i < arr[1]->num_index; ++i) {
                shared_ptr<Obj> obj[3];
                obj[0] = make_shared<Obj>(arr[1]->name, i, arr[1]->var);
                obj[1] = make_shared<Obj>(0);
                obj[0]->var->values.push_back(obj[1]);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                IRCodeList.push_back(t);
            }
        }
    } else if (dim == 2) {
        int first = arr[1]->var->exps[0];
        int second = arr[1]->var->exps[1];
        int i;
        for (i = 0; i < initVal->initVals.size(); ++i) {
            int j;
            for (j = 0; j < initVal->initVals[i]->initVals.size(); ++j) {
                shared_ptr<Obj> obj[3];
                obj[0] = make_shared<Obj>(arr[1]->name, i * second + j, arr[1]->var);
                obj[1] = programExp(initVal->initVals[i]->initVals[j]->exp);
                obj[0]->var->values.push_back(obj[1]);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                IRCodeList.push_back(t);
            }
            if (NowLevel == 0) {
                for (; j < second; ++j) {
                    shared_ptr<Obj> obj[3];
                    obj[0] = make_shared<Obj>(arr[1]->name, i * second + j, arr[1]->var);
                    obj[1] = make_shared<Obj>(0);
                    obj[0]->var->values.push_back(obj[1]);
                    shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                    IRCodeList.push_back(t);
                }
            }
        }
        if (NowLevel == 0) {
            for (int j = i * second; j < first * second; ++j) {
                shared_ptr<Obj> obj[3];
                obj[0] = make_shared<Obj>(arr[1]->name, j, arr[1]->var);
                obj[1] = make_shared<Obj>(0);
                obj[0]->var->values.push_back(obj[1]);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                IRCodeList.push_back(t);
            }
        }
    }
}

void IRcodeMaker::programVarDef(shared_ptr<VarDefAST> &varDef) {
    string identName = varDef->name; //标识符的名字
    int dimension = varDef->constExps.size(); //标识符的维度
    vector<shared_ptr<ConstExpAST>> &constExps = varDef->constExps;//标识符a[constExp1][constExp2]

    vector<int> exps;  //存放 a[exp1][exp2]的OBJ
    //最多二维度。

    shared_ptr<Obj> before;
    shared_ptr<Obj> now;

    int needSpace = 1;

    for (int i = 0; i < constExps.size(); ++i) {
        before = now;
        now = programConstExp(constExps[i]);
        needSpace *= now->num;
        exps.push_back(now->num);
    }

    shared_ptr<VarSym> var;
    if (constExps.empty()) {
        // int a;
        var = make_shared<VarSym>(false, identName, dimension, defineType.top(), NowLevel);
    } else {
        // int a[][];
        var = make_shared<VarSym>(false, identName, dimension, exps, defineType.top(), NowLevel);
    }

    var_offset += needSpace;
    if (NowLevel == 0 || dimension != 0) {
        global_offset += needSpace;
        var->setOffsetAndNeedSpace(global_offset, needSpace);
        var->level = 0;
    } else
        var->setOffsetAndNeedSpace(var_offset, needSpace);

//    cout << "needspace is " << needSpace << endl;
//    cout << "offset is " << var_offset << endl;

    symTable.Var.push_back(var);
    //上面是符号表插入

    if (varDef->initVal != nullptr) {
        if (varDef->initVal->initVals.empty()) {
            //var int a = 1;
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName, var)};
            programInitVal(varDef->initVal, obj);
        } else {
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName, needSpace, var)};

            shared_ptr<IRcode> t = make_shared<IRcode>(OpArray, obj);
            IRCodeList.push_back(t);
            //var int a[2];
            //a[0] = 1;
            programInitVal(varDef->initVal, obj);
        }
    } else {
        //没有初始值，只需要定义即可
        //var int i;
        //var int j[1][2];
        if (constExps.empty()) {
            //不是数组
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName, var)};
            if (NowLevel == 0) {
                obj[2] = make_shared<Obj>(0);
                obj[1]->var->setValue(0);
            }
            shared_ptr<IRcode> t = make_shared<IRcode>(OpVar, obj);
            IRCodeList.push_back(t);
        } else {
            //是数组
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()),
                                      make_shared<Obj>(identName, needSpace, var)};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpArray, obj);
            IRCodeList.push_back(t);
            if (NowLevel == 0) {
                for (int i = 0; i < needSpace; ++i) {
                    shared_ptr<Obj> obj[3] = {make_shared<Obj>(identName, i, var),
                                              make_shared<Obj>(0)};
                    obj[0]->var->values.push_back(obj[1]);
                    shared_ptr<IRcode> t = make_shared<IRcode>(OpAssign, obj);
                    IRCodeList.push_back(t);
                }
            }
        }
    }
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

    if (dimension == 0) {
        //para int a;
        shared_ptr<VarSym> var = make_shared<VarSym>(false, name, dimension, defineType.top(), NowLevel);
        symTable.Var.push_back(var);
        exps.push_back(var);
        //符号表
        var_offset += 1;
        var->setOffsetAndNeedSpace(var_offset, 1);

        shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(name, var)};
        shared_ptr<IRcode> t = make_shared<IRcode>(OpPara, obj);
        IRCodeList.push_back(t);
    } else if (dimension == 1) {
        //para int a[];
        vector<int> exp = {0};
        shared_ptr<VarSym> var = make_shared<VarSym>(false, name, dimension, exp, defineType.top(), NowLevel);
        var->isArrParam = true;
        symTable.Var.push_back(var);
        exps.push_back(var);
        //符号表
        var_offset += 1;
        var->setOffsetAndNeedSpace(var_offset, 1);

        shared_ptr<Obj> obj[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(name, var)};
        shared_ptr<IRcode> t = make_shared<IRcode>(OpPara, obj);
        IRCodeList.push_back(t);
    } else if (dimension == 2) {
        //para int a[][2];
        shared_ptr<Obj> obj = programConstExp(constExps[1]);
        vector<int> exp = {0, obj->num};
        shared_ptr<VarSym> var = make_shared<VarSym>(false, name, dimension, exp, defineType.top(), NowLevel);
        var->isArrParam = true;
        symTable.Var.push_back(var);
        exps.push_back(var);

        var_offset += 1;
        var->setOffsetAndNeedSpace(var_offset, 1);

        shared_ptr<Obj> objs[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(name, var)};
        shared_ptr<IRcode> t = make_shared<IRcode>(OpPara, objs);
        IRCodeList.push_back(t);
    }
    defineType.pop();
}

shared_ptr<Obj> IRcodeMaker::programRelExp(shared_ptr<RelExpAST> &relExp, string &label, string &Else, bool haveEq) {
    vector<int> &symbols = relExp->symbols;//< > <= >=

    //实际上只有 < > <= >=
    vector<shared_ptr<AddExpAST>> &addExps = relExp->addExps;

    if (symbols.empty()) {
        shared_ptr<Obj> obj = programAddExp(addExps[0]);
        return obj;
    } else if (!haveEq) {
        int symbol = symbols[0];
        shared_ptr<Obj> obj1 = programAddExp(addExps[0]);
        shared_ptr<Obj> obj2 = programAddExp(addExps[1]);
        if (obj1->branch == 5 && obj2->branch == 5) {
//            shared_ptr<Obj> obj[3] = {obj1, obj2, make_shared<Obj>(label + Else)};
//            shared_ptr<IRcode> t;
            int num;
            if (obj1->num > obj2->num && (symbol == GRE || symbol == GEQ)) {
                num = 1;
            } else if (obj1->num == obj2->num && (symbol == GEQ || symbol == LEQ)) {
                num = 1;
            } else if (obj1->num < obj2->num && (symbol == LEQ || symbol == LSS)) {
                num = 1;
            } else {
                num = 0;
            }
//            if (symbol == LEQ)
//                t = make_shared<IRcode>(OpGRE, obj);
//            if (symbol == LSS)
//                t = make_shared<IRcode>(OpGEQ, obj);
//            if (symbol == GRE)
//                t = make_shared<IRcode>(OpLEQ, obj);
//            if (symbol == GEQ)
//                t = make_shared<IRcode>(OpLSS, obj);
//            IRCodeList.push_back(t);
            return make_shared<Obj>(num);
        }
        //其中至少一个没有获取值
        shared_ptr<Obj> obj[3] = {obj1, obj2, make_shared<Obj>(label + Else)};
        shared_ptr<IRcode> t;
        if (symbol == GRE) {
            t = make_shared<IRcode>(OpLEQ, obj);
        } else if (symbol == GEQ) {
            t = make_shared<IRcode>(OpLSS, obj);
        } else if (symbol == LEQ) {
            t = make_shared<IRcode>(OpGRE, obj);
        } else if (symbol == LSS) {
            t = make_shared<IRcode>(OpGEQ, obj);
        }
        IRCodeList.push_back(t);
        return make_shared<Obj>(); //branch == 0;
    } else {
        // if (a > b == a < c){}
        int symbol = symbols[0];
        shared_ptr<Obj> obj1 = programAddExp(addExps[0]);
        shared_ptr<Obj> obj2 = programAddExp(addExps[1]);
        if (obj1->branch == 5 && obj2->branch == 5) {
            shared_ptr<Obj> obj[3] = {obj1, obj2, make_shared<Obj>(label + Else)};
            int num;
            if (obj1->num > obj2->num && (symbol == GRE || symbol == GEQ)) {
                num = 1;
            } else if (obj1->num == obj2->num && (symbol == GEQ || symbol == LEQ)) {
                num = 1;
            } else if (obj1->num < obj2->num && (symbol == LEQ || symbol == LSS)) {
                num = 1;
            } else {
                num = 0;
            }
            return make_shared<Obj>(num);
        }
        shared_ptr<Obj> obj[3] = {newValue(), obj1, obj2};

        shared_ptr<VarSym> var = make_shared<VarSym>(false, obj[0]->name, 0, INT, NowLevel);

        var_offset += 1;
        var->setOffsetAndNeedSpace(var_offset, 1);

        obj[0]->setVar(var);

        shared_ptr<IRcode> t;
        if (symbol == GRE) {
            t = make_shared<IRcode>(OpSgt, obj);
        } else if (symbol == GEQ) {
            t = make_shared<IRcode>(OpSge, obj);
        } else if (symbol == LEQ) {
            t = make_shared<IRcode>(OpSle, obj);
        } else if (symbol == LSS) {
            t = make_shared<IRcode>(OpSlt, obj);
        }
        IRCodeList.push_back(t);
        return obj[0];
    }
}

shared_ptr<Obj> IRcodeMaker::programEqExp(shared_ptr<EqExpAST> &eqExp, string &label, string &Else) {
    vector<int> &symbols = eqExp->symbols;// == !=

    //实际上这个应该只有 a==b / a!=b 以及 a（a是 b > c或b < c, b<=c, b>=c，或 !d）
    vector<shared_ptr<RelExpAST>> &relExps = eqExp->relExps;

    if (symbols.empty()) {
        //去往下一层
        shared_ptr<Obj> ans = programRelExp(relExps[0], label, Else, false);
        if (ans->var != nullptr) {
            //之前没有输出 比如 if (!a){}
            shared_ptr<Obj> obj[3] = {ans, make_shared<Obj>(0), make_shared<Obj>(label + Else)};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpEQL, obj);
            IRCodeList.push_back(t);
        } else if (ans->branch == 5) {
            // if (1){}
            if (ans->num == 0) {
                shared_ptr<Obj> obj[3];
                obj[0] = make_shared<Obj>(label + Else);
                shared_ptr<IRcode> t = make_shared<IRcode>(OpJmp, obj);
                IRCodeList.push_back(t);
            }
        }
        return ans;
    } else {
        //显然是 a == b或 a != b
        //可以有 a > 1 == a < 20

        int symbol = symbols[0];
        shared_ptr<Obj> obj1 = programRelExp(relExps[0], label, Else, true);
        shared_ptr<Obj> obj2 = programRelExp(relExps[1], label, Else, true);
        if (obj1->branch == 5 && obj2->branch == 5) {
            //两者都是已经获取值
            shared_ptr<Obj> obj[3] = {obj1, obj2, make_shared<Obj>(label + Else)};
            shared_ptr<IRcode> t;
            int num;
            if (obj1->num == obj2->num && symbol == EQL) {
                num = 1;
            } else if (obj1->num != obj2->num && symbol == NEQ) {
                num = 1;
            } else {
                num = 0;
            }
            if (symbol == NEQ)
                t = make_shared<IRcode>(OpEQL, obj);
            if (symbol == EQL)
                t = make_shared<IRcode>(OpNEQ, obj);
            IRCodeList.push_back(t);
            return make_shared<Obj>(num);
        }
        //两者至少一个没有立刻得到值。
        shared_ptr<Obj> obj[3] = {obj1, obj2, make_shared<Obj>(label + Else)};
        shared_ptr<IRcode> t;
        if (symbol == EQL) {
            t = make_shared<IRcode>(OpNEQ, obj);
        } else if (symbol == NEQ) {
            t = make_shared<IRcode>(OpEQL, obj);
        }
        IRCodeList.push_back(t);
        return make_shared<Obj>(); //branch = 0
    }
}

void IRcodeMaker::programLAndExp(shared_ptr<LAndExpAST> &lAndExp, string &label, string &Else) {
    vector<shared_ptr<EqExpAST>> &eqExps = lAndExp->eqExps;
    for (int j = 0; j < eqExps.size(); j++) {
        shared_ptr<Obj> obj = programEqExp(eqExps[j], label, Else);
        if (obj->branch == 5) {
            //说明已经得到了结果，0或非0，0表示假，非0表示真。
            if (obj->num == 0) {
                break;
            }
        }
    }
}

void IRcodeMaker::programLOrExp(shared_ptr<LOrExpAST> &lOrExp, string &label, string &Else) {
    vector<shared_ptr<LAndExpAST>> &lAndExps = lOrExp->lAndExps;

    if (lAndExps.size() == 1) {
        //没有||语句。
        programLAndExp(lAndExps[0], label, Else);
    } else {
        for (int i = 0; i < lAndExps.size(); ++i) {
            if (i != lAndExps.size() - 1) {
                string tmp = "else_" + to_string(i);
                programLAndExp(lAndExps[i], label, tmp);
                shared_ptr<Obj> obj[3] = {make_shared<Obj>(label + "begin")};
                shared_ptr<IRcode> t = make_shared<IRcode>(OpJmp, obj);
                IRCodeList.push_back(t);
                obj[0] = make_shared<Obj>(label + "else_" + to_string(i));
                t = make_shared<IRcode>(OpLabel, obj);
                IRCodeList.push_back(t);
            } else {
                programLAndExp(lAndExps[i], label, Else);
            }
        }
    }
}

void IRcodeMaker::programCond(shared_ptr<CondAST> &cond, string &label, string &Else) {
    programLOrExp(cond->lOrExp, label, Else);
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
            if (stmt->expSingle != nullptr) {
                programExp(stmt->expSingle);
            }
        }
            break;
        case 3: {  //3 block -----------
            NowLevel++;
            programBLock(stmt->block, false);
            NowLevel--;
        }
            break;
        case 4: { //4 if---------------
            shared_ptr<CondAST> condIf = stmt->condIf;
            shared_ptr<StmtAST> stmtIf = stmt->stmtIf;
            shared_ptr<StmtAST> stmtElse = stmt->stmtElse;
            string Else = stmtElse == nullptr ? "end" : "else";
            string if_label = newLabel(IF);
            //开始处理
            programCond(condIf, if_label, Else);

            shared_ptr<Obj> obj[3] = {make_shared<Obj>(if_label + "begin")};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpLabel, obj);
            IRCodeList.push_back(t);

            programStmt(stmtIf);

            if (stmtElse != nullptr) {
                obj[0] = make_shared<Obj>(if_label + "end");
                t = make_shared<IRcode>(OpJmp, obj);
                IRCodeList.push_back(t);
                obj[0] = make_shared<Obj>(if_label + "else");
                t = make_shared<IRcode>(OpLabel, obj);
                IRCodeList.push_back(t);
                programStmt(stmtElse);
            }

            obj[0] = make_shared<Obj>(if_label + "end");
            t = make_shared<IRcode>(OpLabel, obj);
            IRCodeList.push_back(t);
        }
            break;
        case 5: { //5 while------------------
            shared_ptr<CondAST> condWhile = stmt->condWhile;
            shared_ptr<StmtAST> stmtWhile = stmt->stmtWhile;
            string Else = "end";
            string while_label = newLabel(WHILE);
            while_stack_head.push(make_shared<Obj>(while_label + "head"));
            while_stack_end.push(make_shared<Obj>(while_label + "end"));
            //开始处理

            //label
            shared_ptr<Obj> obj[3] = {make_shared<Obj>(while_label + "head")};
            shared_ptr<IRcode> t = make_shared<IRcode>(OpLabel, obj);
            IRCodeList.push_back(t);

            programCond(condWhile, while_label, Else);

            obj[0] = make_shared<Obj>(while_label + "begin");
            t = make_shared<IRcode>(OpLabel, obj);
            IRCodeList.push_back(t);

            programStmt(stmtWhile);

            obj[0] = make_shared<Obj>(while_label + "head");
            t = make_shared<IRcode>(OpJmp, obj);
            IRCodeList.push_back(t);

            obj[0] = make_shared<Obj>(while_label + "end");
            t = make_shared<IRcode>(OpLabel, obj);
            IRCodeList.push_back(t);

            while_stack_head.pop();
            while_stack_end.pop();
        }
            break;
        case 6: { //6 break or continue------------------
            int category = stmt->category;
            shared_ptr<Obj> obj[3];
            if (category == BREAKTK) {
                obj[0] = while_stack_end.top();
            } else if (category == CONTINUETK) {
                obj[0] = while_stack_head.top();
            }
            shared_ptr<IRcode> t = make_shared<IRcode>(OpJmp, obj);
            IRCodeList.push_back(t);
        }
            break;
        case 7: { //7 return ----------------
            if (isMain) {
                shared_ptr<Obj> decl[3];
                shared_ptr<IRcode> t = make_shared<IRcode>(OpExit, decl);
                IRCodeList.push_back(t);
            } else {
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
        }
            break;
        case 8: { //8 Lval = getint();--------------
            shared_ptr<Obj> scan_obj[3] = {newValue()};

            shared_ptr<VarSym> var = make_shared<VarSym>(false, scan_obj[0]->name, 0, INT, NowLevel);

            var_offset += 1;
            var->setOffsetAndNeedSpace(var_offset, 1);

            scan_obj[0]->setVar(var);

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

string IRcodeMaker::newLabel(SymbolType type) {
    int count;
    if (type == WHILE) {
        static int while_count = -1;
        count = ++while_count;
    } else if (type == IF) {
        static int if_count = -1;
        count = ++if_count;
    }
    string label = symbolTypeString[type] + to_string(count) + "_";
    return label;
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
//            cout << divide[i] << endl;
            if (divide[i].compare("\\n") == 0) {
                shared_ptr<Obj> obj[3] = {make_shared<Obj>("nLine")};
                shared_ptr<IRcode> t = make_shared<IRcode>(OpPrint, obj);
                IRCodeList.push_back(t);
            } else {
                StringData data;
                data.content = divide[i];
                data.label = "str_label_" + to_string(++printLabel);

                stringData.push_back(data);

                shared_ptr<Obj> obj[3] = {make_shared<Obj>(data.label)};
                shared_ptr<IRcode> t = make_shared<IRcode>(OpPrint, obj);
                IRCodeList.push_back(t);
            }
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
        if ((*i)->level == NowLevel || (*i)->originLevel == NowLevel) {
            symTable.Var.erase(i);
            --i;
        }
    }

    if (blockItems.empty()) {
        if (isfunc) {
            shared_ptr<Obj> decl[3];
            shared_ptr<IRcode> t = make_shared<IRcode>(OpReturn, decl);
            IRCodeList.push_back(t);
        }
        return;
    }
    shared_ptr<BlockItemAST> end = blockItems.back();

    if (end->type == 0) {
        if (isfunc) {
            shared_ptr<Obj> decl[3];
            shared_ptr<IRcode> t = make_shared<IRcode>(OpReturn, decl);
            IRCodeList.push_back(t);
        }
    } else {
        shared_ptr<StmtAST> stmt = dynamic_pointer_cast<StmtAST>(end->item);

        if (stmt->type != 7 && isfunc) {
            //没有return.
            shared_ptr<Obj> decl[3];
            shared_ptr<IRcode> t = make_shared<IRcode>(OpReturn, decl);
            IRCodeList.push_back(t);
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
    shared_ptr<FuncSym> func;


    cout << "var_offset is " << var_offset << endl;
    cout << "func is " << funcName << endl;
    //param:

    if (!symTable.Func.empty()) {
        int len = symTable.Func.size();
        symTable.Func[len - 1]->setOffset(var_offset);
//        cout << symTable.Func[len - 1]->name << " 的 offset是 " << var_offset << endl;
    }
    var_offset = 0;

    if (funcFParams != nullptr) {
        int paraNum = funcFParams->funcFParams.size();
        vector<shared_ptr<VarSym>> exps;

        int tmp = var_offset;
        for (int i = 0; i < funcFParams->funcFParams.size(); ++i) {
            programFuncFParam(funcFParams->funcFParams[i], exps);
        }

        func = make_shared<FuncSym>(funcName, paraNum, exps, defineType.top());

        symTable.Func.push_back(func);
    } else {
        func = make_shared<FuncSym>(funcName, defineType.top());
        symTable.Func.push_back(func);
    }

    shared_ptr<Obj> decl[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(funcName, func)};
    shared_ptr<IRcode> t = make_shared<IRcode>(OpFunc, decl);
    IRCodeList.push_back(t); //int func()


    shared_ptr<BlockAST> &block = funcDef->block;
    programBLock(block, true);

    NowLevel--;
    defineType.pop();
}

void IRcodeMaker::programMainDef(shared_ptr<MainFuncDefAST> &mainFunc) {
    string name = mainFunc->name;
    defineType.push(INT);
    NowLevel++;

    if (!symTable.Func.empty()) {
        int len = symTable.Func.size();
        symTable.Func[len - 1]->setOffset(var_offset);
//        cout << symTable.Func[len - 1]->name << " 的 offset是 " << var_offset << endl;
    }
    shared_ptr<FuncSym> func = make_shared<FuncSym>(name, defineType.top());
    var_offset = 0;
    symTable.Func.push_back(func);

    shared_ptr<Obj> decl[3] = {make_shared<Obj>(defineType.top()), make_shared<Obj>(name, func)};
    shared_ptr<IRcode> t = make_shared<IRcode>(OpFunc, decl);
    IRCodeList.push_back(t); //int main()

    shared_ptr<BlockAST> &block = mainFunc->block;
    programBLock(block, false);


    NowLevel--;
    defineType.pop();
}

void IRcodeMaker::program() {
    vector<shared_ptr<DeclAST>> &decls = (compUnitAst->decls);
    for (int i = 0; i < decls.size(); ++i) {
        this->programDecl(decls[i]);
    }

    shared_ptr<Obj> decl[3];
    shared_ptr<IRcode> t = make_shared<IRcode>(OpJMain, decl);
    IRCodeList.push_back(t); //j main();

    vector<shared_ptr<FuncDefAST>> &funcs = compUnitAst->funcs;
    for (int i = 0; i < funcs.size(); ++i) {
        programFunc(funcs[i]);
    }

    isMain = true;
    if (compUnitAst->main != nullptr) {
        shared_ptr<MainFuncDefAST> &mainFunc = compUnitAst->main;
        programMainDef(mainFunc);
        int len = symTable.Func.size();
        symTable.Func[len - 1]->setOffset(var_offset);
//        cout << symTable.Func[len - 1]->name << " 的 offset是 " << var_offset << endl;
    }
    isMain = false;
    return;
}


shared_ptr<Obj> IRcodeMaker::newValue() {
    static int count = -1;
    count++;
    string temp = "tmpVar_" + to_string(count);
    shared_ptr<Obj> obj = make_shared<Obj>(temp);
    return obj;
}
