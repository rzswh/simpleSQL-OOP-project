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
    virtual ValueBase * eval(const Record &, const vector<Attribute> & attrs) = 0;
    virtual ~Expression() = 0;
};

/**
 * 属性表达式
 * */
class AttributeExpression : public Expression {
    std::string name;
public:
    AttributeExpression(const string& name): name(name) {}
    string toString() const;
    ValueBase * eval(const Record &, const vector<Attribute> & attrs) override;
};

/**
 * 常量表达式
 * */
class ConstExpression : public Expression {
    ValueBase * val;
public:
    ConstExpression(ValueBase * v): val(v->copy()) {}
    ValueBase * eval();
    ValueBase * eval(const Record &, const vector<Attribute> & attrs) override;
    string toString() const;
    ~ConstExpression();
};

/**
 * 函数基类。
 * */
class FunctionExpression : public Expression {
public:
};

// 恒等函数
class IdenticalFunction : public FunctionExpression {
    Expression * exp;
public:
    IdenticalFunction(Expression *exp);
    ~IdenticalFunction();
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
    string toString() const;
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
    CountFunction(Expression *exp);
    ~CountFunction();
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
    ValueBase * evalAggregate(vector<Record *>&, const vector<Attribute> & attrs);
    string toString() const;
};

/**
 * 运算符基类。这里每一个运算符都是二元运算。
 * */
class OperatorExpression : public Expression {
protected:
    Expression * left;
    Expression * right;
    string symbol; // 基类实现toString, 节省代码
public:
    OperatorExpression(Expression * a, Expression * b, string sym);
    ~OperatorExpression();
    string toString() const;
};

// 比较运算符
class EqualOperator : public OperatorExpression {
public:
    EqualOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class LessOperator : public OperatorExpression {
public:
    LessOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class GreaterOperator : public OperatorExpression {
public:
    GreaterOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};

// 算术运算符
class PlusOperator : public OperatorExpression {
public:
    PlusOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class MinusOperator : public OperatorExpression {
public:
    MinusOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class MultiplyOperator : public OperatorExpression {
public:
    MultiplyOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class DivideOperator : public OperatorExpression {
public:
    DivideOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class ModOperator : public OperatorExpression {
public:
    ModOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};

// 逻辑运算符
class LogicAndOperator : public OperatorExpression {
public:
    LogicAndOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class LogicOrOperator : public OperatorExpression {
public:
    LogicOrOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
class LogicXorOperator : public OperatorExpression {
public:
    LogicXorOperator(Expression * a, Expression * b);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
};
// 一元操作符不妨看作函数
class LogicNotOperator : public FunctionExpression {
    Expression * exp;
public:
    LogicNotOperator(Expression * a);
    ValueBase * eval(const Record &, const vector<Attribute> & attrs);
    string toString() const;
};