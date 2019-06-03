#include "Value.h"
#include <string>
using std::string;

ValueBase::~ValueBase() {}

BoolValue ValueBase::operator==(const ValueBase & v) const {
	return Null<BoolValue>();
}
BoolValue ValueBase::operator<(const ValueBase & v) const {
	return BoolValue::makeNull(!v.isNull);
}
BoolValue ValueBase::operator>(const ValueBase & v) const {
	return BoolValue::makeNull(false);
}
ostream & ValueBase::print(ostream & out) const {
	auto ptr = &out;
	if (dynamic_cast<std::ofstream *>(ptr)) 
		return out << "\\N"; // 文件，只能用“\N”来表示空值
	return out << "NULL"; // 屏幕
}

BoolValue BoolValue::operator&&(const BoolValue & v) const {
	return (v.isNull || isNull) ? Null<BoolValue>() : BoolValue(this->v && v) ;
}

BoolValue BoolValue::operator||(const BoolValue & v) const {
	return (v.isNull || isNull) ? Null<BoolValue>() : BoolValue(this->v || v) ;
}

BoolValue BoolValue::operator^(const BoolValue & v) const {
	return (v.isNull || isNull) ? Null<BoolValue>() : BoolValue(this->v ^ v) ;
}

BoolValue BoolValue::operator!() const {
	return isNull ? Null<BoolValue>() : BoolValue(!this->v) ;
}

BoolValue BoolValue::makeNull(bool v) {
	BoolValue null(v); null.isNull = true; 
	return null;
}

/*
BoolValue BoolValue::operator==(const ValueBase & v) const {
	BoolValue * nptr = dynamic_cast<BoolValue *>(const_cast<ValueBase *>(&v));
	if (nptr) return BoolValue(this->operator bool() == bool(*nptr));
	return BoolValue(false);
}*/

// 这应当是一个永远用不上的函数，毕竟数据表中没有这种类型
ostream & BoolValue::print(ostream & out) const {
	if (isNull) return ValueBase::print(out);
	return out << (v ? "TRUE": "FALSE");  // ????
} 

long double stolld(string str) {
	long double ret = 0;
	int exp = 0;
	for (int i = str.length() - 1; i >= 0; i--) {
		if (str[i] == '.') exp = i;
		else ret = ret * 0.1 + (str[i] - '0');
	}
	exp--;
	while (exp--) ret *= 10;
	return ret;
}

bool isNull(ValueBase * v) { 
	return !v || v->isNull;
}
bool isNull(const ValueBase & v) { 
	return isNull(const_cast<ValueBase *> (&v));
}

template<class T> Value<T>* convert(ValueBase * b) {
    auto i = dynamic_cast<Value<T>*> (b);
    if (i == nullptr) return nullptr;
    return i->copy();
}
template<> Value<double>* convert(ValueBase * b) {
    auto d = dynamic_cast<Value<double>*>(b);
    if (d != nullptr) return d->copy();
    auto i = dynamic_cast<Value<int>*>(b);
    if (i != nullptr) return new Value<double>(i->operator int());
    return nullptr;
}
template<> Value<int>* convert(ValueBase * b) {
    auto d = dynamic_cast<Value<int>*>(b);
    if (d != nullptr) return d->copy();
    auto i = dynamic_cast<Value<double>*>(b);
    if (i != nullptr) return new Value<int>(i->operator double());
    return nullptr;
}
template Value<string>* convert<string>(ValueBase *b);

template<class T> T* convertT(ValueBase * b) {
	if (isNull(b)) {
		return new Null<T>();
	}
    auto i = dynamic_cast<T*> (b);
    if (i == nullptr) return nullptr;
    return i->copy();
}
template<> DoubleValue* convertT(ValueBase * b) {
	if (isNull(b)) {
		return new Null<DoubleValue>;
	}
    auto i = dynamic_cast<DoubleValue*> (b);
    if (i) return i->copy();
    auto j = dynamic_cast<IntValue*> (b);
    if (j) return new DoubleValue(j->operator int());
    return nullptr;
}
template<> IntValue* convertT(ValueBase * b) {
	if (isNull(b)) {
		return new Null<IntValue>;
	}
    auto i = dynamic_cast<IntValue*> (b);
    if (i) return i->copy();
    auto j = dynamic_cast<DoubleValue*> (b);
    if (j) return new IntValue(j->operator double());
    return nullptr;
}
template CharValue* convertT<CharValue>(ValueBase *b);
