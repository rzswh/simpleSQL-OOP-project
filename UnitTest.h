#pragma once

class TestWhereClause;
class TestTable;
#include "WhereClause.h"
#include "Table.h"
#include <cassert>
#include <iostream>

class TestWhereClause {
public:
    TestWhereClause() = default;
    vector<Attribute> getAttrs1() {
        vector<Attribute> attrs;
        attrs.push_back(Attribute("student_name", "CHAR", true));
        attrs.push_back(Attribute("student_id", "INT", true));
        attrs.push_back(Attribute("student_gpa", "DOUBLE", false));
        return attrs;
    }
    void test();
    void test1();
    void test2();
    void test3();
    void test4();
};

class TestTable {
public:
    TestTable() = default;
    vector<Attribute> getAttrs1() {
        vector<Attribute> attrs;
        attrs.push_back(Attribute("student_name", "CHAR", true));
        attrs.push_back(Attribute("student_id", "INT", true));
        attrs.push_back(Attribute("student_gpa", "DOUBLE", false));
        return attrs;
    }
    void test();
    void test1();
};
void whereClauseUnitTest();
void tableUnitTest();