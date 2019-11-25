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


class BBf {
public:
    BBf(std::string &inputKeyFileIn, std::string &outputFileIn, uint64_t numKeysIn, float fprIn) :
            inputKeyFile(inputKeyFileIn), outputBFFile(outputFileIn) {
        uint64_t numKeys = numKeysIn;
        float fpr = fprIn;
        filterSize = static_cast<uint64_t>(std::ceil(
                -1.0 * numKeys * std::log(fpr) / std::pow(std::log(2.0), 2))); // chosen based on numKeys and fpr
        numHashes = static_cast<uint64_t>(std::ceil(-1.0 * std::log2(fpr)));
        if (numHashes == 0) {
            std::cerr << "ERROR! numHashes is 0. Can't construct any bloom filters\n";
            std::exit(3);
        }
        for (uint32_t i = 0; i < numHashes; i++) {
            seeds.push_back(i * i);
        }
        std::cerr << "filter size: " << filterSize << " , numHashes: " << numHashes << "\n";
        numBlocks = static_cast<uint64_t >(std::ceil((double) filterSize / blockSize));
        filterSize = numBlocks * blockSize;
        bf.resize(filterSize);
    }

    BBf(std::string &bfFileIn) {
        std::ifstream ifile(bfFileIn, std::ios::binary);
        ifile.read(reinterpret_cast<char *>(&numHashes), sizeof(numHashes));
        if (numHashes == 0) {
            std::cerr << "ERROR! numHashes is 0. Can't continue\n";
            std::exit(3);
        }
        for (uint32_t i = 0; i < numHashes; i++) {
            seeds.push_back(i * i);
        }
        bf.deserialize(ifile);
        filterSize = bf.size();
        numBlocks = static_cast<uint64_t >(std::ceil((double) filterSize / blockSize));
        ifile.close();
        if (numBlocks * blockSize != filterSize) {
            std::cerr << "ERROR! The blocksize of the stored Block bloom filter is different "
                         "from " << blockSize << "\n";
            std::cerr << "Stopping here.\n";
            std::exit(3);
        }
        std::cerr << "filter size: " << filterSize << " , numHashes: " << numHashes << "\n";
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
    uint64_t blockSize{512};
    uint64_t numBlocks{0};
    std::vector<uint32_t> seeds;
};


#endif //BF_BF_H
