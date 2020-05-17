#ifndef BIGINT_H
#define BIGINT_H

#include <vector>
#include <string>
#include <cstdint>

#include <iostream>

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
    BigInt(const std::string &asStr, NumberBase base = Hex);
    BigInt(std::vector<word>&& heap);
    BigInt(const BigInt& left) = default;

    void setHexStr(const std::string &asStr);
    void setDecStr(const std::string &asStr);
    void setBinStr(const std::string &asStr);

    std::string getStr(NumberBase repr = NumberBase::Dec) const;
    const std::vector<word>& readHeap() const;

    BigInt operator>>=(const size_t mumOfShifts) const;
    void operator<<=(const size_t numOfShifts);

    BigInt kAryLRExp(const BigInt &exponent);
    BigInt binaryLRExp(const BigInt& exponent);
    BigInt binaryRLExp(const BigInt& exponent);
    BigInt binarySWExp(const BigInt& exponent);
    void generateExpTable();

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

    inline const std::vector<word> &getHeap() const
    {
        return _heap;
    }

    inline void removeLeadingZeros()
    {
        size_t newSize = _heap.size();
        for (size_t i = _heap.size() - 1; i > 0 and _heap[i] == 0; --i)
            --newSize;
        _heap.resize(newSize);
    }

    word getExpConstantK() const;
    void setExpConstantK(const word& expConstantK);

private:
    std::string getDecStr() const;
    std::string getHexcStr() const;
    std::string getBinStr() const;

    std::vector<BigInt> _table;
    word _expConstantK = 3;
    std::vector<word> _heap;
};

#endif // BIGINT_H
