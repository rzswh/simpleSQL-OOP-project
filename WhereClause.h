#pragma once
#include <tuple>
#include <vector>
#include "Value.h"
#include "Record.h"
#include "Attribute.h"
using std::tuple;
using std::vector;
using std::string;
using std::pair;


/**
 * @encodings=utf-8
 * 将where子句的查询条件以逆波兰表达式的形式进行存储。
 * */
class WhereClause {
public:
    enum ArithmicOperation { ARITH_GREATER, ARITH_LESS, ARITH_EQUAL };
    enum LogicOperation {LOGIC_AND, LOGIC_OR};
    /**
     * SubSequence: where子句的基本组成单位，由NAME ARITH_OP VALUE形式组成; 最后一个元素是在表达式中的位置
     * */
    typedef tuple<string, ArithmicOperation, ValueBase *, int> SubSentence;
private:
    vector< SubSentence > subsentences;
    vector<pair<LogicOperation, int> > operation;
public: 
    /**
     * 初始化格式说明：s是一个由四元组tuple组成的vector, 每个tuple形如：
     * (string attribute_name, AritmicOperation op, ValueBase * v, int rank)；
     * o是一个由pair组成的vector, pair形如：(LogicOperation op, int rank)
     * rank是将查询表达式改写为逆波兰表达式后各元素所在位置
     * 如果想构造一个表示student_name = "张三" and student_id < 2017012323 or student_gpa > 3.97的子句
     * 应当如下构造：
     * vector<SubSentence> s; vector<pair<LogicOperation, int> > o;
     * s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_name", ARITH_EQUAL, new Value<string>("张三"), 0) ); // A
     * s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_LESS, new Value<int>(2017012323), 1) ); // B
     * s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_gpa", ARITH_GREATER, new Value<double>(3.97), 3) ); // C
     * o.push_back( make_pair<LogicOperation, int>(LOGIC_AND, 2) ); // X
     * o.push_back( make_pair<LogicOperation, int>(LOGIC_OR, 4) ); // Y
     * 这个查询语句形如 A X B Y C, 经过后缀表达式转化后变为 A B X C Y, 每个字母在序列中的位置即它对应的rank。
     * 调用时请保证同一vector内对象的rank递增。WhereClause本身不检查传入的参数是否能构成合法的表达式。
     */ 
    WhereClause(const vector<SubSentence> & s, const vector<pair<LogicOperation, int> > & o):subsentences(s), operation(o){};
    ~WhereClause() = default;
    /**
     * 中缀表达式转逆波兰表达式。
     * */
    static WhereClause buildFrom(string str);
    /**
     * 测试给定的Record是否满足whereClause的选择条件。
     * 由于Record本身只包含数据，不包含每个数据对应的属性类型等信息，请把attributes传入。
     */
    bool test(const Record &, const vector<Attribute> &) const;
};