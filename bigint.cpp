#include "bigint.h"

#include <fmt/core.h>

#include <stdexcept>
#include <array>

BigInt::BigInt(size_t size)
{
    _heap.resize(size);
}

BigInt::BigInt(const std::string &asStr, BigInt::NumberBase repr)
{
    setStr(asStr, repr);
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

void BigInt::operator>>(size_t n)
{
    for (size_t i = 1; i < n; ++i) {
        _heap.back() >>= 1;
        for (size_t j = _heap.size() - 2; j > 0; --j) {
            _heap[j + 1] |= ~(~word(0) >> (_heap[j] & word(1)));
            _heap[j] >>= 1;
        }
    }
}

void BigInt::operator<<(size_t n)
{
     for (size_t i = 1; i < n; ++i) {
        _heap.front() <<= 1;
        for (size_t j = 1; j < _heap.size(); ++j) {
            _heap[j - 1] |= ~(~word(0) << (_heap[j] & ~(~uint8_t(0) >> 1)));
            _heap[j] <<= 1;
        }
    }
}

void BigInt::operator~()
{
    for (word &chunk : _heap)
        chunk = ~chunk;
}
