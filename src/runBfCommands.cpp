//
// Created by Fatemeh Almodaresi on 2019-11-02.
//

#include <iostream>

#include "clipp.h"
//#include "spdlog/spdlog.h"
//#include "spdlog/fmt/ostr.h"
//#include "spdlog/fmt/fmt.h"
#include "opts.h"

//#include "CLI/Timer.hpp"
using namespace clipp;

int constructBf(Opts &opts);
int queryBf(Opts &opts);

int main(int argc, char* argv[])  {
    (void) argc;
    Opts opts;
    std::string type = "access";

    enum class mode {help, query_bf, construct_bf};
    mode selected = mode::help;

    // Rank Mode, prepare the rank performance distribution over bv size


    auto bfConstructMode = (
            command("build").set(selected, mode::construct_bf),
                    (option("-k", "--input_key_file") & value("inputFile", opts.inputFile)) % "The file containing the input keys",
                    (option("-f", "--fpr") & value("falsePositiveRate", opts.fpRate)) % "The Bloom Filter False Positive Rate (default:0.001)",
                    (option("-n", "--num_keys") & value("numDistinctKeys", opts.numKeys)) % "Total number of distinct keys in input file",
                    (option("-o", "--output_file") & value("outputFile", opts.outputFile)) % "The file storing the bloom filter in"
    );

    auto bfQueryMode = (
            command("query").set(selected, mode::query_bf),
                    (option("-i", "--bf_file") & value("bfFile", opts.inputFile)) % "The bloom filter file",
                    (option("-q", "--query_file") & value("queryFile", opts.queryFile)) % "The query file"
    );


    //Multithreaded console logger(with color support)
//    auto console = spdlog::stderr_color_mt("console");

    bool showHelp = false;
    auto cli = (
            (bfConstructMode | bfQueryMode |
                    command("help").set(selected,mode::help) |
            option("--help", "-h").set(showHelp, true) % "show help"
            ));

    decltype(parse(argc, argv, cli)) res;
    try {
        res = parse(argc, argv, cli);
        if (showHelp) {
            std::cout << make_man_page(cli, "bvOperate");
            return 0;
        }
    } catch (std::exception &e) {
        std::cout << "\n\nparsing command line failed with exception: " << e.what() << "\n";
        std::cout << "\n\n";
        std::cout << make_man_page(cli, "bvOperate");
        return 1;
    }

    if(res) {
        switch(selected) {
            case mode::construct_bf:
                std::cerr << "construct_bf\n";
                constructBf(opts); break;
            case mode::query_bf:
                std::cerr << "query_bf\n";
                queryBf(opts); break;
            case mode::help: std::cout << make_man_page(cli, "bvOperate"); break;
        }
    }
    return EXIT_SUCCESS;
}