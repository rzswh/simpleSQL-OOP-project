#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include"Table.h"
#include"sql.h"
using namespace std;



/*Manager类是管理database的数据类型*/

class Manager{
public:
	void CreateDatabase(SQLCreateDatabase& statement);
	void CreateTable(SQLCreateTable& statement);
	void ShowDatabases();
	void ShowTables();
	void ShowColumns();
	void DropDatabase(SQLDropDatabase& statement);
	void DropTable(SQLDropTable& statement);
	void Use(SQLUse& statement);
	void Insert(SQLInsert& statement);
	void Select(SQLSelect& statement);
	void Delete(SQLDelete& statement);
	void Update(SQLUpdate& statement);
	Database *GetDB();//返回当前数据库的指针
	Database *GetDB(string db);//返回名为db的数据库的指针
	Manager();
private:
	string current_db;
	vector<Database> databases;
	int num;

};
