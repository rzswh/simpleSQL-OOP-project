#include "Attribute.h"
#include <iostream>

/**
 * Convert string form of attribute type into enum type to be used in constructing AttributeType.
 * @param s Attribute type. Upper case. Now only INT, CHAR and DOUBLE are accepted. 
 */ 
AttributeType fromStringToAttrType(const string & s) {
    if (s == "INT")
        return ATTR_INT;
    else if (s == "CHAR")
        return ATTR_CHAR;
    else if (s == "DOUBLE")
        return ATTR_DOUBLE;
    else {
        std::cerr << "Error from Attribute.cpp::fromStringToAttrType(const string &): " << std::endl;
        std::cerr << "\tInvalid AttributeType name. Check if it is UPPER case and is one of INT, CHAR and DOUBLE." << std::endl;
        return ATTR_INT;
    }
}