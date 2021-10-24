//
// Created by gmm on 2021/10/16.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include "TYPE.h"
#include "ErrorHandler.h"


class Parser {
private:
    FILE *out;
    string identifierStr;
    int nowLevel;
    int index;
    int PreviewIndex;
    int CurTok;
    int PreviewTok;
    shared_ptr<CompUnitAST> AST;

    int getLastTokenLine();

    int getCurTokenLine();

public:
    shared_ptr<CompUnitAST> &getAST();

    void setIndex(int ind);

    int PreviewNextToken();

    void syncTokIndex();

    Parser(const char *FILE_OUT);

    virtual ~Parser();

    int getNextToken();

    void program();

    //CompUnit
    void handleCompUnit();

    shared_ptr<CompUnitAST> parseCompUnit();

    //Decl
    shared_ptr<DeclAST> parseDecl();

    shared_ptr<ConstDeclAST> parseConstDecl();

    shared_ptr<VarDeclAST> parseVarDecl();

    shared_ptr<ConstDefAST> parseConstDef();

    shared_ptr<BTypeAST> parseBType();

    shared_ptr<ConstInitValAST> parseConstInitVal();

    shared_ptr<ConstExpAST> parseConstExp();

    shared_ptr<AddExpAST> parseAddExp();

    shared_ptr<MulExpAST> parseMulExp();

    shared_ptr<UnaryExpAST> parseUnaryExp();

    shared_ptr<PrimaryExpAST> parsePrimaryExp();

    shared_ptr<FuncRParamsAST> parseFuncRParams();

    shared_ptr<UnaryOpAST> parseUnaryOp();

    shared_ptr<ExpAST> parseExp();

    shared_ptr<NumberAST> parseNumber();

    shared_ptr<LValAST> parseLVal();

    shared_ptr<VarDefAST> parseVarDef();

    shared_ptr<InitValAST> parseInitVal();

    shared_ptr<FuncTypeAST> parseFuncType();

    shared_ptr<FuncFParamsAST> parseFuncFParams();

    shared_ptr<FuncFParamAST> parseFuncFParam();

    shared_ptr<BlockAST> parseBlock();

    shared_ptr<BlockItemAST> parseBlockItem();

    shared_ptr<CondAST> parseCond();

    shared_ptr<StmtAST> parseStmt();

    shared_ptr<LOrExpAST> parseLOrExp();

    shared_ptr<LAndExpAST> parseLAndExp();

    shared_ptr<EqExpAST> parseEqExp();

    shared_ptr<RelExpAST> parseRelExp();

    //FuncDef
    shared_ptr<FuncDefAST> parseFuncDef();

    shared_ptr<MainFuncDefAST> parseMainDef();

};

#endif //COMPILER_PARSER_H
