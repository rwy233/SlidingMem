//
// Created by z yd on 2020-08-09.
//

#ifndef SLIDINGMEM_SWBF_H
#define SLIDINGMEM_SWBF_H

#include "common/Abstract.h"
#include "common/BF.h"

template<typename DATA_TYPE>
class SWBF : public Abstract<DATA_TYPE>{
public:

    SWBF(uint64_t memory, uint64_t _LEVEL, uint64_t _CYCLE):
            LEVEL(_LEVEL), CYCLE(_CYCLE){
        BLOCK_NUM = new uint64_t [LEVEL];
        BLOCK_SIZE = new uint64_t [LEVEL];
        BF_SIZE = new uint64_t [LEVEL];

        start = new uint64_t* [LEVEL];
        bloom = new BF<DATA_TYPE>** [LEVEL];

        for(uint32_t i = 0;i < LEVEL;++i){
            BLOCK_NUM[i] = (1 << (i + 3));
            BLOCK_SIZE[i] = CYCLE / BLOCK_NUM[i];

            bloom[i] = new BF<DATA_TYPE>* [BLOCK_NUM[i] + 1];
            start[i] = new uint64_t [BLOCK_NUM[i] + 1];

            for (uint32_t j = 0;j < BLOCK_NUM[i] + 1;++j){
                start[i][j] = 0;
                bloom[i][j] = new BF<DATA_TYPE>
                        (memory / BITSIZE / LEVEL / (BLOCK_NUM[i] + 1) - 512, 3);
            }
        }
    }

    ~SWBF(){
        for(uint32_t i = 0;i < LEVEL;++i){
            for (uint32_t j = 0;j < BLOCK_NUM[i] + 1;++j){
                delete bloom[i][j];
            }
            delete [] bloom[i];
            delete [] start[i];
        }

        delete [] bloom;
        delete [] start;
        delete [] BF_SIZE;
        delete [] BLOCK_SIZE;
        delete [] BLOCK_NUM;
    }

    void Insert(const DATA_TYPE item, uint64_t time){
        for(uint32_t i = 0;i < LEVEL;++i){
            uint32_t pos = (time / BLOCK_SIZE[i]) % (BLOCK_NUM[i] + 1);
            if(start[i][pos] < time - CYCLE){
                start[i][pos] = time;
                bloom[i][pos]->Clear();
            }
            bloom[i][pos]->Insert(item);
        }
    }

    bool Query(const DATA_TYPE item, uint64_t time){
        for(uint32_t i = 0;i < LEVEL;++i){
            bool ret = false;
            for (uint32_t j = 0;j < BLOCK_NUM[i] + 1;++j){
                if(start[i][j] < time - CYCLE){
                    start[i][j] = time;
                    bloom[i][j]->Clear();
                }
                if(bloom[i][j]->Query(item)){
                    ret = true;
                    break;
                }
            }
            if(!ret)
                return false;
        }
        return true;
    }

private:
    uint64_t CYCLE;
    uint64_t LEVEL;

    uint64_t* BLOCK_NUM;
    uint64_t* BLOCK_SIZE;
    uint64_t* BF_SIZE;

    BF<DATA_TYPE>*** bloom;
    uint64_t** start;
};

#endif //SLIDINGMEM_SWBF_H
