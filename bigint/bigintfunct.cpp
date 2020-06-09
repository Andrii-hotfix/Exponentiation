
#include "bigintfunct.h"

#include <cmath>

BigInt operator+(const BigInt& left, const BigInt& right)
{
    bool carry = false;
    BigInt result(std::max(left.wordLen(), right.wordLen()) + 1, 0);
    for (size_t i = 0; i < std::min(left.bitsLen(), right.bitsLen()); ++i) {
        bool leftBit = left.getBitAt(i);
        bool rightBit = right.getBitAt(i);
        result.setBitAt(i, leftBit ^ rightBit ^ carry);
        carry = (leftBit & rightBit) | (leftBit & carry) | (rightBit & carry);
    }
    for (size_t i = std::min(left.bitsLen(), right.bitsLen()); i < left.bitsLen(); ++i) {
        bool leftBit = left.getBitAt(i);
        result.setBitAt(i, leftBit ^ carry);
        carry &= leftBit;
    }
    for (size_t i = std::min(left.bitsLen(), right.bitsLen()); i < right.bitsLen(); ++i) {
        bool rightBit = right.getBitAt(i);
        result.setBitAt(i, rightBit ^ carry);
        carry &= rightBit;
    }
    result.bitsLen();
    result.setBitAt(std::max(left.bitsLen(), right.bitsLen()), carry);

    result.removeLeadingZeros();
    return result;
}

BigInt operator-(const BigInt& left, const BigInt& right)
{
    if (left < right)
        throw std::logic_error("This library can not handle negative values (yet)");

    if (left == right)
        return 0;

    bool carry = false;
    BigInt result(std::max(left.wordLen(), right.wordLen()) + 1, 0);
    for (size_t i = 0; i < std::min(left.bitsLen(), right.bitsLen()); ++i) {
        bool leftBit = left.getBitAt(i);
        bool rightBit = right.getBitAt(i);
        result.setBitAt(i, leftBit ^ rightBit ^ carry);
        carry = (!leftBit & rightBit) | (!leftBit & carry) | (leftBit & rightBit & carry);
    }
    for (size_t i = std::min(left.bitsLen(), right.bitsLen()); i < left.bitsLen(); ++i) {
        bool leftBit = left.getBitAt(i);
        result.setBitAt(i, leftBit ^ carry);
        carry &= !leftBit;
    }

    result.removeLeadingZeros();
    return result;
}

BigInt operator*(const BigInt& left, const BigInt& right)
{
    constexpr word max16bitWord = (1 << 16) - 1;
    if ((left <= max16bitWord) and (right <= max16bitWord))
        return BigInt(1, left.getHeap().front() * right.getHeap().front());

    size_t n = std::floor(std::max(left.bitsLen(), right.bitsLen()) / 2);
    const BigInt mask = (BigInt(1) << n) - 1;
    BigInt leftHigh = left >> n;
    BigInt leftLow = left & mask;
    BigInt rightHigh = right >> n;
    BigInt rightLow = right & mask;

    BigInt highPord = leftHigh * rightHigh;
    BigInt lowProd = leftLow * rightLow;
    BigInt crossProd = ((leftHigh + leftLow) * (rightHigh + rightLow)) - highPord - lowProd;
    return (((highPord << n) + crossProd) << n) + lowProd;
}

BigInt operator%(const BigInt& op, const BigInt& modulo)
{
    if (op.isZero())
        return 0;

    if (modulo.isZero())
        return op;

    if (op < maxWord and modulo < maxWord)
        return op.getHeap()[0] % modulo.getHeap()[0];

    BigInt remainder = 0;
    BigInt bPowerT = BigInt(1) << modulo.bitsLen();
    BigInt c = bPowerT - modulo;
    auto[q0, r0] = divisionRemainder(op, bPowerT);
    BigInt q1;
    remainder = r0;
    BigInt r1;
    for (size_t i = 0; q0 > 0; ++i) {
        q1 = divisionRemainder(q0 * c, bPowerT).first;
        r1 = q0 * c  - q1 * bPowerT;
        remainder = remainder + r1;
        q0 = q1;
    }
    while (remainder >= modulo)
        remainder = remainder - modulo;

    return remainder;
}

std::pair<BigInt, BigInt> divisionRemainder(const BigInt& numerator, const BigInt& denominator)
{
    if (denominator.isZero())
        throw std::logic_error("Division by zero is impossible");

    if ((numerator <= BigInt(1, maxWord)) and (denominator <= BigInt(1, maxWord))) {
        auto quotient = static_cast<word>(numerator.getHeap().front() / denominator.getHeap().front());
        word remainder = numerator.getHeap().front() % denominator.getHeap().front();
        return {quotient, remainder};
    }

    BigInt quotient = 0;
    BigInt remainder = 0;
    for (long i = numerator.bitsLen() - 1; i >= 0; --i) {
        remainder = remainder << 1;
        remainder.setBitAt(0, numerator.getBitAt(i));
        if (remainder >= denominator) {
            remainder = remainder - denominator;
            quotient.setBitAt(i, true);
        }
    }
    quotient.removeLeadingZeros();
    remainder.removeLeadingZeros();
    return {quotient, remainder};
}

bool operator==(const BigInt& left, const BigInt& right)
{
    return BigInt(left ^ right).isZero();
}

bool operator<(const BigInt& left, const BigInt& right)
{
    if (left.bitsLen() != right.bitsLen())
        return left.bitsLen() < right.bitsLen();

    for (size_t i = left.bitsLen(); i > 0; --i) {
        bool leftBit = left.getBitAt(i - 1);
        bool rightBit = right.getBitAt(i - 1);
        if (leftBit != rightBit)
            return leftBit < rightBit;
    }
    return false;
}

bool operator<=(const BigInt& left, const BigInt &right)
{
    return (left == right) or (left < right);
}

bool operator>(const BigInt& left, const BigInt& right)
{
    return not (left < right) and not (left == right);
}

bool operator>=(const BigInt& left, const BigInt &right)
{
    return (left == right) or (left > right);
}

BigInt operator&(const BigInt& left, const BigInt& right)
{
    std::vector<word> resultHeap(std::min(left.wordLen(), right.wordLen()), 0);
    for (size_t i = 0; i < std::min(left.wordLen(), right.wordLen()); ++i)
        resultHeap[i] = left.getHeap()[i] & right.readHeap()[i];

    return BigInt(std::move(resultHeap));
}

BigInt operator|(const BigInt& left, const BigInt& right)
{
    std::vector<word> resultHeap(std::max(left.wordLen(), right.wordLen()), 0);
    size_t minLen = std::min(left.wordLen(), right.wordLen());
    for (size_t i = 0; i < minLen; ++i)
        resultHeap[i] = left.getHeap()[i] | right.readHeap()[i];

    for (size_t i = minLen; i < left.wordLen(); ++i)
        resultHeap[i] |= left.getHeap()[i];

    for (size_t i = minLen; i < right.wordLen(); ++i)
        resultHeap[i] |= right.readHeap()[i];

    return BigInt(std::move(resultHeap));
}

BigInt operator^(const BigInt& left, const BigInt& right)
{
    std::vector<word> resultHeap(std::max(left.wordLen(), right.wordLen()), 0);
    size_t minLen = std::min(left.wordLen(), right.wordLen());
    for (size_t i = 0; i < minLen; ++i)
        resultHeap[i] = left.getHeap()[i] ^ right.readHeap()[i];

    for (size_t i = minLen; i < left.wordLen(); ++i)
        resultHeap[i] ^= left.getHeap()[i];

    for (size_t i = minLen; i < right.wordLen(); ++i)
        resultHeap[i] ^= right.readHeap()[i];

    return BigInt(std::move(resultHeap));
}

BigInt operator>>(const BigInt& op, const size_t numOfShifts)
{
    std::vector<word> resultHeap(op.getHeap().begin(), op.getHeap().end());
    for (size_t shiftIteration = 0; shiftIteration < numOfShifts; ++shiftIteration) {
        resultHeap.front() >>= 1;
        for (size_t i = 1; i < op.getHeap().size(); ++i) {
            word carryMask = (word(1) & resultHeap[i]) << (bitsInWord - 1);
            resultHeap[i] >>= 1;
            resultHeap[i - 1] |= carryMask;
        }
    }
    return BigInt(std::move(resultHeap));
}

BigInt operator<<(const BigInt& op, const size_t numOfShifts)
{
    std::vector<word> resultHeap(op.getHeap().begin(), op.getHeap().end());
    for (size_t shiftIteration = 0; shiftIteration < numOfShifts; ++shiftIteration) {
        word carryMask = 0;
        for (size_t i = 0; i < resultHeap.size(); ++i) {
            word newMask = ((~(~word(0) >> 1)) & resultHeap[i]) >> (bitsInWord - 1);
            resultHeap[i] <<= 1;
            resultHeap[i] |= carryMask;
            carryMask = newMask;
        }
        if (carryMask)
            resultHeap.emplace_back(1);
    }
    return BigInt(std::move(resultHeap));
}

BigInt operator~(const BigInt &op)
{
    std::vector<word> resultHeap(op.wordLen());
    for (size_t i = 0; i < op.wordLen(); ++i)
        resultHeap[i] = ~op.getHeap()[i];

    BigInt result(std::move(resultHeap));
    for (long i = op.wordLen() * bitsInWord; i > static_cast<long>(op.bitsLen()); --i)
        result.setBitAt(i - 1, false);

    return result;
}

BigInt gcd(const BigInt& left, const BigInt& right)
{
    BigInt resultingLeft = left;
    BigInt resultingRight = right;
    word shift = 0;

    if (resultingLeft.isZero())
        return resultingRight;

    if (resultingRight.isZero())
        return resultingLeft;

    while (((resultingLeft | resultingRight) & 1).isZero()) {
        ++shift;
        resultingLeft = resultingLeft >> 1;
        resultingRight = resultingRight >> 1;
    }

    while ((resultingLeft & 1).isZero())
        resultingLeft = resultingLeft >> 1;

    do {
        while ((resultingRight & 1).isZero())
            resultingRight = resultingRight >> 1;

        if (resultingLeft > resultingRight) {
            BigInt tmp = resultingRight;
            resultingRight = resultingLeft;
            resultingLeft = tmp;
        }

        resultingRight = resultingRight - resultingLeft;
    } while (not resultingRight.isZero());

    return resultingLeft << shift;
}
