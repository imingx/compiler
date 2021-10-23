//
// Created by gmm on 2021/10/21.
//

#include "include/ErrorHandle.h"

ErrorHandle::ErrorHandle(const char * ERROR) {
    out = fopen(ERROR, "w");
}

ErrorHandle::~ErrorHandle() {
    fclose(out);
}
