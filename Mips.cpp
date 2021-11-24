//
// Created by 郭哲 on 2021/11/24.
//

#include "include/Mips.h"

Mips::Mips(const char *out) {
    this->out = fopen(out, "w");
}

void Mips::saveValue(shared_ptr<Obj> &obj, string reg) {
    shared_ptr<VarSym> var = obj->var;
    if (var != nullptr) {
        int address;
        string preg;

        if (var->level == 0) {
            address = (var->offset) * 4;
            preg = "$gp";
        } else {
            address = -(var->offset) * 4;
            preg = "$fp";
        }
        fprintf(out, "sw %s, %d(%s)\n", reg.c_str(), address, preg.c_str());
    } else {
        fprintf(stderr, "error");
    }
}

void Mips::loadValue(shared_ptr<Obj> &obj, string reg, int &value, bool &isNum, bool useful) {
    if (obj->branch == 5) {
        fprintf(out, "li %s, %d\n", reg.c_str(), obj->num);
        return;
    }
    shared_ptr<VarSym> var = obj->var;
    if (var != nullptr) {
        int address;
        string preg;
        if (var->level == 0) {
            //全局变量
            address = (var->offset) * 4;
            preg = "$gp";
        } else {
            //局部变量
            address = -var->offset * 4;
            preg = "$fp";
        }
        if (useful)
            fprintf(out, "lw %s, %d(%s)\n", reg.c_str(), address, preg.c_str());
    } else {
        fprintf(stderr, "error");
    }
//    if (obj->branch == 5) { //num
//        value = obj->num;
//        fprintf(out, "li %s, %d\n", reg.c_str(), value);
//    } else if (obj->branch == 2) {
//        //str
//    } else if (obj->branch == 3) {
//        //str[tx]
//    } else if (obj->branch == 4) {
//        //str[1]
//    }
}

void Mips::program() {
    fprintf(out, ".data\n");
    for (int i = 0; i < stringData.size(); ++i) {
        fprintf(out, "%s: .asciiz \"%s\"\n", stringData[i].label.c_str(), stringData[i].content.c_str());
    }
    fprintf(out, "nLine: .asciiz \"\\n\"\n");

    fprintf(out, "\n.text\n");

    int value1, value2;
    bool isNum1, isNum2;
    for (int i = 0; i < IRCodeList.size(); ++i) {
        shared_ptr<IRcode> ircode = IRCodeList[i];

        if (ircode->op == OpPrintInt) {
            //输出int
            //loadValue to $a0, value1是其值。
//            loadValue("$a0", value1, ircode->obj[0]);
//            loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
            loadValue(ircode->obj[0], "$a0", value1, isNum1, true);
            fprintf(out, "li $v0, 1\n");
            SYSCALL
        } else if (ircode->op == OpPrint) {
            //输出纯字符串
            fprintf(out, "la $a0, %s\n", ircode->obj[0]->str.c_str());
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
            fprintf(out, "%s:\n", ("func_" + ircode->obj[1]->str).c_str());
        } else if (ircode->op == OpPush) {
            fprintf(out, "");
        } else if (ircode->op == OpCall) {
            //存
            fprintf(out, "jal %s\n");
            //取
        } else if (ircode->op == OpArray) {

        } else if (ircode->op == OpReturn) {

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
            loadValue(ircode->obj[1], "$t0", value1, isNum1, true);
//          loadValue(RS2, $t0, true, isImm1, value1);
            saveValue(ircode->obj[0], "$t0");
        }
    }
}