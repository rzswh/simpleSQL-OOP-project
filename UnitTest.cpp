#include "UnitTest.h"
using std::make_pair;
using std::make_tuple;

using namespace std;

void TestWhereClause::test() {
    test1();
    test2();
    test3();
    test4();
}

void TestWhereClause::test1() {
    vector<Attribute> attrs = getAttrs1();
    Record r(3);
    r[0] = new Value<string>("j");
    r[1] = new Value<int>(2017012345);
    r[2] = new Value<double>(4.0);
    vector<WhereClause::SubSentence> s; vector<pair<LogicOperation, int> > o;
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_name", ARITH_EQUAL, new Value<string>("张三"), 0) ); // A
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_LESS, new Value<int>(2017012323), 1) ); // B
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_gpa", ARITH_GREATER, new Value<double>(3.97), 3) ); // C
    o.push_back( make_pair<LogicOperation, int>(LOGIC_AND, 2) ); // X
    o.push_back( make_pair<LogicOperation, int>(LOGIC_OR, 4) ); // Y
    WhereClause wc(s, o); // A B X C Y
    assert(wc.test(r, attrs));
}
void TestWhereClause::test2() {
    vector<Attribute> attrs = getAttrs1();
    Record r(3);
    r[0] = new Value<string>("j");
    r[1] = new Value<int>(2017012345);
    r[2] = new Value<double>(4.0);
    vector<WhereClause::SubSentence> s; vector<pair<LogicOperation, int> > o;
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_gpa", ARITH_LESS, new Value<double>(3.97), -1) ); // C
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_name", ARITH_EQUAL, new Value<string>("j"), 0) ); // A
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_GREATER, new Value<int>(2017012323), 1) ); // B
    o.push_back( make_pair<LogicOperation, int>(LOGIC_AND, 2) ); // X
    o.push_back( make_pair<LogicOperation, int>(LOGIC_OR, 4) ); // Y
    WhereClause wc(s, o); // C A B X Y
    assert(wc.test(r, attrs));
}
void TestWhereClause::test3() {
    vector<Attribute> attrs = getAttrs1();
    Record r(3);
    r[0] = new Value<string>("j");
    r[1] = new Value<int>(2017012345);
    r[2] = new Value<double>(4.0);
    vector<WhereClause::SubSentence> s; vector<pair<LogicOperation, int> > o;
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_gpa", ARITH_LESS, new Value<double>(3.97), -1) ); // C
    WhereClause wc(s, o); // C
    assert(wc.test(r, attrs) == false);
}
void TestWhereClause::test4() {
    vector<Attribute> attrs = getAttrs1();
    Record r(3);
    r[0] = new Value<string>("j");
    r[1] = new Value<int>(2017012345);
    r[2] = nullptr;
    vector<WhereClause::SubSentence> s; vector<pair<LogicOperation, int> > o;
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_gpa", ARITH_LESS, new Value<double>(3.97), -1) ); // C
    s.push_back( make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_GREATER, new Value<int>(2017012323), 1) ); // B
    o.push_back( make_pair<LogicOperation, int>(LOGIC_OR, 2) ); // X
    WhereClause wc(s, o); // C B X
    assert(wc.test(r, attrs));
}

void TestTable::test() {
    test1();
    test1();
}

void TestTable::test1() {
    vector<Attribute> attrs = getAttrs1();
    Table tbl = Table("test", attrs, "student_id");
    tbl.test_print();
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
    tbl.test_print();
    // update test
    auto w1 = new Value<double> (3.7);
    WhereClause wc1 = WhereClause (
        vector<WhereClause::SubSentence>({
            std::make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_EQUAL, v2, 1)
        }), 
        vector<std::pair<LogicOperation, int> >({})
    );
    assert(tbl.update(vector<string>({"student_gpa"}), vector<ValueBase *>({w1}), wc1));
    tbl.test_print();
    // select
    auto x1 = new Value<int> (2017200000);
    WhereClause wc2 = WhereClause (
        vector<WhereClause::SubSentence>({
            std::make_tuple<string, ArithmicOperation, ValueBase *, int>("student_id", ARITH_LESS, x1, 1)
        }), 
        vector<std::pair<LogicOperation, int> >({})
    );
    PrintableTable * ptb = tbl.select(vector<string>({"*"}), wc2);
    ptb->print(cout);
    // delete
    tbl.del(wc2);
    tbl.test_print();
    delete ptb;
    delete s1, s2;
    delete v1, v2, v3;
    delete u1, u2;
    delete w1;
    delete x1;
}

void whereClauseUnitTest() {
    TestWhereClause twc;
    twc.test();
}
void tableUnitTest() {
    TestTable ttb;
    ttb.test();
}