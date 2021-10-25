# 一、语法分析设计文档

## 1. 需求分析

基于词法分析器所识别单词，识别各类语法成分。

要求：

1. 按照规则，使用递归子程序法对定义中的语法成分进行分析。
2. 输出不仅需要包括词法分析的内容（不能输出预读的内容），还要在每个语法结束后，输出对应的语法成分名字，形如`<stmt>`。
3. 需要预留错误处理的接口。
4. 需要方便开启和关闭语法分析的输出结果。

## 2. 编码前的设计

- 想了想，为了方便预读，还是需要先完整地进行词法分析，关闭词法分析输出，读取到所有单词后，再进行语法分析。在语法分析内部，使用`GetNextToken`来读取下一个单词，并且同时输出单词的值和单词类别码。预读时，使用`PreviewNextToken`来读取，同时内部不进行输出。
- 我应该知道每个文法的分支的First集合，使用switch语句来调用对应分析函数。当遇到First集合有交集时，应该进行预读（不改变实际的读取位置），直到能确定分支走向。
- 应当初步进行错误处理，在遇到读取错误时，输出错误位置和错误码，方便进行调试和后续错误处理。
- 应当在语法分析时建立符号表和AST树。
- 使用`#ifdef`和`#endif`来实现控制语法分析输出。

## 3. 模块设计

**文件结构：**

```
.
├── AST.cpp
├── Lexer.cpp
├── Parser.cpp
├── SymbolTable.cpp
├── include
│   ├── AST.h
│   ├── Lexer.h
│   ├── Parser.h
│   ├── SymbolTable.h
│   ├── TYPE.h
│   └── main.h
└── main.cpp
```

### 3.1 TYPE.h

```c++
enum TOKEN_CATEGORY {
    IDENFR, INTCON, STRCON, MAINTK,
    CONSTTK, INTTK, BREAKTK, CONTINUETK, IFTK,
    ELSETK, NOT, AND, OR, WHILETK, GETINTTK,
    PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK,
    MULT, DIV, MOD, LSS, LEQ, GRE, GEQ, EQL,
    NEQ, ASSIGN, SEMICN, COMMA, LPARENT,
    RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
};

static const int TOKEN_NUM = 100;

static const char *tokenName[TOKEN_NUM] = {"IDENFR", "INTCON", "STRCON", "MAINTK",
                                           "CONSTTK", "INTTK", "BREAKTK", "CONTINUETK", "IFTK",
                                           "ELSETK", "NOT", "AND", "OR", "WHILETK", "GETINTTK",
                                           "PRINTFTK", "RETURNTK", "PLUS", "MINU", "VOIDTK",
                                           "MULT", "DIV", "MOD", "LSS", "LEQ", "GRE", "GEQ", "EQL",
                                           "NEQ", "ASSIGN", "SEMICN", "COMMA", "LPARENT",
                                           "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};

```

为了使其他类调用一些常量值，比如token数组和枚举类型识别码，我将这些常量存放在`TYPE.h`中。

在`main.h`中依旧存放着控制输出的语句`#define PRINT`，这次用来控制语法分析的输出，下次会将其移动到`TYPE.h`

### 3.2 语法分析器类设计

```c++
class Parser {
private:
    FILE *out;
    string identifierStr;
    int index;
    int PreviewIndex;
    int CurTok;
    int PreviewTok;
    shared_ptr<CompUnitAST> AST;
public:
    shared_ptr<CompUnitAST> getAST();
    int PreviewNextToken();
    void syncTokIndex();
    Parser(const char *FILE_OUT);
    virtual ~Parser();
    int getNextToken();
    void program();
    void handleCompUnit();
    //以下部分是解析各类语法成分的函数
    shared_ptr<CompUnitAST> parseCompUnit();
    shared_ptr<DeclAST> parseDecl();
    shared_ptr<ConstDeclAST> parseConstDecl();
    shared_ptr<VarDeclAST> parseVarDecl();
    shared_ptr<ConstDefAST> parseConstDef();
}
```

`out`是输出流，`identifierStr`是当前读取的单词值，`index`是词法分析读取的单词的索引（因为使用vector容器来存放读取的每个单词），`PreviewIndex`是预读取的索引，`CurTok`是当前读取的单词识别码，`PreviewTok`是预读取的单词识别码。

`AST`是待构建的AST树。使用`shared_ptr`共享指针来存放根节点。

`getNextToken()`和`PreviewNextToken()`分别是获取和预读取下一个单词识别码。`program()`是语法分析主函数。剩下的`parse*()`是解析各种语法成分的函数，返回值都是`shared_ptr`共享指针，方便构建AST语法树。

### 3.4 抽象语法树AST

```c++
class AST {
public:
    virtual ~AST() = default;
};

//编译单元语法树
class CompUnitAST : public AST {
    vector<shared_ptr<DeclAST>> decls;
    vector<shared_ptr<FuncDefAST>> funcs;
    shared_ptr<MainFuncDefAST> main;
public:
    CompUnitAST(vector<shared_ptr<DeclAST>> decls, vector<shared_ptr<FuncDefAST>> funcs,
                shared_ptr<MainFuncDefAST> main) : decls(move(decls)), funcs(move(funcs)), main(move(main)) {}
};

//函数定义语法树
class FuncDefAST : public AST {
    shared_ptr<FuncTypeAST> funcType;
    string name;
    shared_ptr<FuncFParamsAST> funcFParams;
    shared_ptr<BlockAST> block;
public:
    FuncDefAST(shared_ptr<FuncTypeAST> funcType,
               const string &name,
               shared_ptr<FuncFParamsAST> funcFParams,
               shared_ptr<BlockAST> block) : funcType(move(funcType)),
                                             name(name), funcFParams(move(funcFParams)), block(move(block)) {}
};
```

如上，在`AST.h`中存放了每个语法成分语法树的派生类，都继承自AST基类。均使用`shared_ptr`共享指针存放每个属性。当属性多于一个时，使用vector容器存储。同时在构造函数中，对每个传入的共享参数使用move方法来转接指针使用权。

### 3.5 语法分析方法框架

`getNextToken()`函数如下，每次调用该函数，会对`CurTok`和`identifierStr`赋值为当前读取的单词属性，然后同步`PreviewIndex`为当前的`index`（防止多次预读错误），然后输出读取单词的识别码和单词值。

这里注意到，因为我在后续读取时，会总是预先读取下一个单词，然后调用对应函数，会使得token提前输出，因此输出时，输出当前index-1的值。

```c++
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
```

主函数：按照正常的思路，每次递归调用子程序前，先预先读取下一个单词。同时对每个`parser*()`函数，返回值是一个语法树，需要进行记录。

```c++
void Parser::program() {
    getNextToken();
    handleCompUnit();
}

void Parser::handleCompUnit() {
    if (AST = parseCompUnit()) {
#ifdef PRINT
        fprintf(out, "<CompUnit>\n");
        cout << "<CompUnit>" << endl;
#endif
    }
}
```

下面展示解析CompUnit的代码：

由于进入函数之前已经预读取，所以进入函数直接开始进入循环，开始判断`CurTok`当前的识别码的值，直到读取到EOF。

根据规则`CompUnit → {Decl} {FuncDef} MainFuncDef  `，分别使用两个vector容器和一个`shared_ptr`记录结果。

如果是`CONSTTK`，则说明是常量声明，则调用`parseDecl`；如果是`VOIDTK`，则说明是函数声明，则调用`parseFuncDef`；如果是`INTTK`，则说明三者都有可能，则预读取下一个单词，如果是`MAINTK`，则说明是主函数，调用`parseMainDef`，否则如果是`IDENFR`，则无法判断是函数定义还是变量定义，还需要预读取，然后如果是`(`，则说明是函数定义，如果是`=`,`[`,`;`，则说明是变量定义。

最后返回构造的语法树，`return make_shared<CompUnitAST>(move(decls), move(funcs), move(main));`，`make_shared`会自动调用尖括号内的构造函数，返回`shared_ptr`。`move`函数是为了转交指针的使用权。

```c++
shared_ptr<CompUnitAST> Parser::parseCompUnit() {
    vector<shared_ptr<DeclAST>> decls;
    vector<shared_ptr<FuncDefAST>> funcs;
    shared_ptr<MainFuncDefAST> main;
    while (true) {
        switch (CurTok) {
            case EOF:
                break;
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
    return make_shared<CompUnitAST>(move(decls), move(funcs), move(main));
}

```



## 4. 编码后的对设计的修改情况

1. 我修改了词法分析的函数结构，将读取一个单词的过程封装为另一个函数`getNext()`，然后`program()`只要循环调用即可。

    ```c++
    void Lexer::program() {
        Word word;
        while (getNext(word) != -1);
    }
    
    int Lexer::getNext(Word &w) {
        bool flag = true;
        Word word;
        while (flag) {
            flag = false;
            char c = fgetc(in);
            string str;
            while (isspace(c)) {
                if (c == '\n') ++(this->line);
                c = fgetc(in);
            }
            if (c == EOF) {
                return -1;
            }
            if (isalpha(c) || c == '_') {
                do {
                    str += c;
                    c = fgetc(in);
                } while (isalnum(c) || c == '_');
                ungetc(c, in);
    
                map<string, TOKEN_CATEGORY>::iterator iterator = keyWord.find(str);
                if (iterator == keyWord.end()) {
                    word.category = IDENFR;
                } else {
                    word.category = iterator->second;
                }
            } 
            word.line = line;
            word.raw = str;
        }
        words.push_back(word);
        w = word;
        return w.category;
    }
    ```

2. 起初使用的是`unique_ptr`独享智能指针，其特点是独享，同一时刻只能被一个变量使用，当转交使用权后，会导致原指针失效，即便是函数传参也是。因为我构建了AST树，后来需要多次遍历AST树，所以AST树内的指针不能失效，而在遍历AST树时，不免遇到需要指针类型转换（转换为派生类指针，也会导致原指针失效）和函数调用传参等，所以改用了更为宽限的`shared_ptr`共享智能指针，可以共享，仅仅增加引用数量的值。并且不用我去释放该指针，仅当引用为0时会自动释放。

3. 词法分析的`keyWord`和`words`两个容器，之前存放在`Lexer.h`中使用`static`修饰，但是会导致只能本文件访问，其他文件不能访问同一个值。所以，改用存放在`Lexer.c`中，在`Lexer.h`中使用`extern`修饰。

# 二、课程要求

**【问题描述】**

请根据给定的文法设计并实现语法分析程序，能基于上次作业的词法分析程序所识别出的单词，识别出各类语法成分。输入输出及处理要求如下：

（1）需按文法规则，用递归子程序法对文法中定义的语法成分进行分析；

（2）为了方便进行自动评测，输入的被编译源文件统一命名为`testfile.txt`（注意不要写错文件名）;输出的结果文件统一命名为`output.txt`（注意不要写错文件名）；结果文件中包含如下两种信息：

  1）按词法分析识别单词的顺序，按行输出每个单词的信息（要求同词法分析作业，对于预读的情况不能输出）。

  2）在文法中出现（除了`<BlockItem>`, `<Decl>`, `<BType>` 之外）的语法分析成分分析结束前，另起一行输出当前语法成分的名字，形如`“<Stmt>”`（注：未要求输出的语法成分仍需要进行分析，但无需输出）

**【输入形式】**`testfile.txt`中的符合文法要求的测试程序。

**【输出形式】**按如上要求将语法分析结果输出至`output.txt`中。

**【特别提醒】**（1）本次作业只考核对正确程序的处理，但需要为今后可能出现的错误情况预留接口。

​         （2）当前要求的输出只是为了便于评测，完成编译器中无需出现这些信息，请设计为方便打开/关闭这些输出的方案。

**【样例输入】**

```
int main(){
    int c;
    c= getint();
    printf("%d",c);
    return c;
}
```

**【样例输出】**

```
INTTK int
MAINTK main
LPARENT (
RPARENT )
LBRACE {
INTTK int
IDENFR c
<VarDef>
SEMICN ;
<VarDecl>
IDENFR c
<LVal>
ASSIGN =
GETINTTK getint
LPARENT (
RPARENT )
SEMICN ;
<Stmt>
PRINTFTK printf
LPARENT (
STRCON "%d"
COMMA ,
IDENFR c
<LVal>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
RPARENT )
SEMICN ;
<Stmt>
RETURNTK return
IDENFR c
<LVal>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
SEMICN ;
<Stmt>
RBRACE }
<Block>
<MainFuncDef>
<CompUnit>
```

**【评分标准】**

按与预期结果不一致的行数扣分，每项扣5%。

**【参考资料】**教材 第18章 18.3.5 

根据PASCAL-S文法的定义，阅读源代码，理解程序的框架，了解各语法成分对应的子程序以及子程序之间的调用关系和接口；对其中与实验作业文法中类似的语法成分，要重点阅读其代码，进行分析、理解，为今后的语义分析打下基础（详见pascals-compiler.docx）

**【开发语言及环境】**用 C/C++/JAVA 实现，机房安装的 C/C++ 开发环境是 CLion 2018.3.4 和 CodeBlocks 20.03；Java 的开发环境为 IDEA 2018.3.6 社区版。产生的 MIPS 汇编在 Mars 模拟器（更正版）上运行，平台支持 C++11 标准。评测机所采用的编译学生代码的版本是：C/C++ gcc 8.1.0，Java jdk 1.8。

**【辅助工具】**2021语法分析测试库  2021实验作业调试（见在线实验） 

**【文档要求】**完成2021语法分析阶段设计文档 （可在词法分析阶段设计文档基础上扩充完成）      

**【资料下载】**PASCALS编译器源代码、Mars4.5（更正版Mars-jdk7-Re.jar）、开发环境及配置说明、编译技术调试实验使用指南均可以从本课程平台的“课程信息”→“课件下载”中获取。

**【提交形式】**将所开发的语法分析程序的源文件（.cpp/.c/.h/.java，不含工程文件）打包为zip或rar后提交。对于使用 java 开发的编译器，程序运行的入口为 src 目录下 Compiler.java 中的 main 方法。上传请直接打包 src 文件夹，如果引用了第三方外部包（不推荐），请将外部 jar 包文件放到 bin 目录下，bin 和 src 两个文件夹同级，将 bin 和 src 一起打包后提交即可。注意 mac 压缩会产生额外的文件到压缩包中，需删掉额外文件后提交。
