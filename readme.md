# 一、课程要求

【问题描述】

请根据给定的文法设计并实现词法分析程序，从源程序中识别出单词，记录其单词类别和单词值，输入输出及处理要求如下：

  （1）数据结构和与语法分析程序的接口请自行定义；类别码需按下表格式统一定义；

  （2）为了方便进行自动评测，输入的被编译源文件统一命名为 testfile.txt（注意不要写错文件名）；输出的结果文件统一命名为 output.txt（注意不要写错文件名），结果文件中每行按如下方式组织：

​        单词类别码 单词的字符/字符串形式(中间仅用一个空格间隔)

​        单词的类别码请统一按如下形式定义：

词法分析类别码定义如下：

| 单词名称         | 类别码     | 单词名称 | 类别码   | 单词名称 | 类别码 | 单词名称 | 类别码  |
| ---------------- | ---------- | -------- | -------- | -------- | ------ | -------- | ------- |
| **Ident**        | IDENFR     | !        | NOT      | *        | MULT   | =        | ASSIGN  |
| **IntConst**     | INTCON     | &&       | AND      | /        | DIV    | ;        | SEMICN  |
| **FormatString** | STRCON     | \|\|     | OR       | %        | MOD    | ,        | COMMA   |
| main             | MAINTK     | while    | WHILETK  | <        | LSS    | (        | LPARENT |
| const            | CONSTTK    | getint   | GETINTTK | <=       | LEQ    | )        | RPARENT |
| int              | INTTK      | printf   | PRINTFTK | >        | GRE    | [        | LBRACK  |
| break            | BREAKTK    | return   | RETURNTK | >=       | GEQ    | ]        | RBRACK  |
| continue         | CONTINUETK | +        | PLUS     | ==       | EQL    | {        | LBRACE  |
| if               | IFTK       | -        | MINU     | !=       | NEQ    | }        | RBRACE  |
| else             | ELSETK     | void     | VOIDTK   |          |        |          |         |

【输入形式】testfile.txt中的符合文法要求的测试程序。
【输出形式】要求将词法分析结果输出至output.txt中。

【特别提醒】 （1）读取的字符串要原样保留着便于输出，特别是数字，这里输出的并不是真正的单词值，其实是读入的字符串，单词值需另行记录。

​          （2）本次作业只考核对正确程序的处理，但需要为今后可能出现的错误情况预留接口。

​          （3）在今后的错误处理作业中，需要输出错误的行号，在词法分析的时候注意记录该信息。

​          （4）单词的类别和单词值以及其他关注的信息，在词法分析阶段获取后，后续的分析阶段会使用，请注意记录；当前要求的输出只是为了便于评测，完成编译器中无需出现这些信息，请设计为方便打开/关闭这些输出的方案。

【文法定义】请到“课件下载”板块中下载课程文档

【样例输入】

```
const int array[2] = {1,2};

int main(){
    int c;
    c = getint();
    printf("output is %d",c);
    return c;
}
```

【样例输出】

```
CONSTTK const
INTTK int
IDENFR array
LBRACK [
INTCON 2
RBRACK ]
ASSIGN =
LBRACE {
INTCON 1
COMMA ,
INTCON 2
RBRACE }
SEMICN ;
INTTK int
MAINTK main
LPARENT (
RPARENT )
LBRACE {
INTTK int
IDENFR c
SEMICN ;
IDENFR c
ASSIGN =
GETINTTK getint
LPARENT (
RPARENT )
SEMICN ;
PRINTFTK printf
LPARENT (
STRCON "output is %d"
COMMA ,
IDENFR c
RPARENT )
SEMICN ;
RETURNTK return
IDENFR c
SEMICN ;
RBRACE }
```

【评分标准】 按与预期结果不一致的项数（每一行单词信息算一项）扣分，每项扣5%。

【参考资料】教材 第17章 17.3 ；第18章 18.3.4 

​          pl0-compiler.doc 中 getsym 及相关子程序（过程）
​          pascals-compiler.docx 中 insymbol 及相关子程序（过程）

【开发语言及环境】用 C/C++/JAVA 实现，机房安装的 C/C++ 开发环境是 CLion 2018.3.4 和 CodeBlocks 20.03；Java 的开发环境为 IDEA 2018.3.6 社区版。产生的 MIPS 汇编在 Mars 模拟器（更正版）上运行，平台支持 C++11 标准。IDE 的安装/环境的配置文档请见“课件下载”中的“集成开发环境安装说明”

【文档要求】完成2021词法分析阶段设计文档      

【资料下载】PL/0、PASCALS编译器源代码、集成开发环境安装说明可以从本课程平台的“课程信息”→“课件下载”中获取。

【提交形式】将所开发的词法分析程序的源文件（.cpp/.c/.h/.java，不含工程文件）打包为zip或rar后提交。对于使用 java 开发的编译器，程序运行的入口为 src 目录下 Compiler.java 中的 main 方法。上传请直接打包 src 文件夹，如果引用了第三方外部包（不推荐），请将外部 jar 包文件放到 bin 目录下，bin 和 src 两个文件夹同级，将 bin 和 src 一起打包后提交即可。

评测机所采用的编译学生代码的版本是：C/C++ gcc/g++ 8.1.0，Java jdk 1.8

（注意在MAC下压缩会产生额外的文件到压缩包中，需删掉额外文件后提交）。

# 一、词法分析设计文档

## 1. 需求分析

根据给定的文法设计并实现词法分析程序，从源程序中识别出单词，记录其单词的类别和单词的值。

要求：

1. 对于读入的字符串需要保留原样以便输出。并且设计方便打开和关闭词法分析的输出的开关。
2. 需要为之后的语法分析和错误处理预留接口
3. 记得保留行号信息，以便错误处理时输出。
4. 记录单词的类别和单词的值，方便后续阶段调用。

## 2. 编码前的设计

- 将词法分析封装为Lexer类，方便之后增添新的功能和解耦。
- 将类别码定义为枚举类型，并且定义token常量字符串数组，方便将枚举类型类别码输出为字符串类别码。
- 在Lexer构造函数中，初始化输入和输出流。并且初始化map容器，将读入的字符串单词名称对应到相应的枚举类型类别码。
- 读入时，每次读入单个字符，每识别出一个单词，就直接进行输出，然后将结果保存在数组内。

## 3. 模块设计

**文件结构：**

```
.
├── include
│   ├── lexer.h
│   └── main.h
├── lexer.cpp
└── main.cpp
```

### 3.1 类别码存放和输出设计

```c++
enum CATEGORY {
    IDENFR, INTCON, STRCON, MAINTK,
    CONSTTK, INTTK, BREAKTK, CONTINUETK, IFTK,
    ELSETK, NOT, AND, OR, WHILETK, GETINTTK,
    PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK,
    MULT, DIV, MOD, LSS, LEQ, GRE, GEQ, EQL,
    NEQ, ASSIGN, SEMICN, COMMA, LPARENT,
    RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
};

static char const *token[CATEGORY_NUM] = {"IDENFR", "INTCON", "STRCON", "MAINTK",
                                   "CONSTTK", "INTTK", "BREAKTK", "CONTINUETK", "IFTK",
                                   "ELSETK", "NOT", "AND", "OR", "WHILETK", "GETINTTK",
                                   "PRINTFTK", "RETURNTK", "PLUS", "MINU", "VOIDTK",
                                   "MULT", "DIV", "MOD", "LSS", "LEQ", "GRE", "GEQ", "EQL",
                                   "NEQ", "ASSIGN", "SEMICN", "COMMA", "LPARENT",
                                   "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};
```

存放在Lexer头文件中，需要使用static类型。token常量字符串数组实现使用枚举类型识别码来输出对应的字符串识别码。

### 3.2 单词记录方式设计

````c++
struct Word {
    enum CATEGORY category;
    string raw;
    int line;
};

static vector<Word> words;
````

在目前阶段，只记录了每个单词的行号，原始字符串值和枚举类型的识别码， words存放识别的单词。

### 3.3 词法分析器类设计

```c++
class Lexer {
private:
    FILE *in, *out;
    int line;
    map<string, CATEGORY> keyWord;
public:
    Lexer(const char *in, const char *out);
    void analyse();
};
```

in和out为输入输出流，line记录当前的行号，keyWord记录单词名称和识别码的对应关系，analyse为词法分析主函数。

### 3.4 词法分析方法框架

首先对keyWord进行初始化，将单词值对应到枚举型识别码：

```c++
keyWord["main"] = MAINTK;
keyWord["const"] = CONSTTK;
keyWord["int"] = INTTK;
keyWord["break"] = BREAKTK;
keyWord["continue"] = CONTINUETK;
│
│
keyWord[")"] = RPARENT;
keyWord["["] = LBRACK;
keyWord["]"] = RBRACK;
keyWord["{"] = LBRACE;
keyWord["}"] = RBRACE;
```

接着是分析设计，根据前面设计的方式读入，下面展示识别标识符的代码，先去掉无用字符串，然后读入下一个单词，当发现是keyWord的键值时，就存放其为对应的枚举类型识别码，否则存放为IDENFR。然后记录原始值和行号，最后进行输出，存入words容器。

```c++
while ((c = fgetc(in)) != EOF) {
    string str;
    Word word;
    while (isspace(c)) {
        if (c == '\n') ++line;
        c = fgetc(in);
    }
    if (c == EOF) {
        break;
    }
    if (isalpha(c) || c == '_') {
        do {
            str += c;
            c = fgetc(in);
        } while (isalnum(c) || c == '_');
        ungetc(c, in);

        map<string, CATEGORY>::iterator iterator = keyWord.find(str);
        if (iterator == keyWord.end()) {
            word.category = IDENFR;
        } else {
            word.category = iterator->second;
        }
    } else {
        ……
    }
    word.raw = str;
    word.line = line;
    #ifdef PRINT
    cout << token[word.category] << " " << str << endl;
    fprintf(out, "%s %s\n", token[word.category], str.c_str());
    #endif
    words.push_back(word);
}
```

输出控制使用`#ifdef PRINT` `endif`来进行控制，在`include/main.h`中`#define PRINT`。

## 4. 编码后的对设计的修改情况

1. 原本使用数组来存放单词和结果，最后发现C++提供了更好的方式，比如使用string存放单词值，`str+=c;`就可以实现单词的扩充。使用动态数组vector更方便操作。
2. （未实现）实际上想到后来需要进行语法分析，所以我们必须考虑是先词法分析读取完，存入容器，再进行语法分析；还是边词法分析边语法分析，即每次读取一个单词，然后交给语法分析器进行分析。为了照顾后面的情况，我需要独立出一个getNextToken函数。待语法分析进行修改。