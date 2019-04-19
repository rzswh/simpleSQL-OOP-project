#include "Value.h"

ValueBase::~ValueBase() {}

template<class T>
bool Value<T>::operator==(const ValueBase & v) const {
    ValueBase * ptr = &v;
    Value<T> * nptr = dynamic_cast<Value<T> *>(ptr);
    return nptr && (*this) == *ptr;
}