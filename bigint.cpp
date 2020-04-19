#include "bigint.h"

#include <fmt/core.h>

#include <stdexcept>
#include <array>
#include <algorithm>

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

void BigInt::smallDivisionAlg(word divisor, word& quotient, word& remainder)
{
    quotient = 0;
    remainder = 0;

    for (size_t i = bitsLen() - 1; i > 0; ++i) {
        remainder <<= 1;
    }
}

size_t BigInt::wordLen() const
{
    return _heap.size();
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

bool BigInt::isZero()
{
    return _heap.size() == 1 and _heap.front() == 0;
}

void BigInt::resize(size_t newSize)
{
    _heap.resize(newSize, 0);
}

void BigInt::removeLeadingZeros()
{
    size_t newSize = _heap.size();
    for (size_t i = _heap.size() - 1; i > 0 and _heap[i] == 0; --i)
        --newSize;
    _heap.resize(newSize);
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
