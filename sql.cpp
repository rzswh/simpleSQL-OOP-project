#include"sql.h"	
#include<stack>
#include <regex>

string to_lower(string str){
	transform(str.begin(),str.end(),str.begin(),::tolower);
	return str;
}
string to_upper(string str){
	transform(str.begin(),str.end(),str.begin(),::toupper);
	return str;
}

unsigned int buildWhereClauseFrom(vector<string> sql_vector, 
		vector<pair<LogicOperation, int> > &o,
		vector<WhereClause::SubSentence> &s,
		unsigned int pos = 0) {
	std::stack<LogicOperation> ss;
	int rank=0;
	pos++;
	bool exp = true; // 下一个是表达式。用于判断子句何时结束
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
			exp = true;
		} else {
			if (!exp) break;
			string v1 = sql_vector[pos];
            pos++;
			ArithmicOperation ar;
			if (sql_vector[pos] == "<") ar = ARITH_LESS;
			else if (sql_vector[pos] == "=") ar = ARITH_EQUAL;
			else if (sql_vector[pos] == ">") ar = ARITH_GREATER;
			pos ++;
			string v2 = sql_vector[pos];
			pos++;
			// special case: form like 1 < id
			if (v1[0] >= '0' && v1[0] <= '9' || v1[0] == '.' || v1[0] == '\'' || v1[0] == '\"') {
				swap(v1, v2);
				if (ar == ARITH_LESS) ar = ARITH_GREATER;
				else if (ar == ARITH_GREATER) ar = ARITH_LESS;
			}
			ValueBase * vb = stringToValue(v2);
			s.push_back( make_tuple(v1, ar,vb,rank++)); // C
			exp = false;
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


Expression * parseExpression(string sentence) {
	#ifdef DEBUG
	std::cerr << '\t' << sentence << endl;
	#endif
	auto isFunc = sentence.find_first_of('(');
	if(sentence=="*") {
		return new AttributeExpression("*");
	} else if (isFunc != string::npos) { // 函数
		string func = to_lower(sentence.substr(0, isFunc));
		Expression * inner = parseExpression(sentence.substr(isFunc+1, sentence.find_last_of(')') - isFunc - 1));
		if (func == "count") 
			return new CountFunction(inner);
		else if (func == "min")
			return new MinFunction(inner);
		else if (func == "max")
			return new MaxFunction(inner);
		else if (func == "sum")
			return new SumFunction(inner);
		else if (func == "avg")
			return new AvgFunction(inner);
		
	}
	else{
		return new AttributeExpression(sentence);
	}
}

int getOperatorRank(string op) {
	op = to_lower(op);
	if (op == "or") return 1;
	else if (op == "and") return 2;
	else if (op == "not") return 3;
	else if (op == "=" || op == "<" || op == ">") return 4;
	else if (op == "+" || op == "-") return 5;
	else if (op == "*" || op == "/" || op == "%" || op == "mod") return 6;
	else if (op == "(") return 7;
	else if (op == ")") return 1;
	else return 0;
}
// if rank left < rank right
bool compareOperator(string op1, string op2) {
	if (op1 == "(") return op2 != ")";
	else return getOperatorRank(op1) < getOperatorRank(op2);
}
bool isFunction(string func) {
	func = to_lower(func);
	return func == "count"||func == "min"||func == "max"||func == "sum"||func == "avg";
}

Expression * readExpressionFromString(const vector<string>& sql_vector, unsigned int & pos) {
	//string exp = sql_vector[pos++];
	vector<string> oprs;
	vector<Expression *> results;
	int opr_rank;
	bool lastOperand = false;// 当连续出现两个操作数的时候，就该认为到达结尾了
	do {
		const string & exp = sql_vector[pos++];
		opr_rank = getOperatorRank(exp); // 0 if not operator
		if (isFunction(exp)) {
			oprs.push_back(exp);
			// sql_vector[pos]应当为'('，把左括号也压入栈中
			oprs.push_back(sql_vector[pos++]);
			lastOperand = false; opr_rank = 1;
		} else if ((opr_rank == 0 || exp == "*") && !lastOperand) { // * 具有二义性
			// 字符串常量、数字常量
			if (exp[0] == '\'' || exp[0] == '"' || regex_match(exp, regex("[0-9.+-]+")))
				results.push_back(new ConstExpression(stringToValue(exp)));
			else results.push_back(new AttributeExpression(exp));  // attr
			lastOperand = true;
		}
		else {
			// '(' < '+'; ')' < '+'; '+' < '('; '(' = ')'
			while (!oprs.empty() && !compareOperator(oprs.back(), exp)) {
				string opr = to_lower(oprs.back()); oprs.pop_back();
				if (opr == "(") {
					string func_name = !oprs.empty() ? to_lower(oprs.back()) : ""; 
					if (!oprs.empty()) oprs.pop_back();
					// 函数的左括号
					if (func_name == "count") {
						results.back() = new CountFunction(results.back());
					}
					else if (func_name == "min") {
						results.back() = new MinFunction(results.back());
					}
					else if (func_name == "max") {
						results.back() = new MaxFunction(results.back());
					}
					else if (func_name == "sum") {
						results.back() = new SumFunction(results.back());
					}
					else if (func_name == "avg") {
						results.back() = new AvgFunction(results.back());
					}
					else if (func_name != "") {
						oprs.push_back(func_name);
						results.back() = new IdenticalFunction(results.back());
					} // 只是个小括号，把不该弹出来的压回去
					break; // 不能让右括号继续比下去
				} 
				// 一元操作符
				else if (opr == "not") {
					auto x = results.back(); results.pop_back();
					results.push_back(new LogicNotOperator(x));
				}
				// 二元操作符
				else {//if (opr != "not") {
					auto x = results.back(); results.pop_back();
					auto y = results.back(); results.pop_back();
					if (opr == "+") results.push_back(new PlusOperator(y, x));
					else if (opr == "-") results.push_back(new MinusOperator(y, x));
					else if (opr == "*") results.push_back(new MultiplyOperator(y, x));
					else if (opr == "/") results.push_back(new DivideOperator(y, x));
					else if (opr == "%" || opr == "mod") results.push_back(new ModOperator(y, x));
					else if (opr == "=") results.push_back(new EqualOperator(y, x));
					else if (opr == "<") results.push_back(new LessOperator(y, x));
					else if (opr == ">") results.push_back(new GreaterOperator(y, x));
					else if (opr == "and") results.push_back(new LogicAndOperator(y, x));
					else if (opr == "or") results.push_back(new LogicOrOperator(y, x));
					else if (opr == "xor") results.push_back(new LogicXorOperator(y, x));
					else {
						cerr << "Wrong operator \"" << opr << "\"" << endl;
					}
				}
			}
			if (exp != ")") oprs.push_back(exp);
			lastOperand = exp == ")"; // 只有这个后面不能接操作数
		}
	} while (opr_rank || lastOperand); // 读到的不是操作数，又不在运算符中，那么读到了奇怪的命令
	pos --;
	return results.front();
}

// 读一系列用“, ”连接的表达式
vector<Expression *> readExpressionsFromString(const vector<string> &sql_vector, unsigned int & pos) {
	vector<Expression *> exps;
	while (true) {
		exps.push_back(readExpressionFromString(sql_vector, pos));
		if(to_lower(sql_vector[pos])!=",") break;
		pos++; // skip ','
	}
	return exps;
}


ValueBase * stringToValue(string tmp) {
	ValueBase * vb;
	if(tmp[0]=='\''){
		if (tmp.find(':')!= string::npos)
			vb = new TimeValue(tmp.substr(1, tmp.find_last_of('\'') - 1));
		else if (tmp.find('-') != string::npos)
			vb = new DateValue(tmp.substr(1, tmp.find_last_of('\'') - 1)); 
		else
			vb = new Value<string>(tmp.substr(1, tmp.find_last_of('\'')-1));
	} 
	else if(tmp[0]=='\"'){
		vb = new Value<string>(tmp.substr(1, tmp.find_last_of('\"')-1));
	} 
	else if(tmp.find('.')!=string::npos){
		vb = new DoubleValue(stolld(tmp));
	}
	else if (to_upper(tmp) == "NULL") {
		return new Null<ValueBase>();
	}

	else {
		bool isnum = true;
		for (int i = 0; i < tmp.size(); i++)
			if (!isdigit(tmp[i]) && tmp[i] != '-')
				isnum = false;
		if (isnum)
		{
			int ttt = atoi(tmp.c_str());
			vb = new IntValue(ttt);
		}
		else {
			vb = new Value<string>(tmp);
		}

	}
	return vb;
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
		if (sql_vector[9] == "-"&&sql_vector[11] == "-")
			vals.push_back(stringToValue(sql_vector[8] + sql_vector[9] + sql_vector[10] + sql_vector[11] + sql_vector[12]));
		else {
			pos += 2;
			vals.push_back(stringToValue(sql_vector[pos]));
		}
	}
}

SQLInsert::~SQLInsert () {
	for (auto i: vals) delete i;
	vals.clear();
}

/* ---------------- SQLSelect ----------------- */
SQLSelect::SQLSelect(vector<string> sql_vector) { 
	order_by = nullptr;
	where_clause = nullptr;
	Parse(sql_vector); 
}

string SQLSelect::get_tb_name() { return tb_name; }
bool SQLSelect::if_load_file() { return load_file; }
string SQLSelect::get_load_file_name() { return load_file_name; }
vector<Expression *> SQLSelect::get_expressions() { return expressions; }
vector<Expression *> SQLSelect::get_group_by() { return group_by; }
Expression * SQLSelect::get_order_by() { return order_by; }

void SQLSelect::Parse(vector<string> sql_vector) /* only support "select * ". */
{
	sql_type = 7;
	load_file = false;
	unsigned int pos = 1; // select -> [expressions]
	expressions = readExpressionsFromString(sql_vector, pos);
	
	if (to_lower(sql_vector[1]) == "addtime")
	{
		TimeValue timeval = TimeValue(sql_vector[3].substr(1, sql_vector[3].length() - 2));
		timeval.addTime(sql_vector[5].substr(1, sql_vector[5].length() - 2));
		cout << timeval<<endl;
		return;
	}
	if (to_lower(sql_vector[1]) == "adddate")
	{
		DateValue timeval = DateValue(sql_vector[3].substr(1, 4)+sql_vector[4]+sql_vector[5]+sql_vector[6]+sql_vector[7].substr(1,2));
		timeval.addDate(sql_vector[9].substr(1, sql_vector[9].length() - 2));
		cout << timeval << endl;
		return;
	}
	// ; / from / where / group / order / into / 
	while (pos + 1 < sql_vector.size() && sql_vector[pos] != ";") { /* sql statement like: "select * from tb;". */
		string cmd = to_lower(sql_vector[pos]);
		if (cmd == "where") {
			pos++; // where -> [expression]
			where_clause = readExpressionFromString(sql_vector, pos);
		}
		else if (cmd == "from") { // 题目要求一定要有from
			pos++; // from -> [table_name]
			tb_name = sql_vector[pos];
			pos++; 
		}
		else if (cmd == "group") { // group by
			pos+=2; // group -> by -> [expressions]
			group_by = readExpressionsFromString(sql_vector, pos); // [expressions] -> [next keyword]
		}
		else if (cmd == "order") { // order by
			pos+=2; // order -> by -> [expression]
			order_by = readExpressionFromString(sql_vector, pos); // [expressions] -> [next keyword]
		}
		else if (cmd == "into") {
			load_file = true;
			pos ++; // into -> outfile
			pos ++; // outfile -> 'file_name'
			load_file_name = sql_vector[pos].substr(1, sql_vector[pos].length() - 2); // get rid of quotes
			pos ++; // into -> [next]
		} else {
			cerr << "Unexpected subsequence: " << sql_vector[pos] << endl;
			pos++;
		}
	}
}

SQLSelect::~SQLSelect() {
	// for (auto i: s) 
	// 	delete std::get<2>(i);
	for (auto i: expressions)
		delete i;
	for (auto i: group_by)
		delete i;
	delete order_by, where_clause;
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

/* ----------------- SQLLoad ------------------ */
SQLLoad::SQLLoad(vector<string> sql_vector) { Parse(sql_vector); }

string SQLLoad::get_tb_name() { return tb_name; }

void SQLLoad::set_tb_name(string tbname){ tb_name = tb_name;}

string SQLLoad::get_file_name(){ return file_name;}

void SQLLoad::set_file_name(string filename){ file_name = filename;}

void SQLLoad::Parse(vector<string> sql_vector)
{
	sql_type = 10;
	unsigned int pos = 6;
	bool is_attr = true;
	tb_name = sql_vector[pos];
	file_name = sql_vector[3].substr(1, sql_vector[3].length() - 2);
	int num = 0;
	pos += 2;
	while (is_attr)
	{
		is_attr = false;
		attrNames.push_back(sql_vector[pos]);
		num++;
		pos++;
		if (sql_vector[pos] != ")") is_attr = true;
		pos++;
	}
	ifstream infile;
	infile.open(file_name,ios::in);
	if (!infile.is_open())return;
	string strline;

	while (getline(infile, strline))
	{
		vector<ValueBase *> val;
		vector<string> tmpvals;
		strSplit(strline, tmpvals, "\t");
		for (int i = 0; i < num; i++)
		{
			val.push_back(stringToValue(tmpvals[i]));
		}
		vals.push_back(val);
	}
}

void SQLLoad::strSplit(const string & s, vector<string>& v, const string & c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}


SQLLoad::~SQLLoad() {
	for (auto i : vals) 
		for (auto j: i)
			delete j;
	vals.clear();
}
