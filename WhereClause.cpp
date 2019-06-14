// encodings=UTF-8
#include "WhereClause.h"
#include <stack>

bool WhereClause::test(const Record & r, const vector<Attribute> & va) const {
    vector<BoolValue> res;
    // 用lambda函数简化代码
    auto check = [](SubSentence s, const ValueBase * v, bool & flag, AttributeType tp) -> BoolValue {
        ValueBase * sv = nullptr;
        ValueBase * nv = const_cast<ValueBase *>(v);
        if (tp == ATTR_CHAR) {
            sv = convertT<CharValue>(std::get<2>(s));
            v = convertT<CharValue>(const_cast<ValueBase*>(v));
        } else if (tp == ATTR_DOUBLE || tp == ATTR_INT) {
            sv = convertT<DoubleValue>(std::get<2>(s));
            v = convertT<DoubleValue>(const_cast<ValueBase*>(v));
        }
        if (std::get<1>(s) == ARITH_GREATER) {
            return v && (*v > *sv).isTrue();
        }
        else if (std::get<1>(s) == ARITH_LESS) {
            return v && (*v < *sv).isTrue();
        }
        else if (std::get<1>(s) == ARITH_EQUAL) {
            return v && (*v == *sv).isTrue();
        }
        else {/* undefined */}
        flag = false;
        return false;
    };
    // 将每个子句转化为对应的判断结果
    for (auto &i: subsentences) {
        bool flag = true;
        BoolValue result;
        for (int j = 0; j < va.size(); j++) {
            if (va[j].name == std::get<0>(i)) {
                result = check(i, r[j], flag, va[j].type);
                if (!flag) return false;
                break;
            }
        }
        res.push_back(result);
    }
    if (res.size() == 0) 
        return true;
    // 用逆波兰表达式对逻辑表达式求值，计算判断结果
    vector<BoolValue> stack;
    stack.push_back(res[0]);
    int i = 1, j = 0, n = subsentences.size(), m = operation.size();
    for (; j < m; ) {
        // 将优先级高（rank较小）的一方（子句或运算符）入栈
        if (i < n && std::get<3>(subsentences[i]) < operation[j].second) {
            stack.push_back(res[i]);
            i ++;
        } else {
            LogicOperation lo = operation[j].first;
            bool v1 = stack.back(); stack.pop_back();
            bool v2 = stack.back(); stack.pop_back();
            if (lo == LOGIC_AND) {
                stack.push_back(v1 && v2);
            } else if (lo == LOGIC_OR) {
                stack.push_back(v1 || v2);
            } else { /* undefined*/ }
            j ++;
        }
    }
    return bool(stack[0]) && !isNull(stack[0]);
}