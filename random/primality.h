#ifndef RANDOM_H
#define RANDOM_H

#include "bigint.h"
#include "bigintfunct.h"

#include <random>
#include <chrono>

// Miller-Rabin primality test
bool primalityTest(const BigInt &prime, unsigned iterations)
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
        BigInt t = prime - 1;
        word s = 0;
        while ((t % 2) == 0) {
            t = divisionRemainder(t, 2).first;
            s += 1;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<unsigned long> distr(2, maxWord);
        for (unsigned i = 0; i < iterations; i++) {
            BigInt base = distr(gen);
            BigInt exp = base.binarySWExp(t) % prime;
            if (exp == 1 || exp == prime - 1) {
                continue;
            } else {
                for (unsigned j = 0; j < s - 1; j++) {
                    exp = exp.binarySWExp(2) % prime;
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

#endif // RANDOM_H
