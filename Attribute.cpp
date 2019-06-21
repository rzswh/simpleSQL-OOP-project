#include "Attribute.h"
#include <iostream>

AttributeType fromStringToAttrType(const string & s) {
	if (s == "INT")
		return ATTR_INT;
	else if (s == "CHAR")
		return ATTR_CHAR;
	else if (s == "DOUBLE")
		return ATTR_DOUBLE;
	else if (s == "TIME")
		return ATTR_TIME;
	else if (s == "DATE")
		return ATTR_DATE;
    else {
        std::cerr << "Error from Attribute.cpp::fromStringToAttrType(const string &): " << std::endl;
        std::cerr << "\tInvalid AttributeType name. Check if it is UPPER case and is one of INT, CHAR and DOUBLE." << std::endl;
        return ATTR_INT;
    }
}