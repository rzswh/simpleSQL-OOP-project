#pragma once
#include <string>
using std::string;

enum AttributeType {ATTR_INT, ATTR_CHAR, ATTR_DOUBLE};

/**
 * Convert string form of attribute type into enum type to be used in constructing AttributeType.
 * @param s Attribute type. Upper case. Now only INT, CHAR and DOUBLE are accepted. 
 * note: we store char using std::string. It's strange... but we insist on doing this.
 */ 
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