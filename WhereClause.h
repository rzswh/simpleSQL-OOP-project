#pragma once
#include <tuple>
#include "Value.h"
using std::tuple;
using std::vector;
using std::string;


/**
 * @encodings=utf-8
 * 将where子句的查询条件以逆波兰表达式的形式进行存储。
 * */
class WhereClause {
public:
    enum ArithmicOperation { ARITH_GREATER, ARITH_LESS, ARITH_EQUAL };
    enum LogicOperation {LOGIC_AND, LOGIC_OR};
    /**
     * SubSequence: where子句的基本组成单位，由NAME ARITH_OP VALUE形式组成
     * */
    typedef tuple<string, ArithmicOperation, ValueBase *> SubSequence;
private:
    vector< SubSequence > subsequences;
    vector<LogicOperation> operation;
public: 
    WhereClause(const vector<SubSequence> & s, const vector<LogicOperation> & o):subsequences(s), operation(o){};
    ~WhereClause() = default;
    /**
     * 中缀表达式转逆波兰表达式。
     * */
    static WhereClause buildFrom(string str);
};