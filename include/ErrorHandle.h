//
// Created by gmm on 2021/10/21.
//

#ifndef COMPILER_ERRORHANDLE_H
#define COMPILER_ERRORHANDLE_H

#include <bits/stdc++.h>
using namespace std;

class ErrorHandle {
    FILE * out;
public:
    ErrorHandle(const char * ERROR);
    ~ErrorHandle();
};


#endif //COMPILER_ERRORHANDLE_H
