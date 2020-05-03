#include "../bigint.h"

#include <gtest/gtest.h>
#include <gmpxx.h>

#include <iostream>
#include <cstdlib>
#include <random>

TEST(BigIntFunct, SimpleStrings)
{
    gmp_randclass randomMachine(gmp_randinit_default);

    for (size_t i = 124; i < 2048; ++i) {
        mpz_class gmpBigNum = randomMachine.get_z_bits(i);
        BigInt myBigNum(gmpBigNum.get_str(16));

        ASSERT_EQ(gmpBigNum.get_str(2).size(), myBigNum.bitsLen());
        ASSERT_TRUE(std::string(gmpBigNum.get_str(2)) == myBigNum.getStr(BigInt::Bin));
        ASSERT_TRUE(std::string(gmpBigNum.get_str(10)) == myBigNum.getStr(BigInt::Dec));
    }
}

TEST(BigIntFunct, And)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    for (size_t i = 124; i < 2048; ++i) {
        mpz_class gmpLeft = randomMachine.get_z_bits(i);
        mpz_class gmpRight = randomMachine.get_z_bits(i);

        BigInt leftBigNum(gmpLeft.get_str(16));
        BigInt rightBigNum(gmpRight.get_str(16));

        mpz_class andGmpResult = gmpLeft & gmpRight;
        BigInt andBigIntResult = leftBigNum & rightBigNum;

        ASSERT_TRUE(std::string(andGmpResult.get_str(16)) == andBigIntResult.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, Or)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    for (size_t i = 124; i < 2048; ++i) {
        mpz_class gmpLeft = randomMachine.get_z_bits(i);
        mpz_class gmpRight = randomMachine.get_z_bits(i);

        BigInt leftBigNum(gmpLeft.get_str(16));
        BigInt rightBigNum(gmpRight.get_str(16));

        mpz_class andGmpResult = gmpLeft | gmpRight;
        BigInt andBigIntResult = leftBigNum | rightBigNum;

        ASSERT_TRUE(std::string(andGmpResult.get_str(16)) == andBigIntResult.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, Xor)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    for (size_t i = 124; i < 2048; ++i) {
        mpz_class gmpLeft = randomMachine.get_z_bits(i);
        mpz_class gmpRight = randomMachine.get_z_bits(i);

        BigInt leftBigNum(gmpLeft.get_str(16));
        BigInt rightBigNum(gmpRight.get_str(16));

        mpz_class xorGmpResult = gmpLeft ^ gmpRight;
        BigInt xorBigIntResult = leftBigNum ^ rightBigNum;

        ASSERT_TRUE(std::string(xorGmpResult.get_str(16)) == xorBigIntResult.getStr(BigInt::Hex));

    }
}

TEST(BigIntFunct, Complementary)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    for (size_t i = 124; i < 2048; ++i) {
        // As mpz_class does not have any simple complementary
        // (mpz_t can be any bits long, so they care about leading zeros, but I don't :D).
        // So I can check this funct only using bitwise &, which is also may be unreliable, but
        // checked previously with mpz_t thus must be reliable anyway.
        mpz_class gmpRand = randomMachine.get_z_bits(i);
        BigInt bigNumRand(gmpRand.get_str(16));
        BigInt randCompl = ~bigNumRand;
        BigInt zero = bigNumRand & randCompl;

        ASSERT_TRUE(zero.isZero());
    }
}

TEST(BigIntFunct, ShiftRight)
{
    gmp_randclass randomMachine(gmp_randinit_default);

    for (size_t i = 124; i < 2048; ++i) {
        mpz_class gmpBigNum = randomMachine.get_z_bits(i);
        BigInt myBigNum(gmpBigNum.get_str(16));

        std::default_random_engine gen;
        std::uniform_int_distribution<size_t> distr(0, i);
        for (int j = 0; j < 10; ++j) {
            size_t randomShift = distr(gen);

            gmpBigNum >>= randomShift;
            myBigNum = myBigNum >> randomShift;

            ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr(BigInt::Hex));
        }
    }
}

TEST(BigIntFunct, ShiftLeft)
{
    gmp_randclass randomMachine(gmp_randinit_default);

    for (size_t i = 124; i < 2048; ++i) {
        mpz_class gmpBigNum = randomMachine.get_z_bits(i);
        BigInt myBigNum(gmpBigNum.get_str(16));

        std::default_random_engine gen;
        std::uniform_int_distribution<size_t> distr(0, i);
        for (int j = 0; j < 10; ++j) {
            size_t randomShift = distr(gen);
            gmpBigNum <<= randomShift;
            myBigNum = myBigNum << randomShift;

            ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr(BigInt::Hex));
        }
    }
}

TEST(BigIntFunct, BitPositionGetSet)
{
    constexpr size_t numberOfBits = 100;
    gmp_randclass randomMachine(gmp_randinit_default);
    mpz_class gmpBigNum = randomMachine.get_z_bits(numberOfBits);
    BigInt myBigNum(gmpBigNum.get_str(16));

    BigInt setted(myBigNum.wordLen(), 1);
    for (size_t i = 0; i < myBigNum.bitsLen(); ++i) {
        ASSERT_EQ(myBigNum.getBitAt(i), (gmpBigNum >> i) & 1);
        setted.setBitAt(i, myBigNum.getBitAt(i));
    }
    ASSERT_TRUE(setted == myBigNum);
}

TEST(BigIntFunct, Comparisons)
{
    constexpr size_t numberOfBits = 256;
    gmp_randclass randomMachine(gmp_randinit_default);
    for (size_t i = 0; i < 500; ++i) {
        mpz_class left = randomMachine.get_z_bits(numberOfBits);
        mpz_class right = randomMachine.get_z_bits(numberOfBits);

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));

        ASSERT_EQ(left == right, myLeft == myRight);
        ASSERT_EQ(left < right, myLeft < myRight);
        ASSERT_EQ(left <= right, myLeft <= myRight);
        ASSERT_EQ(left > right, myLeft > myRight);
        ASSERT_EQ(left >= right, myLeft >= myRight);
    }
}

TEST(BigIntFunct, Addition)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    for (size_t i = 0; i < 500; ++i) {
        std::uniform_int_distribution<size_t> distr(0, i);
        mpz_class left = randomMachine.get_z_bits(distr(gen));
        mpz_class right = randomMachine.get_z_bits(distr(gen));

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));

        mpz_class sum = left + right;
        BigInt mySum = myLeft + myRight;

        ASSERT_TRUE(std::string(sum.get_str(16)) == mySum.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, Substraction)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    for (size_t i = 0; i < 500; ++i) {
        std::uniform_int_distribution<size_t> distr(0, i);
        mpz_class right = randomMachine.get_z_bits(distr(gen));
        mpz_class left = right + randomMachine.get_z_bits(distr(gen));

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));

        mpz_class diff = left - right;
        try {
            BigInt myDiff = myLeft - myRight;
            ASSERT_TRUE(std::string(diff.get_str(16)) == myDiff.getStr(BigInt::Hex));
        } catch (const std::exception &err) {
            EXPECT_STREQ(err.what(), "This library can not hold negative values (yet)");
            EXPECT_TRUE(left < right);
        }
    }
}

TEST(BigIntFunct, DivisionRemainder)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    for (size_t i = 2; i < 500; ++i) {
        std::uniform_int_distribution<size_t> distr(1, i);
        mpz_class left = randomMachine.get_z_bits(distr(gen));
        mpz_class right = randomMachine.get_z_bits(distr(gen));
        if (right == 0)
            continue;
        mpz_class quotient, remainder;

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));

        mpz_fdiv_qr(quotient.get_mpz_t(), remainder.get_mpz_t(), left.get_mpz_t(), right.get_mpz_t());
        auto[myQuotient, myRemainder] = myLeft.divisionRemainder(myRight);

        ASSERT_TRUE(std::string(quotient.get_str(16)) == myQuotient.getStr(BigInt::Hex));
        ASSERT_TRUE(std::string(remainder.get_str(16)) == myRemainder.getStr(BigInt::Hex));
    }
}
