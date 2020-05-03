#ifndef BIGINT_H
#define BIGINT_H

#include <vector>
#include <string>
#include <cstdint>
#include <bitset>

using word = uint32_t;

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
    BigInt(size_t size, word val);
    BigInt(const std::string &asStr);
    BigInt(std::vector<word>&& heap);
    BigInt(const BigInt& left) = default;
    // TODO copy constr?

    void setHexStr(const std::string &asStr);

    std::string getStr(NumberBase repr = NumberBase::Dec) const;
    const std::vector<word>& readHeap() const;

    BigInt operator*(const BigInt &right) const;
    BigInt operator%(const BigInt &right) const;
    BigInt operator&(const BigInt &right) const;
    BigInt operator|(const BigInt &right) const;
    BigInt operator^(const BigInt &right) const;
    BigInt operator~() const;
    BigInt operator>>(const size_t numOfShifts) const;
    BigInt operator<<(const size_t numOfShifts) const;

    BigInt operator+(const BigInt &right) const;
    BigInt operator-(const BigInt &right) const;
    std::pair<BigInt, BigInt> divisionRemainder(const BigInt &denominator);

    bool operator==(const BigInt &right) const;
    bool operator<(const BigInt &right) const;
    bool operator<=(const BigInt &right) const;
    bool operator>(const BigInt &right) const;
    bool operator>=(const BigInt &right) const;

    void operator*=(const BigInt &right);
    void operator%=(const BigInt &right);
    void operator&=(const BigInt &right);
    void operator|=(const BigInt &right);

    size_t bitsLen() const;
    bool getBitAt(size_t index) const;
    void setBitAt(size_t index, bool value);

    inline size_t wordLen() const
    {
        return _heap.size();
    }

    inline bool isZero()
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
//std::vector<uint8_t> str2Bytes(const std::string byteStr)
//{
//    std::vector<uint8_t> bytes;
//    for (size_t i = 0, j = 0; i < byteStr.size(); i += 2, ++j)
//        bytes.push_back(static_cast<uint8_t>(stoul(byteStr.substr(i, 2), nullptr, 16)));
//    return bytes;
//}
    std::vector<word> _heap;
};

#endif // BIGINT_H
