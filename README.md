# 数据库大作业

## 最近更新

- 2019.5.25  
  - 增加了对于NULL类型的支持。
  - 添加了一揽子测试数据，主要是OJ能直接下载到的评测数据，也包括我自己写的测试数据点（8.sql）
  - 支持select into outfile 功能
- 2019.6.2
  - 支持函数求值、ORDER BY和GROUP BY了！已经集成到前端中。需要进一步测试……
  - 想到一个问题，是不是现在的解析器不支持多余的空格？包括这种：' '，两个引号是否会被分开？
  - 架构重要变化：新增了Expression类及其子类，可以用来表示select中打印在各列中的内容，包括表格的字段名、函数、常量、计算式……
- 2019.6.14
  - Value.h发生变化，重新实现了IntValue和DoubleValue，允许DoubleValue与IntValue之间进行比较，修复了当本来为NULL的值copy后变为非NULL的bug。
  - Expression家族得到大补充，现在支持数值运算、逻辑运算、比较，之前的WhereClause类被废弃，用Expression代替。
    这样，Where子句、Select语句都具有了巨大的灵活度。
  - 测例可以用脚本一键检验，防止新增加的feature导致之前的bug。
- 其他进展请在这里进行同步……

## 运行说明

### 编译条件

本程序在Ubuntu 18.04.2 64-bit, g++ 7.3.0下编译通过。
本程序只需要使用make工具即可编译，不依赖于C++标准库之外的库。
make clean需要在Linux下使用（因为用的是rm）。

### 运行条件

本程序在Ubuntu下可以正常运行。
