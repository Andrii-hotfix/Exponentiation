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
        // So I can check this funct only by bitwise &, which is also may be unreliable, but
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
        for (int j = 0; j < 500; ++j) {
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
        mpz_class gmpBigNum = randomMachine.get_z_bits(70);
        BigInt myBigNum(gmpBigNum.get_str(16));

        std::cout << gmpBigNum.get_str(2) << " vs " << myBigNum.getStr(BigInt::Bin) << std::endl;
        gmpBigNum <<= 1;
        myBigNum = myBigNum << 1;
        std::cout << gmpBigNum.get_str(2) << " vs " << myBigNum.getStr(BigInt::Bin) << std::endl;
//        std::default_random_engine gen;
//        std::uniform_int_distribution<size_t> distr(0, i);
//        for (int j = 0; j < 1000; ++j) {
//            size_t randomShift = distr(gen);

//            gmpBigNum <<= randomShift;
//            myBigNum = myBigNum << randomShift;

//            ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr(BigInt::Hex));
//        }

        ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr(BigInt::Hex));
    }
}
