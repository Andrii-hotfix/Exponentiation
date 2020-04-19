#include "bigint.h"

#include <fmt/core.h>

#include <stdexcept>
#include <array>
#include <algorithm>

BigInt::BigInt(size_t size, word val)
{
    _heap.resize(size, val);
}

BigInt::BigInt(const std::string &asStr, BigInt::NumberBase repr)
{
    setStr(asStr, repr);
}

BigInt::BigInt(std::vector<word>&& heap)
{
    _heap.insert(_heap.begin(), std::make_move_iterator(heap.begin()), std::make_move_iterator(heap.end()));
}

void BigInt::setStr(const std::string &asStr, BigInt::NumberBase repr)
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
}

std::string BigInt::getStr(BigInt::NumberBase repr) const
{
    std::string result;
    // No trailing 0-digits
    result += fmt::format("{0:x}", _heap.back());
    for (auto it = _heap.rbegin() + 1; it != _heap.rend(); ++it)
        result += fmt::format("{:0>8x}", *it);
    return result;
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

    // Remove leading zeros
    size_t newSize = resultHeap.size();
    for (size_t i = resultHeap.size() - 1; i > 0 and resultHeap[i] == 0; --i)
        --newSize;
    resultHeap.resize(newSize);

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

    // Remove leading zeros
    size_t newSize = resultHeap.size();
    for (size_t i = resultHeap.size() - 1; i > 0 and resultHeap[i] == 0; --i)
        --newSize;
    resultHeap.resize(newSize);

    return BigInt(std::move(resultHeap));
}

size_t BigInt::wordLen() const
{
    return _heap.size();
}

void BigInt::resize(size_t newSize)
{
    _heap.resize(newSize, 0);
}
