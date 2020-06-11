#ifndef RANDOM_H
#define RANDOM_H

#include "bigint.h"
#include "bigintfunct.h"

#include <random>
#include <chrono>

// Miller-Rabin primality test
bool primalityTest(const uint64_t &prime, unsigned iterations)
{
    // initial conditions
    if (prime == 0) {
        return false;
    } else if (prime == 1) {
        return true;
    } else if (prime % 2 == 0) {
        return false;
    } else {
        // get prime - 1 as (2^s)*t (t is odd)
        uint64_t t = prime - 1;
        uint64_t s = 0;
        while ((t % 2) == 0) {
            t = std::floor(t / 2);
            s += 1;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<unsigned long> distr(2, maxWord);
        for (unsigned i = 0; i < iterations; i++) {
            uint64_t base = distr(gen);
            uint64_t exp = static_cast<uint64_t>(std::pow(base, t)) % prime;
            if (exp == 1 || exp == prime - 1) {
                continue;
            } else {
                for (unsigned j = 0; j < s - 1; j++) {
                    exp = static_cast<uint64_t>(std::pow(exp, 2)) % prime;
                    if (exp == 1) {
                        return false;
                    } else if (exp == prime - 1) {
                        break;
                    }
                }
                return false;
            }
        }
        return true;
    }
}


uint64_t blumsPrime(uint64_t nBits)
{
    if (nBits >= 64)
        throw std::logic_error("Blums prime generator is not capable of generating numbers that big");

    uint64_t max(1);
    max <<= nBits;
    uint64_t maxQuotient = std::ceil(max / 4);

    for (size_t i = 1; primalityTest(4 * maxQuotient + 3, 5); ++i)
        maxQuotient++;

    return maxQuotient * 4 + 3;
}

#endif // RANDOM_H
