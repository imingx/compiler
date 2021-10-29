期中考试增加了文法，支持char类型定义和char返回值。

1. 增加类别码：

    | 单词名             | 类别码      |
    | ------------------ | ----------- |
    | `char`             | CHARTK      |
    | `NormalChar`单个字符 | CHAR        |
    | `'`                | SINGLEQUATE |

2. 文法新增：

```c++
基本类型    BType → 'int' | 'char'
函数类型    FuncType → 'void' | 'int' | 'char'
基本表达式  	PrimaryExp → '(' Exp ')' | LVal | Number | ConstChar
常量字符	ConstChar → '''NormalChar'''
```

3. 例子

输入：

```c++
const char a = 'a';
char b = 'b';
char c['a'];
char func(char a, char c){
    return 'a';
}

int main(){
    if ( 'a' < 'b') {
        printf("%c < %c\n", 'a', 'b');
    }
    'a';
    func('a', 'b');
    b = 'c';
    return 0;
}
```

输出：

```c++
CONSTTK const
CHARTK char
IDENFR a
ASSIGN =
SINGLEQUOTE '
CHAR a
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<ConstExp>
<ConstInitVal>
<ConstDef>
SEMICN ;
<ConstDecl>
CHARTK char
IDENFR b
ASSIGN =
SINGLEQUOTE '
CHAR b
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
<InitVal>
<VarDef>
SEMICN ;
<VarDecl>
CHARTK char
IDENFR c
LBRACK [
SINGLEQUOTE '
CHAR a
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<ConstExp>
RBRACK ]
<VarDef>
SEMICN ;
<VarDecl>
CHARTK char
<FuncType>
IDENFR func
LPARENT (
CHARTK char
IDENFR a
<FuncFParam>
COMMA ,
CHARTK char
IDENFR c
<FuncFParam>
<FuncFParams>
RPARENT )
LBRACE {
RETURNTK return
SINGLEQUOTE '
CHAR a
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
SEMICN ;
<Stmt>
RBRACE }
<Block>
<FuncDef>
INTTK int
MAINTK main
LPARENT (
RPARENT )
LBRACE {
IFTK if
LPARENT (
SINGLEQUOTE '
CHAR a
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<RelExp>
LSS <
SINGLEQUOTE '
CHAR b
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<RelExp>
<EqExp>
<LAndExp>
<LOrExp>
<Cond>
RPARENT )
LBRACE {
PRINTFTK printf
LPARENT (
STRCON "%c < %c\n"
COMMA ,
SINGLEQUOTE '
CHAR a
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
COMMA ,
SINGLEQUOTE '
CHAR b
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
RPARENT )
SEMICN ;
<Stmt>
RBRACE }
<Block>
<Stmt>
<Stmt>
SINGLEQUOTE '
CHAR a
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
SEMICN ;
<Stmt>
IDENFR func
LPARENT (
SINGLEQUOTE '
CHAR a
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
COMMA ,
SINGLEQUOTE '
CHAR b
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
<FuncRParams>
RPARENT )
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
SEMICN ;
<Stmt>
IDENFR b
<LVal>
ASSIGN =
SINGLEQUOTE '
CHAR c
SINGLEQUOTE '
<ConstChar>
<PrimaryExp>
<UnaryExp>
<MulExp>
<AddExp>
<Exp>
SEMICN ;
<Stmt>
RETURNTK return
INTCON 0
<Number>
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
