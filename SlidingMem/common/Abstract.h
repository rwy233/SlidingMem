#ifndef SLIDINGMEM_ABSTRACT_H
#define SLIDINGMEM_ABSTRACT_H

#include "hash.h"

template<typename DATA_TYPE>
class Abstract{
public:
    Abstract(){}
    virtual ~Abstract(){};

    virtual void Insert(const DATA_TYPE item, uint64_t time) = 0;
    virtual bool Query(const DATA_TYPE item, uint64_t time) = 0;
};

#endif //SLIDINGMEM_ABSTRACT_H
