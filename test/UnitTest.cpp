#include "UnitTest.h"
#include "../Expression.h"
#include "../Table.h"
#include <cassert>
#include <iostream>

void functionTest() {
    vector<Attribute> attrs = getAttrs1();
    Table tbl = Table("test", attrs, "student_id");
    // insert
    auto v1 = new Value<string>("m");
    auto v2 = new Value<int>(2017654321);
    auto v3 = new Value<double>(3.3);
    assert(tbl.insert(vector<string>({"student_id", "student_name", "student_gpa"}),
        vector<ValueBase *>({v2, v1, v3})) == true);
    auto u1 = new Value<string>("h");
    auto u2 = new Value<int>(2017123456);
    assert(tbl.insert(vector<string>({"student_name", "student_id"}),
        vector<ValueBase *>({u1, u2, nullptr})) == true);
    auto s1 = new Value<string>("b");
    auto s2 = new Value<int>(2016011010);
    assert(tbl.insert(vector<string>({"student_name", "student_id"}),
        vector<ValueBase *>({s1, s2, nullptr})) == true);
    assert(tbl.insert(vector<string>({"student_gpa","student_name", "student_id"}),
        vector<ValueBase *>({v3, s1, u2})) == false);
    auto x1 = new Value<int> (2017200000);
    WhereClause wc2 = WhereClause (
        vector<WhereClause::SubSentence>({
            std::make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_LESS, x1, 1)
        }), 
        vector<std::pair<LogicOperation, int> >({})
    );
    PrintableTable * ptb = tbl.select(vector<string>({"*"}), wc2);
    ptb->print(std::cout);
    delete ptb;
    delete s1, s2;
    delete v1, v2, v3;
    delete u1, u2;
    // delete w1;
    delete x1;
}

void unitTests() {
    functionTest();
}
