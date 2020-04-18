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
    for (size_t i = 0; i < formattedStr.size(); i += 8)
        _heap.push_back(static_cast<word>(stoul(formattedStr.substr(i, 8), nullptr, 16)));
}

std::string BigInt::getStr(BigInt::NumberBase repr) const
{
    std::string result;
    // No trailing 0-digits
    result += fmt::format("{:>x}", _heap.front());
    for (size_t i = 1; i < _heap.size();  ++i)
        result += fmt::format("{:0>8x}", _heap[i]);
    return result;
}

const std::vector<word>& BigInt::readHeap() const
{
    return _heap;
}

BigInt BigInt::operator&(const BigInt &right) const
{
    std::vector<word> resultHeap(std::max(right.wordLen(), wordLen()), 0);

    long sizeDiff = std::labs(static_cast<long long>(right.wordLen() - wordLen()));
    for (size_t i = resultHeap.size(), j = std::min; i > static_cast<size_t>(sizeDiff); --i)
        resultHeap[i - 1] = _heap[i - 1] & right.readHeap()[i - 1];
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
