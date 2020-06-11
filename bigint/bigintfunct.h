#ifndef BIGINTFUNCT_H
#define BIGINTFUNCT_H

#include "bigint.h"

// Arithmetic
BigInt operator+(const BigInt& left, const BigInt& right);
BigInt operator-(const BigInt& left, const BigInt& right);
BigInt operator*(const BigInt& left, const BigInt& right);
BigInt operator%(const BigInt& op, const BigInt& modulo);
std::pair<BigInt, BigInt> divisionRemainder(const BigInt& numerator, const BigInt& denominator);

// Comparisons
bool operator==(const BigInt& left, const BigInt& right);
bool operator!=(const BigInt& left, const BigInt& right);
bool operator<(const BigInt& left, const BigInt& right);
bool operator<=(const BigInt& left, const BigInt& right);
bool operator>(const BigInt& left, const BigInt& right);
bool operator>=(const BigInt& left, const BigInt& right);

// Logic
BigInt operator&(const BigInt& left, const BigInt &right);
BigInt operator|(const BigInt& left, const BigInt& right);
BigInt operator^(const BigInt& left, const BigInt& right);
BigInt operator>>(const BigInt& op, const size_t numOfShifts);
BigInt operator<<(const BigInt& op, const size_t numOfShifts);
BigInt operator~(const BigInt& op);

// Algorithms
BigInt gcd(const BigInt& left, const BigInt& right);

#endif // BIGINTFUNCT_H
