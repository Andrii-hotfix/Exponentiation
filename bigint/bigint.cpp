#include "bigint.h"

#include <fmt/core.h>

#include <stdexcept>
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "bigintfunct.h"

BigInt::BigInt(uint32_t value)
{
    _heap.clear();
    _heap.emplace_back(value);
}

BigInt::BigInt(size_t size, uint32_t value)
{
    _heap.resize(size, value);
}

BigInt::BigInt(const std::string& asStr, Radix base)
{
    setStr(asStr, base);
}

BigInt::BigInt(std::vector<word>&& heap)
{
    _heap.insert(_heap.begin(), std::make_move_iterator(heap.begin()), std::make_move_iterator(heap.end()));
    _table.clear();
    removeLeadingZeros();
}

void BigInt::setStr(const std::string &asStr, BigInt::Radix base)
{
    if (base == BigInt::Hex)
        setHexStr(asStr);
    else if (base == BigInt::Dec)
        setDecStr(asStr);
    else if (base == BigInt::Bin)
        setBinStr(asStr);
}

std::string BigInt::getStr(BigInt::Radix repr) const
{
    switch (repr) {
    case Radix::Bin:
        return getBinStr();
    case Radix::Hex:
        return getHexStr();
    case Radix::Dec:
        return getDecStr();
    }
    return getHexStr();
}

const std::vector<word>& BigInt::readHeap() const
{
    return _heap;
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

BigInt BigInt::mAryLRExp(const BigInt& exponent)
{
    if (_table.empty())
        generateExpTable();

    if (*this == 0 and exponent == 0)
        return 1;

    if (*this < BigInt(2))
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

BigInt BigInt::binaryLRExp(const BigInt& exponent)
{
    BigInt result = 1;
    for (size_t i = exponent.bitsLen(); i > 0; --i) {
        result = result * result;
        if (exponent.getBitAt(i - 1) == true)
            result = result * (*this);
    }
    return result;
}

BigInt BigInt::binaryRLExp(const BigInt& exponent)
{
    BigInt a = 1;
    BigInt s = *this;
    BigInt e = exponent;
    while (not e.isZero()) {
        if (e.getBitAt(0) == true)
            a = a * s;
        e = divisionRemainder(e, BigInt(2)).first;
        if (not e.isZero())
            s = s * s;
    }
    return a;
}

BigInt BigInt::binarySWExp(const BigInt& exponent)
{
    if (_table.empty())
        generateExpTable();

    if (_heap.size() == 1 and exponent.wordLen() == 1)
        return std::pow(_heap.front(), exponent.getHeap().front());

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

word BigInt::getExpConstantK() const
{
    return _expConstantK;
}

void BigInt::setExpConstantK(const word& expConstantK)
{
    _expConstantK = expConstantK;
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
        quotientRemainder = divisionRemainder(numerator, base);
        parts.emplace_back(fmt::format("{:0>9d}", quotientRemainder.second.getHeap().front()));
        numerator = quotientRemainder.first;
    }
    quotientRemainder = divisionRemainder(numerator, base);
    parts.emplace_back(fmt::format("{0:d}", quotientRemainder.second.getHeap().front()));
    std::reverse(parts.begin(), parts.end());
    for (const std::string &part : parts)
        result += part;

    return result;
}

std::string BigInt::getHexStr() const
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

void BigInt::setHexStr(const std::string& asStr)
{
    _heap.clear();
    // Add leading 0 digits if any needed
    std::string trailingNulls;
    constexpr size_t digitsBase = 8;
    for (size_t i = 0; i < (digitsBase - asStr.size() % digitsBase) % digitsBase; ++i)
        trailingNulls += '0';

    std::string formattedStr = trailingNulls + asStr;
    size_t heapSize = static_cast<size_t>(formattedStr.size() / digitsBase);
    _heap.resize(heapSize);
    for (size_t i = 0, j = heapSize - 1; i < formattedStr.size(); i += digitsBase, --j)
        _heap[j] = static_cast<word>(stoul(formattedStr.substr(i, digitsBase), nullptr, 16));

    removeLeadingZeros();
}

void BigInt::setDecStr(const std::string& asStr)
{
    _heap.clear();
    std::string trailingNulls;
    constexpr int32_t digitsBase = 8;
    for (size_t i = 0; i < (digitsBase - asStr.size() % digitsBase) % digitsBase; ++i)
        trailingNulls += '0';

    std::string formattedStr = trailingNulls + asStr;
    for (int32_t i = formattedStr.size() - 1, j = 0; i - digitsBase + 1 >= 0; i -= digitsBase, j += digitsBase) {
        BigInt multiplier = BigInt(10).binarySWExp(j);
        std::string subs = formattedStr.substr(i - digitsBase + 1, digitsBase);
        auto foo = static_cast<word>(stoul(subs, nullptr, 10));
        *this = *this + (foo * multiplier);
    }
}

void BigInt::setBinStr(const std::string& asStr)
{
    _heap.clear();
    // Add leading 0 digits if any needed
    std::string trailingNulls;
    constexpr size_t digitsBase = 32;
    for (size_t i = 0; i < (digitsBase - asStr.size() % digitsBase) % digitsBase; ++i)
        trailingNulls += '0';

    std::string formattedStr = trailingNulls + asStr;
    size_t heapSize = static_cast<size_t>(formattedStr.size() / digitsBase);
    _heap.resize(heapSize);
    for (size_t i = 0, j = heapSize - 1; i < formattedStr.size(); i += digitsBase, --j) {
        try {
            _heap[j] = static_cast<word>(stoul(formattedStr.substr(i, digitsBase), nullptr, 2));
        } catch (std::exception &err) {
            std::cerr << err.what() << std::endl;
        }
    }

    removeLeadingZeros();
}
