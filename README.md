# TinyCompiler
编译原理课程实验  

## 构建与运行

环境：Linux x86-64, Ubuntu22.04, 系统自带 gcc(11.4.0)  

编译选项：-std=c++11

```bash
git clone git@github.com:huangym07/TinyCompiler.git
cd TinyCompiler
make run
```

更多构建信息可查看 Makefile

## 实验一：
文法一：  
<无符号整型数值型数据> -> <数字><数字>\*  
<数字> -> 0|1|2|3|4|5|6|7|8|9

文法二：  
<算符> -> +|-|\*|/

根据上述词法对给定的字符串进行词法分析，得到 token 序列，打印 token 序列。若存在无法识别的字符，则打印错误信息

## 实验二
文法：  
E -> E+T|E-T|T  
T -> T\*F|T/F|F  
F -> i

根据上述算术表达式文法，对词法分析得到的 token 序列进行递归下降语法分析，建立具体语法树。  
token 序列匹配文法则打印语法树；否则打印错误信息，指出错误的 token 位置。
