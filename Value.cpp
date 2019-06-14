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

ostream & BoolValue::print(ostream & out) const {
	if (isNull) return ValueBase::print(out);
	return out << (v ? "TRUE": "FALSE"); 
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

// IntValue
BoolValue IntValue::operator==(const IntValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(v == w.v);
}
BoolValue IntValue::operator==(const DoubleValue & b) const {
    return (isNull || ::isNull(b)) ? BoolValue::makeNull() : BoolValue(v == b.operator double());
}
BoolValue IntValue::operator==(const ValueBase & v) const {
	ValueBase * ptr = const_cast<ValueBase *>(&v);
	if (dynamic_cast<IntValue *>(ptr)) return operator==(*dynamic_cast<IntValue *>(ptr));
	else if (dynamic_cast<DoubleValue *>(ptr)) return operator==(*dynamic_cast<DoubleValue *>(ptr));
	else return BoolValue::makeNull();
}
BoolValue IntValue::operator<(const IntValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull(w) : BoolValue(v < w);// 排序时NULL < 1被认为是true
}
BoolValue IntValue::operator<(const DoubleValue & b) const {
	return (isNull || ::isNull(b)) ? BoolValue::makeNull(b) : BoolValue(v < b.operator double());
}
BoolValue IntValue::operator<(const ValueBase & v) const {
	ValueBase * ptr = const_cast<ValueBase *>(&v);
	if (dynamic_cast<IntValue *>(ptr)) return operator<(*dynamic_cast<IntValue *>(ptr));
	else if (dynamic_cast<DoubleValue *>(ptr)) return operator<(*dynamic_cast<DoubleValue *>(ptr));
	else return BoolValue::makeNull();
}
BoolValue IntValue::operator>(const IntValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull(w) : BoolValue(v > w);// 排序时NULL < 1被认为是true
}
BoolValue IntValue::operator>(const DoubleValue & b) const {
	return (isNull || ::isNull(b)) ? BoolValue::makeNull(b) : BoolValue(v > b.operator double());
}
BoolValue IntValue::operator>(const ValueBase & v) const {
	ValueBase * ptr = const_cast<ValueBase *>(&v);
	if (dynamic_cast<IntValue *>(ptr)) return operator>(*dynamic_cast<IntValue *>(ptr));
	else if (dynamic_cast<DoubleValue *>(ptr)) return operator>(*dynamic_cast<DoubleValue *>(ptr));
	else return BoolValue::makeNull();
}

IntValue * IntValue::copy() const { 
	auto ret = new IntValue(v); 
	if (isNull) ret->isNull = true;
	return ret;
}

ostream & IntValue::print(ostream & out) const { 
	if (isNull) return ValueBase::print(out);
	return out << std::fixed << std::setprecision(4) << v; 
}
// 四则运算
/*
// 包装一下的意义就是……emmm……处理null
IntValue operator+(const IntValue & w) const { 
	if (isNull || ::isNull(w)) return Null<IntValue >(); 
	return IntValue(v + w);
}
DoubleValue operator+(const DoubleValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v + w);
}
IntValue operator-(const IntValue & w) const { 
	if (isNull || ::isNull(w)) return Null<IntValue >(); 
	return IntValue(v - w);
}
DoubleValue operator-(const DoubleValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v - w);
}
IntValue operator*(const IntValue & w) const { 
	if (isNull || ::isNull(w)) return Null<IntValue >(); 
	return IntValue(v * w);
}
DoubleValue operator*(const DoubleValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v * w);
}
DoubleValue operator/(const IntValue & w) const { 
	if (isNull || ::isNull(w) || w.v == 0) return Null<DoubleValue >(); 
	return DoubleValue(1.0 * v / w);
}
DoubleValue operator/(const DoubleValue & w) const { 
	if (isNull || ::isNull(w) || w.operator double() == 0) return Null<DoubleValue >(); 
	return DoubleValue(v / w);
}
IntValue operator%(const IntValue & w) const {
	if (isNull || ::isNull(w) || w.operator int() == 0) return Null<IntValue >(); 
	return IntValue (v % w);
}*/

// DoubleValue

BoolValue DoubleValue::operator==(const IntValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(v == w.operator int());
}
BoolValue DoubleValue::operator==(const DoubleValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(v == w.v);
}
BoolValue DoubleValue::operator==(const ValueBase & v) const  {
	ValueBase * ptr = const_cast<ValueBase *>(&v);
	if (dynamic_cast<IntValue *>(ptr)) return operator==(*dynamic_cast<IntValue *>(ptr));
	else if (dynamic_cast<DoubleValue *>(ptr)) return operator==(*dynamic_cast<DoubleValue *>(ptr));
	else return BoolValue::makeNull();
}

BoolValue DoubleValue::operator<(const IntValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull(w) : BoolValue(v < w.operator int());// 排序时NULL < 1被认为是true
}
BoolValue DoubleValue::operator<(const DoubleValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull(w) : BoolValue(v < w.v);
}
BoolValue DoubleValue::operator<(const ValueBase & v) const {
	ValueBase * ptr = const_cast<ValueBase *>(&v);
	if (dynamic_cast<IntValue *>(ptr)) return operator<(*dynamic_cast<IntValue *>(ptr));
	else if (dynamic_cast<DoubleValue *>(ptr)) return operator<(*dynamic_cast<DoubleValue *>(ptr));
	else return BoolValue::makeNull();
}

BoolValue DoubleValue::operator>(const IntValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull(w) : BoolValue(v > w.operator int());
}
BoolValue DoubleValue::operator>(const DoubleValue & w) const { 
	return (isNull || ::isNull(w)) ? BoolValue::makeNull(w) : BoolValue(v > w.v);
}
BoolValue DoubleValue::operator>(const ValueBase & v) const {
	ValueBase * ptr = const_cast<ValueBase *>(&v);
	if (dynamic_cast<IntValue *>(ptr)) return operator>(*dynamic_cast<IntValue *>(ptr));
	else if (dynamic_cast<DoubleValue *>(ptr)) return operator>(*dynamic_cast<DoubleValue *>(ptr));
	else return BoolValue::makeNull();
}
ostream & DoubleValue::print(ostream & out) const { 
	if (isNull) return ValueBase::print(out);
	return out << std::fixed << std::setprecision(4) << v; 
}

/*
// 四则运算
DoubleValue operator+(const IntValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v + w);
}
DoubleValue operator+(const DoubleValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v + w);
}
DoubleValue operator-(const IntValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v - w);
}
DoubleValue operator-(const DoubleValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v - w);
}
DoubleValue operator*(const IntValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v * w);
}
DoubleValue operator*(const DoubleValue & w) const { 
	if (isNull || ::isNull(w)) return Null<DoubleValue >(); 
	return DoubleValue(v * w);
}
DoubleValue operator/(const IntValue & w) const { 
	if (isNull || ::isNull(w) || w.operator int() == 0) return Null<DoubleValue >(); 
	return DoubleValue(v / w);
}
DoubleValue operator/(const DoubleValue & w) const { 
	if (isNull || ::isNull(w) || w.operator double() == 0) return Null<DoubleValue >(); 
	return DoubleValue(v / w);
}*/

// convert

template<class T> Value<T>* convert(ValueBase * b) {
    auto i = dynamic_cast<Value<T>*> (b);
    if (i == nullptr) return nullptr;
    return i->copy();
}
/*
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
}*/
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
	// 
    // auto j = dynamic_cast<DoubleValue*> (b);
    // if (j) return new IntValue(j->operator double());
    return nullptr;
}
template CharValue* convertT<CharValue>(ValueBase *b);
