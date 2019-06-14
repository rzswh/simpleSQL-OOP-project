#include "UnitTest.h"
#include "../Expression.h"
#include "../Table.h"
#include <cassert>
#include <iostream>

vector<Attribute> getAttrs1() {
    vector<Attribute> attrs;
    attrs.push_back(Attribute("student_name", "CHAR", true));
    attrs.push_back(Attribute("student_id", "INT", true));
    attrs.push_back(Attribute("student_gpa", "DOUBLE", false));
    return attrs;
}
void functionTest() {
    vector<Attribute> attrs = getAttrs1();
    Table tbl = Table("test", attrs, "student_id");
    // insert
    auto v1 = new Value<string>("h");
    auto v2 = new IntValue(2017654321);
    auto v3 = new DoubleValue(3.3);
    assert(tbl.insert(vector<string>({"student_id", "student_name", "student_gpa"}),
        vector<ValueBase *>({v2, v1, v3})) == true);
    auto u1 = new Value<string>("b");
    auto u2 = new IntValue(2017123456);
    assert(tbl.insert(vector<string>({"student_name", "student_id"}),
        vector<ValueBase *>({u1, u2, nullptr})) == true);
    auto s1 = new Value<string>("b");
    auto s2 = new IntValue(2016011010);
    assert(tbl.insert(vector<string>({"student_name", "student_id"}),
        vector<ValueBase *>({s1, s2, nullptr})) == true);
    assert(tbl.insert(vector<string>({"student_gpa","student_name", "student_id"}),
        vector<ValueBase *>({v3, s1, u2})) == false);
    auto x1 = new IntValue (2017200000);
    WhereClause wc2 = WhereClause (
        vector<WhereClause::SubSentence>({
            //std::make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_LESS, x1, 1)
        }), 
        vector<std::pair<LogicOperation, int> >({})
    );
    CountFunction * count = new CountFunction(new AttributeExpression("*"));
    AttributeExpression * id = new AttributeExpression("student_gpa");
    AttributeExpression * nm = new AttributeExpression("student_name");
    PrintableTable * ptb = tbl.select(vector<Expression*>({count}), wc2,
            vector<Expression*>({nm}), 
            nm);
    ptb->print(std::cout);
    delete ptb;
    delete s1, s2;
    delete v1, v2, v3;
    delete u1, u2;
    delete count, id;
    // delete w1;
    delete x1;
}

void valueTest() {
    ValueBase * null = new Null<IntValue>();
    ValueBase * one = new IntValue(1);
    std::cout << (*null < *null) << std::endl;
    std::cout << (*one < *one) << std::endl;
    std::cout << (*null < *one) << std::endl;
    std::cout << (*one < *null) << std::endl;
}

void unitTests() {
    //functionTest();
    valueTest();
}
