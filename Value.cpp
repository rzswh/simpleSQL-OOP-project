#include "Value.h"
#include <string>
using std::string;

ValueBase::~ValueBase() {}

ValueBase * stringToValue(string tmp) {
	ValueBase * vb;
	if(tmp[0]=='\''){
		vb = new Value<string>(tmp.substr(1, tmp.find_last_of('\'')-1));
	} 
	else if(tmp[0]=='\"'){
		vb = new Value<string>(tmp.substr(1, tmp.find_last_of('\"')-1));
	} 
	else if(tmp.find('.')!=string::npos){
		vb = new Value<double>(atof(tmp.c_str()));
	}
	else {
		int ttt=atoi(tmp.c_str());
		vb = new Value<int>(ttt);
	}
	return vb;
}