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
		return out << "NULL"; // 文件。还是用“\N”来表示空值？
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

// convert

template<class T> Value<T>* convert(ValueBase * b) {
    auto i = dynamic_cast<Value<T>*> (b);
    if (i == nullptr) return nullptr;
    return i->copy();
}
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
    return nullptr;
}
template<> TimeValue* convertT(ValueBase * b) {
	if (isNull(b)) {
		return new Null<TimeValue>;
	}
	auto i = dynamic_cast<TimeValue*> (b);
	if (i) return i->copy();
	return nullptr;
}
template<> DateValue* convertT(ValueBase * b) {
	if (isNull(b)) {
		return new Null<DateValue>;
	}
	auto i = dynamic_cast<DateValue*> (b);
	if (i) return i->copy();
	return nullptr;
}
//template CharValue* convertT<CharValue>(ValueBase *b);

TimeValue::TimeValue(string time)
{
	int position = 0;
	int pos[6];
	int poscnt = 0;
	while ((position = time.find_first_of(':', position)) != string::npos)
	{
		pos[poscnt++] = position;
		position++;
	}
	int p = 0;
	hour = stoi(time.substr(0, pos[p]));
	minute = stoi(time.substr(pos[p] + 1, pos[p + 1] - pos[p] - 1));
	second = stoi(time.substr(pos[p + 1] + 1, time.length() - pos[p + 1] - 1));
}

BoolValue TimeValue::operator==(const TimeValue & w) const
{
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(timesum() == w.timesum());
}

BoolValue TimeValue::operator<(const TimeValue & w) const
{
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(timesum() < w.timesum());
}

BoolValue TimeValue::operator>(const TimeValue & w) const
{
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(timesum() > w.timesum());
}

ostream & TimeValue::print(ostream & out) const
{
	if (isNull) return ValueBase::print(out);
	return out << std::setw(2) << std::setfill('0') << hour
		<<":"<< std::setw(2) << std::setfill('0') << minute 
		<<":"<< std::setw(2) << std::setfill('0') << second ;
}

void TimeValue::addTime(string time)
{
	int position = 0;
	int pos[6];
	int poscnt = 0;
	while ((position = time.find_first_of(':', position)) != string::npos)
	{
		pos[poscnt++] = position;
		position++;
	}
	//std::cout << pos[0] << " " << pos[1] << std::endl;
	int p = 0;
	int dhour = stoi(time.substr(0, pos[p]));
	int dminute = stoi(time.substr(pos[p] + 1, pos[p + 1] - pos[p] - 1));
	int dsecond = stoi(time.substr(pos[p + 1] + 1, time.length() - pos[p + 1] - 1));
	addTime(dhour, dminute, dsecond);
}

void TimeValue::addTime(int dhour,int dminute,int dsecond)
{
	int time = timesum();
	time += dsecond;
	time += dminute * 60;
	time += dhour * 3600;
	if (time < 0)
	{
		while (time < 0)
			time += 86400;
	}
	time = time % 86400;
	hour = time / 3600;
	time = time % 3600;
	minute = time / 60;
	second = time % 60;
}

template CharValue* convertT<CharValue>(ValueBase *b);

DateValue::DateValue(string time)
{
	int position = 0;
	int pos[6];
	int poscnt = 0;
	while ((position = time.find_first_of('-', position)) != string::npos)
	{
		pos[poscnt++] = position;
		position++;
	}
	int p = 0;
	year = stoi(time.substr(0, pos[p]));
	month = stoi(time.substr(pos[p] + 1, pos[p + 1] - pos[p] - 1));
	day = stoi(time.substr(pos[p + 1] + 1, time.length() - pos[p + 1] - 1));
}

int DateValue::timesum() const
{
	return year * 1000 + month * 100 + day;
}

BoolValue DateValue::operator==(const DateValue & w) const
{
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(timesum() == w.timesum());
}

BoolValue DateValue::operator<(const DateValue & w) const
{
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(timesum() < w.timesum());
}

BoolValue DateValue::operator>(const DateValue & w) const
{
	return (isNull || ::isNull(w)) ? BoolValue::makeNull() : BoolValue(timesum() > w.timesum());
}

ostream & DateValue::print(ostream & out) const
{
	if (isNull) return ValueBase::print(out);
	return out << std::setw(4) << std::setfill('0') << year
		<< "-" << std::setw(2) << std::setfill('0') << month
		<< "-" << std::setw(2) << std::setfill('0') << day;
}

void DateValue::addoneday()
{
	if (month == 12 && day == 31)
	{
		year += 1;
		month = 1;
		day = 1;
	}
	else if (day == ds[month - 1])
	{
		day = 1;
		month += 1;
	}
	else day += 1;

}

void DateValue::addDate(string s)
{
	int dday = stoi(s);
	while (dday--)
	{
		addoneday();
	}
}
void DateValue::addMonth(string s)
{
	int dmonth = stoi(s);
	month += dmonth;
	year += month / 12;
	month = month % 12;
}

void DateValue::addYear(string s)
{
	int dyear = stoi(s);
	year += dyear;
}