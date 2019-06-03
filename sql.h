/*sql类解析输入sql的语句*/
#pragma once
#include<string>
#include<vector>
#include<algorithm>
#include"Attribute.h"
#include"WhereClause.h"
#include "Expression.h"
#include<iostream>
using namespace std;

string to_lower(string str);
string to_upper(string str);
/**
 * 语句被切割后放在sql_vector中，本函数将sql_vector中从pos下标开始的where子句转换为其组成要素。
 */ 
unsigned int buildWhereClauseFrom(vector<string> sql_vector, 
		vector<pair<LogicOperation, int> > &o,
		vector<WhereClause::SubSentence> &s,
		unsigned int pos);

ValueBase * stringToValue(string);

Expression * parseExpression(string sentence);
vector<Expression *> readExpressionsFromString(const vector<string> &sql_vector, unsigned int & pos);
Expression * readExpressionFromString(const vector<string>& sql_vector, unsigned int & pos);

class SQL{
public:
	SQL():sql_type(-1){}
	SQL(int type):sql_type(type){}
	int get_sql_type(){return sql_type;}
	void set_sql_type(int type){sql_type=type;}
	virtual void Parse(vector<string> sql) = 0;
protected:
	int sql_type;
};

/* class: SQLCreateDatabase */
class SQLCreateDatabase: public SQL
{
public:
	SQLCreateDatabase(vector<string> sql);
	string get_db_name(){return database_name;}
	void set_db_name(string dbname){database_name=dbname;}
	void Parse(vector<string> sql);//对sql命令进行语法分析
private:
	string database_name;
};

/* class: SQLCreateTable: create table tbname (attr1, char(100), attr2, char(20), primary key(attr1));*/
class SQLCreateTable: public SQL 
{
public:
	SQLCreateTable(vector<string> sql);
	string get_tb_name(){return table_name;}
	string get_primary(){return primary;}
	void set_tb_name(string name){table_name=name;}
	vector<Attribute> GetAttributes(){return attrs;}
	void SetAttributes(vector<Attribute> attr){attrs=vector<Attribute>(attr);}
	void Parse(vector<string> sql);
private:
	string primary;
	string table_name;
	vector<Attribute> attrs;
};


class SQLShowColumns: public SQL 
{
public:
	SQLShowColumns(vector<string> sql);
	string get_tb_name();
	void Parse(vector<string> sql);
private:
	string tb_name;
};

class SQLDropDatabase: public SQL 
{
public:
	SQLDropDatabase(vector<string> sql);
	string get_db_name();
	void set_db_name(string dbname);
	void Parse(vector<string> sql);
private:
	string db_name;
};


class SQLDropTable: public SQL
{
public:
	SQLDropTable(vector<string> sql);
	string get_tb_name();
	void set_tb_name(string tbname);
	void Parse(vector<string> sql);
private:
	string tb_name;
};

/* class: SQLUse: use dbname; */
class SQLUse: public SQL
{
public:
	SQLUse(vector<string> sql);
	string get_db_name();
	void set_db_name(string dbname);
	void Parse(vector<string> sql);
private:
	string db_name;
};

/* class: SQLLoad: load data infile FILE_NAME into dbname; 
class SQLLoad: public SQL
{
public:
	SQLLoad(vector<string> sql);
	string get_db_name();
	void set_db_name(string dbname);
	string get_file_name();
	void set_file_name(string filename);
	void Parse(vector<string> sql);
private:
	string db_name;
	string file_name;
};*/

class SQLInsert: public SQL
{
public:
	SQLInsert(vector<string> sql);
	~SQLInsert();
	string get_tb_name();
	void Parse(vector<string> sql);
	vector<string> attrNames;
	vector<ValueBase *> vals;
	string tb_name;
};


class SQLSelect: public SQL
{
public:
	SQLSelect(vector<string> sql);
	string get_tb_name();
	bool if_load_file();
	string get_load_file_name();
	void Parse(vector<string> sql);
	vector<string> attrFilter;
	vector<WhereClause::SubSentence> s; 
	vector<pair<LogicOperation, int> > o;
	vector<Expression *> get_expressions();
	vector<Expression *> get_group_by();
	Expression * get_order_by();
	~SQLSelect();
private:
	vector<Expression *> expressions;
	vector<Expression *> group_by;
	Expression * order_by;
	string tb_name;
	bool load_file;
	string load_file_name;
};

class SQLDelete: public SQL
{
public:
	SQLDelete(vector<string> sql);
	string get_tb_name();
	void Parse(vector<string> sql);
	vector<WhereClause::SubSentence> s; 
	vector<pair<LogicOperation, int> > o;
	~SQLDelete();
private:
	string tb_name;
	
};

class SQLUpdate: public SQL
{
public:
	SQLUpdate(vector<string> sql);
	string get_tb_name();
	void Parse(vector<string> sql);
	vector<string> attrNames;
	vector<ValueBase *> vals;
	vector<WhereClause::SubSentence> s; 
	vector<pair<LogicOperation, int> > o;
	~SQLUpdate();
private:
	string tb_name;
};
