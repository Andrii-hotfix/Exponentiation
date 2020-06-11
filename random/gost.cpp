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
    uint64_t p = blumsPrime(variablesT.back());
    int32_t m = variablesT.size() - 2;

    BigInt k = 0;
step5:
    word rm = std::ceil(variablesT[m + 1] / 16);

    // Generate ys
step6:
    std::vector<BigInt> variablesY;
    for (word i = 0; i < rm; ++i)
        variablesY.push_back(congruent32());

    BigInt Y = 0;
    BigInt pow2to161 = 1;
    pow2to161 <<= 161;

    for (word i = 0; i < rm - 1; ++i)
        Y = Y + variablesY[i] * pow2to161;

    _congr32State = variablesY[std::min(rm, 0u)];
    BigInt N = divisionRemainder((1 << (variablesT[m] - 1)), p).first + divisionRemainder((1 << variablesT[m]) * Y, (p * (1 << (16 * rm)))).first;
    if (N.getBitAt(0) == 1)
        N = N + 1;

step11:
    BigInt pm = p * (N + k) + 1;

    if (pm > (1 << (variablesT[m])))
        goto step6;

    BigInt exp1 = m;
    exp1 = exp1.binarySWExp(N + k);
    if ((BigInt(2).binarySWExp(exp1) % pm) != 1 or (BigInt(2).binarySWExp(N + k) % pm) != 1) {
        k = k + 2;
        goto step11;
    }

    m = m - 1;

    if (m >= 0)
        goto step5;

    return p;

}

BigInt GOST::congruent32()
{
    _congr32State = (_paramB * _congr32State + BigInt(_paramC)) % _modulo32;
    return _congr32State;
}
