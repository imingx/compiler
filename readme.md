## 2021编译技术实验记录

### 步骤

- [00\_文法解读][00_文法解读]
    - [文法解读测试用例][文法解读测试用例]

- [01\_词法分析][01_词法分析]

- [02\_语法分析][02_语法分析]
    - [语法分析测试用例][语法分析测试用例]



### 提交信息

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

### 参考

1. [unique_ptr][unique_ptr]
2. [tryC - a small interpreter written by C][tryC - a small interpreter written by C]
3. [My First Language Frontend with LLVM Tutorial][My First Language Frontend with LLVM Tutorial]






[00_文法解读]: https://github.com/imingx/Compiler/tree/00_%E6%96%87%E6%B3%95%E8%A7%A3%E8%AF%BB "00_文法解读"
[01_词法分析]: https://github.com/imingx/Compiler/tree/01_Lexer	"01_词法分析"
[02_语法分析]: https://github.com/imingx/Compiler/tree/02_Parser  "02_语法分析"
[文法解读测试用例]: https://github.com/imingx/Compiler/tree/00_%E6%96%87%E6%B3%95%E8%A7%A3%E8%AF%BB_testFile "文法解读测试用例"
[语法分析测试用例]: https://github.com/imingx/Compiler/tree/02_Parser_testFile "语法分析测试用例"
[unique_ptr]: https://blog.csdn.net/shaosunrise/article/details/85158249 "unique_ptr"
[My First Language Frontend with LLVM Tutorial]: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html "My First Language Frontend with LLVM Tutorial"
[tryC - a small interpreter written by C]: https://github.com/imingx/tryC "tryC - a small interpreter written by C"
