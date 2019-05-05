#include "Table.h"

using std::endl;
ostream & PrintableTable::print(ostream & out) const {
    if (data.empty()) return out;
    int n = attrs.size();
    for (int i = 0; i < n; i++) out << attrs[i].name << "\t";
    out << endl;
    for (auto & j: data) {
        for (int i = 0; i < n; i++) {
            if (j[i]) out << (*j[i]);
            else out << "NULL";
            out << "\t";
        }
        out << endl;
    }
    return out;
}

PrintableTable::~PrintableTable() {
    int n = attrs.size();
    for (auto item: data) {
        for (int j = 0; j < n; j++) {
            delete item[j];
        }
        delete [] item;
    }
    data.clear();
}