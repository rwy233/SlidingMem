//
// Created by z yd on 2020-08-09.
//

#ifndef SLIDINGMEM_SLIDINGBLOOM_H
#define SLIDINGMEM_SLIDINGBLOOM_H

#include "common/Abstract.h"
#include "common/bitset.h"

template<typename DATA_TYPE>
class SlidingBloom : public Abstract<DATA_TYPE>{
public:

    SlidingBloom(uint64_t memory, uint64_t _HASH_NUM, uint64_t _CYCLE):
        HASH_NUM(_HASH_NUM), CYCLE(_CYCLE){
        Clock_Pos = Last_Time = 0;
        LENGTH = memory / BITSIZE / 2;
        STAGE_LEN = LENGTH / HASH_NUM;
        LENGTH = STAGE_LEN * HASH_NUM;

        now = new BitSet(LENGTH);
        future = new BitSet(LENGTH);
    }

    ~SlidingBloom(){
        delete future;
        delete now;
    }

    void Insert(const DATA_TYPE item, uint64_t time){
        Update(time * LENGTH / CYCLE);
        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t position = hash(item, i) % STAGE_LEN + i * STAGE_LEN;
            future->Set(position);
        }
    }

    bool Query(const DATA_TYPE item, uint64_t time){
        Update(time * LENGTH / CYCLE);
        for(uint32_t i = 0;i < HASH_NUM;++i){
            uint32_t position = hash(item, i) % STAGE_LEN + i * STAGE_LEN;
            if(!future->Get(position) && !now->Get(position))
                return false;
        }
        return true;
    }

/*
    bool Query(const DATA_TYPE item, uint64_t time){
        Update(time * LENGTH / CYCLE);
    	
	int size = HASH_NUM * 2;
        unsigned int* result = new unsigned int [size];

	int k = Clock_Pos / STAGE_LEN;
    	unsigned int position = hash(item, k) % STAGE_LEN + k * STAGE_LEN;
   	if(position >= Clock_Pos){
      	    k = (k + HASH_NUM - 1) % HASH_NUM ;
    	}

    	for(int i = 0;i < HASH_NUM;i += 1){
            int ink = (k + HASH_NUM - i) % HASH_NUM;
            position = hash(item, ink) % STAGE_LEN + ink * STAGE_LEN;

            result[i] = (int)future->Get(position);
            result[HASH_NUM + i] = result[i] + (int)now->Get(position);
    	}

        for(int i = 0;i < size;++i){
            int min = result[i];
            for(int j = 1;j < HASH_NUM;++j){
                int pos = i + j;
                if(pos >= size)
                    break;
                if(result[pos] < min){
                    min = result[pos];
                }
            }
            if(min != result[i]){
                int pos = HASH_NUM + i;
                if(pos < size)
                    result[HASH_NUM + i] -= (result[i] - min);
                result[i] = min;
            }
        }

    	int ret = result[HASH_NUM];
    	for(int i = 1;i < HASH_NUM;++i){
            int pos = HASH_NUM + i;
            if(result[pos] < ret)
                ret = result[pos];
        }

        delete [] result;
        return ret;

    }
*/
private:

    uint64_t STAGE_LEN;
    uint64_t CYCLE;
    uint64_t LENGTH;
    uint64_t HASH_NUM;

    uint64_t Clock_Pos;
    uint64_t Last_Time;

    BitSet* future;
    BitSet* now;

    void Update(uint64_t time){
        for(;Last_Time < time;++Last_Time){
            now->Clear(Clock_Pos);
            if(future->Get(Clock_Pos)){
                now->Set(Clock_Pos);
                future->Clear(Clock_Pos);
            }
            Clock_Pos = (Clock_Pos + 1) % LENGTH;
        }
    }
};

#endif //SLIDINGMEM_SLIDINGBLOOM_H
