//
// Created by gmm on 2021/10/16.
//

#include "include/Parser.h"

void Parser::syncTokIndex() {
    PreviewIndex = index - 1;
    if (PreviewIndex < words.size()) {
        PreviewTok = words[PreviewIndex++].category;
    } else {
        PreviewTok = EOF;
    }
}

int Parser::getCurTokenLine() {
    if (index > 0) {
        return words[index - 1].line;
    }
    return 1;
}

int Parser::getLastTokenLine() {
    if (index > 1) {
        return words[index - 2].line;
    }
    return 1;
}

void Parser::setIndex(int ind) {
    this->index = ind - 1;
    if (index < words.size()) {
        identifierStr = words[index].raw;
        CurTok = words[index++].category;
    } else {
        CurTok = EOF;
    }
}

int Parser::getNextToken() {
    if (index < words.size()) {
        identifierStr = words[index].raw;
        PreviewIndex = index + 1;
#ifdef ParserPrint
        if (index > 0) {
            fprintf(out, "%s %s\n", tokenName[words[index - 1].category], words[index - 1].raw.c_str());
            cout << tokenName[words[index - 1].category] << " " << words[index - 1].raw << endl;
        }
#endif
        return CurTok = words[index++].category;
    } else {
#ifdef ParserPrint
        if (index > 0) {
            fprintf(out, "%s %s\n", tokenName[words[index - 1].category], words[index - 1].raw.c_str());
            cout << tokenName[words[index - 1].category] << " " << words[index - 1].raw << endl;
        }
#endif
        return CurTok = EOF;
    }
}

int Parser::PreviewNextToken() {
    if (PreviewIndex < words.size()) {
        return PreviewTok = words[PreviewIndex++].category;
    } else {
        return EOF;
    }
}

void Parser::program() {
    getNextToken();
    handleCompUnit();
}

Parser::Parser(const char *FILE_OUT) {
    this->out = fopen(FILE_OUT, "w");
    this->index = 0;
    this->nowLevel = 0;
}

Parser::~Parser() {
    fclose(out);
}

void Parser::handleCompUnit() {
    if ((AST = parseCompUnit())) {
#ifdef ParserPrint
        fprintf(out, "<CompUnit>\n");
        cout << "<CompUnit>" << endl;
#endif
    }
}

shared_ptr<FuncTypeAST> Parser::parseFuncType() {
    int type = CurTok;
    if (type != VOIDTK && type != INTTK) {
        fprintf(stderr, "parseFuncType error!\n");
        exit(-1);
    }
    getNextToken();
    return make_shared<FuncTypeAST>(type, identifierStr);
}

shared_ptr<FuncFParamAST> Parser::parseFuncFParam() {
    if (CurTok != INTTK) {
        fprintf(stderr, "parseFuncFParam error!\n");
        exit(-1);
    }
    shared_ptr<BTypeAST> bType;
    string name;
    vector<shared_ptr<ConstExpAST>> constExps;

    bType = parseBType();

    if (CurTok != IDENFR) {
        fprintf(stderr, "error!\n");
        exit(-1);
    }
    name = identifierStr;
    int identLine = getCurTokenLine();
    getNextToken();
    if (CurTok == LBRACK) {
        getNextToken();
        if (CurTok != RBRACK) { // ]
            errors.push_back({getLastTokenLine(), "k"});
            printf("%d k, ]缺失-----------------\n", getLastTokenLine());
        } else
            getNextToken();
        constExps.push_back(nullptr);
        while (CurTok == LBRACK) {
            getNextToken();
            auto t = parseConstExp();
#ifdef ParserPrint
            fprintf(out, "<ConstExp>\n");
            cout << "<ConstExp>" << endl;
#endif
            constExps.push_back(move(t));
            if (CurTok != RBRACK) { // ]
                errors.push_back({getLastTokenLine(), "k"});
                printf("%d k, ]缺失-----------------\n", getLastTokenLine());
            } else
                getNextToken();
        }
        return make_shared<FuncFParamAST>(move(bType), name, move(constExps), identLine);
    } else {
        return make_shared<FuncFParamAST>(move(bType), name, move(constExps), identLine);
    }
    return nullptr;
}

shared_ptr<FuncFParamsAST> Parser::parseFuncFParams() {
    vector<shared_ptr<FuncFParamAST>> funcFParams;

    if (CurTok != INTTK) {
        fprintf(stderr, "parseFuncFParams error!1\n");
        exit(-1);
    }
    auto t = parseFuncFParam();
    funcFParams.push_back(move(t));
#ifdef ParserPrint
    fprintf(out, "<FuncFParam>\n");
    cout << "<FuncFParam>" << endl;
#endif
    while (CurTok == COMMA) {
        getNextToken();
        if (CurTok != INTTK) {
            fprintf(stderr, "parseFuncFparams error!2\n");
            exit(-1);
        }
        auto t = parseFuncFParam();
#ifdef ParserPrint
        fprintf(out, "<FuncFParam>\n");
        cout << "<FuncFParam>" << endl;
#endif
        funcFParams.push_back(move(t));
    }
    return make_shared<FuncFParamsAST>(move(funcFParams));
}


shared_ptr<FuncDefAST> Parser::parseFuncDef() {
    shared_ptr<FuncTypeAST> funcType;
    string name;
    shared_ptr<FuncFParamsAST> funcFParams = nullptr;
    shared_ptr<BlockAST> block;

    if (CurTok != VOIDTK && CurTok != INTTK) {
        fprintf(stderr, "parseFuncDef error1\n");
        exit(-1);
    }
    funcType = parseFuncType();
#ifdef ParserPrint
    fprintf(out, "<FuncType>\n");
    cout << "<FuncType>" << endl;
#endif
    if (CurTok != IDENFR) {
        fprintf(stderr, "parseFuncDef error2\n");
        exit(-2);
    }
    name = identifierStr;
    int identLine = getCurTokenLine();
    getNextToken();
    if (CurTok != LPARENT) {
        fprintf(stderr, "parseFuncDef error3\n");
        exit(-3);
    }
    getNextToken();

    if (CurTok == INTTK) {
        funcFParams = parseFuncFParams();
#ifdef ParserPrint
        fprintf(out, "<FuncFParams>\n");
        cout << "<FuncFParams>" << endl;
#endif
    }
    if (CurTok != RPARENT) { // )
        errors.push_back({getLastTokenLine(), "j"});
        printf("%d j, )缺失-----------------\n", getLastTokenLine());
    } else
        getNextToken(); // get {
    if (CurTok != LBRACE) {
        fprintf(stderr, "parseFuncDef error4\n");
        exit(-4);
    }
    block = parseBlock();
#ifdef ParserPrint
    fprintf(out, "<Block>\n");
    cout << "<Block>" << endl;
#endif
    return make_shared<FuncDefAST>(move(funcType), name, move(funcFParams), move(block), identLine);
}


shared_ptr<EqExpAST> Parser::parseEqExp() {
    vector<shared_ptr<RelExpAST>> relExps;
    vector<int> symbols; // ==  !=
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseRelExp();
            relExps.push_back(move(t));
#ifdef ParserPrint
            fprintf(out, "<RelExp>\n");
            cout << "<RelExp>" << endl;
#endif
            while (CurTok == EQL || CurTok == NEQ) {
#ifdef ParserPrint
                fprintf(out, "<EqExp>\n");
                cout << "<EqExp>" << endl;
#endif
                symbols.push_back(CurTok);
                getNextToken();
                auto t = parseRelExp();
#ifdef ParserPrint
                fprintf(out, "<RelExp>\n");
                cout << "<RelExp>" << endl;
#endif
                relExps.push_back(move(t));
            }
            return make_shared<EqExpAST>(symbols, move(relExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

shared_ptr<RelExpAST> Parser::parseRelExp() {
    vector<shared_ptr<AddExpAST>> addExps;
    vector<int> symbols; // ==  !=
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseAddExp();
#ifdef ParserPrint
            fprintf(out, "<AddExp>\n");
            cout << "<AddExp>" << endl;
#endif
            addExps.push_back(move(t));

            while (CurTok == LSS || CurTok == LEQ || CurTok == GRE || CurTok == GEQ) {
#ifdef ParserPrint
                fprintf(out, "<RelExp>\n");
                cout << "<RelExp>" << endl;
#endif
                symbols.push_back(CurTok);
                getNextToken();
                auto t = parseAddExp();
#ifdef ParserPrint
                fprintf(out, "<AddExp>\n");
                cout << "<AddExp>" << endl;
#endif
                addExps.push_back(move(t));
            }
            return make_shared<RelExpAST>(symbols, move(addExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

shared_ptr<LAndExpAST> Parser::parseLAndExp() {
    vector<shared_ptr<EqExpAST>> eqExps;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseEqExp();
#ifdef ParserPrint
            fprintf(out, "<EqExp>\n");
            cout << "<EqExp>" << endl;
#endif
            eqExps.push_back(move(t));
            while (CurTok == AND) {
#ifdef ParserPrint
                fprintf(out, "<LAndExp>\n");
                cout << "<LAndExp>" << endl;
#endif
                getNextToken();
                auto t = parseEqExp();
#ifdef ParserPrint
                fprintf(out, "<EqExp>\n");
                cout << "<EqExp>" << endl;
#endif
                eqExps.push_back(move(t));
            }
            return make_shared<LAndExpAST>(move(eqExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

shared_ptr<LOrExpAST> Parser::parseLOrExp() {
    vector<shared_ptr<LAndExpAST>> lAndExps;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseLAndExp();
#ifdef ParserPrint
            fprintf(out, "<LAndExp>\n");
            cout << "<LAndExp>" << endl;
#endif
            lAndExps.push_back(move(t));
            while (CurTok == OR) {
#ifdef ParserPrint
                fprintf(out, "<LOrExp>\n");
                cout << "<LOrExp>" << endl;
#endif
                getNextToken();
                auto t = parseLAndExp();
#ifdef ParserPrint
                fprintf(out, "<LAndExp>\n");
                cout << "<LAndExp>" << endl;
#endif
                lAndExps.push_back(move(t));
            }
            return make_shared<LOrExpAST>(move(lAndExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

shared_ptr<CondAST> Parser::parseCond() {
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseLOrExp();
#ifdef ParserPrint
            fprintf(out, "<LOrExp>\n");
            cout << "<LOrExp>" << endl;
#endif
            return make_shared<CondAST>(move(t));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

shared_ptr<StmtAST> Parser::parseStmt() {
    //1 lval = exp
    shared_ptr<LValAST> lVal;
    shared_ptr<ExpAST> exp;

    //2 [exp];
    shared_ptr<ExpAST> expSingle = nullptr;

    //3 block
    shared_ptr<BlockAST> block;

    //4 if
    shared_ptr<CondAST> condIf;
    shared_ptr<StmtAST> stmtIf = nullptr;
    shared_ptr<StmtAST> stmtElse = nullptr;

    //5 while
    shared_ptr<CondAST> condWhile = nullptr;
    shared_ptr<StmtAST> stmtWhile = nullptr;

    //6 break or continue;
    int category;

    //7 return;
    shared_ptr<ExpAST> expReturn = nullptr;

    //8 lval = getint();
    shared_ptr<LValAST> lValGetint = nullptr;

    //9 printf
    string formatString;
    vector<shared_ptr<ExpAST>> expsPrintf;

    switch (CurTok) {
        case PRINTFTK: {
            int printfLine = getCurTokenLine();
            getNextToken();
            if (CurTok != LPARENT) {
                fprintf(stderr, "parseStmt error!\n");
                exit(-1);
            }
            getNextToken();
            if (CurTok != STRCON) {
                fprintf(stderr, "parseStmt error!\n");
                exit(-1);
            }
            formatString = identifierStr;
            for (int i = 0; i < formatString.size(); ++i) {
                if (i == 0 || i == formatString.size() - 1)
                    continue;
                if (formatString[i] == '%') {
                    if (formatString[i + 1] == 'd') {
                        ++i;
                        continue;
                    } else {
                        errors.push_back({getCurTokenLine(), "a"});
                        printf("%d a, 非法字符---------\n", getCurTokenLine());
                        break;
                    }
                }
                if (formatString[i] == '\\') {
                    if (formatString[i + 1] == 'n') {
                        ++i;
                        continue;
                    } else {
                        errors.push_back({getCurTokenLine(), "a"});
                        printf("%d a, 非法字符---------\n", getCurTokenLine());
                        break;
                    }
                }
                if (formatString[i] == 32 || formatString[i] == 33 ||
                    (formatString[i] >= 40 && formatString[i] <= 126)) {
                    continue;
                } else {
                    errors.push_back({getCurTokenLine(), "a"});
                    printf("%d a, 非法字符---------\n", getCurTokenLine());
                    break;
                }
            }

            getNextToken();
            while (CurTok == COMMA) {
                getNextToken();
                auto t = parseExp();
#ifdef ParserPrint
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                expsPrintf.push_back(move(t));
            }
            if (CurTok != RPARENT) { // )
                errors.push_back({getLastTokenLine(), "j"});
                printf("%d j, )缺失-----------------\n", getLastTokenLine());
            } else
                getNextToken(); //get ;
            if (CurTok != SEMICN) {
                errors.push_back({getLastTokenLine(), "i"});
                printf("%d i, 分号缺失--------------\n", getLastTokenLine());
            } else
                getNextToken(); //get next
            return make_shared<StmtAST>(formatString, move(expsPrintf), printfLine);
        }
            break;
        case RETURNTK: {
            int returnLine = getCurTokenLine();
            getNextToken();
            if (CurTok == SEMICN) {
                getNextToken();
                return make_shared<StmtAST>(move(expReturn), "RETURN", returnLine);
            } else {
                expReturn = parseExp();
#ifdef ParserPrint
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                if (CurTok != SEMICN) {
                    errors.push_back({getLastTokenLine(), "i"});
                    printf("%d i, 分号缺失--------------\n", getLastTokenLine());
                } else
                    getNextToken();
                return make_shared<StmtAST>(move(expReturn), "RETURN", returnLine);
            }
        }
            break;
        case BREAKTK:
        case CONTINUETK: {
            int bcLine = getCurTokenLine();
            category = CurTok;
            getNextToken(); //get ;
            if (CurTok != SEMICN) {
                errors.push_back({getLastTokenLine(), "i"});
                printf("%d i, 分号缺失----------------\n", getLastTokenLine());
            } else
                getNextToken(); //get next
            return make_shared<StmtAST>(category, bcLine);
        }
            break;
        case WHILETK: {
            getNextToken(); // get (
            getNextToken();
            condWhile = parseCond();
#ifdef ParserPrint
            fprintf(out, "<Cond>\n");
            cout << "<Cond>" << endl;
#endif
            // now we get )
            if (CurTok != RPARENT) { // )
                errors.push_back({getLastTokenLine(), "j"});
                printf("%d j, )缺失-----------------\n", getLastTokenLine());
            } else
                getNextToken();
            stmtWhile = parseStmt();
#ifdef ParserPrint
            fprintf(out, "<Stmt>\n");
            cout << "<Stmt>" << endl;
#endif
            return make_shared<StmtAST>(move(condWhile), move(stmtWhile));
        }
            break;
        case IFTK: {
            getNextToken(); //get (
            getNextToken();
            condIf = parseCond();
#ifdef ParserPrint
            fprintf(out, "<Cond>\n");
            cout << "<Cond>" << endl;
#endif
            if (CurTok != RPARENT) { // )
                errors.push_back({getLastTokenLine(), "j"});
                printf("%d j, )缺失-----------------\n", getLastTokenLine());
            } else
                getNextToken();
            stmtIf = parseStmt();
#ifdef ParserPrint
            fprintf(out, "<Stmt>\n");
            cout << "<Stmt>" << endl;
#endif
            if (CurTok == ELSETK) {
                getNextToken();
                stmtElse = parseStmt();
#ifdef ParserPrint
                fprintf(out, "<Stmt>\n");
                cout << "<Stmt>" << endl;
#endif
            }
            return make_shared<StmtAST>(move(condIf), move(stmtIf), move(stmtElse));
        }
            break;
        case LBRACE: {
            block = parseBlock();
#ifdef ParserPrint
            fprintf(out, "<Block>\n");
            cout << "<Block>" << endl;
#endif
            return make_shared<StmtAST>(move(block));
        }
            break;
        case SEMICN: {
            getNextToken();
            return make_shared<StmtAST>(move(expSingle));
        }
            break;
        case PLUS:
        case MINU:
        case LPARENT:
        case INTCON: {
            expSingle = parseExp(); //get ;
#ifdef ParserPrint
            fprintf(out, "<Exp>\n");
            cout << "<Exp>" << endl;
#endif
            if (CurTok != SEMICN) {
                errors.push_back({getLastTokenLine(), "i"});
                printf("%d i, 分号缺失---------------\n", getLastTokenLine());
            } else
                getNextToken(); //get next
            return make_shared<StmtAST>(move(expSingle));
        }
            break;
        case IDENFR: {
            int identLine = getCurTokenLine();
            PreviewNextToken();
            if (PreviewTok == LPARENT) { // 下面是函数 func();
                expSingle = parseExp(); //get ;
#ifdef ParserPrint
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                if (CurTok != SEMICN) {
                    errors.push_back({getLastTokenLine(), "i"});
                    printf("%d i, 分号缺失---------------------\n", getLastTokenLine());
                } else
                    getNextToken(); //get next;
                return make_shared<StmtAST>(move(expSingle));
            }
            bool flag = false;
            int ind = this->index;
            syncTokIndex();
            parseLVal();
            if (CurTok == ASSIGN) {
                flag = true;
            }
            syncTokIndex();
//            cout << "now is " << tokenName[CurTok] << endl;
//            cout << "now is " << tokenName[PreviewTok] << endl;
//            cout << PreviewIndex;
            setIndex(ind);
//            while (PreviewTok != SEMICN) {
//                if (PreviewTok == ASSIGN) {
//                    flag = true;
//                    break;
//                }
//                PreviewNextToken();
//            }

            if (flag) {
                //有=
//                cout << "now is " << tokenName[CurTok] << endl;
//                cout << "now is " << tokenName[PreviewTok] << endl;
//                cout << PreviewIndex;
                PreviewNextToken();
//                cout << "now is " << tokenName[CurTok] << endl;
//                cout << "now is " << tokenName[PreviewTok] << endl;
//                cout << PreviewIndex;

                if (PreviewTok == GETINTTK) {
                    lValGetint = parseLVal(); // =
#ifdef ParserPrint
                    fprintf(out, "<LVal>\n");
                    cout << "<LVal>" << endl;
#endif
                    getNextToken();   // getint
                    getNextToken();   // (
                    getNextToken(); // )
                    if (CurTok != RPARENT) { // )
                        errors.push_back({getLastTokenLine(), "j"});
                        printf("%d j, )缺失-----------------\n", getLastTokenLine());
                    } else
                        getNextToken(); //;
                    if (CurTok != SEMICN) {
                        errors.push_back({getLastTokenLine(), "i"});
                        printf("%d i, 分号缺失-------------------\n", getLastTokenLine());
                    } else
                        getNextToken();  //next
                    return make_shared<StmtAST>(move(lValGetint), identLine);
                } else {
                    lVal = parseLVal(); // =
#ifdef ParserPrint
                    fprintf(out, "<LVal>\n");
                    cout << "<LVal>" << endl;
#endif
                    getNextToken(); // next
                    exp = parseExp(); //;
#ifdef ParserPrint
                    fprintf(out, "<Exp>\n");
                    cout << "<Exp>" << endl;
#endif
                    if (CurTok != SEMICN) {
                        errors.push_back({getLastTokenLine(), "i"});
                        printf("%d i, 分号缺失-------------------\n", getLastTokenLine());
                    } else
                        getNextToken(); //next
                    return make_shared<StmtAST>(move(lVal), move(exp), identLine);
                }
            } else {
                //没有=
                expSingle = parseExp(); //get ;
#ifdef ParserPrint
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                if (CurTok != SEMICN) {
                    errors.push_back({getLastTokenLine(), "i"});
                    printf("%d i, 分号缺失---------------------\n", getLastTokenLine());
                } else
                    getNextToken(); //get next;
                return make_shared<StmtAST>(move(expSingle));
            }

        }
            break;
    }
    return nullptr;
}

shared_ptr<BlockItemAST> Parser::parseBlockItem() {
    switch (CurTok) {
        case CONSTTK:
        case INTTK: {
            auto t = parseDecl();
            return make_shared<BlockItemAST>(move(t), 0);
        }
            break;
        case IDENFR:
        case SEMICN:

        case PLUS:
        case MINU:
        case LPARENT:
        case INTCON:

        case LBRACE:
        case IFTK:
        case WHILETK:
        case BREAKTK:
        case CONTINUETK:
        case RETURNTK:
        case PRINTFTK: {
            auto t = parseStmt();
#ifdef ParserPrint
            fprintf(out, "<Stmt>\n");
            cout << "<Stmt>" << endl;
#endif
            return make_shared<BlockItemAST>(move(t), 1);
        }
            break;
        default: {
            fprintf(stderr, "parseBlockItem wrong!\n");
            exit(-1);
        }
            break;
    }
    return nullptr;
}

shared_ptr<BlockAST> Parser::parseBlock() {
    vector<shared_ptr<BlockItemAST>> blockItems;
    if (CurTok != LBRACE) {
        fprintf(stderr, "parseBlock wrong!\n");
        exit(-1);
    }
    getNextToken();
    if (CurTok == RBRACE) {
        int rbraceLine = getCurTokenLine();
        getNextToken();
        return make_shared<BlockAST>(move(blockItems), rbraceLine);
    }

    while (true) {
        switch (CurTok) {
            case RBRACE:
                break;
            case CONSTTK:
            case INTTK:

            case IDENFR:
            case SEMICN:

            case PLUS:
            case MINU:
            case LPARENT:
            case INTCON:

            case LBRACE:
            case IFTK:
            case WHILETK:
            case BREAKTK:
            case CONTINUETK:
            case RETURNTK:
            case PRINTFTK: {
                auto t = parseBlockItem();
                blockItems.push_back(move(t));
            }
                break;
            default: {
                fprintf(stderr, "parseBlock error!\n");
                exit(-1);
            }
                break;
        }
        if (CurTok == RBRACE) {
            break;
        }
    }
    int rbraceLine = getCurTokenLine();
    getNextToken();
    return make_shared<BlockAST>(move(blockItems), rbraceLine);
}

shared_ptr<MainFuncDefAST> Parser::parseMainDef() {
    if (CurTok != INTTK) {
        fprintf(stderr, "parseMainDef wrong!\n");
        exit(-1);
    }
    getNextToken();
    int mainLine = getCurTokenLine();
    if (CurTok != MAINTK) {
        fprintf(stderr, "parseMainDef wrong!\n");
        exit(-1);
    }
    getNextToken();
    if (CurTok != LPARENT) {
        fprintf(stderr, "parseMainDef wrong!\n");
        exit(-1);
    }
    getNextToken();
    if (CurTok != RPARENT) { // )
        errors.push_back({getLastTokenLine(), "j"});
        printf("%d j, )缺失-----------------\n", getLastTokenLine());
    } else
        getNextToken();
    if (CurTok != LBRACE) {
        fprintf(stderr, "parseMainDef wrong!\n");
        exit(-1);
    }
    auto t = parseBlock();
#ifdef ParserPrint
    fprintf(out, "<Block>\n");
    cout << "<Block>" << endl;
#endif
    return make_shared<MainFuncDefAST>(move(t), mainLine);
}


//todo
shared_ptr<ConstInitValAST> Parser::parseConstInitVal() {
    shared_ptr<ConstExpAST> constExp;
    vector<shared_ptr<ConstInitValAST>> constInitVals;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON: {
            constExp = parseConstExp();
#ifdef ParserPrint
            fprintf(out, "<ConstExp>\n");
            cout << "<ConstExp>" << endl;
#endif
            return make_shared<ConstInitValAST>(move(constExp));
        }
            break;
        case LBRACE: {
            getNextToken();
            switch (CurTok) {
                case RBRACE: {
                    getNextToken();
                    return make_shared<ConstInitValAST>(move(constInitVals));
                }
                case PLUS:
                case MINU:
                case IDENFR:
                case LPARENT:
                case INTCON:
                case LBRACE: {
                    auto t = parseConstInitVal();
#ifdef ParserPrint
                    fprintf(out, "<ConstInitVal>\n");
                    cout << "<ConstInitVal>" << endl;
#endif
                    constInitVals.push_back(move(t));
                    while (CurTok == COMMA) {
                        getNextToken();
                        auto t = parseConstInitVal();
#ifdef ParserPrint
                        fprintf(out, "<ConstInitVal>\n");
                        cout << "<ConstInitVal>" << endl;
#endif
                        constInitVals.push_back(move(t));
                    }
                    if (CurTok != RBRACE) {
                        fprintf(stderr, "parseConstInitVal ERROR!\n");
                        exit(-1);
                    }
                    getNextToken();
                    return make_shared<ConstInitValAST>(move(constInitVals));
                }
                    break;
            }
        }
            break;
        default: {
            fprintf(stderr, "parseConstInitVal error!\n");
            exit(-1);
        }
    }
    return nullptr;
}

shared_ptr<NumberAST> Parser::parseNumber() {
    if (CurTok != INTCON) {
        fprintf(stderr, "parseNumber error!");
        exit(-1);
    }
    long long number = stoll(identifierStr.c_str());
    getNextToken();
    return make_shared<NumberAST>(number, identifierStr);
}

shared_ptr<LValAST> Parser::parseLVal() {
    if (CurTok != IDENFR) {
        fprintf(stderr, "parseLVal error!");
        exit(-1);
    }
    vector<shared_ptr<ExpAST>> exps;
    string name = identifierStr;
    int identLine = getCurTokenLine();
    getNextToken();
    while (CurTok == LBRACK) {
        getNextToken();
        switch (CurTok) {
            case PLUS:
            case MINU:
            case IDENFR:
            case LPARENT:
            case INTCON: {
                auto t = parseExp();
#ifdef ParserPrint
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                exps.push_back(move(t));
                if (CurTok != RBRACK) { // ]
                    errors.push_back({getLastTokenLine(), "k"});
                    printf("%d k, ]缺失-----------------\n", getLastTokenLine());
                } else
                    getNextToken();
            }
                break;
        }
    }
    return make_shared<LValAST>(name, move(exps), identLine);
}

shared_ptr<PrimaryExpAST> Parser::parsePrimaryExp() {
    switch (CurTok) {
        case INTCON: {
            auto t = parseNumber();
#ifdef ParserPrint
            fprintf(out, "<Number>\n");
            cout << "<Number>" << endl;
#endif
            return make_shared<PrimaryExpAST>(move(t), 3);
        }
        case LPARENT: {
            getNextToken();
            auto t = parseExp();
#ifdef ParserPrint
            fprintf(out, "<Exp>\n");
            cout << "<Exp>" << endl;
#endif
            if (CurTok != RPARENT) { // )
                errors.push_back({getLastTokenLine(), "j"});
                printf("%d j, )缺失-----------------\n", getLastTokenLine());
            } else
                getNextToken();
            return make_shared<PrimaryExpAST>(move(t), 1);
        }
            break;
        case IDENFR: {
            auto t = parseLVal();
#ifdef ParserPrint
            fprintf(out, "<LVal>\n");
            cout << "<LVal>" << endl;
#endif
            return make_shared<PrimaryExpAST>(move(t), 2);
        }
            break;
        default:
            break;
    }
    return nullptr;
}

shared_ptr<ExpAST> Parser::parseExp() {
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseAddExp();
#ifdef ParserPrint
            fprintf(out, "<AddExp>\n");
            cout << "<AddExp>" << endl;
#endif
            return make_shared<ExpAST>(move(t));
        }
            break;
        default:
            break;
    }
    return nullptr;
}

shared_ptr<FuncRParamsAST> Parser::parseFuncRParams() {
    vector<shared_ptr<ExpAST>> exps;
    while (true) {
        switch (CurTok) {
            case PLUS:
            case MINU:
            case IDENFR:
            case LPARENT:
            case INTCON: {
                auto t = parseExp();
#ifdef ParserPrint
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                exps.push_back(move(t));
            }
                break;
            default: {
                fprintf(stderr, "parseFuncRParams error!\n");
                exit(-1);
            }
        }
        if (CurTok != COMMA)
            break;
        else
            getNextToken();
    }
    return make_shared<FuncRParamsAST>(move(exps));
}

shared_ptr<UnaryOpAST> Parser::parseUnaryOp() {
    if (CurTok != PLUS && CurTok != MINU && CurTok != NOT) {
        fprintf(stderr, "parseUnaryOp error!");
    }
    getNextToken();
    return make_shared<UnaryOpAST>(CurTok);
}

shared_ptr<UnaryExpAST> Parser::parseUnaryExp() {

    switch (CurTok) {
        case PLUS:
        case MINU:
        case NOT: {
            auto t = parseUnaryOp();
#ifdef ParserPrint
            fprintf(out, "<UnaryOp>\n");
            cout << "<UnaryOp>" << endl;
#endif
            auto tt = parseUnaryExp();
#ifdef ParserPrint
            fprintf(out, "<UnaryExp>\n");
            cout << "<UnaryExp>" << endl;
#endif
            return make_shared<UnaryExpAST>(move(t), move(tt));
        }
            break;
        case INTCON:
        case LPARENT: {
            auto t = parsePrimaryExp();
#ifdef ParserPrint
            fprintf(out, "<PrimaryExp>\n");
            cout << "<PrimaryExp>" << endl;
#endif
            return make_shared<UnaryExpAST>(move(t));
        }
            break;
        case IDENFR: {
            syncTokIndex(); //因为之前预测过，需要同步indexx.
            PreviewNextToken();
            switch (PreviewTok) {
                case LPARENT: {
                    string name = identifierStr;
                    int identLine = getCurTokenLine();
                    getNextToken(); //get (
                    getNextToken(); //get FuncRParams
                    switch (CurTok) {
                        case PLUS:
                        case MINU:
                        case IDENFR:
                        case LPARENT:
                        case INTCON: {
                            auto t = parseFuncRParams();
#ifdef ParserPrint
                            fprintf(out, "<FuncRParams>\n");
                            cout << "<FuncRParams>" << endl;
#endif
                            if (CurTok != RPARENT) { // )
                                errors.push_back({getLastTokenLine(), "j"});
                                printf("%d j, )缺失-----------------\n", getLastTokenLine());
                            } else
                                getNextToken();
                            return make_shared<UnaryExpAST>(name, move(t), identLine);
                        }
                            break;
                        case RPARENT: {
                            getNextToken();
                            return make_shared<UnaryExpAST>(name, nullptr, identLine);
                        }
                            break;
                        default: {
                            if (CurTok != RPARENT) { // )
                                errors.push_back({getLastTokenLine(), "j"});
                                printf("%d j, )缺失-----------------\n", getLastTokenLine());
                            }
                            return make_shared<UnaryExpAST>(name, nullptr, identLine);
                        }
                            break;
                    }
                }
                    break;
                case LBRACK: {
                    auto t = parsePrimaryExp();
#ifdef ParserPrint
                    fprintf(out, "<PrimaryExp>\n");
                    cout << "<PrimaryExp>" << endl;
#endif
                    return make_shared<UnaryExpAST>(move(t));
                }
                default: { // Ident {  [ exp ] }
                    auto t = parsePrimaryExp();
#ifdef ParserPrint
                    fprintf(out, "<PrimaryExp>\n");
                    cout << "<PrimaryExp>" << endl;
#endif
                    return make_shared<UnaryExpAST>(move(t));
                }
            }
        }
            break;
        default: {
            fprintf(stderr, "parseUnaryExp wrong!3");
            exit(-1);
        }
    }

    return nullptr;
}

shared_ptr<MulExpAST> Parser::parseMulExp() {
    vector<shared_ptr<UnaryExpAST>> unaryExps;
    vector<int> symbol; // * % /
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseUnaryExp();
#ifdef ParserPrint
            fprintf(out, "<UnaryExp>\n");
            cout << "<UnaryExp>" << endl;
#endif
            unaryExps.push_back(move(t));
        }
            break;
        default:
            exit(-1);
    }

    while (CurTok == MULT || CurTok == DIV || CurTok == MOD) {
#ifdef ParserPrint
        fprintf(out, "<MulExp>\n");
        cout << "<MulExp>" << endl;
#endif
        symbol.push_back(CurTok);
        getNextToken();
        switch (CurTok) {
            case PLUS:
            case MINU:
            case IDENFR:
            case LPARENT:
            case INTCON:
            case NOT: {
                auto t = parseUnaryExp();
#ifdef ParserPrint
                fprintf(out, "<UnaryExp>\n");
                cout << "<UnaryExp>" << endl;
#endif
                unaryExps.push_back(move(t));
            }
                break;
            default:
                exit(-1);
        }
    }
    return make_shared<MulExpAST>(symbol, move(unaryExps));
}

//todo
shared_ptr<AddExpAST> Parser::parseAddExp() {
    vector<shared_ptr<MulExpAST>> mulExps;
    vector<int> symbol; // + 或 - 
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseMulExp();
#ifdef ParserPrint
            fprintf(out, "<MulExp>\n");
            cout << "<MulExp>" << endl;
#endif
            mulExps.push_back(move(t));
        }
            break;
        default:
            exit(-1);
    }

    while (CurTok == PLUS || CurTok == MINU) {
#ifdef ParserPrint
        fprintf(out, "<AddExp>\n");
        cout << "<AddExp>" << endl;
#endif
        symbol.push_back(CurTok);
        getNextToken();
        switch (CurTok) {
            case PLUS:
            case MINU:
            case IDENFR:
            case LPARENT:
            case INTCON:
            case NOT: {
                auto t = parseMulExp();
#ifdef ParserPrint
                fprintf(out, "<MulExp>\n");
                cout << "<MulExp>" << endl;
#endif
                mulExps.push_back(move(t));
            }
                break;
            default:
                exit(-1);
        }
    }
    return make_shared<AddExpAST>(symbol, move(mulExps));
}

shared_ptr<ConstExpAST> Parser::parseConstExp() {
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON: {
            auto t = parseAddExp();
#ifdef ParserPrint
            fprintf(out, "<AddExp>\n");
            cout << "<AddExp>" << endl;
#endif
            return make_shared<ConstExpAST>(move(t));
        }
            break;
        default:
            exit(-1);
    }
    return nullptr;
}

shared_ptr<ConstDefAST> Parser::parseConstDef() {
    string name;
    vector<shared_ptr<ConstExpAST>> constExps;
    shared_ptr<ConstInitValAST> constInitVal;
    if (CurTok != IDENFR)
        exit(-1);
    name = identifierStr;
    int identLine = getCurTokenLine();
    getNextToken();
    while (true) {
        if (CurTok == ASSIGN) {
            getNextToken();
            constInitVal = parseConstInitVal();
#ifdef ParserPrint
            fprintf(out, "<ConstInitVal>\n");
            cout << "<ConstInitVal>" << endl;
#endif
            break;
        } else if (CurTok == LBRACK) { //[
            getNextToken();
            auto t = parseConstExp();
#ifdef ParserPrint
            fprintf(out, "<ConstExp>\n");
            cout << "<ConstExp>" << endl;
#endif
            constExps.push_back(move(t));

            if (CurTok != RBRACK) { // ]
                errors.push_back({getLastTokenLine(), "k"});
                printf("%d k, ]缺失-----------------\n", getLastTokenLine());
            } else
                getNextToken();
        } else {
            exit(-1);
        }
    }

    return make_shared<ConstDefAST>(name, move(constExps), move(constInitVal), identLine);
}

shared_ptr<BTypeAST> Parser::parseBType() {
    switch (CurTok) {
        case INTTK:
            getNextToken();
            return make_shared<BTypeAST>(CurTok, identifierStr);
        default:
            exit(-1);
    }
    return nullptr;
}

shared_ptr<ConstDeclAST> Parser::parseConstDecl() {
    if (CurTok != CONSTTK)
        exit(-1);
    string symbol = identifierStr;
    shared_ptr<BTypeAST> btype;
    vector<shared_ptr<ConstDefAST>> constDefs;
    getNextToken();
    switch (CurTok) {
        case INTTK:
            btype = parseBType();
            break;
        default:
            exit(-1);
            break;
    }

    if (CurTok != IDENFR)
        exit(-1);
    auto t = parseConstDef();
#ifdef ParserPrint
    fprintf(out, "<ConstDef>\n");
    cout << "<ConstDef>" << endl;
#endif
    constDefs.push_back(move(t));

    while (CurTok == COMMA) {
        getNextToken();
        if (CurTok != IDENFR)
            exit(-1);
        auto t = parseConstDef();
#ifdef ParserPrint
        fprintf(out, "<ConstDef>\n");
        cout << "<ConstDef>" << endl;
#endif
        constDefs.push_back(move(t));
    }

    if (CurTok != SEMICN) {
        errors.push_back({getLastTokenLine(), "i"});
        printf("%d i, 分号缺失---------------\n", getLastTokenLine());
    } else
        getNextToken();
    return make_shared<ConstDeclAST>(symbol, move(btype), move(constDefs));
}

shared_ptr<InitValAST> Parser::parseInitVal() {
    shared_ptr<ExpAST> exp;
    vector<shared_ptr<InitValAST>> initVals;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON: {
            exp = parseExp();
#ifdef ParserPrint
            fprintf(out, "<Exp>\n");
            cout << "<Exp>" << endl;
#endif
            return make_shared<InitValAST>(move(exp));
        }
            break;
        case LBRACE: {
            getNextToken();
            switch (CurTok) {
                case RBRACE:
                    getNextToken();
                    return make_shared<InitValAST>(move(initVals));
                case PLUS:
                case MINU:
                case IDENFR:
                case LPARENT:
                case INTCON:
                case LBRACE: {
                    auto t = parseInitVal();
#ifdef ParserPrint
                    fprintf(out, "<InitVal>\n");
                    cout << "<InitVal>" << endl;
#endif
                    initVals.push_back(move(t));
                    while (CurTok == COMMA) {
                        getNextToken();
                        auto t = parseInitVal();
#ifdef ParserPrint
                        fprintf(out, "<InitVal>\n");
                        cout << "<InitVal>" << endl;
#endif
                        initVals.push_back(move(t));
                    }
                    if (CurTok != RBRACE) {
                        fprintf(stderr, "parseInitval ERROR!\n");
                        exit(-1);
                    }
                    getNextToken();
                    return make_shared<InitValAST>(move(initVals));
                }
                    break;
            }
        }
            break;
        default: {
            fprintf(stderr, "parseInitVal error!\n");
            exit(-1);
        }
    }
    return nullptr;
}

shared_ptr<VarDefAST> Parser::parseVarDef() {
    string name;
    vector<shared_ptr<ConstExpAST>> constExps;
    shared_ptr<InitValAST> initVal;
    if (CurTok != IDENFR) {
        fprintf(stderr, "parseVarDef error1\n");
        exit(-1);
    }
    name = identifierStr;
    int identLine = getCurTokenLine();
    getNextToken();
    while (CurTok == LBRACK) {
        getNextToken();
        auto t = parseConstExp();
#ifdef ParserPrint
        fprintf(out, "<ConstExp>\n");
        cout << "<ConstExp>" << endl;
#endif
        constExps.push_back(move(t));
        if (CurTok != RBRACK) { // ]
            errors.push_back({getLastTokenLine(), "k"});
            printf("%d k, ]缺失-----------------\n", getLastTokenLine());
        } else
            getNextToken();
    }

    if (CurTok == ASSIGN) {
        getNextToken();
        initVal = parseInitVal();
#ifdef ParserPrint
        fprintf(out, "<InitVal>\n");
        cout << "<InitVal>" << endl;
#endif
        return make_shared<VarDefAST>(name, move(constExps), move(initVal), identLine);
    } else {
        return make_shared<VarDefAST>(name, move(constExps), nullptr, identLine);
    }
    return nullptr;
}

shared_ptr<VarDeclAST> Parser::parseVarDecl() {
    shared_ptr<BTypeAST> btype;
    vector<shared_ptr<VarDefAST>> varDefs;
    if (CurTok != INTTK) {
        fprintf(stderr, "parseVarDecl error!");
        exit(-1);
    }
    btype = parseBType();

    auto t = parseVarDef();
#ifdef ParserPrint
    fprintf(out, "<VarDef>\n");
    cout << "<VarDef>" << endl;
#endif
    varDefs.push_back(move(t));

    while (CurTok == COMMA) {
        getNextToken();
        auto t = parseVarDef();
#ifdef ParserPrint
        fprintf(out, "<VarDef>\n");
        cout << "<VarDef>" << endl;
#endif
        varDefs.push_back(move(t));
    }

    if (CurTok != SEMICN) {
        errors.push_back({getLastTokenLine(), "i"});
        printf("%d i, 分号缺失---------------\n", getLastTokenLine());
    } else
        getNextToken();
    return make_shared<VarDeclAST>(move(btype), move(varDefs));
}

shared_ptr<DeclAST> Parser::parseDecl() {
    switch (CurTok) {
        case CONSTTK: {
            auto t = parseConstDecl();
#ifdef ParserPrint
            fprintf(out, "<ConstDecl>\n");
            cout << "<ConstDecl>" << endl;
#endif
            return make_shared<DeclAST>(move(t), 1);
        }
            break;
        case INTTK: {
            auto t = parseVarDecl();
#ifdef ParserPrint
            fprintf(out, "<VarDecl>\n");
            cout << "<VarDecl>" << endl;
#endif
            return make_shared<DeclAST>(move(t), 0);
        }
            break;
        default:
            exit(-1);
            break;
    }
    return nullptr;
}

shared_ptr<CompUnitAST> Parser::parseCompUnit() {
    vector<shared_ptr<DeclAST>> decls;
    vector<shared_ptr<FuncDefAST>> funcs;
    shared_ptr<MainFuncDefAST> main;
    while (true) {
        switch (CurTok) {
            case EOF:
                break;
//            case SEMICN:
//                getNextToken();
//                break;
            case CONSTTK: {
                auto decl = parseDecl();
                decls.push_back(move(decl));
            }
                break;
            case VOIDTK: {
                auto func = parseFuncDef();
#ifdef ParserPrint
                fprintf(out, "<FuncDef>\n");
                cout << "<FuncDef>" << endl;
#endif
                funcs.push_back(move(func));
                break;
            }
            case INTTK: {
                PreviewNextToken();
                switch (PreviewTok) {
                    case MAINTK: {
                        main = parseMainDef();
#ifdef ParserPrint
                        fprintf(out, "<MainFuncDef>\n");
                        cout << "<MainFuncDef>" << endl;
#endif
                    }
                        break;
                    case IDENFR: {
                        PreviewNextToken();
                        switch (PreviewTok) {
                            case LPARENT://(
                            {
                                auto func = parseFuncDef();
                                funcs.push_back(move(func));
#ifdef ParserPrint
                                fprintf(out, "<FuncDef>\n");
                                cout << "<FuncDef>" << endl;
#endif
                            }
                                break;
                            case ASSIGN: //=
                            case LBRACK: //[
                            case SEMICN: {
                                auto decl = parseDecl();
                                decls.push_back(move(decl));
                            }
                                break;
                            default:
                                break;
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
                break;
            default:
                break;
        }
        if (CurTok == EOF) {
            break;
        }
    }
    return make_shared<CompUnitAST>(move(decls), move(funcs), move(main));
}

shared_ptr<CompUnitAST> &Parser::getAST() {
    return AST;
}
