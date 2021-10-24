//
// Created by gmm on 2021/10/16.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include "TYPE.h"

template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts &&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

class Parser {
private:
    FILE *out;
    string identifierStr;
    int nowLevel;
    int index;
    int PreviewIndex;
    int CurTok;
    int PreviewTok;
    unique_ptr<CompUnitAST> AST;

public:
    unique_ptr<CompUnitAST> getAST();

    int PreviewNextToken();

    void syncTokIndex();

    Parser(const char *FILE_OUT);

    virtual ~Parser();

    int getNextToken();

    void program();

    //CompUnit
    void handleCompUnit();

    unique_ptr<CompUnitAST> parseCompUnit();

    //Decl
    unique_ptr<DeclAST> parseDecl();

    unique_ptr<ConstDeclAST> parseConstDecl();

    unique_ptr<VarDeclAST> parseVarDecl();

    unique_ptr<ConstDefAST> parseConstDef();

    unique_ptr<BTypeAST> parseBType();

    unique_ptr<ConstInitValAST> parseConstInitVal();

    unique_ptr<ConstExpAST> parseConstExp();

    unique_ptr<AddExpAST> parseAddExp();

    unique_ptr<MulExpAST> parseMulExp();

    unique_ptr<UnaryExpAST> parseUnaryExp();

    unique_ptr<PrimaryExpAST> parsePrimaryExp();

    unique_ptr<FuncRParamsAST> parseFuncRParams();

    unique_ptr<UnaryOpAST> parseUnaryOp();

    unique_ptr<ExpAST> parseExp();

    unique_ptr<NumberAST> parseNumber();

    unique_ptr<LValAST> parseLVal();

    unique_ptr<VarDefAST> parseVarDef();

    unique_ptr<InitValAST> parseInitVal();

    unique_ptr<FuncTypeAST> parseFuncType();

    unique_ptr<FuncFParamsAST> parseFuncFParams();

    unique_ptr<FuncFParamAST> parseFuncFParam();

    unique_ptr<BlockAST> parseBlock();

    unique_ptr<BlockItemAST> parseBlockItem();

    unique_ptr<CondAST> parseCond();

    unique_ptr<StmtAST> parseStmt();

    unique_ptr<LOrExpAST> parseLOrExp();

    unique_ptr<LAndExpAST> parseLAndExp();

    unique_ptr<EqExpAST> parseEqExp();

    unique_ptr<RelExpAST> parseRelExp();

    //FuncDef
    unique_ptr<FuncDefAST> parseFuncDef();

    unique_ptr<MainFuncDefAST> parseMainDef();

};

#endif //COMPILER_PARSER_H
