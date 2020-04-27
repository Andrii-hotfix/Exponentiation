#include "bigint.h"

#include <fmt/core.h>

#include <stdexcept>
#include <array>
#include <algorithm>
#include <cmath>

BigInt::BigInt(size_t size, word val)
{
    _heap.resize(size, val);
    removeLeadingZeros();
}

BigInt::BigInt(const std::string &asStr)
{
    setHexStr(asStr);
}

BigInt::BigInt(std::vector<word>&& heap)
{
    _heap.insert(_heap.begin(), std::make_move_iterator(heap.begin()), std::make_move_iterator(heap.end()));
    removeLeadingZeros();
}

void BigInt::setHexStr(const std::string &asStr)
{
    _heap.clear();
    // Add trailing 0 digits if any needed
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
    std::string result;
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

    return BigInt(std::move(resultHeap));
}

BigInt BigInt::operator>>(const size_t numOfShifts) const
{
    std::vector<word> resultHeap(_heap.begin(), _heap.end());
    for (size_t shiftIteration = 0; shiftIteration < numOfShifts; ++shiftIteration) {
        resultHeap.front() >>= 1;
        for (size_t i = 1; i < _heap.size(); ++i) {
            word carryMask = (word(1) & resultHeap[i]) << 31;
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
            word newMask = ((~(~word(0) >> 1)) & resultHeap[i]) >> 31;
            resultHeap[i] <<= 1;
            resultHeap[i] |= carryMask;
            carryMask = newMask;
        }
        if (carryMask)
            resultHeap.emplace_back(1);
    }
    return BigInt(std::move(resultHeap));
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

bool BigInt::operator>(const BigInt &right) const
{
    return !(*this < right) and !(*this == right);
}

std::pair<BigInt, BigInt> BigInt::divisionRemainder(const BigInt &denominator)
{
    BigInt quotient;
    BigInt remainder;

    for (size_t i = bitsLen(); i > 0; --i) {
        remainder = remainder << 1;
        remainder.setBitAt(0, getBitAt(i));
    }
}

void BigInt::smallDivisionAlg(word divisor, word& quotient, word& remainder)
{
    quotient = 0;
    remainder = 0;

    for (size_t i = bitsLen() - 1; i > 0; ++i) {
        remainder <<= 1;
    }
}

size_t BigInt::bitsLen() const
{
    size_t numberOfbits = (wordLen()) * 32;

    word lastWord = _heap.back();
    for (size_t i = 0; i < 32 and (lastWord & ~(~word(0) >> 1)) == 0; ++i) {
        lastWord <<= 1;
        --numberOfbits;
    }
    return numberOfbits;
}

bool BigInt::getBitAt(size_t index) const
{
    if (index > bitsLen())
        throw std::logic_error("Bad index");

    constexpr size_t bitsInWord = 32;
    auto wordNum = static_cast<size_t>(std::floor(index / bitsInWord));
    size_t offset = index % bitsInWord;

    return (_heap[wordNum] >> offset) & word(1);
}

void BigInt::setBitAt(size_t index, bool value)
{
    if (index > bitsLen())
        throw std::logic_error("Bad index");

    constexpr size_t bitsInWord = 32;
    auto wordNum = static_cast<size_t>(std::floor(index / bitsInWord));
    size_t offset = index % bitsInWord;
    if (value) {
        word mask = word(1) << offset;
        _heap[wordNum] |= mask;
    } else {
        word mask = ~(word(1) << offset);
        _heap[wordNum] &= mask;
    }
    removeLeadingZeros();
}

std::string BigInt::getDecStr() const
{
    std::string result;
    std::string chunk = std::to_string(_heap.back());
//    word base = 0;
//    for (size_t i = 0; base < 10; ++i)
//        base =
    return "";
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
