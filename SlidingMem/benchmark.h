//
// Created by z yd on 2020-08-09.
//

#ifndef SLIDINGMEM_BENCHMARK_H
#define SLIDINGMEM_BENCHMARK_H

#include <sys/stat.h>

#include <vector>
#include <chrono>
#include <algorithm>

#include <unordered_map>

#include "FBF.h"
#include "SWBF.h"
#include "SlidingBloom.h"
#include "SlidingBloomOpt.h"

#include "common/MMap.h"

template<typename DATA_TYPE>
class TupleBench{
public:

    typedef std::vector<Abstract<DATA_TYPE>*> AbsVector;
    typedef std::unordered_map<DATA_TYPE, int32_t> HashMap;

    TupleBench(const char* PATH){
        result = Load(PATH);

        start = (DATA_TYPE*)result.start;
        SIZE = result.length / sizeof(DATA_TYPE);
    }

    ~TupleBench(){
        UnLoad(result);
    }

    void Error(uint64_t TOTAL, uint64_t CYCLE){
        AbsVector FPIs = {
        	new SlidingBloom<DATA_TYPE>(600000, 15, CYCLE),
                new SlidingBloom<DATA_TYPE>(700000, 15, CYCLE),
                new SlidingBloom<DATA_TYPE>(800000, 15, CYCLE),
                new SlidingBloom<DATA_TYPE>(900000, 15, CYCLE),
		new SlidingBloom<DATA_TYPE>(1000000, 15, CYCLE),

		new SlidingBloomOpt<DATA_TYPE>(600000, 15, CYCLE),
                new SlidingBloomOpt<DATA_TYPE>(700000, 15, CYCLE),
                new SlidingBloomOpt<DATA_TYPE>(800000, 15, CYCLE),
                new SlidingBloomOpt<DATA_TYPE>(900000, 15, CYCLE),
                new SlidingBloomOpt<DATA_TYPE>(1000000, 15, CYCLE),
/*
                new FBF<DATA_TYPE>(2000000, 1600000, 8, CYCLE),
                new FBF<DATA_TYPE>(3000000, 2400000, 8, CYCLE),
                new FBF<DATA_TYPE>(4000000, 3200000, 8, CYCLE),
                new FBF<DATA_TYPE>(5000000, 4000000, 8, CYCLE),

                new SWBF<DATA_TYPE>(2000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(3000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(4000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(5000000, 2, CYCLE),
*/	};

        BenchInsert(FPIs, TOTAL, CYCLE);
    }

    void Thp(uint64_t TOTAL, uint64_t CYCLE){
        for(uint32_t i = 0;i < 3;++i){
            std::cout << i << std::endl;

            AbsVector FPIs = {
	    	new SlidingBloom<DATA_TYPE>(2000000, 15, CYCLE),
                new SlidingBloom<DATA_TYPE>(3000000, 15, CYCLE),
                new SlidingBloom<DATA_TYPE>(4000000, 15, CYCLE),
                new SlidingBloom<DATA_TYPE>(5000000, 15, CYCLE),

                new FBF<DATA_TYPE>(2000000, 1600000, 8, CYCLE),
                new FBF<DATA_TYPE>(3000000, 2400000, 8, CYCLE),
                new FBF<DATA_TYPE>(4000000, 3200000, 8, CYCLE),
                new FBF<DATA_TYPE>(5000000, 4000000, 8, CYCLE),

                new SWBF<DATA_TYPE>(2000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(3000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(4000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(5000000, 2, CYCLE),
	    };

            for(auto FPI : FPIs){
                InsertThp(FPI, TOTAL);
                delete FPI;
            }
        }
    }

private:
    LoadResult result;
    DATA_TYPE* start;

    uint64_t SIZE;

    typedef std::chrono::high_resolution_clock::time_point TP;

    inline TP now(){
        return std::chrono::high_resolution_clock::now();
    }

    inline double durationms(TP finish, TP start){
        return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
    }

    void BenchInsert(AbsVector sketches, uint64_t TOTAL, uint64_t CYCLE){
        HashMap B;
        uint32_t size = sketches.size();

        double* FNR = new double [size];
        double* FPR = new double [size];

        memset(FNR, 0, size * sizeof(double));
        memset(FPR, 0, size * sizeof(double));

        double out_num = 0;

        for(uint32_t i = 1;i <= TOTAL;++i){
            if(i >= CYCLE){
                B[start[i - CYCLE]] -= 1;
            }
            B[start[i]] += 1;

            for(auto sketch : sketches){
                sketch->Insert(start[i], i);
            }

            if((i%(CYCLE/10)== 0) && (i>=2*CYCLE)){
                out_num += 1;

                double in = 0, out = 0;

                double* in_err = new double [size];
                double* out_err = new double [size];

                memset(in_err, 0, size * sizeof(double));
                memset(out_err, 0, size * sizeof(double));

                for(auto it = B.begin();it != B.end();it++){
                    if(it->second <= 0){
                        out += 1;
                        for(uint32_t j = 0;j < size;++j){
                            out_err[j] += sketches[j]->Query(it->first, i);
                        }
                    }
                    else{
                        in += 1;
                        for(uint32_t j = 0;j < size;++j){
                            in_err[j] += (!sketches[j]->Query(it->first, i));
                        }
                    }
                }

                for(uint32_t j = 0;j < size;++j){
                    FNR[j] += in_err[j] / in;
                    FPR[j] += out_err[j] / out;
                }

                delete [] in_err;
                delete [] out_err;
            }
        }

        for(uint32_t j = 0;j < size;++j){
            std::cout << "FNR: " << FNR[j] / out_num << std::endl;
            std::cout << "FPR: " << FPR[j] / out_num << std::endl;
        }

        delete [] FNR;
        delete [] FPR;
    }

    void InsertThp(Abstract<DATA_TYPE>* sketch, uint64_t TOTAL){
        TP initial, finish;

        initial = now();
        for(uint32_t i = 1;i <= TOTAL;++i){
            sketch->Insert(start[i], i);
        }
        finish = now();

        std::cout << "Throughput: " << TOTAL / durationms(finish, initial) << std::endl;
    }
};

template<typename DATA_TYPE>
class TimeBench{
public:

    typedef std::vector<Abstract<DATA_TYPE>*> AbsVector;
    typedef std::unordered_map<DATA_TYPE, int32_t> HashMap;

    struct Tuple{
        DATA_TYPE item;
        DATA_TYPE time;

	bool operator < (const Tuple& other) const{
	    return (time < other.time || (time == other.time && item < other.item));
	}
    };

    TimeBench(const char* PATH){
        result = Load(PATH);

        start = (Tuple*)(result.start);
        SIZE = result.length / sizeof(Tuple);

	std::sort(start, start + SIZE);
	std::cout << (start[SIZE - 1].time - start[0].time) / (double)SIZE;
    }

    ~TimeBench(){
        UnLoad(result);
    }

    void Error(uint64_t TOTAL, uint64_t CYCLE){
	CYCLE = (start[SIZE - 1].time - start[0].time) / (double)SIZE * CYCLE;

        AbsVector FPIs = {

       		new SlidingBloom<DATA_TYPE>(10000000, 18, CYCLE),
                new SlidingBloom<DATA_TYPE>(15000000, 18, CYCLE),
                new SlidingBloom<DATA_TYPE>(20000000, 18, CYCLE),
                new SlidingBloom<DATA_TYPE>(25000000, 18, CYCLE),

                new FBF<DATA_TYPE>(10000000, 8000000, 8, CYCLE),
                new FBF<DATA_TYPE>(15000000, 12000000, 8, CYCLE),
                new FBF<DATA_TYPE>(20000000, 16000000, 8, CYCLE),
                new FBF<DATA_TYPE>(25000000, 20000000, 8, CYCLE),

                new SWBF<DATA_TYPE>(10000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(15000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(20000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(25000000, 2, CYCLE), 
	};

        BenchInsert(FPIs, TOTAL, CYCLE);
    }

    void Thp(uint64_t TOTAL, uint64_t CYCLE){
	CYCLE = (start[SIZE - 1].time - start[0].time) / (double)SIZE * CYCLE;

        for(uint32_t i = 0;i < 3;++i){
            std::cout << i << std::endl;

            AbsVector FPIs = {
	    	new SlidingBloom<DATA_TYPE>(10000000, 18, CYCLE),
                new SlidingBloom<DATA_TYPE>(15000000, 18, CYCLE),
                new SlidingBloom<DATA_TYPE>(20000000, 18, CYCLE),
                new SlidingBloom<DATA_TYPE>(25000000, 18, CYCLE),

                new FBF<DATA_TYPE>(10000000, 8000000, 8, CYCLE),
                new FBF<DATA_TYPE>(15000000, 12000000, 8, CYCLE),
                new FBF<DATA_TYPE>(20000000, 16000000, 8, CYCLE),
                new FBF<DATA_TYPE>(25000000, 20000000, 8, CYCLE),

                new SWBF<DATA_TYPE>(10000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(15000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(20000000, 2, CYCLE),
                new SWBF<DATA_TYPE>(25000000, 2, CYCLE),
	    };

            for(auto FPI : FPIs){
                InsertThp(FPI, TOTAL);
                delete FPI;
            }
        }
    }

private:
    LoadResult result;
    Tuple* start;

    uint64_t SIZE;

    typedef std::chrono::high_resolution_clock::time_point TP;

    inline TP now(){
        return std::chrono::high_resolution_clock::now();
    }

    inline double durationms(TP finish, TP start){
        return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
    }

    void BenchInsert(AbsVector sketches, uint64_t TOTAL, uint64_t CYCLE){
        HashMap B;
        uint32_t size = sketches.size(), pre = 0, i = 0;

        double* FNR = new double [size];
        double* FPR = new double [size];

        memset(FNR, 0, size * sizeof(double));
        memset(FPR, 0, size * sizeof(double));

        double out_num = 0;

	TOTAL = (start[SIZE - 1].time - start[0].time) / (double)SIZE * TOTAL;

        while(start[i + 1].time - start[0].time < TOTAL){
	    i += 1;
            while(start[i].time - start[pre].time >= CYCLE){
                B[start[pre].item] -= 1;
                pre += 1;
            }
            B[start[i].item] += 1;

            for(auto sketch : sketches){
                sketch->Insert(start[i].item, start[i].time - start[0].time);
            }

            if(((start[i].time - start[0].time) / (CYCLE/10) > 
		(start[i-1].time - start[0].time) / (CYCLE/10)) &&
                (start[i].time - start[0].time >= 2*CYCLE)){
                out_num += 1;

                double in = 0, out = 0;

                double* in_err = new double [size];
                double* out_err = new double [size];

                memset(in_err, 0, size * sizeof(double));
                memset(out_err, 0, size * sizeof(double));

                for(auto it = B.begin();it != B.end();it++){
                    if(it->second <= 0){
                        out += 1;
                        for(uint32_t j = 0;j < size;++j){
                            out_err[j] += sketches[j]->Query(it->first, start[i].time - start[0].time);
                        }
                    }
                    else{
                        in += 1;
                        for(uint32_t j = 0;j < size;++j){
                            in_err[j] += (!sketches[j]->Query(it->first, start[i].time - start[0].time));
                        }
                    }
                }

                for(uint32_t j = 0;j < size;++j){
                    FNR[j] += in_err[j] / in;
                    FPR[j] += out_err[j] / out;
                }

                delete [] in_err;
                delete [] out_err;
            }
        }

        for(uint32_t j = 0;j < size;++j){
            std::cout << "FNR: " << FNR[j] / out_num << std::endl;
            std::cout << "FPR: " << FPR[j] / out_num << std::endl;
        }

        delete [] FNR;
        delete [] FPR;
    }

    void InsertThp(Abstract<DATA_TYPE>* sketch, uint64_t TOTAL){
        TP initial, finish;
	uint32_t i = 0;

	TOTAL = (start[SIZE - 1].time - start[0].time) / (double)SIZE * TOTAL;

        initial = now();
        while(start[i + 1].time - start[0].time < TOTAL){
            i += 1;
	    sketch->Insert(start[i].item, start[i].time - start[0].time);
        }
        finish = now();

        std::cout << "Throughput: " << TOTAL / durationms(finish, initial) << std::endl;
    }
};

#endif //SLIDINGMEM_BENCHMARK_H
