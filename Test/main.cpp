#include "../bigint.h"

#include <gtest/gtest.h>
#include <gmp.h>

#include <iostream>
#include <cstdlib>
#include <random>

TEST(BigIntFunct, SimpleStrings)
{
    gmp_randstate_t state;
    constexpr size_t seed = 123456789;
    gmp_randinit_default(state);

    gmp_randseed_ui(state, seed);


    for (size_t i = 124; i < 2048; ++i) {
        mpz_t gmpBigNum;
        mpz_init(gmpBigNum);
        mpz_urandomb(gmpBigNum, state, i);
        BigInt myBigNum(mpz_get_str(nullptr, 16, gmpBigNum));

        ASSERT_TRUE(std::string(mpz_get_str(nullptr, 16, gmpBigNum)) == myBigNum.getStr());
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

TEST(BigIntFunct, Not)
{
    gmp_randstate_t state;
    constexpr size_t seed = 123456789;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, seed);

    for (size_t i = 124; i < 2048; ++i) {
        mpz_t gmpBigNum;
        mpz_init(gmpBigNum);
        mpz_urandomb(gmpBigNum, state, i);

        BigInt myBigNum(mpz_get_str(nullptr, 16, gmpBigNum));
//        std::cout << gmpBigNum.get_str(16) << " vs " << myBigNum.getStr() << std::endl;

        gmpBigNum = ~gmpBigNum;
        ~myBigNum;

        std::cout << gmpBigNum.get_str(16) << " vs " << myBigNum.getStr() << std::endl;

        ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr());
    }
}
