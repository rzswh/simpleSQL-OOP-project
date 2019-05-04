#include"sql.h"	
#include<stack>

/* -------------- SQLCreateDatabase ----------------- */
SQLCreateDatabase::SQLCreateDatabase(vector<string> sql) { Parse(sql); }

void SQLCreateDatabase::Parse(vector<string> sql)
{
	sql_type = 1;
	if (sql.size() <= 2) return;
	else
		database_name = sql[2];
}

/* -------------- SQLCreateTable ----------------- */
SQLCreateTable::SQLCreateTable(vector<string> sql) { Parse(sql); }

void SQLCreateTable::Parse(vector<string> sql_vector)
{
	sql_type = 2;
	unsigned int pos = 2;
	if (sql_vector.size() <= pos) return;
	
	//cout << "SQLCreateTable::Parse().logging: Table Name: " << sql_vector[pos] << endl;
	table_name = sql_vector[pos];
	pos ++;

	if (sql_vector[pos] != "(") return;
	pos ++;

	bool is_attr = true;
	bool has_primary_key = false;

	while (is_attr)
	{
		is_attr = false;
		
		if (sql_vector[pos] != "primary")
		{
			bool notnull=false;
			if(sql_vector[pos+2]!=",")
				notnull=true;
			Attribute attr(sql_vector[pos],sql_vector[pos+1],notnull);
			attrs.push_back(attr);

			if(notnull) pos+=5;
			else pos+=3;
			if (sql_vector[pos] != ")") is_attr = true;
		}
		else  /* primary key */
		{
			if (has_primary_key) return;
			pos +=3;
			primary=sql_vector[pos];
			/*for (auto att = attrs.begin(); att != attrs.end() ; att++)
			{
				if ((*att).name == sql_vector[pos])
				{
					(*att).type=1;//设定为主键
				}
			}*/
			pos +=2;
			has_primary_key = true;
		}
	}
}

/* -------------- SQLShowColumns -------------------- */
SQLShowColumns::SQLShowColumns(vector<string> sql_vector) { Parse(sql_vector); }

string SQLShowColumns::get_tb_name() { return tb_name; }

void SQLDropTable::Parse(vector<string> sql_vector)
{
	sql_type = 13;
	tb_name = sql_vector[3];
}


/* -------------- SQLDropDatabase ----------------- */
SQLDropDatabase::SQLDropDatabase(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropDatabase::get_db_name() { return db_name; }

void SQLDropDatabase::set_db_name(string dbname) { db_name = dbname; }

void SQLDropDatabase::Parse(vector<string> sql_vector)
{
	sql_type = 3;
	db_name = sql_vector[2];
}

/* -------------- SQLDropTable -------------------- */
SQLDropTable::SQLDropTable(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDropTable::get_tb_name() { return tb_name; }

void SQLDropTable::set_tb_name(string tbname) { tb_name = tbname; }

void SQLDropTable::Parse(vector<string> sql_vector)
{
	sql_type = 4;
	tb_name = sql_vector[2];
}



/* -------------- SQLUse ----------------- */
SQLUse::SQLUse(vector<string> sql_vector) { Parse(sql_vector); }

string SQLUse::get_db_name() { return db_name; }

void SQLUse::set_db_name(string dbname) { db_name = dbname; }

void SQLUse::Parse(vector<string> sql_vector)
{
	sql_type = 5;
	db_name=  sql_vector[1];
}

/* ----------------- SQLInsert ------------------ */
SQLInsert::SQLInsert(vector<string> sql_vector) { Parse(sql_vector); }

string SQLInsert::get_tb_name() { return tb_name; }

void SQLInsert::Parse(vector<string> sql_vector)
{
	sql_type = 6;
	unsigned int pos = 2;
	bool is_attr = true;
	tb_name = sql_vector[pos];
	pos =+2;
	int num=0;
	while (is_attr)
	{
		is_attr = false;
		attrNames.push_back(sql_vector[pos]);
		num++;
		pos ++;
		if (sql_vector[pos] != ")") is_attr = true;
		pos ++;
	}

	pos=+2;
	for(int i=0;i<num;i++)
	{
		string tmp=sql_vector[pos];
		if(tmp[0]=='\"'){
			char ttt[20];int kk=1;
			for(;tmp[kk]!='\"';kk++)
				ttt[kk-1]=tmp[kk];
			ttt[kk]='\0';
			Value<string> tt(ttt);
			vals.push_back(&tt);
		}
		else if(tmp.find('.')==string::npos){
			Value<double> tt(atof(tmp.c_str()));		//string 转 double	
			vals.push_back(&tt);
		}
		else {int ttt=atoi(tmp.c_str());
			Value<int> tt(ttt);
			vals.push_back(&tt);
		pos+=2;
		}	
	}
}

/* ---------------- SQLSelect ----------------- */
SQLSelect::SQLSelect(vector<string> sql_vector) { Parse(sql_vector); }

string SQLSelect::get_tb_name() { return tb_name; }

void SQLSelect::Parse(vector<string> sql_vector) /* only support "select * ". */
{
	sql_type = 7;
	unsigned int pos = 1;
	while(true){
		if(sql_vector[pos]=="*") {attrFilter.push_back("*");break;}
		else{
			attrFilter.push_back(sql_vector[pos++]);
			if(sql_vector[pos]=="from") break;
			pos++;
		}
	}
	pos++;
	tb_name = sql_vector[pos];
	pos++;
	if (sql_vector.size() == pos) return; /* sql statement like: "select * from tb;". */
	pos ++;
	
	stack<LogicOperation> ss;
	int rank=0;
	while (true)
	{
		if(sql_vector[pos]=="and"||"or"){
			if(ss.size){
				LogicOperation tt=ss.top();
				ss.pop();
				o.push_back(make_pair<LogicOperation, int>(tt,rank++));}
			if(sql_vector[pos]=="and")
			ss.push(LOGIC_AND);
			else ss.push(LOGIC_OR);
		}

		else{
		string key;
		ArithmicOperation ar;
		ValueBase * vb;
		key = sql_vector[pos];
		pos ++;
		if (sql_vector[pos] == "<") ar = ARITH_LESS;
		else if (sql_vector[pos] == "=") ar = ARITH_EQUAL;
		else if (sql_vector[pos] == ">") ar = ARITH_GREATER;
		pos ++;

		string tmp=sql_vector[pos];
		if(tmp[0]=='\"'){char ttt[20];int kk=1;
			for(;tmp[kk]!='\"';kk++)
				ttt[kk-1]=tmp[kk];
			ttt[kk]='\0';
			vb=new Value<string>(ttt);
		}
		else if(tmp.find('.')==string::npos){
			vb=new Value<double>(atof(tmp.c_str()));
		}
		else {int ttt=atoi(tmp.c_str());
		vb=new Value<int>(ttt);}
		
	    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>(key, ar,vb,rank++)); // C
		
	}
		pos++;
		if (sql_vector[pos] == ";") break; /* one where condition, break. */
	}

}


/* ---------------- SQLDelete ---------------- */
SQLDelete::SQLDelete(vector<string> sql_vector) { Parse(sql_vector); }

string SQLDelete::get_tb_name() { return tb_name; }


void SQLDelete::Parse(vector<string> sql_vector) 
{
	sql_type = 8;
	unsigned int pos = 2;
	tb_name = sql_vector[pos];
	pos ++;
	
	if (sql_vector.size() == pos) return; /* sql statement like: "delete from tb;". */
	pos ++;
	stack<LogicOperation> ss;
	int rank=0;
	while (true)
	{
		if(sql_vector[pos]=="and"||"or"){
			if(ss.size){
				LogicOperation tt=ss.top();
				ss.pop();
				o.push_back(make_pair<LogicOperation, int>(tt,rank++));}
			if(sql_vector[pos]=="and")
			ss.push(LOGIC_AND);
			else ss.push(LOGIC_OR);
		}

		else{
		string key;
		ArithmicOperation ar;
		ValueBase * vb;
		key = sql_vector[pos];
		pos ++;
		if (sql_vector[pos] == "<") ar = ARITH_LESS;
		else if (sql_vector[pos] == "=") ar = ARITH_EQUAL;
		else if (sql_vector[pos] == ">") ar = ARITH_GREATER;
		pos ++;

		string tmp=sql_vector[pos];
		if(tmp[0]=='\"'){char ttt[20];int kk=1;
			for(;tmp[kk]!='\"';kk++)
				ttt[kk-1]=tmp[kk];
			ttt[kk]='\0';
			vb=new Value<string>(ttt);
		}
		else if(tmp.find('.')==string::npos){
			vb=new Value<double>(atof(tmp.c_str()));
		}
		else {int ttt=atoi(tmp.c_str());
		vb=new Value<int>(ttt);}
		
	    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>(key, ar,vb,rank++)); // C
		
	}
		pos++;
		if (sql_vector[pos] == ";") break; /* one where condition, break. */
	}
}

/* ---------------  SQLUpdate ----------------*/
SQLUpdate::SQLUpdate(vector<string> sql_vector) { Parse(sql_vector); }

string SQLUpdate::get_tb_name() { return tb_name; }

void SQLUpdate::Parse(vector<string> sql_vector)
{
	sql_type = 9;
	unsigned int pos = 1;
	if (sql_vector.size() <= 1) return;
	tb_name = sql_vector[pos];
	pos +=2;

	while (true)
	{
		attrNames.push_back(sql_vector[pos]);
		pos +=2;
		string tmp=sql_vector[pos];
		ValueBase *vb;
		if(tmp[0]=='\"'){char ttt[20];int kk=1;
			for(;tmp[kk]!='\"';kk++)
				ttt[kk-1]=tmp[kk];
			ttt[kk]='\0';
			vb=new Value<string>(ttt);
		}

		else if(tmp.find('.')==string::npos){
			vb=new Value<double>(atof(tmp.c_str()));
		}
		else {int ttt=atoi(tmp.c_str());
		vb=new Value<int>(ttt);}
		
		vals.push_back(vb);

		pos ++;

		if (sql_vector[pos] == ",") pos ++;
		else if (to_lower(sql_vector[pos]) == "where") break;
	}
	pos ++;

	stack<LogicOperation> ss;
	int rank=0;
	while (true)
	{
		if(sql_vector[pos]=="and"||"or"){
			if(ss.size){
				LogicOperation tt=ss.top();
				ss.pop();
				o.push_back(make_pair<LogicOperation, int>(tt,rank++));}
			if(sql_vector[pos]=="and")
			ss.push(LOGIC_AND);
			else ss.push(LOGIC_OR);
		}

		else{
		string key;
		ArithmicOperation ar;
		ValueBase * vb;
		key = sql_vector[pos];
		pos ++;
		if (sql_vector[pos] == "<") ar = ARITH_LESS;
		else if (sql_vector[pos] == "=") ar = ARITH_EQUAL;
		else if (sql_vector[pos] == ">") ar = ARITH_GREATER;
		pos ++;

		string tmp=sql_vector[pos];
		if(tmp[0]=='\"'){char ttt[20];int kk=1;
			for(;tmp[kk]!='\"';kk++)
				ttt[kk-1]=tmp[kk];
			ttt[kk]='\0';
			vb=new Value<string>(ttt);
		}

		else if(tmp.find('.')==string::npos){
			vb=new Value<double>(atof(tmp.c_str()));
		}
		else {int ttt=atoi(tmp.c_str());
		vb=new Value<int>(ttt);}
		
	    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>(key, ar,vb,rank++)); // C
		
	}
		pos++;
		if (sql_vector[pos] == ";") break; /* one where condition, break. */
	}
}
