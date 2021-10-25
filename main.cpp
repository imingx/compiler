#include "include/main.h"

using namespace std;

int main() {

    /*
    1. 使用lexer.getNext(word)读取每个词
        Word word;
        while (lexer.getNext(word)) {
            cout << word.raw << " " << tokenName[word.category] << endl;
        }
    2. 或者使用lexer.program()直接将每个词存入Words，然后读取Words，适用于代码不多的情况。
        lexer.program();
        for(auto word : words) {
            cout << word.raw << " " << tokenName[word.category] << endl;
        }
    */

    // lexer
    Lexer lexer(FILE_IN, FILE_OUT);
    lexer.program();

    // parser
    Parser parser(FILE_OUT);
    parser.program();

//    shared_ptr<CompUnitAST> AST = parser.getAST();

    return 0;
}
