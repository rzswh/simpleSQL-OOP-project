// encodings=UTF-8
#pragma once

#include <iostream>
#include <iomanip>
#include <string>
using std::ostream;
using std::string;
/**
 * 数据库内所储存数据的抽象基类。
 */
class ValueBase {
public:
    virtual bool operator==(const ValueBase & v) const = 0;
    virtual bool operator>(const ValueBase & v) const = 0;
    virtual bool operator<(const ValueBase & v) const = 0;
    virtual ValueBase * copy() const = 0;
    virtual ~ValueBase() = 0;
    virtual ostream & print(ostream & out) const = 0;
    friend ostream & operator<<(ostream & out, ValueBase & v) {
        return v.print(out); 
    }
};

/**
 * 程序内用Value<int>, Value<double>, Value<string>等类代表各种数据类型。
 * NULL没有对应的类型，在传递数据对象指针时用nullptr代表。
 * 问题：Value<int>和Value<double>比较总是false。
 */
template <class T>
class Value : public ValueBase{
    T v;
public: 
    Value(T v):v(v) {}
    operator T() const { return v; }
    bool operator==(const ValueBase & v) const override {
        ValueBase * ptr = const_cast<ValueBase *>(&v);
        Value<T> * nptr = dynamic_cast<Value<T> *>(ptr);
        return nptr && this->operator T() == T(*nptr);
    }
    bool operator<(const ValueBase & v) const override {
        ValueBase * ptr = const_cast<ValueBase *>(&v);
        Value<T> * nptr = dynamic_cast<Value<T> *>(ptr);
        return nptr && this->operator T() < T(*nptr);
    }
    bool operator>(const ValueBase & v) const override {
        ValueBase * ptr = const_cast<ValueBase *>(&v);
        Value<T> * nptr = dynamic_cast<Value<T> *>(ptr);
        return nptr && this->operator T() > T(*nptr);
    }
    Value<T> * copy() const override { return new Value<T>(v); }
    ostream & print(ostream & out) const override { 
        return out << std::fixed << std::setprecision(4) << v; 
    }
    ~Value(){}
};

template<class T> Value<T>* convert(ValueBase * b);
template<> Value<double>* convert(ValueBase * b);
template<> Value<int>* convert(ValueBase * b);

ValueBase * stringToValue(string);
long double stolld(string);