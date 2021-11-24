//
// Created by 郭哲 on 2021/11/24.
//

#ifndef COMPILER_MIPS_H
#define COMPILER_MIPS_H

#include "IRcode.h"

using namespace std;

#define SYSCALL  fprintf(out, "syscall\n");

extern vector<shared_ptr<IRcode>> IRCodeList;
extern SymTable symTable;
extern vector<StringData> stringData;

class Mips {
private:
    FILE * out;
    void loadValue(string reg, int &value, shared_ptr<Obj> &obj);
public:
    Mips(const char * out);

    void program();
};


#endif //COMPILER_MIPS_H
