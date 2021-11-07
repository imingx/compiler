# 2021编译技术实验记录

## 简介

SysY 语言是本次课程要实现的编程语言，是 C 语言的一个子集。每个 SysY 程序的源码存储在一个扩展名为 sy 的文件中。该文件中有且仅有一个名为 main 的主函数定义，还可以包含若干全局变量声明、常量声明和其他函数定义。SysY 语言支持 **int** 类型和元素为 int 类型且按行优先存储的多维数组类型，其中 int 型整数为 32 位有符号数;**const** 修饰符用于声明常量。

文法见[![1](https://img.shields.io/badge/repo-miniSysY-9cf?logo=github)](https://github.com/BUAA-SE-Compiling/miniSysY-tutorial/blob/master/miniSysY.md)  [![2](https://img.shields.io/badge/pdf-%E6%96%87%E6%B3%95%E5%AE%9A%E4%B9%89%E8%AF%B4%E6%98%8E-9cf?logo=gitbook)](https://github.com/imingx/Compiler/blob/main/docs/2021%E7%BC%96%E8%AF%91%E6%8A%80%E6%9C%AF%E5%AE%9E%E9%AA%8C%E6%96%87%E6%B3%95%E5%AE%9A%E4%B9%89%E5%8F%8A%E7%9B%B8%E5%85%B3%E8%AF%B4%E6%98%8E.pdf)

## 步骤

- [00\_文法解读][00_文法解读]
    - [文法解读测试用例][文法解读测试用例]

- [01\_词法分析][01_词法分析]

- [02\_语法分析][02_语法分析]
    - [语法分析测试用例][语法分析测试用例]

- [03\_错误处理][03_错误处理]
    - [错误处理测试用例][错误处理测试用例] 

- [期中考试][期中考试]


## 提交信息

```
(1) type:
feat		增加特性
fix	        修复错误
docs		修改文档
style		更改空格、缩进、样式，不改变代码逻辑
refactor	代码重构
perf		优化相关，提升性能和体验
test		增加测试用例
build		依赖相关的内容
ci              ci配置相关，对k8s, docker的配置文件修改
chore		改变构建流程、增加依赖库和工具
revert		回滚版本

(2) scope: commit影响的范围
(3) subject: commit目的的简短描述，以第一人称现在时，小写字母开头，不加句号

type(scope): subject

some useful template:

docs(readme): 增加内容/improve content
🎉init commit
增加项目文件用feat
random message: "`curl -s http://whatthecommit.com/index.txt`"

git commit的message可以换行，在输入第一个"后，输入回车并不会直接执行命令。
在第二行输入的message称为body，最后还有foot。
```

## 参考

1. [`unique_ptr`][unique_ptr], [`shared_ptr`][share_ptr]
2. [TryC - a small interpreter written by C][tryC - a small interpreter written by C]
3. [My First Language Frontend with LLVM Tutorial][My First Language Frontend with LLVM Tutorial]





[期中考试]: https://github.com/imingx/Compiler/tree/midtermExam "期中考试"

[share_ptr]: http://www.cplusplus.com/reference/memory/shared_ptr/ "share_ptr"
[00_文法解读]: https://github.com/imingx/Compiler/tree/00_%E6%96%87%E6%B3%95%E8%A7%A3%E8%AF%BB "00_文法解读"
[01_词法分析]: https://github.com/imingx/Compiler/tree/01_Lexer	"01_词法分析"
[02_语法分析]: https://github.com/imingx/Compiler/tree/02_Parser  "02_语法分析"
[文法解读测试用例]: https://github.com/imingx/Compiler/tree/00_%E6%96%87%E6%B3%95%E8%A7%A3%E8%AF%BB_testFile "文法解读测试用例"
[语法分析测试用例]: https://github.com/imingx/Compiler/tree/02_Parser_testFile "语法分析测试用例"
[03_错误处理]: https://github.com/imingx/Compiler/tree/03_ErrorHandler "03_错误处理"
[错误处理测试用例]: https://github.com/imingx/Compiler/tree/03_ErrorHandler_testFile "错误处理测试用例"
[unique_ptr]: https://blog.csdn.net/shaosunrise/article/details/85158249 "unique_ptr"
[My First Language Frontend with LLVM Tutorial]: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html "My First Language Frontend with LLVM Tutorial"
[tryC - a small interpreter written by C]: https://github.com/imingx/tryC "tryC - a small interpreter written by C"
