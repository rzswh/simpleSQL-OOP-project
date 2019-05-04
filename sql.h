/*interpreter类型查询语句语法分析与结果输出
根据输入调用合适的数据库接口，并将结果输出
沟通程序内部数据库实现与外围输入输出的桥梁
调用方式见sqlmain.cpp
*/
#pragma once
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include <string>
#include <vector>
#include "sql.h"
#include"manager.h"

using namespace std;

class Interpreter
{
public:
	Interpreter();
	~Interpreter();
	void SQL(string statement);
private:
	Manager *manager;
	string sql;
	vector<string> sql_vector;
	int sql_type;
	void SplitSQL(string statement);
	void GetSQLType();
	void ParseSQL();
};

#endif
