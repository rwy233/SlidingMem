//
// Created by z yd on 2020-08-09.
//

#ifndef SLIDINGMEM_BF_H
#define SLIDINGMEM_BF_H

#include "bitset.h"

template<typename DATA_TYPE>
class BF{
public:
    BF(uint64_t _LENGTH, uint32_t _HASH_NUM):
        LENGTH(_LENGTH), HASH_NUM(_HASH_NUM){
        bitset = new BitSet(LENGTH);
    }

    ~BF(){
        delete bitset;
    }

    void Insert(const DATA_TYPE item){
        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t position = hash(item, i) % LENGTH;
            bitset->Set(position);
        }
    }

    bool Query(const DATA_TYPE item){
        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t position = hash(item, i) % LENGTH;
            if(!bitset->Get(position))
                return false;
        }
        return true;
    }

    inline void Clear(){
        bitset->Clear();
    }

private:
    uint64_t LENGTH;
    uint64_t HASH_NUM;

    BitSet* bitset;
};


#endif //SLIDINGMEM_BF_H
