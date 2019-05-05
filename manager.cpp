#include"manager.h"

// #define DEBUG

Manager::Manager():num(0){}


void Manager::CreateDatabase(SQLCreateDatabase& statement){
	#ifdef DEBUG
	cout << "Creating DataBase: " << statement.get_db_name() << endl;
	#endif
	//DataBase *temp=new DataBase(statement.get_db_name());//此处应修改为相应构造函数
	//databases.push_back(*temp);
	databases.push_back(DataBase(statement.get_db_name()));
	//cout << (databases.back()).name << "---" << endl;
	num++;
}

DataBase* Manager::GetDB(){
	for(int i=0;i<num;i++)
		if(databases[i].name==current_db)
			return &(databases[i]);
	return NULL;
}

DataBase* Manager::GetDB(string db){
	for(int i=0;i<num;i++)
		if(databases[i].name==db)
			return &(databases[i]);
	return NULL;
}
void Manager::CreateTable(SQLCreateTable& statement){
	#ifdef DEBUG
	cout << "Creating table: " << statement.get_tb_name() << endl;
	#endif

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
	//cout<< "The number of databases is "<<num<<endl;
	for (int i=0; i<num; i++)
		cout<</*i<<":"<<*/databases[i].name << endl;
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
	tb->show(cout);

}


void Manager::DropDatabase(SQLDropDatabase& statement)
{
	#ifdef DEBUG
	cout << "Droping DataBase: " << statement.get_db_name() << endl;
	#endif
	
	//to be finished: delete statement.get_db_name; 
	for (auto iter = databases.begin(); iter != databases.end(); iter++) {
		if (iter->name == statement.get_db_name()) {
			databases.erase(iter);
			num--;
			break;
		}
	}
	
	if (statement.get_db_name() == current_db){
		current_db = "";
	}
}

void Manager::DropTable(SQLDropTable& statement)
{
	#ifdef DEBUG
	cout << "Droping table: " << statement.get_tb_name() << endl;
	#endif
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
	#ifdef DEBUG
	cout << "Item Info: ***" << endl;
	for (auto i: statement.attrNames) cout << i << " "; cout << endl;
	for (auto i: statement.vals) (i->print(cout)) << " "; cout << endl << "*****" << endl;
	#endif
	if (!tb->insert(statement.attrNames,statement.vals)) {
		cout << "Insertion failed. " << endl;
	}
}

void Manager::Select(SQLSelect& statement)
{
	if (current_db.length() == 0) return;
	Table *tb = GetDB()->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	WhereClause c(statement.s,statement.o);
	tb->select(statement.attrFilter,c)->print(cout);
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
	if (!tb->update(statement.attrNames,statement.vals,c) ) {
		cout << "Updating failed. Error Meesage: " << tb->getErrorMsg() << endl;
	}
}
