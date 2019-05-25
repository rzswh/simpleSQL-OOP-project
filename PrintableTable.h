#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#include "Attribute.h"
#include "Value.h"
#include "Record.h"
using std::string;
using std::vector;
using std::ostream;
using std::ofstream;

class PrintableTable {
    const vector<Attribute> attrs;
    vector<ValueBase **> data;
    ostream & printData(ostream & os) const;
    ostream & printHead(ostream & os) const;
public:
    PrintableTable(vector<Attribute> a):attrs(a) {}
    void insert(ValueBase ** vs) { data.push_back(vs); }
    ostream & print(ostream & out) const;
    ofstream & print(ofstream & out) const;
    ~PrintableTable();
};
