#pragma once
#include<vector>
#include<string>
#include"Table.h"
#include"sql.h"
using std::string;
using std::vector;
class DataBase
{
public:
	DataBase(string name);
	void CreateTable(SQLCreateTable& statement);
	void DropTable(string name);
	Table* getTB(string name);
	DataBase(const DataBase &) = delete;
	DataBase(DataBase &&) = default;
	vector<Table*> GetTables();
private:
	vector<Table> tables;
	const string name;
};