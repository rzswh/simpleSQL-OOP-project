#pragma once
#include "Value.h"
#include "Record.h"
#include "Attribute.h"
#include <vector>
using std::vector;
using std::string;

/**
 * 表达式基类。用于select语句。
 * 当表达式作用于某一个具体的Record时才能求值。
 * */
class Expression {
public:
    virtual string toString() const = 0;
    virtual ~Expression() = 0;
};

class AttributeExpression : public Expression {
    std::string name;
public:
    AttributeExpression(const string& name): name(name) {}
    string toString() const;
};

class ConstExpression : public Expression {
    ValueBase * val;
public:
    ConstExpression(ValueBase * v): val(v->copy()) {}
    ValueBase * eval();
    string toString() const;
    ~ConstExpression();
};

/**
 * 函数基类。每个函数都要保证能作用在单独的一个元素上。
 * */
class FunctionExpression : public Expression {
public:
    virtual ValueBase * eval(const Record &, const vector<Attribute> & attrs) = 0;
};

/**
 * AVG，SUM，MAX，MIN，COUNT等需要一“组”而不是一“条”记录，因此单独抽象出来
 * */
class AggregationFunctionExpression : public FunctionExpression {
public:
    virtual ValueBase * evalAggregate(vector<Record *>&, const vector<Attribute> & attrs) = 0;
};

class CountFunction : public AggregationFunctionExpression {
    Expression * exp;
public:
    CountFunction(Expression *exp) : exp(exp) {}
    ~CountFunction();
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
    ValueBase * evalAggregate(vector<Record *>&, const vector<Attribute> & attrs);
};