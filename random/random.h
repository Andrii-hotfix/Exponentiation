#ifndef RANDOM_H
#define RANDOM_H

#include "bigint.h"
#include "bigintfunct.h"

#include <random>
#include <chrono>

std::vector<unsigned char> BBS(BigInt p, BigInt q, int nBits)
{
//    const char* p_str = "D5BBB96D30086EC484EBA3D7F9CAEB07";
//    const char* q_str = "425D2B9BFDB25B9CF6C416CC6E37B59C1F";

//    BigInt p(p_str, BigInt::Hex);
//    BigInt q(q_str, BigInt::Hex);
    BigInt n = p * q;

    std::default_random_engine gen;
    std::uniform_int_distribution<word> distr(0, maxWord);
    std::vector<word> seedHeap(std::ceil(nBits / bitsInWord), 0);
    for (word &item : seedHeap)
        item = distr(gen);
    BigInt result(std::move(seedHeap));

    constexpr word power = 2;
    return result;
}

#endif // RANDOM_H
