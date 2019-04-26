#include "WhereClause.h"

WhereClause * buildFrom(string str) {
    // To be implemented
    return nullptr;
}

// TODO: Take NULL into consideration
bool WhereClause::test(const Record & r, const vector<Attribute> & va) const {
    vector<bool> res;
    // convert sentence to bool value
    auto check = [](SubSentence s, const ValueBase * v) -> bool {
        if (std::get<1>(s) == ARITH_GREATER) {
            return *v > *std::get<2>(s);
        }
        else if (std::get<1>(s) == ARITH_LESS) {
            return *v < *std::get<2>(s);
        }
        else if (std::get<1>(s) == ARITH_EQUAL) {
            return *v == *std::get<2>(s);
        }
        else {/* undefined */}
        return false;
    };
    for (auto &i: subsentences) {
        bool flag = true;
        for (int j = 0; j < va.size(); j++) {
            if (va[j].name == std::get<0>(i)) {
                flag = flag && check(i, r[j]);
                if (!flag) return false;
                else break;
            }
        }
        res.push_back(flag);
    }
    // 用逆波兰表达式计算判断结果
    vector<bool> stack;
    int i = 0, j = 0, n = subsentences.size(), m = operation.size();
    for (; j < m; ) {
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
    return stack[0];
}