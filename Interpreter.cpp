#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// #define DEBUG

Interpreter::Interpreter() : sql_type(-1){manager=new Manager;}

Interpreter::~Interpreter(){delete manager;}

bool seg(char c){
	if(c=='='||c=='>'||c=='<') return true;
	if(c=='('||c==')'||c==','||c==';') return true;
	if(c=='+'||c=='-'||c=='*'||c=='/' || c=='%') return true;
	return false;
}

void Interpreter::SplitSQL(string statement)
{
	char* cstr = const_cast<char*>(statement.c_str());
	char current[256];
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
	sql_vector[0] = to_lower(sql_vector[0]);
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
		sql_vector[1] = to_lower(sql_vector[1]);
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
		sql_vector[1] = to_lower(sql_vector[1]);
		if (sql_vector[1] == "databases") { sql_type = 11; } /* SHOW DATABASES */
		else if (sql_vector[1] == "tables") { sql_type = 12; } /* SHOW TABLES */
		else sql_type=13;/*SHOW COLUMNS*/
	}
	else if (sql_vector[0] == "drop")
	{
		if(sql_vector.size() <= 1){
		}
		sql_vector[1] = to_lower(sql_vector[1]);
		if (sql_vector[1] == "database") { sql_type = 3; } /* DROP DATABASE */
		else if (sql_vector[1] == "table") { sql_type = 4; } /* DROP TABLE */
		else { sql_type = -1; } // undefined operation
	
	}
	else if (sql_vector[0] == "use") { sql_type = 5; } /* USE */
	else if (sql_vector[0] == "insert") { sql_type = 6; } /* INSERT */
	else if (sql_vector[0] == "select") { sql_type = 7; } /* SELECT */
	else if (sql_vector[0] == "delete") { sql_type = 8; } /* DELETE */
	else if (sql_vector[0] == "update") { sql_type = 9; } /* UPDATE */
	else if (sql_vector[0] == "load") { sql_type = 10; } /* LOAD */
	else {
		sql_type = -1;
		cout << "SyntaxError: use 'help;' to get correct command." << endl;
		return;
	}
	
}

bool isEnd(string str){
	if(str==";"||str=="union") return true;
	return false;
}

void Interpreter::ParseSQL()
{
	switch (sql_type){
		case 11:
		{
			manager->ShowDatabases();
			break;
		}
		case 12:
		{
			manager->ShowTables();
			break;
		}
		case 13:
		{
			SQLShowColumns *st = new SQLShowColumns(sql_vector);
			manager->ShowColumns(*st);
			delete st;
			break;
		}

		case 1:
		{
			SQLCreateDatabase *st = new SQLCreateDatabase(sql_vector);
			manager->CreateDatabase(*st);
			delete st;
			break;
		}
		case 2:
		{
			SQLCreateTable *st = new SQLCreateTable(sql_vector);
			manager->CreateTable(*st);
			delete st;
			break;
		}
		
		case 3:
		{
			SQLDropDatabase *st = new SQLDropDatabase(sql_vector);
			manager->DropDatabase(*st);
			delete st;
			break;
		}
		case 4:
		{
			SQLDropTable *st = new SQLDropTable(sql_vector);
			manager->DropTable(*st);
			delete st;
			break;
		}
		
		case 5:
		{
			SQLUse *st = new SQLUse(sql_vector);
			manager->Use(*st);
			delete st;
			break;
		}
		case 6:
		{
			SQLInsert *st = new SQLInsert(sql_vector);
			manager->Insert(*st);
			delete st;
			break;
		}
		
		case 7:
		{
			if(sql_vector[4]=="union"){
				vector<vector<string>> sql_vectors;
				vector<int>type;
				int q=0;
				type.push_back(0);
				bool first=true;
				while(q<sql_vector.size()){
				vector<string> sql_temp;
				while(!isEnd(sql_vector[q]))
					sql_temp.push_back(sql_vector[q++]);
				sql_temp.push_back(";");
				sql_vectors.push_back(sql_temp);
				q++;
				if(q<sql_vectors.size()&&sql_vector[q]=="all") {type.push_back(1);q++;}
				else type.push_back(0);
				}
				manager->SelectUnion(sql_vectors,type);
			}

			else{
			SQLSelect *st = new SQLSelect(sql_vector);
			manager->Select(*st);
			delete st;}
			break;
		}
			
		case 8:
		{
			SQLDelete *st = new SQLDelete(sql_vector);
			manager->Delete(*st);
			delete st;
			break;
		}
		case 9:
		{
			SQLUpdate *st = new SQLUpdate(sql_vector);
			manager->Update(*st);
			delete st;
			break;
		}
		case 10:
		{
			SQLLoad *st = new SQLLoad(sql_vector);
			manager->Load(*st);
			delete st;
			break;
		}
		default:
			break;
	}
}
	

void Interpreter::SQL(string statement)
{
	sql_vector.clear();
	sql = statement;  /* get SQL statement */
	if (sql.empty())
		return;
	SplitSQL(statement);
	GetSQLType();		/* get sql type */
	#ifdef DEBUG
	cout << "[debug] split into: ";
	for (auto i: sql_vector) { cout << i << " "; }
	cout << endl;
	#endif
	ParseSQL();			/* construct SQL object and pass to manager */
}
