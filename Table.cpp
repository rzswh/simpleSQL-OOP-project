#include "Table.h"

Table::Table(string name, vector<Attribute> a, string primary)
        :name(name), attrs(a), primary(primary) 
{}

Table::Table(Table && t) {}

Table::~Table() {}

bool Table::insert(vector<string> attrNames, vector<ValueBase *> vals) {
    Record t(attrs.size());
    bool succ = true;
    for (int i = 0; i < attrNames.size(); i++) {
        for (int j = 0; j < attrs.size(); j++) {
            if (attrNames[i] == attrs[j].name) {
                // 检查是否指定了重复的属性
                for (int k = 0; k < attrNames.size(); k ++) 
                    if (attrNames[i]== attrNames[k])
                        succ = false;
                switch (attrs[j].type)
                {
                    case ATTR_INT:
                        auto intv = dynamic_cast<Value<int>* >(vals[i]);
                        if (intv) {
                            t[j] = new Value<int>(int(*intv));
                        } else succ = false;
                        break;
                    case ATTR_DOUBLE:
                        auto doublev = dynamic_cast<Value<double>* >(vals[i]);
                        if (doublev) {
                            t[j] = new Value<double>(double(*doublev));
                        } else succ = false;
                        break;
                    case ATTR_CHAR:
                        auto charv = dynamic_cast<Value<string>* >(vals[i]);
                        if (charv) {
                            t[j] = new Value<string>(string(*charv));
                        } else succ = false;
                        break;
                    default:
                        break;
                }
                break;
            }
        }
        if (!succ) break;
    }
    for (int i = 0; i < attrs.size(); i++) {
        // notNull check(primary key is forced to be notNull)
        if (t[i] == nullptr && (attrs[i].notNull || primary == attrs[i].name)) {
            succ = false;
            break;
        }
        // primary key
        if (primary == attrs[i].name) {
            for (auto& j: data) {
                if (*j[i] == *t[i]) {
                    succ = false;
                    break;
                }
            }
        }
        if (!succ) break;
    }
    if (succ) data.push_back(t);
    else {
        for (int i = 0; i < attrs.size(); i++) 
            delete t[i];
    }
    return succ;
}