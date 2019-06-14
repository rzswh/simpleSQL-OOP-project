# 开发者说明文档

## 术语说明

本文档中数据表中的“属性”与“字段”是同义词。

## 结构图

![avatar](TableStructure.png)

* `Table`: 数据表。存储数据，支持插入新行、删除满足条件的行、更新满足条件的行为给定值、选择出满足条件的行以组成一张可打印出到屏幕的表格。
* `Attribute`: 数据表内每列数据的属性。包括属性名称、属性的数据类型、属性是否为空。
* `Record`: 一条记录，即一行内的数据。
* `WhereClause`: Where子句类，将查询表达式以方便计算机计算的方式存储，并能判断某一条记录是否满足该子句的条件。
* `ValueBase`: 数据类型的基类。这是一个抽象类（接口），为了方便在其上重载了一些比较运算符，以及方便进行输出的方法。
* `IntValue`、`DoubleValue`、`BoolValue`等ValueBase的子类：各种数据类型。
* `Expression` 及其子类: 运算表达式，用于表达select语句的表达式。比如字段名、常量、函数、运算。
* `PrintableTable`: 将用于屏幕输出的属性和数据集合。可以视作另一种只读、不可修改、只可以打印的数据表。
* `Sql`:解析输入的语句，根据输入调用合适的数据库接口，并且将结果输出。
* `Interpreter`:查询语句语法分析与结果输出，通过Interpreter调用Manager类型，根据输入调用合适的数据库接口，并将结果输出。
* `Manager`：包含一个Database向量，管理全部数据库。传入经过sql类型处理后的sql命令，调用相应函数，实现命令的要求。支持创建、删除数据库、显示所有数据库、访问特定数据库。
* `Database`:数据库类，管理数据表。支持插入、删除数据表、访问特定数据表。

# 接口说明

## Table.h

### class Table

存储每张特定数据表的数据存储器。

#### 私有成员变量
**`vector<Record> data`**

表格中所有记录的数据存储位置。

**`AggregationFunctionExpression* sortExpression`**

Select语句中用于排序的累积表达式。用于处理order by sum(字段)的情形。这一变量必须要在分组求值的时候确定。

#### 公有成员变量
**`const string name`**

**`const vector<Attribute> attrs`**

属性列表。其顺序固定，不可以调换。  

**`const string primary`**

主键对应的属性名称。主键信息不储存在`Attribute`类中，通过在`Table`类中判断每一条属性的名称是否与主键相同，判断该条属性是否是主键。

#### 私有成员函数
**`Record eval(const Record & r, vector<Expression*> exps)`**

根据一条记录的数据，对一组表达式进行求值，将结果以一条记录的形式返回。

**`ValueBase* eval(const Record & r, Expression* exps)`**

根据一条记录的数据，对单个表达式进行求值，将结果以一条记录的形式返回。

**`vector<Record> groupIntoTable(vector<Record *> tot, vector<Expression* > agg, const vector<Expression *>& exps, vector<ValueBase *>* sortMetric = nullptr)`**

分治的思路将一组记录tot按照对agg中各个函数求得的值进行分组，分组完成后再分别对exps中的表达式进行求值之后返回。

因为分组过程顺带着可以完成排序的功能，所以需要排序的表达式放在agg首位值，
相当于先按照order by表达式进行一次分组，并将分组结果按照其表达式求得的值进行排序。
如果排序依据是一个累积函数则比较特殊，因为无法在分组过程中确定它的值，
而只能在分组完成后确定，因此采取排序后置的策略，先按照agg中其他表达式分好组，
在分成组的时候顺带着把order by表达式求值，最后在回溯到处理order by表达式一层的时候
将结果合并起来。

agg只有第一个表达式可以为累积函数，其他的都不能是累积函数，可以为字段表达式、
普通函数表达式（如ADD DATE）等可以通过作用在*单条记录*上得到值的函数。

sortMetric是一个辅助变量，如果是非空地址指针，那么每当在递归边界处完成一个分组时，
指针指向的数组插入一个该组对this->sortExpression求得的值。
可以用于当order by表达式为累积函数的情形。

**\[static\]`int findAttributeIndex(vector<Attribute> attrs, string name)`**

传入属性的名称，返回它在attrs中的下标。失败时返回attrs中属性个数。

#### 公有成员函数
**`Table(string name, vector<Attribute> a, string primary)`**

构造函数。 

@param name: 表名. attrs: 属性列表. primary: 主键名.
调用者应当避免不合法的参数，例如属性名冲突，主键名不存在于属性列表中，等等。
主键强制非空，无论传入的参数如何。

**`Table(const Table &) = delete`**

**`Table(Table &&)`**

**`bool insert(vector<string> attrNames, vector<ValueBase *> vals)`**

指定属性名`attrNames`和值`values`，插入新行。

值向量的顺序应当和参数`attrNames`的顺序匹配，但是后者不必和建表时指定属性名的顺序匹配。

返回是否成功。如果表中某一列不能为空但对应数据缺失，或者指定值类型与表格中属性类型不匹配，
或者指定的主键和已有的键发生冲突，或者重复指定了同一属性的值，就会发生失败。

在赋值时，传入的值会被复制而不是移动。即，传入值的指针不会被本`Table`对象接管。调用者
需要在插入操作完成后处理传入值的内存回收。

空值NULL以`nullptr`的形式传进来。

**`bool del(WhereClause c)`**

删除所有满足`whereClause`的行。

与本表无关的属性将会被忽略。如果`whereClause`中没有不被忽略的属性，表中所有的行都将被删除。

返回值总是`true`（成功）。

**`bool update(vector<string> attrNames, vector<ValueBase *> vals, WhereClause c)`**

更新满足`whereClause`的行，将`attrNames`中的属性值改为`vals`中的对应元素。

与本表无关的属性将会被忽略，无论实在`attrNames`中还是`whereClause`中。
如果没有不被忽略的属性`whereClause`，则表中所有的行都将被更新。

返回是否成功。如果指定的`vals`类型无法匹配，或者不能为空的列被修改为空，或主键被修改为了和已有
重复的值，会发生失败。

在赋值时，传入的值会被复制而不是移动。即，传入值的指针不会被本Table对象接管。调用者需要在
插入操作完成后处理传入值的内存回收。

**`PrintableTable * select(vector<string> attrFilter, WhereClause c)`**

select 第一阶段实现的朴素版本。现已废弃。

选出满足`whereClause`的行，取出包含在`attrFilter`中的属性，组成一张新表并返回。

`attrFilter`: 需要选出的属性名称。如果欲选出所有属性，请传入一个仅包含字符串“*”的向量。

**`PrintableTable * select(vector<Expression *> exps, Expression* where_clause, vector<Expression*> group_by, Expression* order_by)`**

select 表内分组排序版本。

选出满足`where_clause`的行，按照`group_by`分组、`order_by`排序，对`exps`中表达式进行求值，得到各列，组成一张新表并返回。
传入的表达式对象`Expression`不会被删除，需要调用者手动进行内存回收。

`exps`: 需要进行求值的表达式。接口扩展以后，传入的可以是"*"、属性名称、函数表达式、运算表达式等等一切可以归纳为一个表达式的对象。

`where_clause`: where子句。select函数选出使`where_clause`运算结果是`True`的记录。若表达式结果为`False`、`NULL`、非布尔类型，记录都不会被选中。

`group_by`: 需要特别分组的属性。按照其中属性进行分组，所有表达式只作用于同一组上。举例：

`SELECT stu_name, COUNT(*) from oop_info GROUP BY stu_name;`

选出同名学生名称和人数。

不过考虑到`exps`中会有属性表达式，最终分组结果由`exps`、`group_by`、`order_by`中的属性表达式共同决定。
只有函数`AVG`，`SUM`，`MAX`，`MIN`，`COUNT`需要这一特性。

`order_by`: 按照某一字段进行排序。这一字段不必出现在表达式`exps`里；但这一字段会参与分组

**`virtual ~Table()`**

**`ostream & show(ostream & out) const`**

通过传入的`ostream`对象输出表的属性信息。

### class PrintableTable

#### 私有成员变量
**`const vector<Attribute> attrs`**

属性列表。

**`vector<ValueBase **> data`**

数据列表。和`Table`不同之处在于，这里data使用二维动态数组存储。

#### 私有成员函数

**`ostream & printHead(ostream & os) const`**

打印表头，包含属性、表达式名称。

**`ostream & printData(ostream & os) const`**

打印表格内容，包含表格中存储的数据。

#### 公有成员函数
**`PrintableTable(vector<Attribute> a)`**
**`PrintableTable(vector<Expression *>  exps)`**

**`void insert(ValueBase ** vs)`**

插入一条记录。

**`void setData(const vector<Record> & data)`**

通过一组Record直接设置数据。

**`ostream & print(ostream & out)`**

通过给定的`ostream`将表的属性和全部数据输出。因此，理论上你可以通过`cout`输出，也可以输出到文件中，还可以通过`stringstream`转成`string`。

**`ofstream & print(ofstream & out)`**

针对文件输出的特殊版本print。只输出表格内容，不输出表头。

**`~PrintableTable()`**

## Attribute.h

### class Attribute

#### 公有成员变量
**`const AttributeType type`**

属性类型。

**`const bool notNull`**

是否禁止非空。

**`const string name`**

属性名称。

#### 公有成员函数
**`Attribute(string name, AttributeType type, bool notNull)`**

直接构造。

**`Attribute(string name, string type, bool notNull)`**

根据输入构造，其中的`type`可以为`CHAR`,`INT`, `DOUBLE`等SQL规定数据类型中的一种。在第一阶段实现中，仅支持以上三种类型。

### enum AttributeType
`enum AttributeType {ATTR_INT, ATTR_CHAR, ATTR_DOUBLE}`

枚举类型，SQL中支持的数据类型集合。

### fromStringToAttrType(const string & )

`AttributeType fromStringToAttrType(const string & n)`

将字符串形式的属性类型转换为枚举类型形式，以用于`Attribute`的构造。

`@param s`

属性类型。大写。暂时只接受`"INT"`, `"CHAR"`和`"DOUBLE"`。

注：我们用`string`类型存储字符，这看起来很奇怪，因为字符长度总是为1，用`char`似乎即可……但是我们并不想改回去，原因是`CHAR`在MySQL中还有可能是字符串。

## Record.h

### class Record
每一行的数据组成一个Record.

内存管理提示：Record析构时会将其内部的所有数据进行析构。请谨慎将Record声明为局部变量。
#### 私有成员变量
**`int n`**

记录中包含多少属性的数据。总是与其所属的`Table`属性数量相同。

**`ValueBase ** data`**

#### 公有成员函数

**`Record(int n)`**

**`Record(const Record & r) `**

复制构造函数。

**`Record(Record && r) `**

移动构造函数。

**`Record & operator=(const Record & r)`**

赋值运算符重载。

**`ValueBase *& operator[](int i) `**

下标运算符重载，可以修改对应的元素。

**`ValueBase * operator[](int i) const`**

下标运算符重载，可以通过下标访问常量`Record`对象的数据。

**`~Record()`**

## Value.h
### class ValueBase

数据库内所储存数据的抽象基类。

#### 保护成员变量
**`bool isNull`**

本对象是否是NULL。如果为假，那么这个对象表示的是值本身；
如果为真，无论对象为`ValueBase`的哪个派生类，存储的值是什么，都只表示NULL的含义。

#### 保护成员函数

**`ValueBase(bool null = false)`**

这是一个不对外部开放的构造函数。这允许ValueBase的工厂函数通过调用此构造函数来生成NULL对象。

**`virtual void setNull() final`**

允许子类通过这个函数来将本对象设为NULL。不过由于`isNull`不是私有而是保护的，这个函数意义不大。

#### 公有成员方法
**`virtual BoolValue operator==(const ValueBase & v) const = 0`**

**`virtual BoolValue operator>(const ValueBase & v) const = 0`**

**`virtual BoolValue operator<(const ValueBase & v) const = 0`**

**`virtual ValueBase * copy() const = 0`**

创建自身的一个副本。返回值类型BoolValue是bool的一个包装，还可以表示第三种结果NULL。BoolValue在后面会介绍。

**`virtual ostream & print(ostream & out) const = 0`**

通过指定的`ostream`打印本对象。和下面重载的运算符相比，这一个可以由子类实现。

**`friend ostream & operator<<(ostream & out, ValueBase & v)`**

重载运算符。

**`virtual ~ValueBase() = 0`**

#### 公有静态方法

**`ValueBase makeNull()`**

按值返回一个NULL对象。此函数作用上和`return Null<ValueBase>()`基本等价，建议使用`Null<ValueBase>`。

**`ValueBase* newNull()`**

在内存中分配一个NULL对象，并且返回指向它的指针。此函数作用上和`return new Null<ValueBase>()`基本等价，建议使用`new Null<ValueBase>`。

### class Value<T>: public ValueBase

#### 定义

`template<class T> class Value<T>: public ValueBase`

因为此模板类只提供对数据类型最简陋的包装，难以实现像`IntValue`和`DoubleValue`相比较的操作，因此现在只有`CharValue`用本模板实现。

程序内用`IntValue`, `DoubleValue`, `Value<string>`等类代表各种数据类型。（旧版使用`Value<int>`、`Value<double>`，改版后不再使用。）

NULL不是一种*类型*，而是一种*属性*。在程序内部，NULL会用不同的类的对象来表示。判断一个对象是否为NULL请用函数`isNull(v)`。

#### 私有成员变量

**`T v`**

该类所包装的数据。

#### 公有成员函数

**`operator T() const`**

类型转换函数。

**`BoolValue operator==(const ValueBase & v) const override`**

**`BoolValue operator>(const ValueBase & v) const override`**

**`BoolValue operator<(const ValueBase & v) const override`**

尝试将v转换为Value<T>然后进行比较；若转换失败，返回false。

**`Value<T> * copy() const override`** 

**`ostream & print(ostream & out) const`**

**`~Value()`**

### class BoolValue: public ValueBase

#### 私有成员变量

**`bool v`**

存储的数据。与此对象是否表示NULL无关。

#### 全局成员函数

**`BoolValue(bool v = false)`**

**`BoolValue(const BoolValue & v)`**

**`operator bool() const`**

自动类型转换，只返回内部存储的值`v`，与是否是NULL无关。

**`bool isTrue() const`**

判断本对象是否表示`True`。需要同时满足`v`为真且`isNull`为假。

**`ValueBase * copy() const override`**

**`ostream & print(ostream & out) const override`**

**`BoolValue operator&&(const BoolValue & v) const`**

**`BoolValue operator||(const BoolValue & v) const`**

**`BoolValue operator!() const`**

**`BoolValue operator^(const BoolValue & v) const`**

以上四个函数分别表示与、或、非、异或运算。

#### 静态成员函数

**`BoolValue makeNull(bool v = false)`**

返回一个表示NULL的`BoolValue`对象。支持传入一个参数的理由是在某些情况下，程序内部需要
用这个`BoolValue`对象表示一个二值的意义（只能为True和False，不能为NULL），但是它对用户的意义应当是NULL。

比如：NULL和其他值参与排序时，NULL需要排到其他值前面，`NULL < 1`的结果对用户而言应该是NULL，但是对于排序程序而言，结果应该是`true`。

这样设计的好处：扩展后直接把`bool`改为`BoolValue`，原有的接口保持不变（多亏了自动类型转换），可以在不考虑NULL的情况下继续正常工作

### class IntValue : public ValueBase

整数类型。支持和`IntValue`、`DoubleValue`的各种比较运算。

### class DoubleValue : public ValueBase

浮点数类型。支持和`IntValue`、`DoubleValue`的各种比较运算。

### typedef Value\<string\> CharValue

### 全局函数

**`bool isNull(ValueBase *)`**
**`bool isNull(const ValueBase & v)`**

判断传入的值是否是NULL。

## Expression.h

### class Expression

表达式抽象基类。

表达式用于select语句。在将来版本中可能将用于WhereClause语句。

#### 公有成员函数

**`string toString() const = 0`**

将Expression转为其所表达语句的字符串形式。

**`ValueBase * eval(const Record &, const vector<Attribute> & attrs) = 0`**

传入一条记录和记录各元素对应的属性名称，返回一个求得的值。

### class AttributeExpression : public Expression

属性表达式。它的作用相当于一个未知数，当作用到某一条记录上时，表达式的值为该记录此属性对应的值。

#### 私有成员变量

**`string name`**

属性名称。

#### 公有成员函数

**`AttributeExpression(const string& name)`**

**`string toString() override`**

**`ValueBase * eval(const Record &, const vector<Attribute> & attrs) override`**

### ConstExpression : public Expression

常量表达式。它无需作用在记录上也可以求值。

#### 私有成员变量

**`ValueBase * val`**

常量的值。

#### 公有成员函数

**`ConstExpression(ValueBase * v)`**

**`string toString() override`**

**`ValueBase * eval()`**

返回常量的值。

**`ValueBase * eval(const Record &, const vector<Attribute> & attrs) override`**

### class FunctionExpression : public Expression

函数表达式。它是一个抽象类。它没有什么自己的特点。

### class AggregationFunctionExpression : public FunctionExpression

累积函数表达式。它是一个抽象类。除了eval，它还规定了作用于一群记录的接口。

这一类函数有：AVG，SUM，MAX，MIN，COUNT（暂未全部实现）等。
它们需要一“组”而不是一“条”记录，因此单独抽象出来。

#### 公有成员函数

**`ValueBase * evalAggregate(vector<Record *>&, const vector<Attribute> & attrs) = 0`**

传入一系列记录和记录各元素对应的属性名称，进行累积运算，返回一个求得的值。

### class CountFunction : public AggregationFunctionExpression

COUNT函数。接受一个表达式，计数有多少使表达式结果非NULL的记录。

#### 公有成员函数

**`CountFunction(Expression *exp)`**

**`ValueBase * eval(const Record &, const vector<Attribute> & attrs)`**

**`ValueBase * evalAggregate(vector<Record *>&, const vector<Attribute> & attrs)`**

**`string toString()`**

### class OperatorExpression : public Expression

二元运算符基类。这是一个抽象类，但是实现了`toString`函数，节省子类的代码量。

#### 保护成员变量

**`Expression * left, * right`**

左操作数和右操作数。

**`string symbol`**

此运算的符号。用于toString。由子类在构造时候赋值。

#### 公有成员函数

**`OperatorExpression(Expression * a, Expression * b, string sym)`**

**`~OperatorExpression()`**

**`string toString() const`**

类似于`left+symbol+right`形式，将算式转化为字符串。

### class EqualOperator : public OperatorExpression

相等判断。适用于`IntValue`、`DoubleValue`之间，并适用于`CharValue`。返回一个BoolValue。

本类的公有成员函数与以下各类的相同，直到特殊说明为止。

#### 公有成员函数

**`EqualOperator(Expression * a, Expression * b)`**

**`ValueBase * eval(const Record &, const vector<Attribute> & attrs)`**

### class LessOperator : public OperatorExpression

小于判断。适用于`IntValue`、`DoubleValue`之间，并适用于`CharValue`。返回一个BoolValue。

### class GreaterOperator : public OperatorExpression

大于判断。适用于`IntValue`、`DoubleValue`之间，并适用于`CharValue`。返回一个BoolValue。

### class PlusOperator : public OperatorExpression

加法运算。适用于`IntValue`、`DoubleValue`之间。
如果两操作数都是`IntValue`，则返回`IntValue`；否则二者都提升至`DoubleValue`类型再运算，返回`DoubleValue`。

### class MinusOperator : public OperatorExpression

减法运算。适用于`IntValue`、`DoubleValue`之间。
如果两操作数都是`IntValue`，则返回`IntValue`；否则二者都提升至`DoubleValue`类型再运算，返回`DoubleValue`。

### class MultiplyOperator : public OperatorExpression

乘法运算。适用于`IntValue`、`DoubleValue`之间。
如果两操作数都是`IntValue`，则返回`IntValue`；否则二者都提升至`DoubleValue`类型再运算，返回`DoubleValue`。

### class DivideOperator : public OperatorExpression

除法运算。适用于`IntValue`、`DoubleValue`之间。
如果两操作数都是`IntValue`，则返回`IntValue`，结果为整除；否则二者都提升至`DoubleValue`类型再运算，返回`DoubleValue`，结果是真除。
当除数为0时，返回NULL。

### class ModOperator : public OperatorExpression

取模运算。适用于`IntValue`。
返回`IntValue`。
当除数为0时，返回NULL。

### class LogicAndOperator : public OperatorExpression

逻辑与运算。仅适用于`BoolValue`。

### class LogicOrOperator : public OperatorExpression

逻辑或运算。仅适用于`BoolValue`。

### class LogicXorOperator : public OperatorExpression

逻辑异或运算。仅适用于`BoolValue`。

### class LogicNotOperator : public FunctionExpression

逻辑非运算。仅适用于`BoolValue`。
这是一个一元操作符，可以看做一元函数，故直接继承自`FunctionExpression`。

#### 私有成员变量

**`Expression * exp`**

取反的表达式。

#### 公有成员函数

**`LogicNotOperator(Expression * v)`**

**`string toString() override`**

**`ValueBase * eval(const Record &, const vector<Attribute> & attrs) override`**

## sql.h

解析处理输入的sql语句

### class sql

解析输入的sql语言的基类，继承它的子类如下：

### class SQLCreateDatabase：

解析CREATE DATADATABASE命令。

### class SQLCreateTable：

解析 CREATE TABLE tableName(attrNameN TypeN)命令。

### class SQLShowColumns：

解析SHOW columns from tablenName命令。

### class SQLDropDatabase:

解析DROP DATABASE DBname命令。

### class SQLDropTable:

解析DROP TABLE tableName命令。

### class SQLUse:

解析USE DBname命令。

### class SQLInsert:

解析INSERT INTO [tableName(attrName1, attrName2,…, attrNameN)] VALUES (attrValue1, attrValue2,…, attrValueN) 命令。

### class SQLSelect:

解析SELECT AttrName FROM tableName WHERE whereClauses命令。

### class SQLDelete:

解析DELETE FROM tableName WHERE whereClauses命令。

### class SQLUpdate:

解析CREATE DATADATABASE命令。

### 主要接口：

**`void Parse(vector<string> sql) `**

以上数据类型根据各自处理的语法要求，分别对sql语句进行相应的分割和存储，方便后续接口操作进行。

## Interpreter.h

查询语句语法分析与结果输出，通过interpreter调用manager类型，根据输入调用合适的数据库接口，并将结果输出，是沟通程序内部数据库实现与外围输入输出的桥梁。

### class Interpreter

#### 私有成员变量

**` Manager *manager`**

存储了manager类型的指针，用于解析语句后，调用manager类的函数，实现功能。

**`string sql `**

存储输入语句。

**`vector<string> sql_vector `**

用向量存储输入语句的字符。

#### 公有成员函数

**`void SQL(string statement)`**

程序的外部接口，对sql命令进行相应的操作。

#### 私有成员函数
    
**` void SplitSQL(string statement)`**  

将输入的string类型命令依据空格、标点符号等切分为一组string向量，便于后续的操作。
    
**`void GetSQLType() `** 

得到sql命令的类型。
    
**` void ParseSQL()`** 

根据sql类型，调用manager中的函数，实现功能。
    
    


## Manager.h

### class Manager

包含一个database向量，是管理全部数据库的数据类型。

传入经过sql类型处理后的sql命令，调用相应函数，实现命令的要求。

#### 私有成员变量

**`string current_db `**

当前访问的数据库名称。

**`vector<Database> databases`**

存储数据库的向量。

**`int num `**

数据库总数

#### 公有成员函数

**` void CreateDatabase(SQLCreateDatabase& statement) `**

创建数据库指令 。
   
**`void CreateTable(SQLCreateTable& statement) `**

在使⽤USEDBname来选定数据库后，进行数据表的创建与删除。

**`void ShowDatabases()`**

列出现有的数据库。

**`void ShowTables()`**

列出现有表。

**`void ShowColumns(SQLShowColumns &statement); `**

列出制定表项的各项信息，包括属性、属性类型、主键信息。

**`void DropDatabase(SQLDropDatabase& statement) `**

删除数据库。

**` void DropTable(SQLDropTable& statement) `**

删除对应的表。
  
**`void Use(SQLUse& statement) `**

切换数据库。

**` void Insert(SQLInsert& statement)`**

表的数据加⼊。
  
**`void Select(SQLSelect& statement) `**

表的数据查询。

**`void Delete(SQLDelete& statement) `**

表的数据删除。

**`  void Update(SQLUpdate& statement)`**

表的数据修改。
  
**`DataBase *GetDB() `**

返回当前数据库的指针。

**`DataBase *GetDB(string db) `**

返回名为db的数据库的指针。

## Database.h

### class Database

存储每个特定数据库的的数据存储器，用于管理数据表。

#### 私有成员变量

**` vector<Table*> tables`**

存储数据表指针的向量。

#### 公有成员变量

**`const string name `**

数据库名称。

#### 公有成员函数
**` 	DataBase(string name)`**

构造函数。

**`void CreateTable(SQLCreateTable& statement) `**

创建数据表。

**`void DropTable(string name) `**

删除数据表。

**` Table* getTB(string name)`**

获得某个数据表的指针。

**`vector<Table*> GetTables() `**

返回存储数据表的指针的向量。

**`DataBase(const DataBase &) = delete `**

**`DataBase(DataBase && d) `**

**` DataBase & operator=(DataBase && d)`**

**`~DataBase() `**
