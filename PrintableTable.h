#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#include "Attribute.h"
#include "Value.h"
#include "Record.h"
#include "Expression.h"
using std::string;
using std::vector;
using std::ostream;
using std::ofstream;

class PrintableTable {
    vector<string> attrs;
    vector<ValueBase **> data;
    ostream & printData(ostream & os) const;
    ostream & printHead(ostream & os) const;
public:
    PrintableTable(vector<Attribute> a);
    PrintableTable(vector<Expression *>  exps);
    void insert(ValueBase ** vs) { data.push_back(vs); }
    // 通过一组Record直接设置数据
    void setData(const vector<Record> & data);
    ostream & print(ostream & out) const;
    ofstream & print(ofstream & out) const;
    ~PrintableTable();
};
