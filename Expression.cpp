#include "Expression.h"
#include <cfloat>
#include <sstream>

Expression::~Expression() {}

ValueBase * AttributeExpression::eval(const Record &r, const vector<Attribute> & attrs) {
    for (int i = 0; i < attrs.size(); i++) {
        if (name == attrs[i].name)
            return r[i]->copy();
    }
    return new Null<ValueBase>();
}

string AttributeExpression::toString() const {
    return name;
}

ValueBase * ConstExpression::eval() {
    return val->copy();
}

ValueBase * ConstExpression::eval(const Record &, const vector<Attribute> & attrs) {
    return this->eval();
}

ConstExpression::~ConstExpression() {
    delete val;
}

string ConstExpression::toString() const {
    std::stringstream ss;
    ss << *val;
    string ret;
    ss >> ret;
    return ret;
}

IdenticalFunction::IdenticalFunction(Expression * exp) : exp(exp) {}

IdenticalFunction::~IdenticalFunction() {
    delete exp;
}

ValueBase * IdenticalFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    return exp->eval(r, attrs);
}

string IdenticalFunction::toString() const {
    return "(" + exp->toString() + ")";
}

AbsFunction::AbsFunction(Expression * exp) : exp(exp) {}

AbsFunction::~AbsFunction() {
    delete exp;
}

ValueBase * AbsFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    auto val = exp->eval(r, attrs);
    ValueBase * ret;
    if (isNull(val)) ret = new Null<DoubleValue>();
    else if (dynamic_cast<IntValue*>(val)) ret = new IntValue(abs(*dynamic_cast<IntValue*>(val)));
    else if (dynamic_cast<DoubleValue*>(val)) ret = new DoubleValue(abs(*dynamic_cast<DoubleValue*>(val)));
    else ret = new Null<DoubleValue>();
    delete val;
    return ret;
}

string AbsFunction::toString() const {
    return "ABS(" + exp->toString() + ")";
}

CountFunction::CountFunction(Expression *exp) : exp(exp) 
{}

CountFunction ::~CountFunction() {
    delete exp;
}

ValueBase * CountFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    vector<Record *> _tmp; _tmp.push_back(const_cast<Record *>(&r));
    return evalAggregate(_tmp, attrs);
    
}
ValueBase * CountFunction::evalAggregate(vector<Record *>& rs, const vector<Attribute> & attrs) {
    int ret = 0;
    bool star = dynamic_cast<AttributeExpression*>(exp) && dynamic_cast<AttributeExpression*>(exp)->toString() == "*";
    for (auto it: rs) {
        auto val = exp->eval(*it, attrs);
        if (!isNull(val) || star) ret++;
        delete val;
    }
    return new IntValue(ret);
}

string CountFunction::toString() const {
    return "COUNT(" + exp->toString() + ")";
}

/*---------- MinFunction -------*/
MinFunction::MinFunction(Expression *exp) : exp(exp) 
{}

MinFunction ::~MinFunction() {
    delete exp;
}

ValueBase * MinFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    vector<Record *> _tmp; _tmp.push_back(const_cast<Record *>(&r));
    return evalAggregate(_tmp, attrs);
    
}
ValueBase * MinFunction::evalAggregate(vector<Record *>& rs, const vector<Attribute> & attrs) {
    double ret = DBL_MAX;
    for (auto it: rs) {
        auto val = exp->eval(*it, attrs);
        auto num = convertT<DoubleValue>(val);
        if (!isNull(val) && num->operator double() < ret) ret = *num;
        delete val, num;
    }
    if (ret == int(ret)) return new IntValue(ret);
    if (ret == DBL_MAX) return new Null<DoubleValue>();
    return new DoubleValue(ret);
}


string MinFunction::toString() const{
	return "MIN(" + exp->toString() + ")";
}

/*---------- MaxFunction -------*/
MaxFunction::MaxFunction(Expression *exp) : exp(exp) 
{}

MaxFunction ::~MaxFunction() {
    delete exp;
}

ValueBase * MaxFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    vector<Record *> _tmp; _tmp.push_back(const_cast<Record *>(&r));
    return evalAggregate(_tmp, attrs);
    
}
ValueBase * MaxFunction::evalAggregate(vector<Record *>& rs, const vector<Attribute> & attrs) {
    double ret = -DBL_MAX;
    for (auto it: rs) {
        auto val = exp->eval(*it, attrs);
        auto num = convertT<DoubleValue>(val);
        if (!isNull(val) && num->operator double() > ret) ret = *num;
        delete val, num;
    }
    if (ret == int(ret)) return new IntValue(ret);
    if (ret == DBL_MAX) return new Null<DoubleValue>();
    return new DoubleValue(ret);
}


string MaxFunction::toString() const{
	return "MAX(" + exp->toString() + ")";
}

/*---------- SumFunction -------*/
SumFunction::SumFunction(Expression *exp) : exp(exp) 
{}

SumFunction ::~SumFunction() {
    delete exp;
}

ValueBase * SumFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    vector<Record *> _tmp; _tmp.push_back(const_cast<Record *>(&r));
    return evalAggregate(_tmp, attrs);
    
}
ValueBase * SumFunction::evalAggregate(vector<Record *>& rs, const vector<Attribute> & attrs) {
    double ret = 0;
    for (auto it: rs) {
        auto val = exp->eval(*it, attrs);
        auto num = convertT<DoubleValue>(val);
        if (!isNull(val)) ret += *num;
        delete val, num;
    }
    if (ret == int(ret)) return new IntValue(ret);
    if (ret == DBL_MAX) return new Null<DoubleValue>();
    return new DoubleValue(ret);
}


string SumFunction::toString() const{
	return "SUM(" + exp->toString() + ")";
}
/*---------- AvgFunction -------*/
AvgFunction::AvgFunction(Expression *exp) : exp(exp) 
{}

AvgFunction ::~AvgFunction() {
    delete exp;
}

ValueBase * AvgFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    vector<Record *> _tmp; _tmp.push_back(const_cast<Record *>(&r));
    return evalAggregate(_tmp, attrs);
    
}
ValueBase * AvgFunction::evalAggregate(vector<Record *>& rs, const vector<Attribute> & attrs) {
    double ret = 0;int cnt = 0;
    for (auto it: rs) {
        auto val = exp->eval(*it, attrs);
        auto num = convertT<DoubleValue>(val);
        if (!isNull(val)) ret += *num, cnt++;
        delete val, num;
    }
    return cnt ? new DoubleValue(ret / cnt) : new Null<DoubleValue>();
}


string AvgFunction::toString() const{
	return "AVG(" + exp->toString() + ")";
}

/*-------------------------------*/

OperatorExpression::OperatorExpression(Expression * a, Expression * b, string sym) : left(a),right(b), symbol(sym)
{}

OperatorExpression::~OperatorExpression() {
    delete left, right;
}

string OperatorExpression::toString() const {
    return left->toString() + symbol + right->toString();
}

EqualOperator::EqualOperator(Expression * a, Expression * b) : OperatorExpression(a, b, "=") {}
LessOperator::LessOperator(Expression * a, Expression * b) : OperatorExpression(a, b, "<") {}
GreaterOperator::GreaterOperator(Expression * a, Expression * b) : OperatorExpression(a, b, ">") {}

ValueBase * EqualOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    if (isNull(a) || isNull(b)) return new Null<BoolValue>();
    ValueBase * ret;
    // 单独处理不同数值类型相互比较的情形
    if (dynamic_cast<IntValue *>(a) && dynamic_cast<DoubleValue *>(b)) 
        ret = new BoolValue(dynamic_cast<IntValue *>(a)->operator int() == dynamic_cast<DoubleValue *>(b)->operator double());
    else if (dynamic_cast<DoubleValue *>(a) && dynamic_cast<IntValue *>(b))
        ret = new BoolValue(dynamic_cast<IntValue *>(b)->operator int() == dynamic_cast<DoubleValue *>(a)->operator double());
    else ret = new BoolValue(*a == *b);
    delete a, b;
    return ret;
}
ValueBase * LessOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    if (isNull(a) || isNull(b)) return new Null<BoolValue>();
    ValueBase * ret;
    if (dynamic_cast<IntValue *>(a) && dynamic_cast<DoubleValue *>(b)) 
        ret = new BoolValue(dynamic_cast<IntValue *>(a)->operator int() < dynamic_cast<DoubleValue *>(b)->operator double());
    else if (dynamic_cast<DoubleValue *>(a) && dynamic_cast<IntValue *>(b)) 
        ret = new BoolValue(dynamic_cast<IntValue *>(b)->operator int() > dynamic_cast<DoubleValue *>(a)->operator double());
    else ret = new BoolValue(*a < *b);
    delete a, b;
    return ret;
}
ValueBase * GreaterOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    if (isNull(a) || isNull(b)) return new Null<BoolValue>();
    ValueBase * ret;
    if (dynamic_cast<IntValue *>(a) && dynamic_cast<DoubleValue *>(b)) 
        ret = new BoolValue(dynamic_cast<IntValue *>(a)->operator int() > dynamic_cast<DoubleValue *>(b)->operator double());
    else if (dynamic_cast<DoubleValue *>(a) && dynamic_cast<IntValue *>(b)) 
        ret = new BoolValue(dynamic_cast<IntValue *>(b)->operator int() < dynamic_cast<DoubleValue *>(a)->operator double());
    else ret = new BoolValue(*a > *b);
    delete a, b;
    return ret;
}


PlusOperator::PlusOperator(Expression * a, Expression * b) : OperatorExpression(a, b, "+") {}
ValueBase * PlusOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    auto inta = dynamic_cast<IntValue*>(a), intb = dynamic_cast<IntValue*>(b);
    auto dba = convertT<DoubleValue>(a), dbb = convertT<DoubleValue>(b);
    if (inta && intb) ret = new IntValue(*inta + *intb);
    else if (dba && dbb) ret = new DoubleValue(*dba + *dbb);
    else ret = new Null<DoubleValue>();
    delete a, b, dba, dbb;
    return ret;
}
MinusOperator::MinusOperator(Expression * a, Expression * b) : OperatorExpression(a, b, "-") {}
ValueBase * MinusOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    auto inta = dynamic_cast<IntValue*>(a), intb = dynamic_cast<IntValue*>(b);
    auto dba = convertT<DoubleValue>(a), dbb = convertT<DoubleValue>(b);
    if (inta && intb) ret = new IntValue(*inta - *intb);
    else if (dba && dbb) ret = new DoubleValue(*dba - *dbb);
    else ret = new Null<DoubleValue>();
    delete a, b, dba, dbb;
    return ret;
}
MultiplyOperator::MultiplyOperator(Expression * a, Expression * b) : OperatorExpression(a, b, "*") {}
ValueBase * MultiplyOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    auto inta = dynamic_cast<IntValue*>(a), intb = dynamic_cast<IntValue*>(b);
    auto dba = convertT<DoubleValue>(a), dbb = convertT<DoubleValue>(b);
    if (inta && intb) ret = new IntValue(*inta * *intb);
    else if (dba && dbb) ret = new DoubleValue(*dba * *dbb);
    else ret = new Null<DoubleValue>();
    delete a, b, dba, dbb;
    return ret;
}
DivideOperator::DivideOperator(Expression * a, Expression * b) : OperatorExpression(a, b, "/") {}
ValueBase * DivideOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    // auto dba = convertT<DoubleValue>(a), dbb = convertT<DoubleValue>(b);
    auto inta = dynamic_cast<IntValue*>(a), intb = dynamic_cast<IntValue*>(b);
    auto dba = dynamic_cast<DoubleValue*>(a), dbb = dynamic_cast<DoubleValue*>(b);
    if (inta && intb && intb->operator int()) ret = new IntValue(*inta / *intb);
    else if (dba && dbb && dbb->operator double()) ret = new DoubleValue(*dba / *dbb);
    else ret = new Null<DoubleValue>();
    delete a, b;
    return ret;
}
ModOperator::ModOperator(Expression * a, Expression * b) : OperatorExpression(a, b, "%") {}
ValueBase * ModOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    auto inta = convertT<IntValue>(a), intb = convertT<IntValue>(b);
    if (inta && intb && intb ->operator int()) ret = new IntValue(*inta % *intb);
    else ret = new Null<IntValue>();
    delete a, b, inta, intb;
    return ret;
}


LogicAndOperator::LogicAndOperator(Expression * a, Expression * b) : OperatorExpression(a, b, " and ") {}
ValueBase * LogicAndOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    if (isNull(a) || isNull(b)) ret = BoolValue::newNull();
    auto ba = dynamic_cast<BoolValue *>(a), bb = dynamic_cast<BoolValue *>(b);
    if (ba && bb) ret = new BoolValue(*ba && *bb);
    else ret = new Null<BoolValue>();
    delete a, b;
    return ret;
}
LogicOrOperator::LogicOrOperator(Expression * a, Expression * b) : OperatorExpression(a, b, " or ") {}
ValueBase * LogicOrOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    if (isNull(a) || isNull(b)) ret = BoolValue::newNull();
    auto ba = dynamic_cast<BoolValue *>(a), bb = dynamic_cast<BoolValue *>(b);
    if (ba && bb) ret = new BoolValue(*ba || *bb);
    else ret = new Null<BoolValue>();
    delete a, b;
    return ret;
}
LogicXorOperator::LogicXorOperator(Expression * a, Expression * b) : OperatorExpression(a, b, " xor ") {}
ValueBase * LogicXorOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = left->eval(r, attrs), b = right->eval(r, attrs);
    ValueBase * ret;
    if (isNull(a) || isNull(b)) ret = BoolValue::newNull();
    auto ba = dynamic_cast<BoolValue *>(a), bb = dynamic_cast<BoolValue *>(b);
    if (ba && bb) ret = new BoolValue(*ba ^ *bb);
    else ret = new Null<BoolValue>();
    delete a, b;
    return ret;
}

LogicNotOperator::LogicNotOperator(Expression * a) : exp(a) {}
ValueBase * LogicNotOperator::eval(const Record &r, const vector<Attribute> & attrs) {
    auto a = exp->eval(r, attrs);
    ValueBase * ret;
    if (isNull(a)) ret = BoolValue::newNull();
    auto ba = dynamic_cast<BoolValue *>(a);
    if (ba) ret = new BoolValue(!*ba);
    else ret = new Null<BoolValue>();
    delete a;
    return ret;
}
string LogicNotOperator::toString() const {
    return "not " + exp->toString();
}
