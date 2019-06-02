#include "Expression.h"
#include <sstream>

Expression::~Expression() {}

string AttributeExpression::toString() const {
    return name;
}

ValueBase * ConstExpression::eval() {
    return val->copy();
}

ConstExpression::~ConstExpression() {
    delete val;
}

string ConstExpression::toString() const {
    std::stringstream ss;
    ss << *val;
    string ret;
    ss >> ret;
    return ret;
}

CountFunction ::~CountFunction() {
    delete exp;
}

ValueBase * CountFunction::eval(const Record &r, const vector<Attribute> & attrs) {
    vector<Record *> _tmp; _tmp.push_back(const_cast<Record *>(&r));
    return evalAggregate(_tmp, attrs);
    
}
ValueBase * CountFunction::evalAggregate(vector<Record *>& rs, const vector<Attribute> & attrs) {
    if (dynamic_cast<ConstExpression *> (exp)) 
        return new IntValue(rs.size() * isNull(dynamic_cast<ConstExpression *> (exp)->eval()) );
    else if (dynamic_cast<AttributeExpression *>(exp)) {
        auto ae = dynamic_cast<AttributeExpression *>(exp);
        int ret = 0;
        for (auto it: rs) {
            Record & r = *it;
            for (int i = 0; i < attrs.size(); i++) {
                if (!isNull(r[i]) && (ae->toString() == "*" || ae->toString() == attrs[i].name)) {
                    ret ++;
                    break;
                }
            }
        }
        return new IntValue(ret);
    } else return ValueBase::newNull();
}

string CountFunction::toString() const {
    return string("COUNT(") + exp->toString() + ")";
}