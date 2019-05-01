#pragma once
#include "Value.h"

/**
 * 每一行的数据组成一个Record.
 * 内存管理提示：Record析构时会将其内部的所有数据进行析构。请谨慎将Record声明为局部变量。
 */ 
class Record {
    int n;
    ValueBase ** data;
public:
    Record(int n):n(n) {
        data = new ValueBase *[n];
        for (int i = 0; i < n; i++) data[i] = nullptr;
    }
    Record(const Record & r) {
        data = new ValueBase *[n = r.n];
        for (int i = 0; i < n; i++) {
            data[i] = r.data[i] ? r.data[i]->copy() : r.data[i];
        }
    }
    Record(Record && r) {
        n = r.n;
        data = r.data;
        r.data = nullptr;
    }
    Record & operator=(const Record & r) {
        if (&r != this) {
            if (data) {
                for (int i = 0; i < n; i++) {
                    delete data[i];
                }
                delete [] data;
            }
            data = new ValueBase *[n = r.n];
            for (int i = 0; i < n; i++) {
                data[i] = r.data[i] ? r.data[i]->copy() : r.data[i];
            }
        }
        return *this;
    }
    ValueBase *& operator[](int i) { return data[i]; }
    ValueBase * operator[](int i) const { return data[i]; }
    ~Record() {
        if (data) {
            for (int i = 0; i < n; i++) {
                delete data[i];
            }
            delete [] data;
        }
    }
};