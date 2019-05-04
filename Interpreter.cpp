#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

Interpreter::Interpreter() : sql_type(-1){manager=new Manager;}

Interpreter::~Interpreter(){delete manager;}

bool seg(char c){
	if(c=='='||c=='>'||c=='<') return true;
	if(c=='('||c==')'||c==','||c==';') return true;
	return false;
}

void Interpreter::SplitSQL(string statement)
{
	char* cstr = const_cast<char*>(statement.c_str());
	char current[20];
	int i=0,j=0;
	while(i<statement.size()){
		if(statement[i]==' ') {
			i++;
			if(j){
			current[j]='\0';
			sql_vector.push_back(current);
			j=0;
			}
		}
		else if(seg(statement[i])) {
			if(j){
			current[j]='\0';
			sql_vector.push_back(current);
			}
			current[0]=statement[i];
			current[1]='\0';
			sql_vector.push_back(current);
			j=0;i++;
		}

		else{
			current[j++]=statement[i++];
		}
	}
}

void Interpreter::GetSQLType()
{
	if (sql_vector.size() == 0){
		sql_type = -1;
		cout << "logging: SQL TYPE: empty" << endl;
		return;
	}
	
	if (sql_vector[0] == "create")
	{
		if(sql_vector.size() <= 1){
			sql_type = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}
		to_lower(sql_vector[1]);
		if (sql_vector[1] == "database") { sql_type = 1; } /* CREATE DATABASE */
		else if (sql_vector[1] == "table") { sql_type = 2; } /* CREATE TABLE */
		else { sql_type = -1; }
	}
	else if (sql_vector[0] == "show")
	{
		if(sql_vector.size() <= 1){
			sql_type = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}
		to_lower(sql_vector[1]);
		if (sql_vector[1] == "databases") { sql_type = 11; } /* SHOW DATABASES */
		else if (sql_vector[1] == "tables") { sql_type = 12; } /* SHOW TABLES */
		else sql_type=13;
	}
	else if (sql_vector[0] == "drop")
	{
		if(sql_vector.size() <= 1){
			sql_type = -1;
			cout << "SyntaxError: use 'help;' to get correct command." << endl;
			return;
		}
		to_lower(sql_vector[1]);
		if (sql_vector[1] == "database") { sql_type = 3; } /* DROP DATABASE */
		else if (sql_vector[1] == "table") { sql_type = 4; } /* DROP TABLE */
	
	}
	else if (sql_vector[0] == "use") { sql_type = 5; } /* USE */
	else if (sql_vector[0] == "insert") { sql_type = 6; } /* INSERT */
	else if (sql_vector[0] == "select") { sql_type = 7; } /* SELECT */
	else if (sql_vector[0] == "delete") { sql_type = 8; } /* DELETE */
	else if (sql_vector[0] == "update") { sql_type = 9; } /* UPDATE */
	
}

void Interpreter::ParseSQL()
{
	switch (sql_type){
		case 11:
		{
			manager->ShowDatabases();
		}
			break;
		case 12:
		{
			manager->ShowTables();
		}
			break;
		case 13:
		{
			manager->ShowColumns();
		}

		case 1:
		{
			SQLCreateDatabase *st = new SQLCreateDatabase(sql_vector);
			manager->CreateDatabase(*st);
			delete st;
		}
			break;
		case 2:
		{
			SQLCreateTable *st = new SQLCreateTable(sql_vector);
			manager->CreateTable(*st);
			delete st;
		}
			break;
		
		case 3:
		{
			SQLDropDatabase *st = new SQLDropDatabase(sql_vector);
			manager->DropDatabase(*st);
			delete st;
		}
			break;
		case 4:
		{
			SQLDropTable *st = new SQLDropTable(sql_vector);
			manager->DropTable(*st);
			delete st;
		}
			break;
		
		case 5:
		{
			SQLUse *st = new SQLUse(sql_vector);
			manager->Use(*st);
			delete st;
		}
			break;
		case 6:
		{
			SQLInsert *st = new SQLInsert(sql_vector);
			manager->Insert(*st);
			delete st;
		}
			break;
		
		case 7:
		{
			SQLSelect *st = new SQLSelect(sql_vector);
			manager->Select(*st);
			delete st;
		}
			break;
		case 8:
		{
			SQLDelete *st = new SQLDelete(sql_vector);
			manager->Delete(*st);
			delete st;
		}
			break;
		case 9:
		{
			SQLUpdate *st = new SQLUpdate(sql_vector);
			manager->Update(*st);
			delete st;
		}
			break;
		default:
			break;
	}
}
	

void Interpreter::SQL(string statement)
{
	sql = statement;  /* get SQL statement */
	SplitSQL(statement);
	GetSQLType();		/* get sql type */
	ParseSQL();			/* construct SQL object and pass to manager */
}
