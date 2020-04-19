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

//TEST(BigIntFunct, ShiftRight)
//{
//    gmp_randclass randomMachine(gmp_randinit_default);
//    std::default_random_engine gen;
//    std::uniform_int_distribution<size_t> distr(0, 124);

//    for (size_t i = 124; i < 2048; ++i) {
//        mpz_class gmpBigNum = randomMachine.get_z_bits(i);
//        BigInt myBigNum(gmpBigNum.get_str(16));

//        size_t randomShift = distr(gen);

//        std::cout << gmpBigNum.get_str(16) << " vs " << myBigNum.getHexStr() << " i " << i << " shift " << randomShift << std::endl;

//        gmpBigNum >>= randomShift;
//        myBigNum >> randomShift;

//        std::cout << gmpBigNum.get_str(16) << " vs " << myBigNum.getHexStr() << " i " << i << " shift " << randomShift << std::endl;

//        ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getHexStr());

////        if (HasFailure()) {
////            std::cout << "iteration: " << i << "val: " << gmpBigNum.get_str() << std::endl;
////        }
//    }
//}

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
        // chechked previously with mpz_t, thus must be reliable anyway.
        mpz_class gmpRand = randomMachine.get_z_bits(i);
        BigInt bigNumRand(gmpRand.get_str(16));
        BigInt randCompl = ~bigNumRand;
        BigInt zero = bigNumRand & randCompl;

        ASSERT_TRUE(zero.isZero());
    }
}

TEST(BigIntFunct, ShiftLeft)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    mpz_class tmp1 = randomMachine.get_z_bits(64);
    for (size_t i = 0; i < 5; ++i) {
        tmp1 <<= 1;
        std::cout << tmp1.get_str(2) << std::endl;
    }
    mpz_class tmp2 = randomMachine.get_z_bits(64);
    for (size_t i = 0; i < 5; ++i) {
        tmp2 >>= 1;
        std::cout << tmp2.get_str(2) << std::endl;
    }
    mpz_class inversed1 = ~tmp1;
    mpz_class inversed2 = ~tmp2;
    std::cout << inversed1.get_str(2) << std::endl;
    std::cout << inversed2.get_str(2) << std::endl;
}
