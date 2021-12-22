//
// Created by gmm on 2021/12/02.
//

#ifndef COMPILER_OPT_H
#define COMPILER_OPT_H

#include "IRcode.h"
#include "type.h"
#include <bits/stdc++.h>

extern vector<shared_ptr<IRcode>> IRCodeList;

class Opt {
public:
    void program();
    void print();
    void programFunc();
    void programWhile();
};


#endif //COMPILER_OPT_H
