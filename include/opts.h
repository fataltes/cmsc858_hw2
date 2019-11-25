//
// Created by Fatemeh Almodaresi on 2019-11-03.
//

#ifndef BVOPERATORS_OPTS_H
#define BVOPERATORS_OPTS_H

#include <cstdint>

struct Opts {
    uint64_t numKeys{10000000};
    float fpRate{0.001};
    std::string prefix = "console";
    std::string inputFile;
    std::string outputFile;
    std::string queryFile;
};

struct ReportOpts {
    uint64_t numKeys{10000000};
    float fpRateStart{0.01};
    float fpRateEnd{0.30};
    float fpRateJump{0.05};
    std::string prefix = "console";
    std::string inputDir;
    std::string outputDir;
    std::string queryDir;
    std::string reportFile;
};

#endif //BVOPERATORS_OPTS_H
