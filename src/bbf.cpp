//
// Created by Fatemeh Almodaresi on 2019-11-24.
//

#include "bbf.h"
#include "hashUtil.h"

#include <chrono>

#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

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

int reportBBf(ReportOpts &opts) {
    std::string path = opts.inputDir;
    std::ofstream report(opts.reportFile);
    for (const auto & entry : fs::directory_iterator(path)) {
        auto pathObj = entry.path();
        std::string fileName = pathObj.string();
        std::cerr << fileName << std::endl;
        // get total number of lines in the input file (total keys)
        uint64_t count = 0;
        std::string line;
        std::ifstream file(fileName);
        while (getline(file, line))
            count++;
        file.close();
        float fpr = opts.fpRateStart;
        while (fpr < opts.fpRateEnd) {
            std::string idxFile = opts.outputDir + pathObj.filename().string() + "_" + std::to_string(fpr) + ".idx";
            std::cerr << idxFile << "\n";
            auto start = std::chrono::high_resolution_clock::now();
            BBf bf(fileName, idxFile, count, fpr);
            bf.construct();
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            uint64_t constructionTime = elapsed.count();
            std::cerr << count << " " << fpr << " " << elapsed.count() << "\n";
            for (const auto & queryf : fs::directory_iterator(opts.queryDir)) {
                auto pathObj = queryf.path();
                std::string queryfile = queryf.path();
                std::cerr << queryfile << std::endl;
                std::ifstream queries(queryfile);
                std::string query;
                start = std::chrono::high_resolution_clock::now();
                uint64_t posCount{0};
                while (queries.good()) {
                    queries >> query;
                    if (queries.good())
                        if (bf.query(query)) {
                            posCount++;
                        }
                }
                finish = std::chrono::high_resolution_clock::now();
                elapsed = finish - start;
                report << count << "\t" << fpr << "\t" << posCount
                       << "\t" << elapsed.count() << "\t" << constructionTime
                       << "\t" << queryf.path().filename() << "\n";
            }
            fpr += opts.fpRateJump;
        }
    }
    report.close();
}