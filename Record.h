#pragma once
#include "Value.h"

class Record {
    int n;
    ValueBase ** data;
public:
    Record(int n):n(n) {
        data = new ValueBase *[n];
    }
    Record(Record && r) {
        n = r.n;
        data = r.data;
        r.data = nullptr;
    }
    ValueBase *& operator[](int i) { return data[i]; };
    ~Record() {
        delete [] data;
    }
};