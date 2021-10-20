## 一、课程要求

本课程的实验，是根据给定的文法实现一个小型编译器。该编译器的实现分成几个阶段来完成，请根据每次作业的提示，逐步增加该编译器的模块，最终形成一个生成特定目标代码的完整编译器。

开始编写编译器代码之前，需要仔细阅读和分析给定的文法、设计编译器的总体结构和各模块之间的接口、考虑编译的每个阶段可能出现的错误类型。为此，第一项作业从文法的解读开始。

请仔细阅读下面的文法，要求对文法中每条规则所定义的语法成分进行分析，了解其作用、限定条件、组合情况和可能产生出的句子，在此基础上，编写4-6个测试程序，要求所有测试程序能共同覆盖所有语法规则及每条规则内的常见组合情况，每个测试程序有且仅有10条写语句（每个测试程序的第一条写语句请用printf '('＜字符串＞ ')'输出自己的学号，其他写语句需尽量反映出程序定义的数据及其运算结果，以便在后续阶段测试各种语法成分翻译的正确性）。请提供每个测试程序的输入数据(有<读语句>则提供，否则无需提供)、输出数据（输出数据必须提供。若输入输出数据没有正确提供，评测时会报错），放到文件中，按下述要求为文件命名：

测试程序及对应的输入输出数据文件分别为 
```

​     testfile1.txt  input1.txt  output1.txt

​ 	   testfile2.txt  input2.txt  output2.txt

​                        ...

​        testfilen.txt  inputn.txt  outputn.txt
```
   将以上文件打包为rar或zip文件后上传（请直接打包文件，不用置于文件夹中打包）。

   需独立完成，发现雷同按作弊处理

   注：本学期实验的所有作业都针对此文法，后续作业需在上一次作业的基础上进行增量式开发



  2021 年编译器课程设计文法如下（详见“课件下载”中的文档《2021编译技术实验文法定义及相关说明》，由于9月19日文档有修订，请及时更新）：

  补充更新内容：

1. 常量数组不能作为参数传递；

2. 相邻的 UnaryOp 不能相同，如 i = --+4; 是不符合语义约束的；

3. UnaryOp 为 '!' 时只能出现在条件表达式中；

  4. ConstExp -> AddExp 这条规则所涉及的 Ident 必须为常量；

5. printf 语句中的转义字符有且仅有 '\n'，即为了与 gcc 评测结果保持一致， '\' 不能单独出现（将视为不符合文法），只可以与 'n' 结对出现。

6. 为了简化后续同学们开发编译器的难度，有返回值的函数的最后一个 `<stmt>`一定是 return 语句（注意是 `<FuncDef>`中 `<Block> `的最后一个语句）

7. 对于整数数字的要求：不含前导0，但数字 0 是合法的。

8. main 函数的返回值只能为 0。

  【提示】有不少同学的错误在于输入输出不匹配，输出文件的行尾缺少本应该输出的空格。这种情况请用电脑自带的文本编辑器来保存文件并上传，有的 IDE 如 jetbrain 那一套，保存后会自动抹去行尾空格，造成输出不一致的问题。


```

  编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef // 1.是否存在Decl 2.是否存在FuncDef

  声明 Decl → ConstDecl | VarDecl // 覆盖两种声明

  常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';' // 1.花括号内重复0次 2.花括号内重复多次

  基本类型 BType → 'int' // 存在即可

  常数定义 ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal // 包含普通变量、一维数组、二维数组共三种情况

  常量初值 ConstInitVal → ConstExp
    | '{' [ ConstInitVal { ',' ConstInitVal } ] '}' // 1.常表达式初值 2.一维数组初值 3.二维数组初值

  变量声明 VarDecl → BType VarDef { ',' VarDef } ';' // 1.花括号内重复0次 2.花括号内重复多次

  变量定义 VarDef → Ident { '[' ConstExp ']' } // 包含普通变量、一维数组、二维数组定义
    | Ident { '[' ConstExp ']' } '=' InitVal

  变量初值 InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'// 1.表达式初值 2.一维数组初值 3.二维数组初值

  函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block // 1.无形参 2.有形参

  主函数定义 MainFuncDef → 'int' 'main' '(' ')' Block // 存在main函数

  函数类型 FuncType → 'void' | 'int' // 覆盖两种类型的函数 

  函数形参表 FuncFParams → FuncFParam { ',' FuncFParam } // 1.花括号内重复0次 2.花括号内重复多次

  函数形参 FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }] // 1.普通变量 2.一维数组变量 3.二维数组变量

  语句块 Block → '{' { BlockItem } '}' // 1.花括号内重复0次 2.花括号内重复多次

  语句块项 BlockItem → Decl | Stmt // 覆盖两种语句块项

  语句 Stmt → LVal '=' Exp ';' // 每种类型的语句都要覆盖
    | [Exp] ';' //有无Exp两种情况
    | Block 
    | 'if' '( Cond ')' Stmt [ 'else' Stmt ] // 1.有else 2.无else
    | 'while' '(' Cond ')' Stmt
    | 'break' ';' | 'continue' ';'
    | 'return' [Exp] ';' // 1.有Exp 2.无Exp
    | LVal = 'getint''('')'';'
    | 'printf' '('FormatString {',' Exp} ')'';' // 1.有Exp 2.无Exp

  表达式 Exp → AddExp 注：SysY 表达式是int 型表达式 // 存在即可

  条件表达式 Cond → LOrExp // 存在即可

  左值表达式 LVal → Ident {'[' Exp ']'} //1.普通变量 2.一维数组 3.二维数组

  基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number // 三种情况均需覆盖

  数值 Number → IntConst // 存在即可

  一元表达式 UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' // 三种情况均需覆盖,函数调用也需要覆盖FuncRParams的不同情况
    | UnaryOp UnaryExp // 存在即可

  单目运算符 UnaryOp → '+' | '−' | '!' 注：'!'仅出现在条件表达式中 // 三种均需覆盖

  函数实参表 FuncRParams → Exp { ',' Exp } // 1.花括号内重复0次 2.花括号内重复多次 3. Exp需要覆盖数组传参和部分数组传参

  乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp // 1.UnaryExp 2.* 3./ 4.% 均需覆盖

  加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp // 1.MulExp 2.+ 3.- 均需覆盖

  关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp // 1.AddExp 2.< 3.> 4.<= 5.>= 均需覆盖

  相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp // 1.RelExp 2.== 3.!= 均需覆盖

  逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp // 1.EqExp 2.&& 均需覆盖

  逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp // 1.LAndExp 2.|| 均需覆盖

  常量表达式 ConstExp → AddExp 注：使用的Ident 必须是常量 // 存在即可

  格式化字符 FormatChar → %d

  普通字符 NormalChar → 十进制编码为32,33,40-126的ASCII字符

  字符 Char → FormatChar | NormalChar 

  格式化字符串 FormatString → '"'{ Char }'"'

```

**请注意：**

1. 评测程序对各测试程序进行编译运行，检查输入输出是否与给定的文件一致；计算每个测试程序的覆盖率和所有测试程序的累积覆盖率，得分率=累积覆盖的项数/检查的总项数。若得分率小于90%，不给出提示，请仔细阅读文法补充或完善测试程序；得分率大于等于90%，则可以得到关于未覆盖规则或组合的提示。

2. 测试程序难度分为三个等级：A、B、C，难度依次递减。其中 B 级难度需要包含文法中黄色高亮内容，A 级难度需要包含绿色高亮内容，其中绿色高亮部分，即程序中的条件判断需要【短路求值】规则。上传文件时，请保证有且仅有一个 A 级难度 testfile ，编号为 1；有且仅有两个 B 级难度 testfile，编号为 2 和 3；其余为 C 级难度。即 testfile 编号越小，难度越高。

3. 请同学在本地使用 gcc 8.5.0 以上的版本进行调测，需要替换输入的 getint() 函数为scanf（详见文档第一节）。

4. 如果对测试结果有异议的话，请联系各班助教，课程组将进行错误排查。

5. 评测机输出说明

6. 1. 每一个 testfile的 status 会有三种情况

    2. 1. RIGHT：完全正确（有的时候爆栈也会显示 RIGHT ，但是覆盖率是 0 ，这种情况请联系助教排查）
        2. WRONG：如果覆盖率为 0，则表示程序不符合文法及语义约束，同时 detail 中会提示某个 testfile 有问题；如果有覆盖率，则表示程序的输入输出不匹配
        3. Runtime error：gcc 没有跑通你的测试程序

    3. Total 的 status 只有 WRONG 和 RIGHT，RIGHT 的出现当且仅当所有 testfile 的 status 都是 RIGHT

    4. Score 是得分，若有一个 testfile 的 status 不为 RIGHT，则得分为0；若都为 RIGHT，则得分为总覆盖率

7. 将文件保存成 utf-8（无 bom）或者 ANSI 的格式，否则可能会影响评测结果。

8. 即使没有输入，也要给一个空的 input.txt 文件。