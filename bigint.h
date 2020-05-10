#ifndef BIGINT_H
#define BIGINT_H

#include <vector>
#include <string>
#include <cstdint>

using word = uint32_t;
constexpr size_t bitsInWord = 32;
constexpr word maxWord = ~word(0);

class BigInt
{
public:
    enum NumberBase
    {
        Hex = 16,
        Dec = 10,
        Bin = 2
    };

    BigInt() = default;
    BigInt(word value);
    BigInt(size_t size, word value);
    BigInt(const std::string &asStr);
    BigInt(std::vector<word>&& heap);
    BigInt(const BigInt& left) = default;

    void setHexStr(const std::string &asStr);

    std::string getStr(NumberBase repr = NumberBase::Dec) const;
    const std::vector<word>& readHeap() const;

    BigInt operator*(const BigInt &right) const;
    BigInt operator%(const BigInt &modulo) const;
    BigInt operator&(const BigInt &right) const;
    BigInt operator|(const BigInt &right) const;
    BigInt operator^(const BigInt &right) const;
    BigInt operator~() const;
    BigInt operator>>(const size_t numOfShifts) const;
    BigInt operator>>=(const size_t mumOfShifts) const;
    BigInt operator<<(const size_t numOfShifts) const;
    void operator<<=(const size_t numOfShifts);

    BigInt operator+(const BigInt &right) const;
    BigInt operator-(const BigInt &right) const;
    std::pair<BigInt, BigInt> divisionRemainder(const BigInt &denominator) const;

    bool operator==(const BigInt &right) const;
    bool operator<(const BigInt &right) const;
    bool operator<=(const BigInt &right) const;
    bool operator>(const BigInt &right) const;
    bool operator>=(const BigInt &right) const;

    void operator*=(const BigInt &right);
    void operator%=(const BigInt &right);
//    void operator&=(const BigInt &right);
    void operator|=(const BigInt &right);

    BigInt gcd(const BigInt &right);

    size_t bitsLen() const;
    bool getBitAt(size_t index) const;
    void setBitAt(size_t index, bool value);

    inline size_t wordLen() const
    {
        return _heap.size();
    }

    inline bool isZero() const
    {
        return _heap.size() == 1 and _heap.front() == 0;
    }

    inline void resize(size_t newSize)
    {
        _heap.resize(newSize, 0);
    }

private:
    inline void removeLeadingZeros()
    {
        size_t newSize = _heap.size();
        for (size_t i = _heap.size() - 1; i > 0 and _heap[i] == 0; --i)
            --newSize;
        _heap.resize(newSize);
    }

    inline const std::vector<word> &getHeap() const
    {
        return _heap;
    }

    std::string getDecStr() const;
    std::string getHexcStr() const;
    std::string getBinStr() const;
    std::vector<word> _heap;
};

#endif // BIGINT_H
