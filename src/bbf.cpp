//
// Created by Fatemeh Almodaresi on 2019-11-24.
//

#include "bbf.h"
#include "hashUtil.h"

void BBf::construct() {
    std::ifstream infile(inputKeyFile);
    std::string key;
    while (infile.good()) {
        infile >> key;
        if (infile.good())
            insert(key);
    }
    infile.close();
    std::ofstream out(outputBFFile, std::ios::out | std::ios::binary);
    out.write(reinterpret_cast<char *>(&numHashes), sizeof(numHashes));
    bf.serialize(out);
}

void BBf::insert(std::string &key) {
    auto h = MurmurHash64A(key.data(), static_cast<int>(key.size()), seeds[0]);
    auto block = h % numBlocks;
    auto startIdx = block*blockSize;
    for (auto i = 1; i < numHashes; i++) {
        h = MurmurHash64A(key.data(), static_cast<int>(key.size()), seeds[i]);
        bf[startIdx + h % blockSize] = 1;
    }
}

bool BBf::query(std::string &query) {
    bool found = true;
    auto h = MurmurHash64A(query.data(), static_cast<int>(query.size()), seeds[0]);
    auto block = h % numBlocks;
    auto startIdx = block*blockSize;
    for (auto i = 1; i < numHashes; i++) {
        h = MurmurHash64A(query.data(), static_cast<int>(query.size()), seeds[i]);
        if (bf[startIdx + h % blockSize] == 0) {
            found = false;
            break;
        }
    }
    return found;
}

int constructBBf(Opts &opts) {
    BBf bf(opts.inputFile, opts.outputFile, opts.numKeys,opts.fpRate);
    bf.construct();
    std::cerr << "Done storing the bf in " << opts.outputFile << "\n";
    return EXIT_SUCCESS;
}

int queryBBf(Opts &opts) {
    std::cerr << opts.inputFile << "\n" << opts.queryFile << "\n";
    BBf bf(opts.inputFile);
    std::ifstream queries(opts.queryFile);
    std::string query;
    while (queries.good()) {
        queries >> query;
        bool found = bf.query(query);
        std::cout << query << ":" << (found?"Y":"N") << "\n";
    }
    return EXIT_SUCCESS;
}