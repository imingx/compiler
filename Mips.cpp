//
// Created by 郭哲 on 2021/11/24.
//

#include "include/Mips.h"

stack<shared_ptr<Obj>> funcParamsStack;

void Mips::saveValue(shared_ptr<Obj> &obj, string reg) {
    shared_ptr<VarSym> var = obj->var;
//    if (var->isArrParam) {
//        fprintf(out, "lw $s0, 4($fp)\n");
//
//    } else
    if (var->dim == 0) {
        //sw单个数据
        int address = var->level == 0 ? (var->offset) * 4 : -(var->offset) * 4;
        string preg = var->level == 0 ? "$gp" : "$fp";
        fprintf(out, "sw %s %d(%s)\n", reg.c_str(), address, preg.c_str());
    } else if (var->dim == 1 || var->dim == 2) {
        //sw,一维数组,二维数组
        int address = var->level == 0 ? (var->offset) * 4 : -(var->offset) * 4;
        string preg = var->level == 0 ? "$gp" : "$fp";
        if (obj->branch == 3) {
            //n[x]
            shared_ptr<Obj> index = obj->index;
            int value1;
            bool isNum1;
            loadValue(index, "$t2", value1, isNum1, true);

            if (var->isArrParam) {
                string preg = "$fp";
                int address = -(var->offset) * 4;
                fprintf(out, "lw $t1, %d(%s)\n", address, preg.c_str());
                //t1存放offset.
                fprintf(out, "addu $t1, $gp, $t1\n");
            } else
                fprintf(out, "addi $t1, %s, %d\n", preg.c_str(), address);

            fprintf(out, "sll $t2, $t2, 2\n");
            string r = var->level == 0 ? "subu" : "addu";
            if (var->isArrParam) r = "subu";
            fprintf(out, "%s $t2, $t1, $t2\n", r.c_str());
            fprintf(out, "sw %s 0($t2)\n", reg.c_str());
        } else if (obj->branch == 4) {
            //n[1]
            int num_index = obj->num_index;
            if (var->isArrParam) {
                string preg = "$fp";
                int address = -(var->offset) * 4;
                fprintf(out, "lw $t1, %d(%s)\n", address, preg.c_str());
                //t1存放offset.
                fprintf(out, "addu $t1, $gp, $t1\n");
                fprintf(out, "sw %s %d($t1)\n", reg.c_str(), -num_index * 4);
            } else {
                address = var->level == 0 ? address - num_index * 4 : address + num_index * 4;
                fprintf(out, "sw %s %d(%s)\n", reg.c_str(), address, preg.c_str());
            }
        }
    }
}

void Mips::loadValue(shared_ptr<Obj> &obj, string reg, int &value, bool &isNum, bool useful) {
    if (obj->branch == 5) {
        fprintf(out, "li %s, %d\n", reg.c_str(), obj->num);
        return;
    }

    shared_ptr<VarSym> var = obj->var;

    if (var->dim == 0) {
        //lw单个数据
        int address = var->level == 0 ? (var->offset) * 4 : -(var->offset) * 4;
        string preg = var->level == 0 ? "$gp" : "$fp";
        if (useful)
            fprintf(out, "lw %s %d(%s)\n", reg.c_str(), address, preg.c_str());
    } else if (var->dim == 1 || var->dim == 2) {
        //一维数组,二维数组
        int address = var->level == 0 ? (var->offset) * 4 : -(var->offset) * 4;
        string preg = var->level == 0 ? "$gp" : "$fp";
        if (obj->branch == 3) {
            //n[x]
            shared_ptr<Obj> index = obj->index;
            int value1;
            bool isNum1;
            loadValue(index, "$t2", value1, isNum1, true);
            if (var->isArrParam) {
                string preg = "$fp";
                int address = -(var->offset) * 4;
                fprintf(out, "lw $t1, %d(%s)\n", address, preg.c_str());
                //t1存放offset.
                fprintf(out, "addu $t1, $gp, $t1\n");
            } else
                fprintf(out, "addi $t1, %s, %d\n", preg.c_str(), address);

            fprintf(out, "sll $t2, $t2, 2\n");
            string r = var->level == 0 ? "subu" : "addu";
            fprintf(out, "%s $t2, $t1, $t2\n", r.c_str());
            fprintf(out, "lw %s 0($t2)\n", reg.c_str());
        } else if (obj->branch == 4) {
            //n[1]
            int num_index = obj->num_index;
            if (var->isArrParam) {
                string preg = "$fp";
                int address = -(var->offset) * 4;
                fprintf(out, "lw $t1, %d(%s)\n", address, preg.c_str());
                //t1存放offset.
                fprintf(out, "addu $t1, $gp, $t1\n");
                fprintf(out, "lw %s %d($t1)\n", reg.c_str(), -num_index * 4);
            } else {
                address = var->level == 0 ? address - num_index * 4 : address + num_index * 4;
                fprintf(out, "lw %s %d(%s)\n", reg.c_str(), address, preg.c_str());
            }
        }
    }
}

void Mips::program() {

    int value1, value2;
    bool isNum1, isNum2;

    fprintf(out, ".data\n");
    for (int i = 0; i < stringData.size(); ++i) {
        fprintf(out, "%s: .asciiz \"%s\"\n", stringData[i].label.c_str(), stringData[i].content.c_str());
    }
    fprintf(out, "nLine: .asciiz \"\\n\"\n");

    fprintf(out, "\n.text\n");

    for (int i = 0; i < IRCodeList.size(); ++i) {
        shared_ptr<IRcode> ircode = IRCodeList[i];

        if (ircode->op == OpPrintInt) {
            //输出int
            loadValue(ircode->obj[0], "$a0", value1, isNum1, true);
            fprintf(out, "li $v0, 1\n");
            SYSCALL
        } else if (ircode->op == OpPrint) {
            //输出纯字符串
            fprintf(out, "la $a0, %s\n", ircode->obj[0]->name.c_str());
            fprintf(out, "li $v0, 4\n");
            SYSCALL
        } else if (ircode->op == OpScanf) {
            //读入int
            fprintf(out, "li $v0, 5\n");
            SYSCALL
            //v0存放读入的值
            saveValue(ircode->obj[0], "$v0");
        } else if (ircode->op == OpVar) {
            if (ircode->obj[2] != nullptr) {
                loadValue(ircode->obj[2], "$t0", value1, isNum1, true);
                saveValue(ircode->obj[1], "$t0");
            }
        } else if (ircode->op == OpConst) {
            loadValue(ircode->obj[2], "$t0", value1, isNum1, true);
            saveValue(ircode->obj[1], "$t0");
        } else if (ircode->op == OpFunc) {
            shared_ptr<FuncSym> func = ircode->obj[1]->func;//函数
            fprintf(out, "%s:\n", ("func_" + ircode->obj[1]->name).c_str());

            if (func->name.compare("main") == 0) {
                fprintf(out, "move $fp, $sp\n");
                fprintf(out, "addi $sp, $sp, %d\n", -(4 * func->offset + 8));
            }

        } else if (ircode->op == OpPush) {
            funcParamsStack.push(ircode->obj[0]);
        } else if (ircode->op == OpPara) {
            //函数的参数
        } else if (ircode->op == OpCall) {
            shared_ptr<Obj> obj = ircode->obj[0];
            shared_ptr<FuncSym> func = obj->func;

            vector<shared_ptr<VarSym>> exps = func->exps;
            //这个存储函数的每个VarSym;
            string name = obj->name;
            int size = func->paraNum;

            while (size > 0) {
                shared_ptr<Obj> param = funcParamsStack.top();
                shared_ptr<VarSym> var = param->var;
                funcParamsStack.pop();
                //可能传入纯数字。
                if (var != nullptr && var->dim != 0 &&
                    (var->dim == exps[size - 1]->dim || (var->dim == 2 && exps[size - 1]->dim == 1))) {
                    //传入数组参数。
                    if (exps[size - 1]->dim == 1 && var->dim == 1) {
                        //一维传一维
                        exps[size - 1]->offset = var->offset;
                        fprintf(out, "li $t0, %d\n", exps[size - 1]->offset * 4);
                        fprintf(out, "sw $t0, %d($sp)\n", -4 * size);
                    } else if (exps[size - 1]->dim == 1 && var->dim == 2) {
                        //二维传入一维。
                        if (param->branch == 4) {
                            //传入了 func(b[4])  , int func(int b[]){}
                            exps[size - 1]->offset = var->offset - param->num_index * var->exps[1];
                            fprintf(out, "li $t0, %d\n", exps[size - 1]->offset * 4);
                            fprintf(out, "sw $t0, %d($sp)\n", -4 * size);
                        } else {
                            //传入了 func(b[c])之类的东西。
                            loadValue(param->index, "$t0", value1, isNum1, true);
                            //计算第几个
                            fprintf(out, "li $t1, %d\n", var->exps[1] * 4);
                            fprintf(out, "mult $t0, $t1\n");
                            fprintf(out, "mflo $t2\n");
                            fprintf(out, "li $t1, %d\n", var->offset * 4);
                            fprintf(out, "subu $t0, $t1, $t2\n");
//                            fprintf(out, "li $t0, %d\n", exps[size - 1]->offset * 4);
                            fprintf(out, "sw $t0, %d($sp)\n", -4 * size);
                        }
                    } else if (exps[size - 1]->dim == 2) {
                        //二维传二维
                        exps[size - 1]->offset = var->offset;
                        fprintf(out, "li $t0, %d\n", exps[size - 1]->offset * 4);
                        fprintf(out, "sw $t0, %d($sp)\n", -4 * size);
                    }

                } else {
                    loadValue(param, "$t0", value1, isNum1, true);
                    fprintf(out, "sw $t0, %d($sp)\n", -4 * size);
                }
                size--;
            }
            int funcOffset = 4 * func->offset + 8;
            fprintf(out, "sw $ra, ($sp)\n");
            fprintf(out, "sw $fp, 4($sp)\n");
            fprintf(out, "addi $sp, $sp, %d\n", -funcOffset);

            fprintf(out, "addi $fp, $sp %d\n", funcOffset);
            fprintf(out, "jal %s\n", ("func_" + name).c_str());
            fprintf(out, "addi $sp, $sp, %d\n", funcOffset);
            fprintf(out, "lw $fp, 4($sp)\n");
            fprintf(out, "lw $ra, ($sp)\n");
        } else if (ircode->op == OpReturn) {
            if (ircode->obj[0] != nullptr) {
                loadValue(ircode->obj[0], "$v0", value1, isNum1, true);
            }
            fprintf(out, "jr $ra\n\n");
        } else if (ircode->op == OpExit) {
            fprintf(out, "li $v0, 10\n");
            SYSCALL
        } else if (ircode->op == OpPLUS) {
            loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[2], "$t1", value2, isNum2, true);

            fprintf(out, "addu $t2, $t0, $t1\n");
            saveValue(ircode->obj[0], "$t2");
        } else if (ircode->op == OpMINU) {
            loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[2], "$t1", value2, isNum2, true);

            fprintf(out, "subu $t2, $t0, $t1\n");
            saveValue(ircode->obj[0], "$t2");
        } else if (ircode->op == OpDIV) {
            loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[2], "$t1", value2, isNum2, true);

            fprintf(out, "div $t0, $t1\n");
            fprintf(out, "mflo $t2\n");
            saveValue(ircode->obj[0], "$t2");
        } else if (ircode->op == OpMULT) {
            loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[2], "$t1", value2, isNum2, true);

            fprintf(out, "mult $t0, $t1\n");
            fprintf(out, "mflo $t2\n");
            saveValue(ircode->obj[0], "$t2");
        } else if (ircode->op == OpMOD) {
            loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[2], "$t1", value2, isNum2, true);

            fprintf(out, "div $t0, $t1\n");
            fprintf(out, "mfhi $t2\n");
            saveValue(ircode->obj[0], "$t2");
        } else if (ircode->op == OpAssign) {
            if (ircode->obj[1]->name == "RET") {
                fprintf(out, "move $t0, $v0\n");
            } else
                loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
            saveValue(ircode->obj[0], "$t0");
        } else if (ircode->op == OpJMain) {
            fprintf(out, "\nj func_main\n\n");
        } else if (ircode->op == OpLSS) {
            loadValue(ircode->obj[0], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[1], "$t1", value2, isNum2, true);
            fprintf(out, "blt $t0, $t1, %s\n", ircode->obj[2]->name.c_str());
        } else if (ircode->op == OpLEQ) {
            loadValue(ircode->obj[0], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[1], "$t1", value2, isNum2, true);
            fprintf(out, "ble $t0, $t1, %s\n", ircode->obj[2]->name.c_str());
        } else if (ircode->op == OpGRE) {
            loadValue(ircode->obj[0], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[1], "$t1", value2, isNum2, true);
            fprintf(out, "bgt $t0, $t1, %s\n", ircode->obj[2]->name.c_str());
        } else if (ircode->op == OpGEQ) {
            loadValue(ircode->obj[0], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[1], "$t1", value2, isNum2, true);
            fprintf(out, "bge $t0, $t1, %s\n", ircode->obj[2]->name.c_str());
        } else if (ircode->op == OpEQL) {
            loadValue(ircode->obj[0], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[1], "$t1", value2, isNum2, true);
            fprintf(out, "beq $t0, $t1, %s\n", ircode->obj[2]->name.c_str());
        } else if (ircode->op == OpNEQ) {
            loadValue(ircode->obj[0], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[1], "$t1", value2, isNum2, true);
            fprintf(out, "bne $t0, $t1, %s\n", ircode->obj[2]->name.c_str());
        }else if (ircode->op == OpJmp) {
            fprintf(out, "j %s\n", ircode->obj[0]->name.c_str());
        } else if (ircode->op == OpLabel) {
            fprintf(out, "%s:\n", ircode->obj[0]->name.c_str());
        }
    }
}


Mips::Mips(const char *out) {
    this->out = fopen(out, "w");
}
