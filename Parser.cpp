//
// Created by gmm on 2021/10/16.
//

#include "include/Parser.h"
#include "include/main.h"

map<int, int> BinopPrecedence;//优先级

void Parser::syncTokIndex() {
    PreviewIndex = index;
}

int Parser::getNextToken() {
    if (index < words.size()) {
        identifierStr = words[index].raw;
        PreviewIndex = index + 1;
#ifdef PRINT
        if (index > 0) {
            fprintf(out, "%s %s\n", tokenName[words[index - 1].category], words[index - 1].raw.c_str());
            cout << tokenName[words[index - 1].category] << " " << words[index - 1].raw << endl;
        }
#endif
        return CurTok = words[index++].category;
    } else {
#ifdef PRINT
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

void Parser::initBinopPrecedence() {

    BinopPrecedence[LPARENT] = 45; //(
    BinopPrecedence[RPARENT] = 45; //)
    BinopPrecedence[LBRACK] = 45; // [
    BinopPrecedence[RBRACK] = 45; // ]

    BinopPrecedence[NOT] = 40; // !

    BinopPrecedence[MULT] = 35; // *
    BinopPrecedence[DIV] = 35; // /
    BinopPrecedence[MOD] = 35; // %

    BinopPrecedence[PLUS] = 30; // +
    BinopPrecedence[MINU] = 30; // -

    BinopPrecedence[LSS] = 25; // <
    BinopPrecedence[LEQ] = 25; // <=
    BinopPrecedence[GRE] = 25; // >
    BinopPrecedence[GEQ] = 25; // >=

    BinopPrecedence[EQL] = 20; // ==
    BinopPrecedence[NEQ] = 20; // !=

    BinopPrecedence[AND] = 15; // &&
    BinopPrecedence[OR] = 10;  // ||
    BinopPrecedence[ASSIGN] = 5; // =
    BinopPrecedence[COMMA] = 1; // ,
}

void Parser::program() {
    getNextToken();
    handleCompUnit();
}

Parser::Parser(const char *FILE_OUT) {
    this->out = fopen(FILE_OUT, "w");
    this->index = 0;
    this->nowLevel = 0;
    this->initBinopPrecedence();
}

Parser::~Parser() {
    fclose(out);
}

void Parser::handleCompUnit() {
    if (AST = parseCompUnit()) {
#ifdef PRINT
        fprintf(out, "<CompUnit>\n");
        cout << "<CompUnit>" << endl;
#endif
    }
}

unique_ptr<FuncTypeAST> Parser::parseFuncType() {
    if (CurTok != VOIDTK && CurTok != INTTK) {
        fprintf(stderr, "parseFuncType error!\n");
        exit(-1);
    }
    getNextToken();
    return make_unique<FuncTypeAST>(CurTok, identifierStr);
}

unique_ptr<FuncFParamAST> Parser::parseFuncFParam() {
    if (CurTok != INTTK) {
        fprintf(stderr, "parseFuncFParam error!\n");
        exit(-1);
    }
    unique_ptr<BTypeAST> bType;
    string name;
    vector<unique_ptr<ConstExpAST>> constExps;

    bType = parseBType();

    if (CurTok != IDENFR) {
        fprintf(stderr, "error!\n");
        exit(-1);
    }
    name = identifierStr;
    getNextToken();
    if (CurTok == LBRACK) {
        getNextToken();
        if (CurTok != RBRACK) {
            fprintf(stderr, "error!\n");
            exit(-1);
        }
        constExps.push_back(nullptr);
        getNextToken();
        while (CurTok == LBRACK) {
            getNextToken();
            auto t = parseConstExp();
#ifdef PRINT
            fprintf(out, "<ConstExp>\n");
            cout << "<ConstExp>" << endl;
#endif
            constExps.push_back(move(t));
            if (CurTok != RBRACK) {
                fprintf(stderr, "error!\n");
                exit(-1);
            }
            getNextToken();
        }
        return make_unique<FuncFParamAST>(move(bType), name, move(constExps));
    } else {
        return make_unique<FuncFParamAST>(move(bType), name, move(constExps));
    }
    return nullptr;
}

unique_ptr<FuncFParamsAST> Parser::parseFuncFParams() {
    vector<unique_ptr<FuncFParamAST>> funcFParams;

    if (CurTok != INTTK) {
        fprintf(stderr, "parseFuncFParams error!\n");
        exit(-1);
    }
    auto t = parseFuncFParam();
    funcFParams.push_back(move(t));
#ifdef PRINT
    fprintf(out, "<FuncFParam>\n");
    cout << "<FuncFParam>" << endl;
#endif
    while (CurTok == COMMA) {
        getNextToken();
        if (CurTok != INTTK) {
            fprintf(stderr, "parseFuncFparams error!\n");
            exit(-1);
        }
        auto t = parseFuncFParam();
#ifdef PRINT
        fprintf(out, "<FuncFParam>\n");
        cout << "<FuncFParam>" << endl;
#endif
        funcFParams.push_back(move(t));
    }
    return make_unique<FuncFParamsAST>(move(funcFParams));
}


unique_ptr<FuncDefAST> Parser::parseFuncDef() {
    unique_ptr<FuncTypeAST> funcType;
    string name;
    unique_ptr<FuncFParamsAST> funcFParams;
    unique_ptr<BlockAST> block;

    if (CurTok != VOIDTK && CurTok != INTTK) {
        fprintf(stderr, "parseFuncDef error1\n");
        exit(-1);
    }
    funcType = parseFuncType();
#ifdef PRINT
    fprintf(out, "<FuncType>\n");
    cout << "<FuncType>" << endl;
#endif
    if (CurTok != IDENFR) {
        fprintf(stderr, "parseFuncDef error2\n");
        exit(-2);
    }
    name = identifierStr;
    getNextToken();
    if (CurTok != LPARENT) {
        fprintf(stderr, "parseFuncDef error3\n");
        exit(-3);
    }
    getNextToken();
    if (CurTok != RPARENT) {
        funcFParams = parseFuncFParams();
#ifdef PRINT
        fprintf(out, "<FuncFParams>\n");
        cout << "<FuncFParams>" << endl;
#endif
    }
    if (CurTok != RPARENT) {
        // get )
    }
    getNextToken(); // get {
    if (CurTok != LBRACE) {
        fprintf(stderr, "parseFuncDef error4\n");
        exit(-4);
    }
    block = parseBlock();
#ifdef PRINT
    fprintf(out, "<Block>\n");
    cout << "<Block>" << endl;
#endif
    return make_unique<FuncDefAST>(move(funcType), name, move(funcFParams), move(block));
}


unique_ptr<EqExpAST> Parser::parseEqExp() {
    vector<unique_ptr<RelExpAST>> relExps;
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
#ifdef PRINT
            fprintf(out, "<RelExp>\n");
            cout << "<RelExp>" << endl;
#endif
            while (CurTok == EQL || CurTok == NEQ) {
#ifdef PRINT
                fprintf(out, "<EqExp>\n");
                cout << "<EqExp>" << endl;
#endif
                symbols.push_back(CurTok);
                getNextToken();
                auto t = parseRelExp();
#ifdef PRINT
                fprintf(out, "<RelExp>\n");
                cout << "<RelExp>" << endl;
#endif
                relExps.push_back(move(t));
            }
            return make_unique<EqExpAST>(symbols, move(relExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

unique_ptr<RelExpAST> Parser::parseRelExp() {
    vector<unique_ptr<AddExpAST>> addExps;
    vector<int> symbols; // ==  !=
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseAddExp();
#ifdef PRINT
            fprintf(out, "<AddExp>\n");
            cout << "<AddExp>" << endl;
#endif
            addExps.push_back(move(t));

            while (CurTok == LSS || CurTok == LEQ || CurTok == GRE || CurTok == GEQ) {
#ifdef PRINT
                fprintf(out, "<RelExp>\n");
                cout << "<RelExp>" << endl;
#endif
                symbols.push_back(CurTok);
                getNextToken();
                auto t = parseAddExp();
#ifdef PRINT
                fprintf(out, "<AddExp>\n");
                cout << "<AddExp>" << endl;
#endif
                addExps.push_back(move(t));
            }
            return make_unique<RelExpAST>(symbols, move(addExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

unique_ptr<LAndExpAST> Parser::parseLAndExp() {
    vector<unique_ptr<EqExpAST>> eqExps;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseEqExp();
#ifdef PRINT
            fprintf(out, "<EqExp>\n");
            cout << "<EqExp>" << endl;
#endif
            eqExps.push_back(move(t));
            while (CurTok == AND) {
#ifdef PRINT
                fprintf(out, "<LAndExp>\n");
                cout << "<LAndExp>" << endl;
#endif
                getNextToken();
                auto t = parseEqExp();
#ifdef PRINT
                fprintf(out, "<EqExp>\n");
                cout << "<EqExp>" << endl;
#endif
                eqExps.push_back(move(t));
            }
            return make_unique<LAndExpAST>(move(eqExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

unique_ptr<LOrExpAST> Parser::parseLOrExp() {
    vector<unique_ptr<LAndExpAST>> lAndExps;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseLAndExp();
#ifdef PRINT
            fprintf(out, "<LAndExp>\n");
            cout << "<LAndExp>" << endl;
#endif
            lAndExps.push_back(move(t));
            while (CurTok == OR) {
#ifdef PRINT
                fprintf(out, "<LOrExp>\n");
                cout << "<LOrExp>" << endl;
#endif
                getNextToken();
                auto t = parseLAndExp();
#ifdef PRINT
                fprintf(out, "<LAndExp>\n");
                cout << "<LAndExp>" << endl;
#endif
                lAndExps.push_back(move(t));
            }
            return make_unique<LOrExpAST>(move(lAndExps));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

unique_ptr<CondAST> Parser::parseCond() {
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseLOrExp();
#ifdef PRINT
            fprintf(out, "<LOrExp>\n");
            cout << "<LOrExp>" << endl;
#endif
            return make_unique<CondAST>(move(t));
        }
            break;
        default: {
            fprintf(stderr, "error\n");
            exit(-1);
        }
    }
    return nullptr;
}

unique_ptr<StmtAST> Parser::parseStmt() {
    //1 lval = exp
    unique_ptr<LValAST> lVal;
    unique_ptr<ExpAST> exp;

    //2 [exp];
    unique_ptr<ExpAST> expSingle = nullptr;

    //3 block
    unique_ptr<BlockAST> block;

    //4 if
    unique_ptr<CondAST> condIf;
    unique_ptr<StmtAST> stmtIf = nullptr;
    unique_ptr<StmtAST> stmtElse = nullptr;

    //5 while
    unique_ptr<CondAST> condWhile = nullptr;
    unique_ptr<StmtAST> stmtWhile = nullptr;

    //6 break or continue;
    int category;

    //7 return;
    unique_ptr<ExpAST> expReturn = nullptr;

    //8 lval = getint();
    unique_ptr<LValAST> lValGetint = nullptr;

    //9 printf
    string formatString;
    vector<unique_ptr<ExpAST>> expsPrintf;

    switch (CurTok) {
        case PRINTFTK: {
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
            getNextToken();

            while (CurTok == COMMA) {
                getNextToken();
                auto t = parseExp();
#ifdef PRINT
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                expsPrintf.push_back(move(t));
            }
            if (CurTok == RPARENT) {
                getNextToken(); //get ;
                getNextToken(); //get next
                return make_unique<StmtAST>(formatString, move(expsPrintf));
            }
        }
            break;
        case RETURNTK: {
            getNextToken();
            if (CurTok == SEMICN) {
                getNextToken();
                return make_unique<StmtAST>(move(expReturn), "RETURN");
            } else {
                expReturn = parseExp();
#ifdef PRINT
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                getNextToken();
                return make_unique<StmtAST>(move(expReturn), "RETURN");
            }
        }
            break;
        case BREAKTK:
        case CONTINUETK: {
            category = CurTok;
            getNextToken(); //get ;
            getNextToken(); //get next
            return make_unique<StmtAST>(category);
        }
            break;
        case WHILETK: {
            getNextToken(); // get (
            getNextToken();
            condWhile = parseCond();
#ifdef PRINT
            fprintf(out, "<Cond>\n");
            cout << "<Cond>" << endl;
#endif
            // now we get )
            getNextToken();
            stmtWhile = parseStmt();
#ifdef PRINT
            fprintf(out, "<Stmt>\n");
            cout << "<Stmt>" << endl;
#endif
            return make_unique<StmtAST>(move(condWhile), move(stmtWhile));
        }
            break;
        case IFTK: {
            getNextToken(); //get (
            getNextToken();
            condIf = parseCond();
#ifdef PRINT
            fprintf(out, "<Cond>\n");
            cout << "<Cond>" << endl;
#endif
            getNextToken();
            stmtIf = parseStmt();
#ifdef PRINT
            fprintf(out, "<Stmt>\n");
            cout << "<Stmt>" << endl;
#endif
            if (CurTok == ELSETK) {
                getNextToken();
                stmtElse = parseStmt();
#ifdef PRINT
                fprintf(out, "<Stmt>\n");
                cout << "<Stmt>" << endl;
#endif
            }
            return make_unique<StmtAST>(move(condIf), move(stmtIf), move(stmtElse));
        }
            break;
        case LBRACE: {
            block = parseBlock();
#ifdef PRINT
            fprintf(out, "<Block>\n");
            cout << "<Block>" << endl;
#endif
            return make_unique<StmtAST>(move(block));
        }
            break;
        case SEMICN: {
            getNextToken();
            return make_unique<StmtAST>(move(expSingle));
        }
            break;
        case PLUS:
        case MINU:
        case LPARENT:
        case INTCON: {
            expSingle = parseExp(); //get ;
#ifdef PRINT
            fprintf(out, "<Exp>\n");
            cout << "<Exp>" << endl;
#endif
            getNextToken(); //get next
            return make_unique<StmtAST>(move(expSingle));
        }
            break;
        case IDENFR: {
            PreviewNextToken();
            if (PreviewTok == LPARENT) {
                expSingle = parseExp(); //get ;
#ifdef PRINT
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                getNextToken(); //get next;
                return make_unique<StmtAST>(move(expSingle));
            }
            bool flag = false;
            while (PreviewTok != SEMICN) {
                if (PreviewTok == ASSIGN) {
                    flag = true;
                    break;
                }
                PreviewNextToken();
            }
            if (flag) {
                //有=
                PreviewNextToken();
                if (PreviewTok == GETINTTK) {
                    lValGetint = parseLVal(); // =
#ifdef PRINT
                    fprintf(out, "<LVal>\n");
                    cout << "<LVal>" << endl;
#endif
                    getNextToken();   // getint
                    getNextToken();   // (
                    getNextToken(); // )
                    getNextToken(); //;
                    getNextToken();  //next
                    return make_unique<StmtAST>(move(lValGetint));
                } else {
                    lVal = parseLVal(); // =
#ifdef PRINT
                    fprintf(out, "<LVal>\n");
                    cout << "<LVal>" << endl;
#endif
                    getNextToken(); // next
                    exp = parseExp(); //;
#ifdef PRINT
                    fprintf(out, "<Exp>\n");
                    cout << "<Exp>" << endl;
#endif
                    getNextToken(); //nexxt
                    return make_unique<StmtAST>(move(lVal), move(exp));
                }
            } else {
                //没有=
                expSingle = parseExp(); //get ;
#ifdef PRINT
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                getNextToken(); //get next;
                return make_unique<StmtAST>(move(expSingle));
            }

        }
            break;
    }
    return nullptr;
}

unique_ptr<BlockItemAST> Parser::parseBlockItem() {
    switch (CurTok) {
        case CONSTTK:
        case INTTK: {
            auto t = parseDecl();
            return make_unique<BlockItemAST>(move(t));
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
#ifdef PRINT
            fprintf(out, "<Stmt>\n");
            cout << "<Stmt>" << endl;
#endif
            return make_unique<BlockItemAST>(move(t));
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

unique_ptr<BlockAST> Parser::parseBlock() {
    vector<unique_ptr<BlockItemAST>> blockItems;
    if (CurTok != LBRACE) {
        fprintf(stderr, "parseBlock wrong!\n");
        exit(-1);
    }
    getNextToken();
    if (CurTok == RBRACE) {
        getNextToken();
        return make_unique<BlockAST>(move(blockItems));
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
    getNextToken();
    return make_unique<BlockAST>(move(blockItems));
}

unique_ptr<MainFuncDefAST> Parser::parseMainDef() {
    if (CurTok != INTTK) {
        fprintf(stderr, "parseMainDef wrong!\n");
        exit(-1);
    }
    getNextToken();
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
    if (CurTok != RPARENT) {
        fprintf(stderr, "parseMainDef wrong!\n");
        exit(-1);
    }
    getNextToken();
    if (CurTok != LBRACE) {
        fprintf(stderr, "parseMainDef wrong!\n");
        exit(-1);
    }
    auto t = parseBlock();
#ifdef PRINT
    fprintf(out, "<Block>\n");
    cout << "<Block>" << endl;
#endif
    return make_unique<MainFuncDefAST>(move(t));
}


//todo
unique_ptr<ConstInitValAST> Parser::parseConstInitVal() {
    unique_ptr<ConstExpAST> constExp;
    vector<unique_ptr<ConstInitValAST>> constInitVals;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON: {
            constExp = parseConstExp();
#ifdef PRINT
            fprintf(out, "<ConstExp>\n");
            cout << "<ConstExp>" << endl;
#endif
            return make_unique<ConstInitValAST>(move(constExp));
        }
            break;
        case LBRACE: {
            getNextToken();
            switch (CurTok) {
                case RBRACE: {
                    getNextToken();
                    return make_unique<ConstInitValAST>(move(constInitVals));
                }
                case PLUS:
                case MINU:
                case IDENFR:
                case LPARENT:
                case INTCON:
                case LBRACE: {
                    auto t = parseConstInitVal();
#ifdef PRINT
                    fprintf(out, "<ConstInitVal>\n");
                    cout << "<ConstInitVal>" << endl;
#endif
                    constInitVals.push_back(move(t));
                    while (CurTok == COMMA) {
                        getNextToken();
                        auto t = parseConstInitVal();
#ifdef PRINT
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
                    return make_unique<ConstInitValAST>(move(constInitVals));
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

unique_ptr<NumberAST> Parser::parseNumber() {
    if (CurTok != INTCON) {
        fprintf(stderr, "parseNumber error!");
        exit(-1);
    }
    long long number = stoll(identifierStr.c_str());
    getNextToken();
    return make_unique<NumberAST>(number, identifierStr);
}

unique_ptr<LValAST> Parser::parseLVal() {
    if (CurTok != IDENFR) {
        fprintf(stderr, "parseLVal error!");
        exit(-1);
    }
    vector<unique_ptr<ExpAST>> exps;
    string name = identifierStr;
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
#ifdef PRINT
                fprintf(out, "<Exp>\n");
                cout << "<Exp>" << endl;
#endif
                exps.push_back(move(t));
                if (CurTok != RBRACK) {
                    fprintf(stderr, "parseLVal error!\n");
                    exit(-1);
                }
                getNextToken();
            }
                break;
        }
    }
    return make_unique<LValAST>(name, move(exps));
}

unique_ptr<PrimaryExpAST> Parser::parsePrimaryExp() {
    switch (CurTok) {
        case INTCON: {
            auto t = parseNumber();
#ifdef PRINT
            fprintf(out, "<Number>\n");
            cout << "<Number>" << endl;
#endif
            return make_unique<PrimaryExpAST>(move(t));
        }
        case LPARENT: {
            getNextToken();
            auto t = parseExp();
#ifdef PRINT
            fprintf(out, "<Exp>\n");
            cout << "<Exp>" << endl;
#endif
            if (CurTok != RPARENT) {
                fprintf(stderr, "parsePrimaryExp error!\n");
                exit(-1);
            }
            getNextToken();
            return make_unique<PrimaryExpAST>(move(t));
        }
            break;
        case IDENFR: {
            auto t = parseLVal();
#ifdef PRINT
            fprintf(out, "<LVal>\n");
            cout << "<LVal>" << endl;
#endif
            return make_unique<PrimaryExpAST>(move(t));
        }
            break;
        default:
            break;
    }
    return nullptr;
}

unique_ptr<ExpAST> Parser::parseExp() {
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseAddExp();
#ifdef PRINT
            fprintf(out, "<AddExp>\n");
            cout << "<AddExp>" << endl;
#endif
            return make_unique<ExpAST>(move(t));
        }
            break;
        default:
            break;
    }
    return nullptr;
}

unique_ptr<FuncRParamsAST> Parser::parseFuncRParams() {
    vector<unique_ptr<ExpAST>> exps;
    while (true) {
        switch (CurTok) {
            case PLUS:
            case MINU:
            case IDENFR:
            case LPARENT:
            case INTCON: {
                auto t = parseExp();
#ifdef PRINT
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
    return make_unique<FuncRParamsAST>(move(exps));
}

unique_ptr<UnaryOpAST> Parser::parseUnaryOp() {
    if (CurTok != PLUS && CurTok != MINU && CurTok != NOT) {
        fprintf(stderr, "parseUnaryOp error!");
    }
    getNextToken();
    return make_unique<UnaryOpAST>(CurTok);
}

unique_ptr<UnaryExpAST> Parser::parseUnaryExp() {

    switch (CurTok) {
        case PLUS:
        case MINU:
        case NOT: {
            auto t = parseUnaryOp();
#ifdef PRINT
            fprintf(out, "<UnaryOp>\n");
            cout << "<UnaryOp>" << endl;
#endif
            auto tt = parseUnaryExp();
#ifdef PRINT
            fprintf(out, "<UnaryExp>\n");
            cout << "<UnaryExp>" << endl;
#endif
            return make_unique<UnaryExpAST>(move(t), move(tt));
        }
            break;
        case INTCON:
        case LPARENT: {
            auto t = parsePrimaryExp();
#ifdef PRINT
            fprintf(out, "<PrimaryExp>\n");
            cout << "<PrimaryExp>" << endl;
#endif
            return make_unique<UnaryExpAST>(move(t));
        }
            break;
        case IDENFR: {
            syncTokIndex(); //因为之前预测过，需要同步indexx.
            PreviewNextToken();
            switch (PreviewTok) {
                case LPARENT: {
                    string name = identifierStr;
                    getNextToken();
                    if (CurTok != LPARENT) {
                        fprintf(stderr, "parseUnaryExp wrong!1");
                        exit(-1);
                    }
                    getNextToken();
                    switch (CurTok) {
                        case PLUS:
                        case MINU:
                        case IDENFR:
                        case LPARENT:
                        case INTCON: {
                            auto t = parseFuncRParams();
#ifdef PRINT
                            fprintf(out, "<FuncRParams>\n");
                            cout << "<FuncRParams>" << endl;
#endif
                            if (CurTok != RPARENT) {
                                fprintf(stderr, "parseUnaryExp wrong!2");
                                exit(-1);
                            }
                            getNextToken();
                            return make_unique<UnaryExpAST>(name, move(t));
                        }
                        case RPARENT: {
                            getNextToken();
                            return make_unique<UnaryExpAST>(name, nullptr);
                        }
                    }
                }
                    break;
                case LBRACK: {
                    auto t = parsePrimaryExp();
#ifdef PRINT
                    fprintf(out, "<PrimaryExp>\n");
                    cout << "<PrimaryExp>" << endl;
#endif
                    return make_unique<UnaryExpAST>(move(t));
                }
                default: { // Ident {  [ exp ] }
                    auto t = parsePrimaryExp();
#ifdef PRINT
                    fprintf(out, "<PrimaryExp>\n");
                    cout << "<PrimaryExp>" << endl;
#endif
                    return make_unique<UnaryExpAST>(move(t));
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

unique_ptr<MulExpAST> Parser::parseMulExp() {
    vector<unique_ptr<UnaryExpAST>> unaryExps;
    vector<int> symbol; // * % /
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseUnaryExp();
#ifdef PRINT
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
#ifdef PRINT
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
#ifdef PRINT
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
    return make_unique<MulExpAST>(symbol, move(unaryExps));
}

//todo
unique_ptr<AddExpAST> Parser::parseAddExp() {
    vector<unique_ptr<MulExpAST>> mulExps;
    vector<int> symbol; // + 或 - 
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON:
        case NOT: {
            auto t = parseMulExp();
#ifdef PRINT
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
#ifdef PRINT
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
#ifdef PRINT
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
    return make_unique<AddExpAST>(symbol, move(mulExps));
}

unique_ptr<ConstExpAST> Parser::parseConstExp() {
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON: {
            auto t = parseAddExp();
#ifdef PRINT
            fprintf(out, "<AddExp>\n");
            cout << "<AddExp>" << endl;
#endif
            return make_unique<ConstExpAST>(move(t));
        }
            break;
        default:
            exit(-1);
    }
    return nullptr;
}

unique_ptr<ConstDefAST> Parser::parseConstDef() {
    string name;
    vector<unique_ptr<ConstExpAST>> constExps;
    unique_ptr<ConstInitValAST> constInitVal;
    if (CurTok != IDENFR)
        exit(-1);
    name = identifierStr;
    getNextToken();
    while (true) {
        if (CurTok == ASSIGN) {
            getNextToken();
            constInitVal = parseConstInitVal();
#ifdef PRINT
            fprintf(out, "<ConstInitVal>\n");
            cout << "<ConstInitVal>" << endl;
#endif
            break;
        } else if (CurTok == LBRACK) {
            getNextToken();
            auto t = parseConstExp();
#ifdef PRINT
            fprintf(out, "<ConstExp>\n");
            cout << "<ConstExp>" << endl;
#endif
            constExps.push_back(move(t));
            if (CurTok != RBRACK)
                exit(-2);
            getNextToken();
        } else {
            exit(-1);
        }
    }

    return make_unique<ConstDefAST>(name, move(constExps), move(constInitVal));
}

unique_ptr<BTypeAST> Parser::parseBType() {
    switch (CurTok) {
        case INTTK:
            getNextToken();
            return make_unique<BTypeAST>(CurTok, identifierStr);
        default:
            exit(-1);
    }
    return nullptr;
}

unique_ptr<ConstDeclAST> Parser::parseConstDecl() {
    if (CurTok != CONSTTK)
        exit(-1);
    string symbol = identifierStr;
    unique_ptr<BTypeAST> btype;
    vector<unique_ptr<ConstDefAST>> constDefs;
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
#ifdef PRINT
    fprintf(out, "<ConstDef>\n");
    cout << "<ConstDef>" << endl;
#endif
    constDefs.push_back(move(t));

    while (CurTok == COMMA) {
        getNextToken();
        if (CurTok != IDENFR)
            exit(-1);
        auto t = parseConstDef();
#ifdef PRINT
        fprintf(out, "<ConstDef>\n");
        cout << "<ConstDef>" << endl;
#endif
        constDefs.push_back(move(t));
    }
    if (CurTok != SEMICN) {
        fprintf(stderr, "error\n");
        exit(-4);
    }
    getNextToken();
    return make_unique<ConstDeclAST>(symbol, move(btype), move(constDefs));
}

unique_ptr<InitValAST> Parser::parseInitVal() {
    unique_ptr<ExpAST> exp;
    vector<unique_ptr<InitValAST>> initVals;
    switch (CurTok) {
        case PLUS:
        case MINU:
        case IDENFR:
        case LPARENT:
        case INTCON: {
            exp = parseExp();
#ifdef PRINT
            fprintf(out, "<Exp>\n");
            cout << "<Exp>" << endl;
#endif
            return make_unique<InitValAST>(move(exp));
        }
            break;
        case LBRACE: {
            getNextToken();
            switch (CurTok) {
                case RBRACE:
                    return make_unique<InitValAST>(move(initVals));
                case PLUS:
                case MINU:
                case IDENFR:
                case LPARENT:
                case INTCON:
                case LBRACE: {
                    auto t = parseInitVal();
#ifdef PRINT
                    fprintf(out, "<InitVal>\n");
                    cout << "<InitVal>" << endl;
#endif
                    initVals.push_back(move(t));
                    while (CurTok == COMMA) {
                        getNextToken();
                        auto t = parseInitVal();
#ifdef PRINT
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
                    return make_unique<InitValAST>(move(initVals));
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

unique_ptr<VarDefAST> Parser::parseVarDef() {
    string name;
    vector<unique_ptr<ConstExpAST>> constExps;
    unique_ptr<InitValAST> initVal;
    if (CurTok != IDENFR) {
        fprintf(stderr, "parseVarDef error1\n");
        exit(-1);
    }
    name = identifierStr;
    getNextToken();
    while (CurTok == LBRACK) {
        getNextToken();
        auto t = parseConstExp();
#ifdef PRINT
        fprintf(out, "<ConstExp>\n");
        cout << "<ConstExp>" << endl;
#endif
        constExps.push_back(move(t));
        if (CurTok != RBRACK) {
            fprintf(stderr, "parseVarDef error2\n");
            exit(-1);
        }
        getNextToken();
    }

    if (CurTok == ASSIGN) {
        getNextToken();
        initVal = parseInitVal();
#ifdef PRINT
        fprintf(out, "<InitVal>\n");
        cout << "<InitVal>" << endl;
#endif
        return make_unique<VarDefAST>(name, move(constExps), move(initVal));
    } else {
        return make_unique<VarDefAST>(name, move(constExps), nullptr);
    }
    return nullptr;
}

unique_ptr<VarDeclAST> Parser::parseVarDecl() {
    unique_ptr<BTypeAST> btype;
    vector<unique_ptr<VarDefAST>> varDefs;
    if (CurTok != INTTK) {
        fprintf(stderr, "parseVarDecl error!");
        exit(-1);
    }
    btype = parseBType();

    auto t = parseVarDef();
#ifdef PRINT
    fprintf(out, "<VarDef>\n");
    cout << "<VarDef>" << endl;
#endif
    varDefs.push_back(move(t));

    while (CurTok == COMMA) {
        getNextToken();
        auto t = parseVarDef();
#ifdef PRINT
        fprintf(out, "<VarDef>\n");
        cout << "<VarDef>" << endl;
#endif
        varDefs.push_back(move(t));
    }

    if (CurTok != SEMICN) {
        fprintf(stderr, "parseVarDecl error!\n");
        exit(-1);
    }
    getNextToken();
    return make_unique<VarDeclAST>(move(btype), move(varDefs));
}

unique_ptr<DeclAST> Parser::parseDecl() {
    switch (CurTok) {
        case CONSTTK: {
            auto t = parseConstDecl();
#ifdef PRINT
            fprintf(out, "<ConstDecl>\n");
            cout << "<ConstDecl>" << endl;
#endif
            return make_unique<DeclAST>(move(t));
        }
            break;
        case INTTK: {
            auto t = parseVarDecl();
#ifdef PRINT
            fprintf(out, "<VarDecl>\n");
            cout << "<VarDecl>" << endl;
#endif
            return make_unique<DeclAST>(move(t));
        }
            break;
        default:
            exit(-1);
            break;
    }
    return nullptr;
}

unique_ptr<CompUnitAST> Parser::parseCompUnit() {
    vector<unique_ptr<DeclAST>> decls;
    vector<unique_ptr<FuncDefAST>> funcs;
    unique_ptr<MainFuncDefAST> main;
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
#ifdef PRINT
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
#ifdef PRINT
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
#ifdef PRINT
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
    return make_unique<CompUnitAST>(move(decls), move(funcs), move(main));
}

unique_ptr<CompUnitAST> Parser::getAST() {
    return move(AST);
}
