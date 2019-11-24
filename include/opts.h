//
// Created by Fatemeh Almodaresi on 2019-11-03.
//

#ifndef BVOPERATORS_OPTS_H
#define BVOPERATORS_OPTS_H

#include <cstdint>

namespace BVOperators {
    constexpr char wvIdxFileName[] = "wv.bin";
    constexpr char idxInfoFileName[] = "idxInfo.bin";
    constexpr char rankStatFileName[] = "rank.stat";
    constexpr char selectStatFileName[] = "select.stat";
    constexpr int64_t INVALID = -1;
}

enum Operation {
    acc, rnk, sel
};
struct Opts {
    uint64_t numKeys{10000000};
    float fpRate{0.001};
    std::string prefix = "console";
    std::string inputFile;
    std::string outputFile;
    std::string queryFile;
    Operation operation = Operation::acc;
};

#endif //BVOPERATORS_OPTS_H
