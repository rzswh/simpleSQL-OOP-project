#include"manager.h"


Manager::Manager():num(0){}


void Manager::CreateDatabase(SQLCreateDatabase& statement){
	cout << "Creating DataBase: " << statement.get_db_name() << endl;
	DataBase *temp=new DataBase(statement.get_db_name());//此处应修改为相应构造函数
	databases.push_back(*temp);
	num++;
}

DataBase* Manager::GetDB(){
	for(int i=0;i<num;i++)
		if(databases[i].get_db_name()==current_db)
			return &(databases[i]);
	return NULL;
}

DataBase* Manager::GetDB(string db){
	for(int i=0;i<num;i++)
		if(databases[i].get_db_name()==db)
			return &(databases[i]);
	return NULL;
}
void Manager::CreateTable(SQLCreateTable& statement){
	cout << "Creating table: " << statement.get_tb_name() << endl;

	DataBase *db = GetDB();
	db->CreateTable(statement);
}

void Manager::ShowDatabases()
{
	//cout << "here" << endl;
	if (num == 0)
	{
		cout << "No databases exist now." << endl;
		cout << "Use 'create DataBase' command to create a new DataBase." << endl;
		return;
	}
	cout<< "The number of databases is"<<num<<endl;
	for (int i=0; i<num; i++)
		cout<<i<<":"<<databases[i].get_db_name() << endl;
}

void Manager::ShowTables()
{
	if (current_db.size() == 0) return;
	DataBase *db = GetDB();
	if (db == NULL) return;
	if (db->GetTables().size() == 0)
	{
		cout << "No table exist now." << endl;
		cout << "Use 'create table' command to create a new table." << endl;
		return;
	}
	cout << "| " << setw(22) << "Tables_in_"+ current_db << " |" << endl;
	
	for (auto tb = db->GetTables().begin(); tb != db->GetTables().end(); tb++)
		cout << "| " << setw(22) << (*tb)->name << " |" << endl;
	
}

void Manager::ShowColumns(SQLShowColumns &statement){
	if (current_db.size() == 0) return;
	DataBase *db = GetDB();
	if (db == NULL) return;
	if (db->GetTables().size() == 0)
	{
		cout << "No table exist now." << endl;
		cout << "Use 'create table' command to create a new table." << endl;
		return;
	}
	Table *tb=db->getTB(statement.get_tb_name());
	/*列出制定表项的各项信息，包括属性、属性类型、主键 信息。*/
	cout<<tb;

}


void Manager::DropDatabase(SQLDropDatabase& statement)
{
	cout << "Droping DataBase: " << statement.get_db_name() << endl;
	
	//to be finished: delete statement.get_db_name; 
	
	if (statement.get_db_name() == current_db){
		current_db = "";
	}
}

void Manager::DropTable(SQLDropTable& statement)
{
	cout << "Droping table: " << statement.get_tb_name() << endl;
	if (current_db.length() == 0) return;

	DataBase *db = GetDB();
	if (db == NULL) return;

	Table *tb = db->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	
	db->DropTable(statement.get_tb_name());
}

void Manager::Use(SQLUse& statement)
{
	DataBase *db = GetDB(statement.get_db_name());
	if (db == NULL) return;
	current_db = statement.get_db_name();
}

void Manager::Insert(SQLInsert& statement)
{
	if (current_db.length() == 0) return;
	DataBase *db = GetDB();
	if (db == NULL) return;
	
	string tb_name=statement.get_tb_name();
	Table *tb=db->getTB(tb_name);
	tb->insert(statement.attrNames,statement.vals);
}

void Manager::Select(SQLSelect& statement)
{
	if (current_db.length() == 0) return;
	Table *tb = GetDB()->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	WhereClause c(statement.s,statement.o);
	tb->select(statement.attrFilter,c);	
}

void Manager::Delete(SQLDelete& statement)
{
	if (current_db.length() == 0) return;
	Table *tb = GetDB()->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	WhereClause c(statement.s,statement.o);
	tb->del(c);
}

void Manager::Update(SQLUpdate& statement)
{
	if (current_db.length() == 0) return;
	DataBase *db = GetDB();
	if (db == NULL) return;

	Table *tb = db->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	WhereClause c(statement.s,statement.o);
	tb->update(statement.attrNames,statement.vals,c);
}
