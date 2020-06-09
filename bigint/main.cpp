#include "bigint.h"

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
            ("input,i", poptions::value<std::vector<std::string>>(), "Input expression to proceed")
            ("mode,m", poptions::value<std::string>(), "Input algorighm mode (sw - sliding window, ma - mary alg, lr - binart left-to-right, rl - binart right-to-left)")
            ("radix,r", poptions::value<std::string>(), "Input radix for input and output");

    poptions::positional_options_description positional;
    positional.add("input", -1);

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

        std::vector<std::string> tokens = variables["input"].as<std::vector<std::string>>();
        // Additional checkzzz
        if (tokens.size() != 3)
            throw std::logic_error("Malformed input expression");

        if (tokens[1] != "^")
            throw std::logic_error("Malformed input expression");

        size_t lenConstraint = 10;
        if (radix == BigInt::Dec)
            lenConstraint = 10;
        else if (radix == BigInt::Hex)
            lenConstraint = 8;
        else if (radix == BigInt::Bin)
            lenConstraint = 32;

        if (tokens.front().size() > lenConstraint)
            throw std::logic_error("Exponent can not be that big");

        std::string mode = variables["mode"].as<std::string>();
        boost::trim(mode);
        BigInt base(tokens[0], radix);
        word exp = std::stoul(tokens.back(), nullptr, static_cast<unsigned>(radix));

        if (mode == "sw")
            std::cout << base.binarySWExp(exp).getStr(radix) << std::endl;
        else if (mode == "ma")
            std::cout << base.mAryLRExp(exp).getStr(radix) << std::endl;
        else if (mode == "lr")
            std::cout << base.binaryLRExp(exp).getStr(radix) << std::endl;
        else if (mode == "rl")
            std::cout << base.binaryRLExp(exp).getStr(radix) << std::endl;
        else
            throw std::logic_error("Unknown exponentiation mode");

    } catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << "Could not parse input expression. Please, refine your input" << std::endl;
        return 1;
    }

    return 0;
}
