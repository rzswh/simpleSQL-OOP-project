// encodings=UTF-8
#include "Table.h"
#include <algorithm>
#include <cassert>
#include<iostream>
using std::cout;
using std::endl;
using std::unique;

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
                        t[j] = convertT<IntValue>(vals[i]);
                        if (vals[i] && !t[j]) succ = false;
                        break;
                    }
                    case ATTR_DOUBLE:
                    {
                        t[j] = convertT<DoubleValue>(vals[i]);
                        if (vals[i] && !t[j]) succ = false;
                        break;
                    }
                    case ATTR_CHAR:
                    {
                        t[j] = convertT<CharValue>(vals[i]);
                        if (vals[i] && !t[j]) succ = false;
                        break;
                    }
					case ATTR_TIME:
					{
						t[j] = convertT<TimeValue>(vals[i]);
						if (vals[i] && !t[j]) succ = false;
						break;
					}	
					case ATTR_DATE:
					{
						t[j] = convertT<DateValue>(vals[i]);
						if (vals[i] && !t[j]) succ = false;
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
    // 把Record中空指针的地方赋值为Null
    for (int j = 0; j < attrs.size(); j++) {
        if (!t[j]) t[j] = new Null<ValueBase>();
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
        if (isNull(t[i]) && (attrs[i].notNull || primaryIndex == i)) {
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

bool Table::del(Expression * c) {
    // 逆序选择元素
    for (int i = data.size()-1; i>= 0; i-- ) {
        auto val = dynamic_cast<BoolValue*>(c->eval(data[i], attrs));
        if (val && val->isTrue()) {
            data.erase(data.begin() + i);
        }
    }
    return true;
}
bool Table::checkType(AttributeType att, ValueBase * v) {
    if (att == ATTR_CHAR && !dynamic_cast<Value<string>*> (v) )
        return false;
    if (att == ATTR_DOUBLE && !dynamic_cast<DoubleValue*> (v))
        return false;
    if (att == ATTR_INT && !dynamic_cast<IntValue*> (v) )
        return false;
	if (att == ATTR_TIME && !dynamic_cast<TimeValue*> (v))
		return false;
	if (att == ATTR_DATE && !dynamic_cast<DateValue*> (v))
		return false;
    return true;
}
//#define DEBUG
#ifdef DEBUG
#include <iostream>
#endif
bool Table::update(vector<string> attrNames, vector<ValueBase *> vals, Expression* c) {
    // 类型匹配，非空检查
    for (int j = 0; j < attrNames.size(); j++) {
        for (int k = 0; k < attrs.size(); k++) {
            if (attrNames[j] == attrs[k].name) {
                // 类型转换
                if (attrs[k].type == ATTR_DOUBLE ) {
                    auto nv = convertT<DoubleValue>(vals[j]);
                    delete vals[j];
                    vals[j] = nv ? nv : new Null<DoubleValue>();//DoubleValue::makeNull();
                }
                if (!checkType(attrs[k].type, vals[j])) {
                    errMsg = "Incompatible type.";
                    #ifdef DEBUG
                    vals[j]->print(std::cout);
                    #endif
                    return false;
                }
                if (attrs[k].notNull && isNull(vals[j])) {
                    errMsg = "Null where it should be not null.";
                    return false;
                }
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
        auto val = dynamic_cast<BoolValue*>(c->eval(data[i], attrs));
        if (!val || !val->isTrue()) {
            if (primaryValue) 
                primaryCount += *data[i][primaryIndex] == *primaryValue;
        } else {
            updateList.push_back(i);
            primaryCount ++; // 对于赋值型更新，每修改一个，重复次数多一次
        }
        if (primaryValue && primaryCount > 1) break;
    }
    if (primaryValue && primaryCount > 1) {
        errMsg = "Duplicated primary value.";
        return false;
    }
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
        if (primaryValue == nullptr) continue;
        Record r = data[i];
        using std::cout;
        using std::endl;
        // cout << "[debug]" << i << " " ;
        // cout << *data[i+1][primaryIndex] << " " << *r[primaryIndex] << " " << (*data[i+1][primaryIndex] < *r[primaryIndex]);
        // cout << endl;
        int j;
        for (j = i; j > 0 && *r[primaryIndex] < *data[j-1][primaryIndex]; j--) 
            data[j] = std::move(data[j-1]);
        data[j] = r;
        r = data[i];
        for (j = i; j + 1 < data.size() && *data[j+1][primaryIndex] < *r[primaryIndex]; j++) 
            data[j] = std::move(data[j+1]);
        data[j] = r;
    }
    return true;
}
/*
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
            if (j < m && attrs[i].name == newTableAttrs[j].name) {
                nr[j] = r[i] ? r[i]->copy() : r[i];
                j++;
            }
        }
        table->insert(nr);
    }
    return table;
}*/

PrintableTable * Table::select(vector<Expression *> exps, Expression * c, vector<Expression*> group_by, Expression* order_by) {
    // 先根据WhereClause过滤记录
    vector<Record> ret;
    for (auto& i: data) {
        auto res = c ? dynamic_cast<BoolValue *>(c->eval(i, attrs)) : nullptr;
        if (!c || res && res->isTrue())
            ret.push_back(i);
        delete res;
    }
    typedef AggregationFunctionExpression AFE;
    typedef AttributeExpression AE;
    // 判断有没有累积函数（需要GROUP BY特殊处理）
    bool aggregate = false; // 有无累积函数
    for (auto& i: exps) {
        if (dynamic_cast<AFE *>(i)) 
            aggregate = true;
        else if (dynamic_cast<AttributeExpression *>(i))
            group_by.push_back(i);
    }
    // 如果含有累积函数，则分组
    if (aggregate) {
        // 分组依据不需要去重，不用担心重复
        //sort(group_by.begin(), group_by.end(), [](Expression * a, Expression * b) { return a->toString() < b->toString(); });
        //unique(group_by.begin(), group_by.end(), [](Expression * a, Expression * b) { return a->toString() < b->toString(); });
        if (order_by) group_by.insert(group_by.begin(), order_by);
        // 根据分组依据分为若干组
        vector<Record *> tot;
        for (auto& i: ret) 
            tot.push_back(&i);
        // 分类后对每组分别求值
        auto gp = groupIntoTable(tot, group_by, exps);
        ret.swap(gp);
    } else {
        // 排序
        if (order_by) {
            sort(ret.begin(), ret.end(), [&](Record & a, Record & b) { 
                return *eval(a, order_by) < *eval(b, order_by);
            });
        }
        // 对每条分别求函数
        vector<Expression *> outputExps;
        for (auto& i: exps) {
            auto ae = dynamic_cast<AttributeExpression*>(i);
            // 如果遇到"*"，则无条件将所有属性加入进去，不管是否会重复
            if (ae && ae->toString() == "*") {
                //outputExps.clear();
                for (auto j: attrs) {
                    outputExps.push_back(new AttributeExpression(j.name));
                }
                //break;
            } else outputExps.push_back(i);
        }
        exps.swap(outputExps);
        // 对所有元素一一求值
        vector<Record> newrecords;
        for (auto& i: ret) {
            newrecords.push_back(eval(i, exps));
        }
        ret.swap(newrecords);
    }
    // 信息打包返回
    PrintableTable * table = new PrintableTable(exps); // 复制表头
    table->setData(ret);
    //for (auto i: exps) delete i;
    //delete order_by;
    return table;
}

Record Table::eval(const Record & r, vector<Expression*> exps) {
    int m = exps.size();
    Record ret(m);
    for (int i = 0; i < m; i++) {
        ret[i] = eval(r, exps[i]);
    }
    return ret;
}


ValueBase* Table::eval(const Record & r, Expression* exp) {
    return exp->eval(r, attrs);
    if (dynamic_cast<AttributeExpression *>(exp)) {
        int ind = Table::findAttributeIndex(attrs, dynamic_cast<AttributeExpression *>(exp)->toString());
        assert(ind < attrs.size());
        return r[ind]->copy();
    } else if (dynamic_cast<ConstExpression *>(exp)) {
        return dynamic_cast<ConstExpression *>(exp)->eval();
    } else {
        return exp->eval(r, attrs);
    }
}

vector<Record> Table::groupIntoTable(vector<Record *> tot, vector<Expression* > agg, const vector<Expression *>& exps, vector<ValueBase *>* sortMetric) {
    // 没有落到这一组的记录
    if (tot.size() == 0) return vector<Record>();
    // 用于分组的表达式已经全部处理过，则可以对exps一一进行求值
    if (agg.size() == 0) {
        // 以第一条记录为模板，对所有的表达式进行求值。
        // 按照期望，只有累积函数的求值结果并不是正确的，需要在全部表达式基础上进行求值
        Record ret = eval(*(tot[0]), exps);
        // 把累积函数找出来，在tot中所有记录的基础上进行求值
        for (int i = 0; i < exps.size(); i++) {
            auto e = exps[i];
            if (dynamic_cast<AggregationFunctionExpression*>(e)) {
                auto ag = dynamic_cast<AggregationFunctionExpression*>(e);
                delete ret[i];
                ret[i] = ag->evalAggregate(tot, attrs);
            }
        }
        // 如果参与排序的表达式是一个累积函数，那么现在可以求值了，就把本组的值压入到数组中
        if (sortMetric) {
            sortMetric->push_back(sortExpression->evalAggregate(tot, attrs));
        }
        return vector<Record> ({ret});
    }
    // agg中还有作为分组依据的表达式没有处理
    // 取出第一个
    Expression * exp = agg[0];
    agg.erase(agg.begin());
    vector<Record> ret;
    // 如果分组依据不是累积函数（本质是，这一表达式的值只需要一条记录就可以确定）
    // 从逻辑上讲，这一类函数不能参与真正的分组，而应该参与排序。应该只有最外面一层才能是累积函数。
    if (!dynamic_cast<AggregationFunctionExpression*>(exp)) {
        // 排序之后，方可去重、合并
        sort(tot.begin(), tot.end(), [&](Record * a, Record * b) { return *eval(*a, exp) < *eval(*b, exp); });
        for (auto i = tot.begin(); i != tot.end(); i++) {
            // 把tot记录中对于表达式exp同值的合并成为一组，放入newtot中
            auto last = i;
            vector<Record*> newtot;
            while ((i+1) != tot.end() && *eval(**(i+1), exp) == *eval(**i, exp)) i++;
            for (auto j = last; j != i+1; j++) newtot.push_back(*j);
            // 递归地分组
            vector<Record> results = groupIntoTable(newtot, agg, exps, sortMetric);
            // 将结果列表合并
            for (auto i: results) ret.push_back(i);
        }
    }
    // 分组依据是累积函数。根据设定，group by表达式中不能有累积函数，因此它只能是order by表达式
    // 根据接口约定，它必须是agg中唯一的累积函数，而且必须位于agg数组首位
    else {
        // 先对agg中其他表达式进行分组
        sortExpression = dynamic_cast<AggregationFunctionExpression*>(exp);
        vector<ValueBase *> order;
        vector<Record> results = groupIntoTable(tot, agg, exps, &order);
        // 以order为关键字，对results进行排序
        vector<int> rank; 
        for (int i = 0; i < order.size(); i++) rank.push_back(i);
        sort(rank.begin(), rank.end(), [&](int i, int j)->bool { return *order[i] < *order[j]; });
        for (int i: rank) ret.push_back(results[i]);
    }
    return ret;
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
            out << "double";
		else if (i.type == ATTR_TIME)
			out << "time";
        else { /* undefined */}
        out << "\t";
        out << (i.notNull ? "NO" : "YES") << "\t" << (primary == i.name ? "PRI" : "") << "\t";
        out << "NULL\t" << std::endl;
    }
	return out;
}

int Table::findAttributeIndex(vector<Attribute> attrs, string name) {
    for (int i = 0; i < attrs.size(); i++) {
        if (attrs[i].name == name) return i;
    }
    std::cerr << name << std::endl;
    return attrs.size();
}

// 以下代码仅供单元测试使用
/*
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
}*/