#include "Value.h"
#include <string>
using std::string;

ValueBase::~ValueBase() {}

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

ValueBase * stringToValue(string tmp) {
	ValueBase * vb;
	if(tmp[0]=='\''){
		vb = new Value<string>(tmp.substr(1, tmp.find_last_of('\'')-1));
	} 
	else if(tmp[0]=='\"'){
		vb = new Value<string>(tmp.substr(1, tmp.find_last_of('\"')-1));
	} 
	else if(tmp.find('.')!=string::npos){
		vb = new Value<double>(stolld(tmp));
	}
	else {
		int ttt=atoi(tmp.c_str());
		vb = new Value<int>(ttt);
	}
	return vb;
}

template<class T> Value<T>* convert(ValueBase * b) {
    auto i = dynamic_cast<Value<T>*> (b);
    if (b == nullptr) return nullptr;
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