#include"sql.h"	
#include<stack>

//#define DEBUG

string to_lower(string str){
	transform(str.begin(),str.end(),str.begin(),::tolower);
	return str;
}
string to_upper(string str){
	transform(str.begin(),str.end(),str.begin(),::toupper);
	return str;
}

int buildWhereClauseFrom(vector<string> sql_vector, 
		vector<pair<LogicOperation, int> > &o,
		vector<WhereClause::SubSentence> &s,
		int pos = 0) {
	std::stack<LogicOperation> ss;
	int rank=0;
	pos++;
	while (true)
	{
		auto logicPriority = [](LogicOperation o)->int {
			if (o == LOGIC_AND) return 1;
			else if (o == LOGIC_OR) return 0;
			return -233; // undefined
		};
		#ifdef DEBUG
		cout <<  "[debug] " << sql_vector[pos] << endl;
		#endif
		string lo_str = to_lower(sql_vector[pos]);
		if(lo_str=="and"||lo_str=="or"){
			LogicOperation lo;
			if (lo_str == "and")
				lo = LOGIC_AND;
			else if (lo_str == "or") 
				lo = LOGIC_OR;
			else {
				// wrong input
				break;
			}
			while(ss.size()){
				LogicOperation tt=ss.top();
				if (logicPriority(tt) < logicPriority(lo))
					break;
				ss.pop();
				o.push_back(std::make_pair(tt,rank++));
            }
			ss.push(lo);
			pos++;
		}
		else{
			string key = sql_vector[pos];
            pos++;
			ArithmicOperation ar;
			if (sql_vector[pos] == "<") ar = ARITH_LESS;
			else if (sql_vector[pos] == "=") ar = ARITH_EQUAL;
			else if (sql_vector[pos] == ">") ar = ARITH_GREATER;
			pos ++;
			ValueBase * vb = stringToValue(sql_vector[pos]);
			s.push_back( make_tuple(key, ar,vb,rank++)); // C
			pos++;
	    }
		if (sql_vector[pos] == ";") break; /* one where condition, break. */
	}
	while (!ss.empty() ) {
		o.push_back(std::make_pair(ss.top(),rank++));
		ss.pop();
	}
	#ifdef DEBUG
	cout << "[debug] where clause build info: " << endl;
	cout << s.size() << " "; for (auto i: s) cout << std::get<3>(i) << " "; cout << endl;
	cout << o.size() << " "; for (auto i: o) cout << i.second << " "; cout << endl;
	#endif
	return pos;
}

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
	// pos ++;

	bool is_attr = true;
	bool has_primary_key = false;

	while (sql_vector[pos] != ")")
	{
		// is_attr = false;
		pos++;
		
		if (to_lower(sql_vector[pos]) != "primary" || to_lower(sql_vector[pos+1]) != "key")
		{
			bool notnull=false;
			if(pos + 4 <= sql_vector.size() && to_lower(sql_vector[pos+2])=="not" && to_lower(sql_vector[pos+3])=="null")
				notnull=true;
			Attribute attr(sql_vector[pos],to_upper(sql_vector[pos+1]),notnull);
			attrs.push_back(attr);

			if(notnull) pos+=4; // [name] [type] not null -> ,/)
			else pos+=2; // [name] [type] -> ,/)
			// if (sql_vector[pos] != ")") is_attr = true;
		}
		else  /* primary key */
		{
			if (has_primary_key) return;
			pos +=3; // primary key ( -> [name]
			primary=sql_vector[pos];
			/*for (auto att = attrs.begin(); att != attrs.end() ; att++)
			{
				if ((*att).name == sql_vector[pos])
				{
					(*att).type=1;//设定为主键
				}
			}*/
			pos +=2; // [name] ) -> ,/)
			has_primary_key = true;
		}
	}
}

/* -------------- SQLShowColumns -------------------- */
SQLShowColumns::SQLShowColumns(vector<string> sql_vector) { Parse(sql_vector); }

string SQLShowColumns::get_tb_name() { return tb_name; }

void SQLShowColumns::Parse(vector<string> sql_vector)
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
	pos +=2;
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

	for(int i=0;i<num;i++)
	{
		pos+=2;
		vals.push_back(stringToValue(sql_vector[pos]));
	}
}

SQLInsert::~SQLInsert () {
	for (auto i: vals) delete i;
	vals.clear();
}

/* ---------------- SQLSelect ----------------- */
SQLSelect::SQLSelect(vector<string> sql_vector) { Parse(sql_vector); }

string SQLSelect::get_tb_name() { return tb_name; }

void SQLSelect::Parse(vector<string> sql_vector) /* only support "select * ". */
{
	sql_type = 7;
	unsigned int pos = 1;
	while(true){
		if(sql_vector[pos]=="*") {
			attrFilter.push_back("*");
			pos++;
			break;
		}
		else{
			attrFilter.push_back(sql_vector[pos++]);
			if(sql_vector[pos]=="from") break;
			pos++;
		}
	}
	pos++; // from -> [table_name]
	tb_name = sql_vector[pos];
	pos++; // [table_name] -> ; / where
	if (sql_vector.size() == pos + 1) return; /* sql statement like: "select * from tb;". */
	pos = buildWhereClauseFrom(sql_vector, o, s, pos);
}

SQLSelect::~SQLSelect() {
	for (auto i: s) 
		delete std::get<2>(i);
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
	
	if (sql_vector.size() == pos + 1) return; /* sql statement like: "delete from tb;". */
	pos = buildWhereClauseFrom(sql_vector, o, s, pos);
}

SQLDelete::~SQLDelete() {
	for (auto i: s) 
		delete std::get<2>(i);
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
		vals.push_back(stringToValue(sql_vector[pos]));
		//vals.back()->print(cout << "[debug] ") << " " << sql_vector[pos] << endl;
		pos ++;

		if (sql_vector[pos] == ",") pos ++;
		else if (to_lower(sql_vector[pos]) == "where") break;
	}
	pos = buildWhereClauseFrom(sql_vector, o, s, pos);
}

SQLUpdate::~SQLUpdate() {
	for (auto i: s) 
		delete std::get<2>(i);
}