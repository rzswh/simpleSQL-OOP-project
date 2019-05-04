# cxx部分代码文档
# 主要文件说明
## sql.h
解析处理输入的sql语句
### SQL数据类型：
解析输入的sql语言的基类，继承它的子类如下：
### 1.class SQLCreateDatabase：
解析*CREATE DATADATABASE*命令
### 2.class SQLCreateTable：
解析 CREATE TABLE tableName(attrNameN TypeN)命令
### 3.class SQLShowColumns：
解析*SHOW columns from tablenName*命令
### 4.class SQLDropDatabase:
解析*DROP DATABASE DBname*命令
### 5.class SQLDropTable:
解析*DROP TABLE tableName*命令
### 6.class SQLUse:
解析*USE DBname*命令
### 7.class SQLInsert:
解析INSERT INTO [tableName(attrName1, attrName2,…, attrNameN)] VALUES (attrValue1, attrValue2,…, attrValueN) 命令
### 8.class SQLSelect:
解析*SELECT AttrName FROM tableName WHERE whereClauses*命令
### 9.class SQLDelete:
解析*DELETE FROM tableName WHERE whereClauses*命令
### 10.class SQLUpdate:
解析*CREATE DATADATABASE*命令

### 主要接口：
    void Parse(vector<string> sql)；

以上数据类型根据各自处理的语法要求，分别对sql语句进行相应的分割和存储，方便后续接口操作进行。

## manager.h
### class Manager：
是管理全部database的数据类型

传入经过sql类型处理后的sql命令，调用相应函数，实现命令的要求
### 主要接口：
     //创建数据库指令 ：
	void CreateDatabase(SQLCreateDatabase& statement);
	
	//在使⽤USEDBname来选定数据库后，进行数据表的创建与删除:
	void CreateTable(SQLCreateTable& statement);
	
	//列出现有的数据库：
	void ShowDatabases();

	//列出现有表:
	void ShowTables();
	
	//列出制定表项的各项信息，包括属性、属性类型、主键信息:
	void ShowColumns(SQLShowColumns &statement);
	
	//删除数据库:
	void DropDatabase(SQLDropDatabase& statement);
	
	//删除对应的表：
	void DropTable(SQLDropTable& statement);
	
	 //切换数据库:
	void Use(SQLUse& statement);
	
	//表的数据加⼊:
	void Insert(SQLInsert& statement);
	
	//表的数据查询:
	void Select(SQLSelect& statement);
	
	//表的数据删除:
	void Delete(SQLDelete& statement);
	
	//表的数据修改:
	void Update(SQLUpdate& statement);
	
	//返回当前数据库的指针:
	DataBase *GetDB();
	
	//返回名为db的数据库的指针:
	DataBase *GetDB(string db);
	
以上接口通过调用Table,DataBase等类中的接口实现功能

## interpreter.h
interpreter类型查询语句语法分析与结果输出，通过interpreter调用manager类型，根据输入调用合适的数据库接口，并将结果输出，是沟通程序内部数据库实现与外围输入输出的桥梁。

### 主要接口：
    
    //程序的外部接口，对sql命令进行相应的操作：
	void SQL(string statement);

	//将输入的string类型命令且分为一组string向量，便于后续的操作：
	void SplitSQL(string statement);
	
	//得到sql命令的类型：
	void GetSQLType();
	
	//根据sql类型，调用manager中的函数，实现功能：
	void ParseSQL();
