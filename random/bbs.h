#ifndef BBS_H
#define BBS_H

#include "bigint.h"
#include "bigintfunct.h"

#include <chrono>
#include <random>

class BBS
{
public:
    explicit BBS(word nBits);
    BBS(BigInt p, BigInt q, word nBits);
    BigInt getRandomBits();

private:
    BigInt getRandSeed(word nBits, std::uniform_int_distribution<word>& distr, std::default_random_engine& gen);

    BigInt _state;
    BigInt _modulo;
};

#endif // BBS_H
