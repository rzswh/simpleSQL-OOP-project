// encodings=UTF-8
#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
using std::ostream;
using std::string;
//using std::unique_ptr;
//using std::make_unique;

/**
 * NULL是一种极为特殊的值。注意：它是值，不是类型。虽然这里我把它包装成为了一个类。
 * NULL可以建立于任意一种数据类型之上。因此需要注意，NULL的类型具有歧义性。
 * 注意：这里的Null本质是一种工厂方法。并非所有的NULL都是Null<T>类型的实例；请通过isNull判断一个值是否为NULL。
 * 处理NULL会遇到很多问题。首先，它是一种”通用“类型，可以与任意值作运算。
 * 用什么类来表示NULL呢？只用ValueBase，会导致接口变得非常复杂；
 * 用任意类型，会导致出现分属不同类型的NULL，NULL的类型出现歧义。
 * */
template <class T>
class Null : public T {
public:
    Null() : T() {
        this->setNull();
    }
};

class BoolValue;
/**
 * 数据库内所储存数据的基类。
 */
class ValueBase {
protected:
    bool isNull;
    ValueBase(bool n = false):isNull(n) {}
    virtual void setNull() final{ isNull = true; }
public:
    virtual BoolValue operator==(const ValueBase & v) const;
    virtual BoolValue operator>(const ValueBase & v) const;
    virtual BoolValue operator<(const ValueBase & v) const;
    virtual ValueBase * copy() const { return new ValueBase(true); }
    //virtual bool lessThan(const ValueBase *) const { return true; }
    virtual ~ValueBase();
    virtual ostream & print(ostream & out) const;

    // 此函数作用上和return Null<ValueBase>()完全等价，建议使用Null<ValueBase>
    static ValueBase makeNull() { return ValueBase(true); }
    // 此函数作用上和return new Null<ValueBase>()完全等价，建议使用new Null<ValueBase>
    static ValueBase* newNull() { return new ValueBase(true); }

    friend ostream & operator<<(ostream & out, ValueBase & v) {
        return v.print(out); 
    }
    friend bool isNull(ValueBase *);
};

bool isNull(ValueBase *);
bool isNull(const ValueBase & v);

// 扩展的布尔类型。今后在程序的逻辑中，会出现NULL < TRUE这样的逻辑运算，二值变为三值
// 扩展后原有的接口保持不变（多亏了自动类型转换），可以在不考虑NULL的情况下继续正常工作
// 如果考虑NULL，需要用isNull来对这一种情况特殊判断，排除掉NULL以后再通过类型转换得到正确的布尔值TRUE/FALSE
class BoolValue: public ValueBase {
    bool v;
public:
    BoolValue(bool v = false): v(v){}
    operator bool() const { return v; }
    virtual ValueBase * copy() const { return new BoolValue(v); }
    ostream & print(ostream & out) const;
    //BoolValue operator==(const ValueBase & v) const override;
    BoolValue operator&&(const BoolValue & v) const;
    BoolValue operator||(const BoolValue & v) const;
    BoolValue operator!() const;
    BoolValue operator^(const BoolValue & v) const;
    static BoolValue makeNull(bool v = false);
    //
};

/**
 * NULL 不是一种类型，而是具有特殊意义的值，和每一种类型都可以交叉。
 * 从这个角度，不妨让NULL
 * 这个运算符表现出一系列极为诡异的性质。首先，有它的运算返回都是NULL（大作业要求范围内），包括比较运算……
 * 然而，在排序中它又表现出非常高的优先级总是位于非空值前面……
 * */
/*
class NullValue : public ValueBase {
public:
    NullValue(){}
    BoolValue operator==(const ValueBase & v) const override {
        return false; // NULL和任何值比较结果都是NULL，包括NULL本身（NULL==NULL返回false）
    }
    BoolValue operator<(const ValueBase & v) const override {
        return false;
    }
    BoolValue operator>(const ValueBase & v) const override {
        return false;
    }
    virtual ostream & print(ostream & out) const {
        auto ptr = &out;
        if (dynamic_cast<std::ofstream *>(ptr)) 
            return out << "\\N"; // 文件，只能用“\N”来表示空值
        return out; // 屏幕：不输出
    }
    virtual ValueBase * copy() const { return new NullValue(); }
};
*/
/**
 * 程序内用Value<int>, Value<double>, Value<string>等类代表各种数据类型。
 * 问题：Value<int>和Value<double>比较总是false。
 */
template <class T>
class Value : public ValueBase{
    T v;
public: 
    Value(T v = 0):v(v) {}
    operator T() const { return v; }
    /*
    // 用于排序
    bool lessThan(const ValueBase *) const {
        Value<T> * nptr = dynamic_cast<Value<T> *>(const_cast<ValueBase *>(&v));
        if (nptr) return this->operator T() < T(*nptr);
        // 视作NULL
        return false;
    }*/
    // 用于运算
    BoolValue operator==(const ValueBase & v) const override {
        if (isNull || ::isNull(v)) return BoolValue::makeNull();
        Value<T> * nptr = dynamic_cast<Value<T> *>(const_cast<ValueBase *>(&v));
        return nptr && this->operator T() == T(*nptr);
    }
    virtual BoolValue operator<(const ValueBase & v) const {
        if (isNull || ::isNull(v)) return BoolValue::makeNull(isNull && !::isNull(v));  // 排序时NULL < 1被认为是true
        Value<T> * nptr = dynamic_cast<Value<T> *>(const_cast<ValueBase *>(&v));
        return nptr && this->operator T() < T(*nptr);
    }
    virtual BoolValue operator>(const ValueBase & v) const {
        if (isNull || ::isNull(v)) return BoolValue::makeNull(!isNull && ::isNull(v));
        Value<T> * nptr = dynamic_cast<Value<T> *>(const_cast<ValueBase *>(&v));
        return nptr && this->operator T() > T(*nptr);
    }
    Value<T> * copy() const override { return new Value<T>(v); }
    ostream & print(ostream & out) const override { 
        if (isNull) return ValueBase::print(out);
        return out << std::fixed << std::setprecision(4) << v; 
    }
    ~Value(){}
};

// 建议使用IntValue作为Value<int>的别名（因为扩展需求中的Date和Time都不再是对C++内建类型的简单包装了）
typedef Value<int> IntValue;
typedef Value<double> DoubleValue;
typedef Value<string> CharValue;


long double stolld(string);

// 被新版的converT取代，即将废弃。此版本没有对NULL的支持，不利于后续数据类型的扩展。
template<class T> Value<T>* convert(ValueBase * b);
template<> Value<double>* convert(ValueBase * b);
template<> Value<int>* convert(ValueBase * b);
// 新的类型转换函数。之前版本设计得太蠢了。
// 新增了对于NULL的支持。
template<class T> T* convertT(ValueBase * b);
template<> DoubleValue* convertT(ValueBase * b);
template<> IntValue* convertT(ValueBase * b);