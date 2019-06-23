#include "PrintableTable.h"

using std::endl;

PrintableTable::PrintableTable(vector<Attribute> a) {
    for (auto &i: a) attrs.push_back(i.name);
}
PrintableTable::PrintableTable(vector<Expression *>  exps) {
    for (auto &i: exps) attrs.push_back(i->toString());
}
ostream & PrintableTable::printHead(ostream & out) const {
    if (data.size() == 0) return out;
    int n = attrs.size();
    for (int i = 0; i < n-1; i++) out << attrs[i] << "\t";
    out << attrs[n-1] << endl;
	return out;
}
ostream & PrintableTable::printData(ostream & out) const {
    int n = attrs.size();
    for (auto & j: data) {
        for (int i = 0; i < n; i++) {
            if (j[i]) out << (*j[i]);
            else out << "NULL";
            if (i + 1 < n)out << "\t";
        }
        out << endl;
    }
    return out;
}

void PrintableTable::setData(const vector<Record> & data) {
    int n = attrs.size(), m = attrs.size();
    for (auto r: data) {
        auto nr = new ValueBase *[m];
        for (int i = 0; i < n; i++) 
            nr[i] = r[i] ? r[i]->copy() : r[i]; 
        insert(nr);
    }
}

ostream & PrintableTable::print(ostream & out) const {
    return printData(printHead(out));
}

ofstream & PrintableTable::print(ofstream & out) const {
    printData(out);
    return out;
}

void PrintableTable::unionAllTable(PrintableTable &prt){
	vector<ValueBase **> prt_datas = prt.get_data();
	for(ValueBase** prt_data:prt_datas)
	insert(prt_data);
} 

void PrintableTable::unionTable(PrintableTable &prt){
	vector<ValueBase **> prt_datas = prt.get_data();
	int size = data.size();

	for(ValueBase** prt_data:prt_datas){
		int i=0;
		for(;i<size;i++){
			if(**(data[i]) == **prt_data)
				break;
		}
		if(i == size){
			insert(prt_data);size++;}
	}
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
