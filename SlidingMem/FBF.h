//
// Created by z yd on 2020-08-09.
//

#ifndef SLIDINGMEM_FBF_H
#define SLIDINGMEM_FBF_H

#include "common/Abstract.h"
#include "common/BF.h"

template<typename DATA_TYPE>
class FBF : public Abstract<DATA_TYPE>{
public:

    FBF(uint64_t memory, uint64_t LENGTH, uint64_t HASH_NUM, uint64_t _CYCLE):
        CYCLE(_CYCLE){
        BLOOM_NUM = memory / BITSIZE / LENGTH;
        Last_Time = future = 0;
        bloom = new BF<DATA_TYPE>*[BLOOM_NUM];
        for(uint32_t i = 0;i < BLOOM_NUM;++i){
            bloom[i] = new BF<DATA_TYPE>(LENGTH, HASH_NUM);
        }
    }

    ~FBF(){
        for(uint32_t i = 0;i < BLOOM_NUM;++i){
            delete bloom[i];
        }
        delete [] bloom;
    }

    void Insert(const DATA_TYPE item, uint64_t time){
        if(!Query(item, time)){
            bloom[future]->Insert(item);
            bloom[(future + 1) % BLOOM_NUM]->Insert(item);
        }
    }

    bool Query(const DATA_TYPE item, uint64_t time){
        Update(time * (BLOOM_NUM - 1) / CYCLE);
        if(bloom[future]->Query(item))
            return true;
        for(int i = (future + 1) % BLOOM_NUM;(i + 1) % BLOOM_NUM != future;i = (i + 1) % BLOOM_NUM){
            if(bloom[i]->Query(item) && bloom[(i + 1) % BLOOM_NUM]->Query(item))
                return true;
        }
        return bloom[(future + BLOOM_NUM - 1) % BLOOM_NUM]->Query(item);
    }

private:
    uint64_t BLOOM_NUM;
    uint64_t CYCLE;

    uint64_t Last_Time;
    uint64_t future;

    BF<DATA_TYPE>** bloom;

    void Update(uint64_t time){
        for(;Last_Time < time;++Last_Time){
            future = (future + BLOOM_NUM - 1) % BLOOM_NUM;
            bloom[future]->Clear();
        }
    }
};

#endif //SLIDINGMEM_FBF_H
