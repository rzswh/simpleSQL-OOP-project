#include"DataBase.h"
#include"sql.h"
#include"Table.h"
#include<vector>
DataBase::DataBase(string name):name(name)
{

}
void DataBase::CreateTable(SQLCreateTable& statement)
{
	tables.push_back(new Table(statement.get_tb_name(), statement.GetAttributes(), statement.get_primary()));
}
void DataBase::DropTable(string name)
{
	for(int i = 0; i < tables.size(); ++i)
		if(tables[i]->name == name)
		{
			tables.erase(tables.begin() + i); //
			break;
		}
}
Table* DataBase::getTB(string name)
{
	for(int i = 0; i < tables.size(); ++i)
		if(tables[i]->name == name)
			return tables[i];
}
vector<Table*> DataBase::GetTables()
{
	return tables;
}
DataBase::~DataBase() {
	for (auto i: tables)
		delete i;
	tables.clear();
}
