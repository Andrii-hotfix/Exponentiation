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

        ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr());
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

//        std::cout << gmpBigNum.get_str(16) << " vs " << myBigNum.getStr() << " i " << i << " shift " << randomShift << std::endl;

//        gmpBigNum >>= randomShift;
//        myBigNum >> randomShift;

//        std::cout << gmpBigNum.get_str(16) << " vs " << myBigNum.getStr() << " i " << i << " shift " << randomShift << std::endl;

//        ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr());

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

        std::cout << gmpLeft.get_str(16) << " vs " << leftBigNum.getStr() << std::endl;
        std::cout << gmpRight.get_str(16) << " vs " << rightBigNum.getStr() << std::endl;

        mpz_class andGmpResult = gmpLeft & gmpRight;
        BigInt andBigIntResult = leftBigNum & rightBigNum;

        std::cout << andGmpResult.get_str(16) << " vs " << andBigIntResult.getStr() << std::endl;

        ASSERT_TRUE(std::string(andGmpResult.get_str(16)) == andBigIntResult.getStr());
    }
}
