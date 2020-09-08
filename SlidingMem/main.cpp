#include <iostream>
#include <fstream>

#include "benchmark.h"

using namespace std;

const char* file[] = {
        "../Data/syn.dat",
        "../Data/web.dat",
        "../Data/net.dat",
        "../Data/ip.dat",
};

int main() {
    
    for(uint32_t i = 0;i < 3;++i){
        std::cout << file[i] << std::endl;
        TupleBench<uint32_t> dataset(file[i]);
        dataset.Error(8000000, 1000000);
//        dataset.Thp(8000000, 1000000);
    }
   
    std::cout << file[3] << std::endl;
    TupleBench<uint64_t> dataset(file[3]);
    dataset.Error(8000000, 1000000);
//    dataset.Thp(8000000, 1000000); 


//    TimeBench<uint32_t> dataset("../Data/sof.dat");
//    dataset.Thp(40000000, 5000000);

    return 0;
}
