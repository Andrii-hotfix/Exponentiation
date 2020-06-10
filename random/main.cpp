#include "bbs.h"
#include "gost.h"

#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

namespace poptions = boost::program_options;

int main(int argc, const char* argv[])
{
    poptions::options_description options("Allowed options");
    options.add_options()
            ("help,h", "Prints this message")
            ("nbits,n", poptions::value<word>(), "N bits to generate randomly")
            ("mode,m", poptions::value<std::string>(), "Input algorighm mode (bbs or gost)")
            ("iterations,i", poptions::value<word>(), "Input how many numbers to generate")
            ("radix,r", poptions::value<std::string>(), "Input radix for input and output");

    poptions::positional_options_description positional;
    positional.add("nbits", -1);

    try {
        // Parse command line
        poptions::command_line_parser parser(argc, argv);
        parser.options(options);
        parser.positional(positional);
        poptions::variables_map variables;
        store(parser.run(), variables);
        notify(variables);

        // Show help
        if (variables.count("help") || argc == 1) {
            std::cout << options;
            return 1;
        }

        BigInt::Radix radix = BigInt::Dec;
        if (variables.count("radix")) {
            std::string radixMode = variables["radix"].as<std::string>();
            boost::trim(radixMode);
            radix = static_cast<BigInt::Radix>(std::stoul(radixMode, nullptr, 10));
        }

        size_t lenConstraint = 10;
        if (radix == BigInt::Dec)
            lenConstraint = 10;
        else if (radix == BigInt::Hex)
            lenConstraint = 8;
        else if (radix == BigInt::Bin)
            lenConstraint = 32;

        std::string mode = variables["mode"].as<std::string>();
        boost::trim(mode);

        word iterations = 1;
        if (variables.count("iterations"))
            iterations = variables["iterations"].as<word>();

        std::cout << "Random generated value: " << std::endl;
        if (mode == "bbs") {
            BBS generator(variables["nbits"].as<word>());
            for (word i = 0; i < iterations; ++i)
                std::cout << generator.getRandomBits().getStr(radix) << std::endl;
        } else if (mode == "gost") {
            GOST generator;
            if (variables["nbits"].as<word>() == 16) {
                for (word i = 0; i < iterations; ++i)
                    std::cout << generator.getRandomBits16().getStr(radix) << std::endl;
            } else if (variables["nbits"].as<word>() == 32) {
                for (word i = 0; i < iterations; ++i)
                    std::cout << generator.congruent32().getStr(radix) << std::endl;
            } else {
                throw std::logic_error("GOST generator can only generate either 16 or 32 bit words");
            }
        } else {
            throw std::logic_error("Unknown exponentiation mode");
        }

    } catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << "Could not parse input expression. Please, refine your input" << std::endl;
        return 1;
    }

    return 0;
}
