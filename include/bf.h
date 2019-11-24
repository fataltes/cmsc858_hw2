//
// Created by Fatemeh Almodaresi on 2019-11-24.
//

#ifndef BF_BF_H
#define BF_BF_H

#include <ostream>
#include <vector>
#include <tgmath.h>

#include "opts.h"
#include "compact_vector/compact_vector.hpp"


class Bf {
public:
    Bf(std::string &inputKeyFileIn, std::string &outputFileIn, uint64_t numKeysIn, float fprIn):
    inputKeyFile(inputKeyFileIn), outputBFFile(outputFileIn)
    {
       uint64_t numKeys = numKeysIn;
       float fpr = fprIn;
       filterSize = static_cast<uint64_t>(std::ceil(-1*numKeys*std::log(fpr)/std::pow(std::log(2.0), 2))); // chosen based on numKeys and fpr
       numHashes = static_cast<uint64_t>(std::ceil(-1*std::log2(fpr)));
       std::cerr << "filter size: " << filterSize << " , numHashes: " << numHashes << "\n";
       bf.resize(filterSize);
       numHashes = 5; // chosen based on numKeys and fpr
       for (uint32_t i = 0; i < numHashes; i++) {
           seeds.push_back(i*i);
       }
    }

    Bf(std::string &bfFileIn) {
        std::ifstream ifile(bfFileIn, std::ios::binary);
        ifile.read(reinterpret_cast<char*>(&numHashes), sizeof(numHashes));
        bf.deserialize(ifile);
        ifile.close();
    }

    void construct();

    void insert(std::string &key);
    bool query(std::string &query);

private:
    compact::vector<uint64_t, 1> bf;
    std::string inputKeyFile;
    std::string outputBFFile;
    uint64_t filterSize;
    uint64_t numHashes;
    std::vector<uint32_t> seeds;
};


#endif //BF_BF_H