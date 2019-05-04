// encodings=UTF-8
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
                    if (i != k && attrNames[i]== attrNames[k])
                        succ = false;
                if (vals[i] == nullptr) {
                    t[j] = vals[i];
                    break;
                }
                switch (attrs[j].type)
                {
                    case ATTR_INT:
                    {
                        auto intv = dynamic_cast<Value<int>* >(vals[i]);
                        if (intv) {
                            t[j] = new Value<int>(int(*intv));
                        } else succ = false;
                        break;
                    }
                    case ATTR_DOUBLE:
                    {
                        auto doublev = dynamic_cast<Value<double>* >(vals[i]);
                        if (doublev) {
                            t[j] = new Value<double>(double(*doublev));
                        } else succ = false;
                        break;
                    }
                    case ATTR_CHAR:
                    {
                        auto charv = dynamic_cast<Value<string>* >(vals[i]);
                        if (charv) {
                            t[j] = new Value<string>(string(*charv));
                        } else succ = false;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
        }
        if (!succ) break;
    }
    int primaryIndex = 0;
    for (int i = 0; i < attrs.size(); i++) {
        if (primary == attrs[i].name) {
            primaryIndex = i;
            // 检查主键唯一性
            for (auto& j: data) {
                if (*j[i] == *t[i]) {
                    succ = false;
                    break;
                }
            }
            break;
        }
    }
    for (int i = 0; i < attrs.size(); i++) {
        // notNull 检查(主键强制非空)
        if (t[i] == nullptr && (attrs[i].notNull || primaryIndex == i)) {
            succ = false;
            break;
        }
        if (!succ) break;
    }
    // 根据要求，记录要按照主键排序
    // （插入排序）
    if (succ) {
        auto iter = data.begin();
        for (; iter != data.end() && *(*iter)[primaryIndex] < *t[primaryIndex]; iter++) ;
        data.insert(iter, std::move(t));
    }
    return succ;
}

bool Table::del(WhereClause c) {
    // 逆序选择元素
    for (int i = data.size()-1; i>= 0; i-- ) {
        if (c.test(data[i], attrs)) {
            data.erase(data.begin() + i);
        }
    }
    return true;
}
bool Table::checkType(AttributeType att, ValueBase * v) {
    if (att == ATTR_CHAR && !dynamic_cast<Value<string>*> (v) )
        return false;
    if (att == ATTR_DOUBLE && !dynamic_cast<Value<double>*> (v) )
        return false;
    if (att == ATTR_INT && !dynamic_cast<Value<int>*> (v) )
        return false;
    return true;
}
bool Table::update(vector<string> attrNames, vector<ValueBase *> vals, WhereClause c) {
    // 类型匹配，非空检查
    for (int j = 0; j < attrNames.size(); j++) {
        for (int k = 0; k < attrs.size(); k++) {
            if (attrNames[j] == attrs[k].name) {
                if (!checkType(attrs[k].type, vals[j]))
                    return false;
                if (attrs[k].notNull && vals[j] == nullptr) 
                    return false;
                break;
            }
        }
    }
    // 主键查重
    vector<int> updateList; // 待修改的行号记录在这里
    // primaryCount: 主键的新值在更新完成后出现了多少次；
    // primaryIndex: 主键在attrs中的下标索引是多少
    int primaryCount = 0, primaryIndex = 0; 
    // primaryValue: 主键的新值是多少（如果主键未被修改，则为nullptr）
    ValueBase * primaryValue = nullptr;
    for (int i = 0; i < attrs.size(); i++) {
        if (attrs[i].name == primary) {
            primaryIndex = i;
            break;
        }
    }
    for (int i = 0 ; i < vals.size(); i++) {
        if (attrNames[i] == primary) 
            primaryValue = vals[i];
    }
    // 合法，则可以进行修改
    for (int i = 0; i < data.size(); i ++) {
        if (!c.test(data[i], attrs)) {
            if (primaryValue) 
                primaryCount += *data[i][primaryIndex] == *primaryValue;
        } else {
            updateList.push_back(i);
            primaryCount ++; // 对于赋值型更新，每修改一个，重复次数多一次
        }
        if (primaryValue && primaryCount > 1) break;
    }
    if (primaryValue && primaryCount > 1) return false;
    for (auto i: updateList) {
        for (int j = 0; j < attrNames.size(); j++) {
            for (int k = 0; k < attrs.size(); k++) {
                if (attrNames[j] == attrs[k].name) {
                    delete data[i][k];
                    data[i][k] = vals[j]->copy();
                    break;
                }
            }
        }
    }
    return true;
}

PrintableTable * Table::select(vector<string> attrFilter, WhereClause c) {
    vector<Attribute> newTableAttrs;
    for (auto i: attrs) {
        bool flag = false;
        for (auto j: attrFilter) {
            if (j == "*" || j == i.name) {
                flag = true;
                break;
            }
        }
        if (flag) 
            newTableAttrs.push_back(i);
    }
    // 这里主键名称为空，不符合约定，因此
    PrintableTable * table = new PrintableTable(newTableAttrs);
    int n = attrs.size(), m = newTableAttrs.size();
    for (auto r: data) {
        if (!c.test(r, attrs)) continue;
        auto nr = new ValueBase *[m];
        for (int i = 0, j = 0; i < n; i++) {
            if (j == m || attrs[i].name == newTableAttrs[j].name) {
                nr[j] = r[i] ? r[i]->copy() : r[i];
                j++;
            }
        }
        table->insert(nr);
    }
    return table;
}

ostream& Table::show(ostream & out) const {
    out << "Field\tType\tNull\tKey\tDefault\tExtra" << std::endl;
    // 括号内的数字是显示宽度(int)/长度(char)/数字位数(double)
    // P.S. 我刚知道CHAR是定长度字符串？而且默认长度是1？？
    for (auto i: attrs) {
        out << i.name << "\t";
        if (i.type == ATTR_CHAR) 
            out << "char(1)";
        else if (i.type == ATTR_INT) 
            out << "int(11)";
        else if (i.type == ATTR_DOUBLE) 
            out << "double(15)";
        else { /* undefined */}
        out << "\t";
        out << (i.notNull ? "NO" : "YES") << "\t" << (primary == i.name ? "PRI" : "") << "\t";
        out << "NULL\t" << std::endl;
    }
}

// 以下代码仅供单元测试使用
#include <iostream>
using std::cout;
using std::endl;
void Table::test_print() {
    int n = attrs.size();
    for (int i = 0; i < n; i++) cout << attrs[i].name << "\t";
    cout << endl;
    for (auto & j: data) {
        for (int i = 0; i < n; i++) {
            if (j[i]) cout << (*j[i]);
            cout << "\t";
        }
        cout << endl;
    }
}