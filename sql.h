/*sql类解析输入sql的语句*/
#pragma once
#include<string>
#include<vector>
#include<algorithm>
#include"Attribute.h"
#include"WhereClause.h"
#include<iostream>
using namespace std;

string to_lower(string str);

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


class SQLInsert: public SQL
{
public:
	SQLInsert(vector<string> sql);
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
	void Parse(vector<string> sql);
	vector<string> attrFilter;
	vector<WhereClause::SubSentence> s; 
	vector<pair<LogicOperation, int> > o;
private:
	string tb_name;
};

class SQLDelete: public SQL
{
public:
	SQLDelete(vector<string> sql);
	string get_tb_name();
	void Parse(vector<string> sql);
	vector<WhereClause::SubSentence> s; 
	vector<pair<LogicOperation, int> > o;
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
private:
	string tb_name;
};
