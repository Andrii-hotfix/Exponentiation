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
    BigInt(size_t size);
    BigInt(const std::string &asStr, NumberBase repr = NumberBase::Dec);
    // TODO copy constr?

    void setStr(const std::string &asStr, NumberBase repr = NumberBase::Dec);

    std::string getStr(NumberBase repr = NumberBase::Dec) const;
    const std::vector<word>& getHeap() const;

    BigInt operator*(const BigInt &right) const;
    BigInt operator%(const BigInt &right) const;
    BigInt operator=(const BigInt &right) const;
    BigInt operator&(const BigInt &right) const;
    BigInt operator|(const BigInt &right) const;
    BigInt operator^(const BigInt &right) const;

    bool operator==(const BigInt &right) const;

    void operator*=(const BigInt &right);
    void operator%=(const BigInt &right);
    void operator&=(const BigInt &right);
    void operator|=(const BigInt &right);

private:
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
