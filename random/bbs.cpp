#include "bbs.h"

BBS::BBS(word nBits)
{
    std::random_device rd;
    std::default_random_engine gen{rd()};
    std::uniform_int_distribution<word> distr(0, maxWord);
    BigInt p = getRandSeed(std::floor(nBits / 2), distr, gen);
    BigInt q = getRandSeed(std::floor(nBits / 2), distr, gen);
    _modulo = p * q;
    _state = getRandSeed(nBits, distr, gen);
}

BBS::BBS(BigInt p, BigInt q, word nBits)
{
    std::random_device rd;
    std::default_random_engine gen{rd()};
    std::uniform_int_distribution<word> distr(0, maxWord);
    _modulo = p * q;
    _state = getRandSeed(nBits, distr, gen);
}

BigInt BBS::getRandomBits()
{
    _state = _state.binarySWExp(2);
    _state = _state % _modulo;
    return _state;
}

BigInt BBS::getRandSeed(word nBits, std::uniform_int_distribution<word>& distr, std::default_random_engine& gen)
{
    std::vector<word> seedHeap(std::max(std::ceil(nBits / bitsInWord), 1.0), 0);
    std::fill(seedHeap.begin(), seedHeap.end(), distr(gen));
    BigInt result(std::move(seedHeap));

    result = result >> static_cast<word>(std::ceil(nBits / bitsInWord)) % nBits;
    return result;
}
