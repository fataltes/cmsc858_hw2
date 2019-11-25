//
// Created by Fatemeh Almodaresi on 2019-11-24.
//

#include "bf.h"
#include "hashUtil.h"

void Bf::construct() {
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

void Bf::insert(std::string &key) {
    for (auto i = 0; i < numHashes; i++) {
        auto h = MurmurHash64A(key.data(), static_cast<int>(key.size()), seeds[i]);
        bf[h % bf.size()] = 1;
    }
}

bool Bf::query(std::string &query) {
    bool found = true;
    for (auto i = 0; i < numHashes; i++) {
        auto h = MurmurHash64A(query.data(), static_cast<int>(query.size()), seeds[i]);
        if (bf[h % bf.size()] == 0) {
            found = false;
            break;
        }
    }
    return found;
}

int constructBf(Opts &opts) {
    Bf bf(opts.inputFile, opts.outputFile, opts.numKeys,opts.fpRate);
    bf.construct();
    std::cerr << "Done storing the bf in " << opts.outputFile << "\n";
    return EXIT_SUCCESS;
}

int queryBf(Opts &opts) {
    std::cerr << opts.inputFile << "\n" << opts.queryFile << "\n";
    Bf bf(opts.inputFile);
    std::ifstream queries(opts.queryFile);
    std::string query;
    while (queries.good()) {
        queries >> query;
        bool found = bf.query(query);
        std::cout << query << ":" << (found?"Y":"N") << "\n";
    }
    return EXIT_SUCCESS;
}