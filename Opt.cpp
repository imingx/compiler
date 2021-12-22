//
// Created by gmm on 2021/12/02.
//

#include "include/Opt.h"

void Opt::print() {
    for (int i = 0; i < IRCodeList.size(); ++i) {
        IRCodeList[i]->print();
    }
}

void Opt::program() {
#ifdef OPTIMIZE
    programFunc();
    programWhile();

#endif
}

void Opt::programFunc() {
    //查找全局变量。
    for (int i = 0; i < IRCodeList.size(); ++i) {
        OperatorType op = IRCodeList[i]->op;
        if (op == OpFunc) {
            string name = IRCodeList[i]->obj[1]->name;
            if (name.find("main") != name.npos) {
                return;
            }
            continue;
        }
        if (op == OpVar) continue;
        for (int j = 0; j < 3; ++j) {
            shared_ptr<Obj> obj = IRCodeList[i]->obj[j]; //
            if (obj != nullptr && obj->var != nullptr) {
                if (obj->var->level == 0) {
                    obj->var->needUse = true;
                }
            }
        }
    }
}

void Opt::programWhile() {
    vector<int> while_end;//记录while的end，帮助删除无用的while
    vector<bool> while_value;
    bool isCond = false;

    for (int i = IRCodeList.size() - 1; i >= 0; --i) {
        OperatorType op = IRCodeList[i]->op;
        if (op == OpScanf || op == OpCall) {
            for (int j = 0; j < while_value.size(); ++j) {
                while_value[j] = true;
            }
        } else if (op == OpPrintInt || op == OpReturn || op == OpPush || op == OpPrint) {
            shared_ptr<Obj> obj = IRCodeList[i]->obj[0]; //这是要输出的变量
            if (obj != nullptr && obj->var != nullptr) {
                if (obj->branch == 3 && obj->index != nullptr && obj->index->var != nullptr) {
                    obj->index->var->needUse = true;
                }
                obj->var->needUse = true;
            }
            for (int j = 0; j < while_value.size(); ++j) {
                while_value[j] = true;
            }

        } else if (op == OpVar) {
            shared_ptr<Obj> obj2 = IRCodeList[i]->obj[2]; //
            //var int a = 10;
            if (obj2 != nullptr && obj2->var != nullptr) {
                obj2->var->needUse = true;
                if (obj2->branch == 3 && obj2->index != nullptr && obj2->index->var != nullptr) {
                    obj2->index->var->needUse = true;
                }
            }

        } else if (op == OpAssign) {
            shared_ptr<Obj> obj0 = IRCodeList[i]->obj[0]; //这是被赋值的变量
            shared_ptr<Obj> obj1 = IRCodeList[i]->obj[1]; //这是赋值的变量
            if (!obj0->var->isArrParam && !obj0->var->needUse) {
//                if (obj1 != nullptr && obj1->var != nullptr && obj1->var->needUse == false) {
//                    obj1->var->needUse = false;
//                }

//                if (while_end.empty()) {
//                    IRCodeList.erase(IRCodeList.begin() + i);
//                } else {
//                    IRCodeList.erase(IRCodeList.begin() + i);
//                    for (int j = 0; j < while_end.size(); ++j) {
//                        while_end[j] = while_end[j] - 1;
//                    }
//                }
                if (while_value.empty()) {
                    //在循环外删除一些无用的东西。
                    IRCodeList.erase(IRCodeList.begin() + i);
                }
            } else {
                if (obj0->index != nullptr && obj0->index->var != nullptr) {
                    obj0->index->var->needUse = true;
                }

                if (obj1 != nullptr && obj1->var != nullptr) {
                    obj1->var->needUse = true;
                    if (obj1->branch == 3 && obj1->index != nullptr && obj1->index->var != nullptr) {
                        obj1->index->var->needUse = true;
                    }
                }
                for (int j = 0; j < while_value.size(); ++j) {
                    while_value[j] = true;
                }
            }
        } else if (op == OpNot || op == OpMULT || op == OpPLUS || op == OpMINU || op == OpDIV || op == OpMOD
                   || op == OpAnd || op == OpOr || op == OpSll || op == OpSrl || op == OpSrlv || op == OpSllv) {
            shared_ptr<Obj> obj0 = IRCodeList[i]->obj[0]; //
            shared_ptr<Obj> obj1 = IRCodeList[i]->obj[1]; //
            shared_ptr<Obj> obj2 = IRCodeList[i]->obj[2]; //

            if (!obj0->var->isArrParam && !obj0->var->needUse) {
//                if (obj1 != nullptr && obj1->var != nullptr)
//                    obj1->var->needUse = false;
//                if (obj2 != nullptr && obj2->var != nullptr)
//                    obj2->var->needUse = false;

//                if (while_end.empty()) {
//                    IRCodeList.erase(IRCodeList.begin() + i);
//                } else {
//                    IRCodeList.erase(IRCodeList.begin() + i);
//                    for (int j = 0; j < while_end.size(); ++j) {
//                        while_end[j] = while_end[j] - 1;
//                    }
//                }
                if (while_value.empty()) {
                    //在循环外删除一些无用的东西。
                    IRCodeList.erase(IRCodeList.begin() + i);
                }
            } else {
                if (obj0->index != nullptr && obj0->index->var != nullptr) {
                    obj0->index->var->needUse = true;
                }

                if (obj1 != nullptr && obj1->var != nullptr) {
                    obj1->var->needUse = true;
                    if (obj1->branch == 3 && obj1->index != nullptr && obj1->index->var != nullptr) {
                        obj1->index->var->needUse = true;
                    }
                }
                if (obj2 != nullptr && obj2->var != nullptr) {
                    obj2->var->needUse = true;
                    if (obj2->branch == 3 && obj2->index != nullptr && obj2->index->var != nullptr) {
                        obj2->index->var->needUse = true;
                    }
                }
                if (!isCond) {
                    for (int j = 0; j < while_value.size(); ++j) {
                        while_value[j] = true;
                    }
                }
            }
        } else if (op == OpLSS || op == OpLEQ || op == OpGRE || op == OpGEQ || op == OpEQL || op == OpNEQ) {
            shared_ptr<Obj> obj0 = IRCodeList[i]->obj[0]; //
            shared_ptr<Obj> obj1 = IRCodeList[i]->obj[1]; //

            if (isCond && while_value.back()) { //如果所在while有用
                if (obj0 != nullptr && obj0->var != nullptr) {
                    obj0->var->needUse = true;
                    if (obj0->branch == 3 && obj0->index != nullptr && obj0->index->var != nullptr) {
                        obj0->index->var->needUse = true;
                    }
                }
                if (obj1 != nullptr && obj1->var != nullptr) {
                    obj1->var->needUse = true;
                    if (obj1->branch == 3 && obj1->index != nullptr && obj1->index->var != nullptr) {
                        obj1->index->var->needUse = true;
                    }
                }
            } else {
                if (obj0 != nullptr && obj0->var != nullptr) {
                    obj0->var->needUse = true;
                    if (obj0->branch == 3 && obj0->index != nullptr && obj0->index->var != nullptr) {
                        obj0->index->var->needUse = true;
                    }
                }
                if (obj1 != nullptr && obj1->var != nullptr) {
                    obj1->var->needUse = true;
                    if (obj1->branch == 3 && obj1->index != nullptr && obj1->index->var != nullptr) {
                        obj1->index->var->needUse = true;
                    }
                }
            }
        } else if (op == OpSlt || op == OpSle || op == OpSge || op == OpSgt || op == OpSeq || op == OpSne) {
            shared_ptr<Obj> obj0 = IRCodeList[i]->obj[0]; //
            shared_ptr<Obj> obj1 = IRCodeList[i]->obj[1]; //
            shared_ptr<Obj> obj2 = IRCodeList[i]->obj[2]; //
            if (obj0 != nullptr && obj0->var != nullptr) {
                obj0->var->needUse = true;
                if (obj0->branch == 3 && obj0->index != nullptr && obj0->index->var != nullptr) {
                    obj0->index->var->needUse = true;
                }
            }
            if (obj1 != nullptr && obj1->var != nullptr) {
                obj1->var->needUse = true;
                if (obj1->branch == 3 && obj1->index != nullptr && obj1->index->var != nullptr) {
                    obj1->index->var->needUse = true;
                }
            }
            if (obj2 != nullptr && obj2->var != nullptr) {
                obj2->var->needUse = true;
                if (obj2->branch == 3 && obj2->index != nullptr && obj2->index->var != nullptr) {
                    obj2->index->var->needUse = true;
                }
            }
        } else if (op == OpLabel) {
            shared_ptr<Obj> obj = IRCodeList[i]->obj[0];
            string name = obj->name;
            if (name.find("while") != name.npos && name.find("end") != name.npos) {
                while_end.push_back(i + 1);
                while_value.push_back(false);//默认是无用的
            }
            if (name.find("while") != name.npos && name.find("begin") != name.npos) {
                isCond = true;
            }
            if (name.find("while") != name.npos && name.find("head") != name.npos) {
                isCond = false;
                int end = while_end.back();
                while_end.pop_back();
                bool value = while_value.back();
                while_value.pop_back();

//                fprintf(stderr, "%s is %d\n", name.c_str(), value);

                if (while_end.empty()) {
                    if (!value) {
                        IRCodeList.erase(IRCodeList.begin() + i, IRCodeList.begin() + end);
                    }
                } else {
                    if (!value) {
                        IRCodeList.erase(IRCodeList.begin() + i, IRCodeList.begin() + end);
                        for (int j = 0; j < while_end.size(); ++j) {
                            while_end[j] = while_end[j] - (end - i);
                        }
                    }
                }
            }
        }
    }
}

