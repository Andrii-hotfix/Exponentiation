#include "gost.h"

#include <chrono>
#include <random>

GOST::GOST()
{
    std::random_device rd;
    std::default_random_engine gen{rd()};
    constexpr uint32_t max16Word = 1 << 16;
    std::uniform_int_distribution<uint32_t> distr(0, max16Word);
    _paramC = distr(gen);
    _state = distr(gen);
}

BigInt GOST::getRandomBits(word nBits)
{
    BigInt y0 = _state;
    // Compute ts
    BigInt t0 = nBits;

}

BigInt GOST::congruent32()
{
    _state = (_paramB * _state + BigInt(_paramC)) % _modulo32;
    return _state;
}
