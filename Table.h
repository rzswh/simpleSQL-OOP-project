#pragma once
#include <string>
#include <vector>
#include <ostream>
#include "Attribute.h"
#include "Value.h"
#include "Record.h"
#include "WhereClause.h"
#include "PrintableTable.h"
#include "Expression.h"

using std::string;
using std::vector;
using std::ostream;

/**
 * @encodings=utf-8
 * class Table: 存储每张特定表的数据存储器. 
 */ 
class Table {
private:
    vector<Record> data;
    bool checkType(AttributeType att, ValueBase * v);
    string errMsg;
    // 根据一条记录的数据，对表达式进行求值
    Record eval(const Record & r, vector<Expression*> exps);
    // 分治的思路将记录进行分组，分别进行求值之后返回
    vector<Record> groupIntoTable(vector<Record *> tot, vector<AttributeExpression> agg, const vector<Expression *>& exps);

    // 找到一个属性的下标。失败时返回属性个数。
    static int findAttributeIndex(vector<Attribute> attrs, string name);
public:
    const string name;
    const vector<Attribute> attrs;
    const string primary;
    /**
     * 构造函数。 
     * 调用者应当避免不合法的参数，例如属性名冲突，主键名不存在于属性列表中，等等。
     * 主键强制非空，无论传入的参数如何。
     * @param name: 表名. attrs: 属性列表. primary: 主键名.
     * */
    Table(string name, vector<Attribute> a, string primary);
    Table(const Table &) = delete;
    Table(Table &&);
    /**
     * insert
     * 指定属性名attrNames和值values，插入新行。
     * 值向量的顺序应当和参数attrNames的顺序匹配，但是后者不必和建表时指定属性名的顺序匹配。
     * 返回是否成功。如果表中某一列不能为空但对应数据缺失，或者指定值类型与表格中属性类型不匹配，或者
     *   指定的主键和已有的键发生冲突，或者重复指定了同一属性的值，就会发生失败。
     * 在赋值时，传入的值会被复制而不是移动。即，传入值的指针不会被本Table对象接管。调用者需要在
     *   插入操作完成后处理传入值的内存回收。
     * 空值NULL以nullptr的形式传进来。
     * */
    bool insert(vector<string> attrNames, vector<ValueBase *> vals);
    /**
     * del
     * 删除所有满足whereClause的行。
     * 与本表无关的属性将会被忽略。如果whereClause中没有不被忽略的属性，表中所有的行都将被删除。
     * 返回值总是true（成功）【除非需求有改变】
     * */
    bool del(WhereClause c);
    /**
     * update
     * 更新满足whereClause的行，将attrNames中的属性值改为vals中的对应元素。
     * 与本表无关的属性将会被忽略，无论实在attrNames中还是whereClause中。
     * 如果没有不被忽略的属性whereClause，则表中所有的行都将被更新。
     * 返回是否成功。如果指定的vals类型无法匹配，或者不能为空的列被修改为空，或主键被修改为了和已有
     *   重复的值，会发生失败。
     * 在赋值时，传入的值会被复制而不是移动。即，传入值的指针不会被本Table对象接管。
     *   调用者需要在插入操作完成后处理传入值的内存回收。
     * */
    bool update(vector<string> attrNames, vector<ValueBase *> vals, WhereClause c);
    /**
     * select 朴素版本
     * 选出满足whereClause的行，取出包含在attrFilter中的属性，组成一张新表并返回。
     * @attrFilter 需要选出的属性名称。如果欲选出所有属性，请传入一个仅包含字符串“*”的向量。
     * */
    PrintableTable * select(vector<string> attrFilter, WhereClause c);
    /**
     * select 表内分组排序版本
     * 选出满足whereClause的行，按照group_by分组、order_by排序，对exps中表达式进行求值，得到各列，组成一张新表并返回。
     * 传入的表达式对象Expression不会被删除，需要调用者手动进行内存回收。
     * @attrFilter 需要选出的属性名称。如果欲选出所有属性，请传入一个仅包含字符串“*”的向量。
     * @group_by 需要特别分组的属性。按照其中属性进行分组，所有表达式只作用于同一组上。举例：
     *  SELECT stu_name, COUNT(*) from oop_info GROUP BY stu_name; 选出同名学生名称和人数。
     *  不过考虑到exps中会有属性表达式，最终分组结果由exps、group_by、order_by中的属性表达式共同决定。
     *  只有函数AVG，SUM，MAX，MIN，COUNT需要这一特性。
     * @order_by 按照某一字段进行排序。这一字段不必出现在表达式exps里；但这一字段会参与分组
     * */
    PrintableTable * select(vector<Expression *> exps, WhereClause c, vector<AttributeExpression> group_by, AttributeExpression* order_by);
    //
    virtual ~Table();
    ostream & show(ostream & out) const;
    // 以下供测试
    // friend class TestTable;
    // void test_print();
    string getErrorMsg() { return errMsg; }

};