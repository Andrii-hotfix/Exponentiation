#ifndef GOST_H
#define GOST_H

#include "bigint.h"
#include "bigintfunct.h"

#include <cstdint>


class GOST
{
public:
    explicit GOST();
    BigInt getRandomBits(word nBits);

private:
    BigInt congruent32();

    uint16_t _paramC;
    BigInt _state;
    BigInt _congr32State;

    BigInt _modulo32 = (BigInt(1) << word(32));
    BigInt _paramB = 19381;
};

#endif // GOST_H
