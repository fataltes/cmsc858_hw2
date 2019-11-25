//
// Created by Fatemeh Almodaresi on 2019-11-24.
//

#include "bf.h"
#include "hashUtil.h"
#include <chrono>

#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

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

int reportBf(ReportOpts &opts) {
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
            Bf bf(fileName, idxFile, count, fpr);
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