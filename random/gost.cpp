#include "gost.h"
#include "primality.h"

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
    if (nBits < 17)
        throw std::logic_error("GOST random generator is not capable of generationg numbers that small");

    _congr32State = _state;
    // Compute ts
    std::vector<word> variablesT;
    variablesT.push_back(nBits);
    for (word i = 1;; ++i) {
        word tNew = std::floor(variablesT[i - 1] / 2);
        if (tNew >= 17)
            variablesT.push_back(tNew);
        else
            break;
    }
    // Find prime Ps
    BigInt p = 1;
    constexpr unsigned numberOfPrimalityWitnesses = 10;
    for (p <<= variablesT.back();; p = p + 1) {
        if (primalityTest(p, numberOfPrimalityWitnesses))
            break;
    }
    int32_t m = variablesT.size() - 2;

    auto fromStep6Funct = [&](word rm, word pm) {
        // Generate ys
        std::vector<BigInt> variablesY;
        for (int32_t i = 0; i < m; ++i)
        variablesY.push_back(congruent32());

        BigInt Y = 0;
        BigInt pow2to161 = 1;
        pow2to161 <<= 161;

        for (word i = 0; i < rm - 1; ++i)
        Y = Y + variablesY[i] * pow2to161;

        BigInt y0 = variablesY[rm];
        BigInt N = divisionRemainder((1 << (variablesT[m] - 1)), p).first + divisionRemainder((1 << variablesT[m]) * Y, (p * (1 << (16 * rm)))).first;
        if (N.getBitAt(0) == 1)
        N = N + 1;

        BigInt k = 0;
        pm = p * (N + k) + 1;
    };

    do {
        word rm = std::ceil(variablesT[m + 1] / 16);

        BigInt pm;
        do {
            // Generate ys
            std::vector<BigInt> variablesY;
            for (int32_t i = 0; i < m; ++i)
                variablesY.push_back(congruent32());

            BigInt Y = 0;
            BigInt pow2to161 = 1;
            pow2to161 <<= 161;

            for (word i = 0; i < rm - 1; ++i)
                Y = Y + variablesY[i] * pow2to161;

            BigInt y0 = variablesY[rm];
            BigInt N = divisionRemainder((1 << (variablesT[m] - 1)), p).first + divisionRemainder((1 << variablesT[m]) * Y, (p * (1 << (16 * rm)))).first;
            if (N.getBitAt(0) == 1)
                N = N + 1;

            BigInt k = 0;
            pm = p * (N + k) + 1;
        } while (pm > (1 << variablesT[m]));

        m = m - 1;
    } while (m >= 0);
    return p;
}

BigInt GOST::congruent32()
{
    _congr32State = (_paramB * _congr32State + BigInt(_paramC)) % _modulo32;
    return _congr32State;
}

void GOST::fromStep6Funct()
{
    // Generate ys
    std::vector<BigInt> variablesY;
    for (int32_t i = 0; i < m; ++i)
    variablesY.push_back(congruent32());

    BigInt Y = 0;
    BigInt pow2to161 = 1;
    pow2to161 <<= 161;

    for (word i = 0; i < rm - 1; ++i)
    Y = Y + variablesY[i] * pow2to161;

    BigInt y0 = variablesY[rm];
    BigInt N = divisionRemainder((1 << (variablesT[m] - 1)), p).first + divisionRemainder((1 << variablesT[m]) * Y, (p * (1 << (16 * rm)))).first;
    if (N.getBitAt(0) == 1)
    N = N + 1;

    BigInt k = 0;
    pm = p * (N + k) + 1;
}
