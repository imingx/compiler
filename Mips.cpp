//
// Created by 郭哲 on 2021/11/24.
//

#include "include/Mips.h"


Mips::Mips(const char *out) {
    this->out = fopen(out, "w");
}

void Mips::loadValue(string reg, int &value, shared_ptr<Obj> &obj) {
    if (obj->branch == 5) { //num
        value = obj->num;
        fprintf(out, "li %s, %d\n", reg.c_str(), value);
    } else if (obj->branch == 2) {
        //str

    } else if (obj->branch == 3) {
        //str[tx]
    } else if (obj->branch == 4) {
        //str[1]
    }
}

void Mips::program() {
    fprintf(out, ".data\n");
    for (int i = 0; i < stringData.size(); ++i) {
        fprintf(out, "%s: .asciiz \"%s\"\n", stringData[i].label.c_str(), stringData[i].content.c_str());
    }
    fprintf(out, "nLine: .asciiz \"\\n\"\n");

    fprintf(out, ".text\n");

    int value1, value2;
    for (int i = 0; i < IRCodeList.size(); ++i) {
        shared_ptr<IRcode> ircode = IRCodeList[i];

        if (ircode->op == OpPrintInt) {
            //输出int
            //loadValue to $a0, value1是其值。
            loadValue("$a0", value1, ircode->obj[0]);
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

            fprintf(out, "sw $v0");


        } else if (ircode->op == OpVar) {

        } else if (ircode->op == OpConst) {

        } else if (ircode->op == OpFunc) {
            fprintf(out, ":");

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

        } else if (ircode->op == OpMINU) {

        } else if (ircode->op == OpDIV) {

        } else if (ircode->op == OpMULT) {

        } else if (ircode->op == OpMOD) {

        } else if (ircode->op == OpAssign) {

        }
    }
}