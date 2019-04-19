#pragma once
#include <string>
using std::string;

enum AttributeType {ATTR_INT, ATTR_CHAR, ATTR_DOUBLE};

AttributeType fromStringToAttrType(const string & n);

class Attribute {
public:
    const AttributeType type;
    const bool notNull;
    const string name;
    Attribute(string name, AttributeType type, bool notNull):
        name(name), type(type), notNull(notNull) {}
    Attribute(string name, string type, bool notNull):
        name(name), type(fromStringToAttrType (type)), notNull(notNull) {}
};