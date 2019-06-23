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
    BoolValue(const BoolValue & v) : v(v.v), ValueBase(v.isNull) {}
    operator bool() const { return v; }
    bool isTrue() const { return v && !isNull; }
    virtual ValueBase * copy() const { return new BoolValue(*this); }
    ostream & print(ostream & out) const;
    //BoolValue operator==(const ValueBase & v) const override;
    BoolValue operator&&(const BoolValue & v) const;
    BoolValue operator||(const BoolValue & v) const;
    BoolValue operator!() const;
    BoolValue operator^(const BoolValue & v) const;
    static BoolValue makeNull(bool v = false);
    //
};

template <class T>
class Value : public ValueBase{
    T v;
public: 
    Value(T v = 0):v(v) {}
    operator T() const { return v; }
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
    Value<T> * copy() const override { return isNull ? new Null<Value<T>>() : new Value<T>(v); }
    ostream & print(ostream & out) const override { 
        if (isNull) return ValueBase::print(out);
        return out << std::fixed << std::setprecision(4) << v; 
    }
    ~Value(){}
};

class DoubleValue;

class IntValue : public ValueBase {
    int v;
public: 
    IntValue(int v = 0):v(v) {}
    operator int() const { return v; }
    // 用于运算
    BoolValue operator==(const IntValue & w) const;
    BoolValue operator==(const DoubleValue & b) const;
    BoolValue operator==(const ValueBase & v) const override;

    BoolValue operator<(const IntValue & w) const;
    BoolValue operator<(const DoubleValue & w) const;
    BoolValue operator<(const ValueBase & v) const override;

    BoolValue operator>(const IntValue & w) const;
    BoolValue operator>(const DoubleValue & w) const;
    BoolValue operator>(const ValueBase & v) const override;

    IntValue * copy() const override;
    ostream & print(ostream & out) const override;
};

class DoubleValue : public ValueBase {
    double v;
public: 
    DoubleValue(double v = 0):v(v) {}
    operator double() const { return v; }
    // 用于运算
    BoolValue operator==(const IntValue & w) const;
    BoolValue operator==(const DoubleValue & w) const;
    BoolValue operator==(const ValueBase & v) const override;

    BoolValue operator<(const IntValue & w) const;
    BoolValue operator<(const DoubleValue & w) const;
    BoolValue operator<(const ValueBase & v) const override;

    BoolValue operator>(const IntValue & w) const;
    BoolValue operator>(const DoubleValue & w) const;
    BoolValue operator>(const ValueBase & v) const override;
    DoubleValue * copy() const override { return isNull ? new Null<DoubleValue>() : new DoubleValue(v); }
    ostream & print(ostream & out) const override;
    ~DoubleValue(){}
};

// 建议使用IntValue作为Value<int>的别名（因为扩展需求中的Date和Time都不再是对C++内建类型的简单包装了）
//typedef Value<int> IntValue;
//typedef Value<double> DoubleValue;
typedef Value<string> CharValue;

class TimeValue : public ValueBase {
	int hour;
	int minute;
	int second;
public:
	TimeValue(string time);
	TimeValue(int hour = 0,int minute =0,int second = 0) :hour(hour),minute(minute),second(second) {}
	int timesum() const { return 3600 * hour + 60 * minute + second; }
	// 用于运算
	BoolValue operator==(const TimeValue & w) const;

	BoolValue operator<(const TimeValue & w) const;

	BoolValue operator>(const TimeValue & w) const;

	TimeValue * copy() const override { return isNull ? new Null<TimeValue>() : new TimeValue(hour,minute,second); };
	ostream & print(ostream & out) const override;

	BoolValue valid() { return BoolValue(hour >= 0 && hour<24 && minute >= 0 && minute<60 && second >= 0 && second<60); }

	void addTime(int dhour,int dminute,int dsecond);

	void addTime(string s);
};
class DateValue : public ValueBase {
	int year;
	int month;
	int day;
	int ds[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	int mtod[12] = { 31,59,90,120,151,181,212,243,273,304,334,365 };
public:
	DateValue(string time);
	DateValue(int year = 0, int month = 1, int day = 1) :year(year), month(month), day(day) {}
	int timesum() const;
	// 用于运算
	BoolValue operator==(const DateValue & w) const;

	BoolValue operator<(const DateValue & w) const;

	BoolValue operator>(const DateValue & w) const;
	DateValue * copy() const override { return isNull ? new Null<DateValue>() : new DateValue(year, month, day); };
	ostream & print(ostream & out) const override;

	BoolValue valid() { return BoolValue(year >= 0 && month >= 1 && month<=12 && day >= 1 && day<=ds[month-1]); }

	void addoneday();

	void addDate(string s);
    void addMonth(string s);
    void addYear(string s);
};
long double stolld(string);

// 被新版的converT取代，即将废弃。此版本没有对NULL的支持，不利于后续数据类型的扩展。
template<class T> Value<T>* convert(ValueBase * b);

// 新的类型转换函数。之前版本设计得太蠢了。 
// 新增了对于NULL的支持。
// 类型转换后，内存会复制一份
template<class T> T* convertT(ValueBase * b);
template<> DoubleValue* convertT(ValueBase * b);
template<> IntValue* convertT(ValueBase * b);
template<> TimeValue* convertT(ValueBase * b);
template<> DateValue* convertT(ValueBase * b);

