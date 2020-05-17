#include "bigint.h"

#include <fmt/core.h>

#include <stdexcept>
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

BigInt::BigInt(uint32_t value)
{
    _heap.clear();
    _heap.emplace_back(value);
}

BigInt::BigInt(size_t size, uint32_t value)
{
    _heap.resize(size, value);
}

BigInt::BigInt(const std::string &asStr)
{
    setHexStr(asStr);
}

BigInt::BigInt(std::vector<word>&& heap)
{
    _heap.insert(_heap.begin(), std::make_move_iterator(heap.begin()), std::make_move_iterator(heap.end()));
    _table.clear();
    removeLeadingZeros();
}

void BigInt::setHexStr(const std::string &asStr)
{
    _heap.clear();
    // Add leading 0 digits if any needed
    std::string trailingNulls;
    for (size_t i = 0; i < (8 - asStr.size() % 8) % 8; ++i)
        trailingNulls += '0';

    std::string formattedStr = trailingNulls + asStr;
    size_t heapSize = static_cast<size_t>(formattedStr.size() / 8);
    _heap.resize(heapSize);
    for (size_t i = 0, j = heapSize - 1; i < formattedStr.size(); i += 8, --j)
        _heap[j] = static_cast<word>(stoul(formattedStr.substr(i, 8), nullptr, 16));
    removeLeadingZeros();
}

std::string BigInt::getStr(BigInt::NumberBase repr) const
{
    switch (repr) {
    case NumberBase::Bin:
        return getBinStr();
    case NumberBase::Hex:
        return getHexcStr();
    case NumberBase::Dec:
        return getDecStr();
    }
}

const std::vector<word>& BigInt::readHeap() const
{
    return _heap;
}

BigInt BigInt::operator*(const BigInt &right) const
{
    constexpr word max16bitWord = (1 << 16) - 1;
    if ((*this <= max16bitWord) and (right <= max16bitWord))
        return BigInt(1, _heap.front() * right.getHeap().front());

    size_t n = std::floor(std::max(bitsLen(), right.bitsLen()) / 2);
    const BigInt mask = (BigInt(1) << n) - 1;
    BigInt leftHigh = *this >> n;
    BigInt leftLow = *this & mask;
    BigInt rightHigh = right >> n;
    BigInt rightLow = right & mask;

    BigInt highPord = leftHigh * rightHigh;
    BigInt lowProd = leftLow * rightLow;
    BigInt crossProd = ((leftHigh + leftLow) * (rightHigh + rightLow)) - highPord - lowProd;
    return (((highPord << n) + crossProd) << n) + lowProd;
}

BigInt BigInt::operator%(const BigInt &modulo) const
{
    if (isZero())
        return 0;

    if (modulo.isZero())
        return *this;

    if (*this <  maxWord and modulo < maxWord)
        return _heap[0] % modulo.getHeap()[0];

    BigInt remainder = 0;
    BigInt bPowerT = BigInt(1) << modulo.bitsLen();
    BigInt c = bPowerT - modulo;
    auto[q0, r0] = divisionRemainder(bPowerT);
    BigInt q1;
    remainder = r0;
    BigInt r1;
    for (size_t i = 0; q0 > 0; ++i) {
        q1 = (q0 * c).divisionRemainder(bPowerT).first;
        r1 = q0 * c  - q1 * bPowerT;
        remainder = remainder + r0;
    }
    while (remainder >= modulo)
        remainder = remainder - modulo;

    return remainder;
}

BigInt BigInt::operator&(const BigInt &right) const
{
    std::vector<word> resultHeap(std::min(right.wordLen(), wordLen()), 0);
    for (size_t i = 0; i < std::min(right.wordLen(), wordLen()); ++i)
        resultHeap[i] = _heap[i] & right.readHeap()[i];

    return BigInt(std::move(resultHeap));
}

BigInt BigInt::operator|(const BigInt &right) const
{
    std::vector<word> resultHeap(std::max(right.wordLen(), wordLen()), 0);
    size_t minLen = std::min(right.wordLen(), wordLen());
    for (size_t i = 0; i < minLen; ++i)
        resultHeap[i] = _heap[i] | right.readHeap()[i];

    for (size_t i = minLen; i < right.wordLen(); ++i)
        resultHeap[i] |= right.readHeap()[i];

    for (size_t i = minLen; i < wordLen(); ++i)
        resultHeap[i] |= _heap[i];

    return BigInt(std::move(resultHeap));
}

BigInt BigInt::operator^(const BigInt &right) const
{
    std::vector<word> resultHeap(std::max(right.wordLen(), wordLen()), 0);
    size_t minLen = std::min(right.wordLen(), wordLen());
    for (size_t i = 0; i < minLen; ++i)
        resultHeap[i] = _heap[i] ^ right.readHeap()[i];

    for (size_t i = minLen; i < right.wordLen(); ++i)
        resultHeap[i] ^= right.readHeap()[i];

    for (size_t i = minLen; i < wordLen(); ++i)
        resultHeap[i] ^= _heap[i];

    return BigInt(std::move(resultHeap));
}

BigInt BigInt::operator~() const
{
    std::vector<word> resultHeap(wordLen());
    for (size_t i = 0; i < wordLen(); ++i)
        resultHeap[i] = ~_heap[i];

    BigInt result(std::move(resultHeap));
    for (long i = wordLen() * bitsInWord; i > static_cast<long>(bitsLen()); --i)
        result.setBitAt(i - 1, false);

    return result;
}

BigInt BigInt::operator>>(const size_t numOfShifts) const
{
    std::vector<word> resultHeap(_heap.begin(), _heap.end());
    for (size_t shiftIteration = 0; shiftIteration < numOfShifts; ++shiftIteration) {
        resultHeap.front() >>= 1;
        for (size_t i = 1; i < _heap.size(); ++i) {
            word carryMask = (word(1) & resultHeap[i]) << (bitsInWord - 1);
            resultHeap[i] >>= 1;
            resultHeap[i - 1] |= carryMask;
        }
    }
    return BigInt(std::move(resultHeap));
}

BigInt BigInt::operator<<(const size_t numOfShifts) const
{
    std::vector<word> resultHeap(_heap.begin(), _heap.end());
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

void BigInt::operator<<=(const size_t numOfShifts)
{
    for (size_t shiftIteration = 0; shiftIteration < numOfShifts; ++shiftIteration) {
        word carryMask = 0;
        for (size_t i = 0; i < _heap.size(); ++i) {
            word newMask = ((~(~word(0) >> 1)) & _heap[i]) >> (bitsInWord - 1);
            _heap[i] <<= 1;
            _heap[i] |= carryMask;
            carryMask = newMask;
        }
        if (carryMask)
            _heap.emplace_back(1);
    }
}

BigInt BigInt::operator+(const BigInt &right) const
{
    bool carry = false;
    BigInt result(std::max(wordLen(), right.wordLen()) + 1, 0);
    for (size_t i = 0; i < std::min(bitsLen(), right.bitsLen()); ++i) {
        bool leftBit = getBitAt(i);
        bool rightBit = right.getBitAt(i);
        result.setBitAt(i, leftBit ^ rightBit ^ carry);
        carry = (leftBit & rightBit) | (leftBit & carry) | (rightBit & carry);
    }
    for (size_t i = std::min(bitsLen(), right.bitsLen()); i < bitsLen(); ++i) {
        bool leftBit = getBitAt(i);
        result.setBitAt(i, leftBit ^ carry);
        carry &= leftBit;
    }
    for (size_t i = std::min(bitsLen(), right.bitsLen()); i < right.bitsLen(); ++i) {
        bool rightBit = right.getBitAt(i);
        result.setBitAt(i, rightBit ^ carry);
        carry &= rightBit;
    }
    result.bitsLen();
    result.setBitAt(std::max(bitsLen(), right.bitsLen()), carry);

    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::operator-(const BigInt &right) const
{
    if (*this < right)
        throw std::logic_error("This library can not handle negative values (yet)");

    if (*this == right)
        return 0;

    bool carry = false;
    BigInt result(std::max(wordLen(), right.wordLen()) + 1, 0);
    for (size_t i = 0; i < std::min(bitsLen(), right.bitsLen()); ++i) {
        bool leftBit = getBitAt(i);
        bool rightBit = right.getBitAt(i);
        result.setBitAt(i, leftBit ^ rightBit ^ carry);
        carry = (!leftBit & rightBit) | (!leftBit & carry) | (leftBit & rightBit & carry);
    }
    for (size_t i = std::min(bitsLen(), right.bitsLen()); i < bitsLen(); ++i) {
        bool leftBit = getBitAt(i);
        result.setBitAt(i, leftBit ^ carry);
        carry &= !leftBit;
    }

    result.removeLeadingZeros();
    return result;
}

bool BigInt::operator==(const BigInt &right) const
{
    return BigInt(*this ^ right).isZero();
}

bool BigInt::operator<(const BigInt &right) const
{
    if (bitsLen() != right.bitsLen())
        return bitsLen() < right.bitsLen();

    for (size_t i = bitsLen(); i > 0; --i) {
        bool leftBit = getBitAt(i - 1);
        bool rightBit = right.getBitAt(i - 1);
        if (leftBit != rightBit)
            return leftBit < rightBit;
    }
    return false;
}

bool BigInt::operator<=(const BigInt &right) const
{
    return (*this == right) or (*this < right);
}

bool BigInt::operator>(const BigInt &right) const
{
    return not (*this < right) and not (*this == right);
}

bool BigInt::operator>=(const BigInt &right) const
{
    return (*this == right) or (*this > right);
}

BigInt BigInt::gcd(const BigInt &right)
{
    BigInt resultingLeft = *this;
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

std::pair<BigInt, BigInt> BigInt::divisionRemainder(const BigInt &denominator) const
{
    if (denominator.isZero())
        throw std::logic_error("Division by zero is impossible");

    if ((*this <= BigInt(1, maxWord)) and (denominator <= BigInt(1, maxWord)))
        return {BigInt(1, static_cast<word>(_heap[0] / denominator.getHeap()[0])), BigInt(1, _heap[0] % denominator.getHeap()[0])};

    BigInt quotient = 0;
    BigInt remainder = 0;
    for (long i = bitsLen() - 1; i >= 0; --i) {
        remainder = remainder << 1;
        remainder.setBitAt(0, getBitAt(i));
        if (remainder >= denominator) {
            remainder = remainder - denominator;
            quotient.setBitAt(i, true);
        }
    }
    quotient.removeLeadingZeros();
    remainder.removeLeadingZeros();
    return {quotient, remainder};
}

BigInt BigInt::kAryLRExp(const BigInt &exponent)
{
    if (_table.empty())
        generateExpTable();

    if (*this < 2)
        return *this;

    std::vector<word> kAryWindows;
    kAryWindows.reserve(std::ceil(exponent.bitsLen() / _expConstantK));
    for (size_t i = 0; i < exponent.bitsLen(); i += _expConstantK)
        kAryWindows.push_back(((exponent >> i) & (~(~word(0) << _expConstantK))).getHeap().front());
    std::reverse(kAryWindows.begin(), kAryWindows.end());

    BigInt result = _table.at(kAryWindows.front());

    for (size_t i = 1; i < kAryWindows.size(); ++i) {
        for (size_t j = 0; j < _expConstantK; ++j)
            result = result * result;

        result = result * _table[kAryWindows[i]];
    }
    return result;
}

BigInt BigInt::binaryLRExp(const BigInt &exponent)
{
    BigInt result = 1;
    for (size_t i = exponent.bitsLen(); i > 0; --i) {
        result = result * result;
        if (exponent.getBitAt(i - 1) == true)
            result = result * (*this);
    }
    return result;
}

BigInt BigInt::binaryRLExp(const BigInt &exponent)
{
    BigInt a = 1;
    BigInt s = *this;
    BigInt e = exponent;
    while (not e.isZero()) {
        if (e.getBitAt(0) == true)
            a = a * s;
        e = e.divisionRemainder(BigInt(2)).first;
        if (not e.isZero())
            s = s * s;
    }
    return a;
}

BigInt BigInt::binarySWExp(const BigInt &exponent)
{
    if (_table.empty())
        generateExpTable();

    BigInt result = 1;
    for (int32_t i = exponent.bitsLen() - 1; i >= 0;) {
        if (exponent.getBitAt(i) == false) {
           result = result * result;
           --i;
        } else {
            word s = std::max(static_cast<int32_t>(i - _expConstantK + 1), 0);
            while (exponent.getBitAt(s) == false)
                ++s;

            for (word h = 0; h < i - s + 1; ++h)
                result = result * result;

            BigInt tmp = (exponent >> s) & (~((~word(0)) << (i - s + 1)));
            word u = tmp.getHeap().front();
            result = result * _table.at(u);
            i = s - 1;
        }
    }
    return result;
}

void BigInt::generateExpTable()
{
    _table.clear();
    _table.resize((1 << _expConstantK));
    _table[0] = 1;
    for (size_t i = 1; i < (1 << _expConstantK); ++i)
        _table[i] = (*this) * _table[i - 1];
}

size_t BigInt::bitsLen() const
{
    if (_heap.empty())
        return 0;

    size_t numberOfbits = (wordLen()) * bitsInWord;

    word lastWord = _heap.back();
    for (long i = _heap.size(); i > 1 and _heap[i - 1] == 0; --i) {
        numberOfbits -= bitsInWord;
        lastWord = _heap[i - 2];
    }

    for (size_t i = 0; i < bitsInWord and (lastWord & ~(~word(0) >> 1)) == 0; ++i) {
        lastWord <<= 1;
        --numberOfbits;
    }
    return numberOfbits;
}

bool BigInt::getBitAt(size_t index) const
{
    if (index > bitsLen())
        throw std::logic_error("Bad index");

    auto wordNum = static_cast<size_t>(std::floor(index / bitsInWord));
    size_t offset = index % bitsInWord;

    return (_heap[wordNum] >> offset) & word(1);
}

void BigInt::setBitAt(size_t index, bool value)
{
    if (index >= _heap.size() * bitsInWord)
        _heap.resize(_heap.size() + std::ceil((index + 1) / bitsInWord));

    auto wordNum = static_cast<size_t>(std::floor(index / bitsInWord));
    size_t offset = index % bitsInWord;
    if (value) {
        word mask = word(1) << offset;
        _heap[wordNum] |= mask;
    } else {
        word mask = ~(word(1) << offset);
        _heap[wordNum] &= mask;
    }
}

std::string BigInt::getDecStr() const
{
    std::string result;
    constexpr word maxDecDivisibleWord = 1000000000;
    BigInt base(1, maxDecDivisibleWord);
    BigInt numerator = *this;
    std::vector<std::string> parts;
    parts.reserve(_heap.size());

    std::pair<BigInt, BigInt> quotientRemainder;
    for (size_t i = 0; numerator >= base; ++i) {
        quotientRemainder = numerator.divisionRemainder(base);
        parts.emplace_back(fmt::format("{:0>9d}", quotientRemainder.second.getHeap().front()));
        numerator = quotientRemainder.first;
    }
    quotientRemainder = numerator.divisionRemainder(base);
    parts.emplace_back(fmt::format("{0:d}", quotientRemainder.second.getHeap().front()));
    std::reverse(parts.begin(), parts.end());
    for (const std::string &part : parts)
        result += part;

    return result;
}

std::string BigInt::getHexcStr() const
{
    std::string result;
    result += fmt::format("{0:x}", _heap.back());
    for (auto it = _heap.rbegin() + 1; it != _heap.rend(); ++it)
        result += fmt::format("{:0>8x}", *it);
    return result;
}

std::string BigInt::getBinStr() const
{
    std::string result;
    result += fmt::format("{0:b}", _heap.back());
    for (auto it = _heap.rbegin() + 1; it != _heap.rend(); ++it)
        result += fmt::format("{:0>32b}", *it);
    return result;
}

word BigInt::getExpConstantK() const
{
    return _expConstantK;
}

void BigInt::setExpConstantK(const word& expConstantK)
{
    _expConstantK = expConstantK;
}

BigInt bigMax(const BigInt &left, const BigInt &right)
{
    return left >= right ? left : right;
}
