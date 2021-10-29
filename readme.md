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
基本表达式   PrimaryExp → '(' Exp ')' | LVal | Number | ''' NormalChar'''
```

3. 例子

输入：

```c++

```

输出：

```c++

```
