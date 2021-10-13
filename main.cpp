#include "include/main.h"
using namespace std;

int main() {
    // lexer
    Lexer lexer(FILE_IN, FILE_OUT);
    lexer.analyse();

    return 0;
}
