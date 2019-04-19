#pragma once

class ValueBase {
public:
    virtual bool operator==(const ValueBase & v) const;
    virtual ~ValueBase() = 0;
};

template <class T>
class Value : public ValueBase{
    T v;
public: 
    Value(T v):v(v) {}
    operator T() { return v; }
    bool operator==(const ValueBase & v) const override;
    ~Value();
};