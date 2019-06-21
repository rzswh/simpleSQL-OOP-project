#include"manager.h"
#include<algorithm>

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
	cout<< "Database"<<endl;
	vector<string> output;
	for (int i=0; i<num; i++) output.push_back(databases[i].name);
	std::sort(output.begin(), output.end());
	for (auto &i: output) cout<< i << endl;
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
	// cout << "| " << setw(22) << "Tables_in_"+ current_db << " |" << endl;
	cout << "Tables_in_" << current_db << endl;

	vector<string> output;
	auto tables = db->GetTables();
	for (auto tb = tables.begin(); tb != tables.end(); tb++)
		output.push_back((*tb)->name);
	std::sort(output.begin(), output.end());
	for (auto &i: output) cout<< i << endl;
	// for (auto tb = db->GetTables().begin(); tb != db->GetTables().end(); tb++)
	// 	cout << "| " << setw(22) << (*tb)->name << " |" << endl;
	
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
	//WhereClause c(statement.s,statement.o);
	//PrintableTable * res = tb->select(estatement.attrFilter,c);
	PrintableTable * res = tb->select(statement.get_expressions(), statement.where_clause, 
			statement.get_group_by(), statement.get_order_by());
	if (!statement.if_load_file()) {
		res->print(cout);
	} else {
		ofstream fout(statement.get_load_file_name());
		res->print(fout);
		fout.close();
	}
	delete res;
}

void Manager::SelectUnion(vector<vector<string>> sql_vectors,vector<int>union_type)
{
	
	if (current_db.length() == 0) return;
	SQLSelect *statement = new SQLSelect(sql_vectors[0]);
	Table *tb = GetDB()->getTB(statement->get_tb_name());
	PrintableTable *res = tb->select(statement->get_expressions(), statement->where_clause, 
			statement->get_group_by(), statement->get_order_by());
	
	for(int i=1;i<sql_vectors.size();i++){
	SQLSelect *temp_st = new SQLSelect(sql_vectors[i]);
	Table *temp_tb = GetDB()->getTB(temp_st->get_tb_name());
	PrintableTable *temp_res = temp_tb->select(temp_st->get_expressions(), temp_st->where_clause, 
			temp_st->get_group_by(), temp_st->get_order_by());
	if(union_type[i]==0)
		res->unionTable(*temp_res);
	else res->unionAllTable(*temp_res);
	}

	if (!statement->if_load_file()) {
		res->print(cout);
	} else {
		ofstream fout(statement->get_load_file_name());
		res->print(fout);
		fout.close();
	}

	delete statement;
	delete res;
}


void Manager::Delete(SQLDelete& statement)
{
	if (current_db.length() == 0) return;
	Table *tb = GetDB()->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	// WhereClause c(statement.s,statement.o);
	tb->del(statement.whereClause);
}

void Manager::Update(SQLUpdate& statement)
{
	if (current_db.length() == 0) return;
	DataBase *db = GetDB();
	if (db == NULL) return;

	Table *tb = db->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	// WhereClause c(statement.s,statement.o);
	if (!tb->update(statement.attrNames,statement.vals,statement.whereClause) ) {
		cout << "Updating failed. Error Meesage: " << tb->getErrorMsg() << endl;
	}
}

void Manager::Load(SQLLoad& statement)
{
	if (current_db.length() == 0) return;
	DataBase *db = GetDB();
	if (db == NULL) return;
	Table *tb = db->getTB(statement.get_tb_name());
	if (tb == NULL) return;
	for (auto i : statement.vals)
	{
		tb->insert(statement.attrNames, i);
	}
}
