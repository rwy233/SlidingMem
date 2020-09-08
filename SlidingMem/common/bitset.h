//
// Created by z yd on 2020-08-09.
//

#ifndef SLIDINGMEM_BITSET_H
#define SLIDINGMEM_BITSET_H

#include <stdint.h>
#include <string.h>

#define BITSIZE (1.0/8.0)

class BitSet{
public:
    BitSet(uint32_t _LENGTH):LENGTH(_LENGTH){
        uint32_t size = (LENGTH + 7) >> 3;
        bitset = new uint8_t[size];
        memset(bitset, 0, size * sizeof(uint8_t));
    }
    ~BitSet(){
        delete [] bitset;
    }

    inline void Set(uint32_t index){
        Set(index >> 3, index & 0x7);
    }

    inline void Set(uint32_t position, uint32_t offset){
        bitset[position] |= (1 << offset);
    }

    inline void Clear(uint32_t index){
        Clear(index >> 3, index & 0x7);
    }

    inline void Clear(uint32_t position, uint32_t offset){
        bitset[position] &= (~(1 << offset));
    }

    inline bool Get(uint32_t index){
        return Get(index >> 3, index & 0x7);
    }

    inline bool Get(uint32_t position, uint32_t offset){
        return ((bitset[position] & (1 << offset)) != 0);
    }

    inline bool SetByte(uint32_t position, uint32_t match){
        bool ret = !(bitset[position] & match);
        bitset[position] |= match;
        return ret;
    }

    inline bool SetInt(uint32_t position, uint32_t match){
        uint32_t* p = (uint32_t*)&bitset[position >> 2];
        bool ret = !((*p) & match);
        (*p) |= match;
        return ret;
    }

    inline void OrInt(uint32_t position, uint32_t match){
        *((uint32_t*)&bitset[position >> 2]) |= match;
    }

    inline bool SetNGet(uint32_t index){
        return SetNGet(index >> 3, index & 0x7);
    }

    inline bool SetNGet(uint32_t position, uint32_t offset){
        bool ret = (bitset[position] & (1 << offset));
        bitset[position] |= (1 << offset);
        return ret;
    }

    inline void Clear(){
        uint32_t size = (LENGTH + 7) >> 3;
        memset(bitset, 0, size * sizeof(uint8_t));
    }

private:

    const uint32_t LENGTH;
    uint8_t* bitset;
};

#endif //SLIDINGMEM_BITSET_H
