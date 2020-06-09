#include "bigintfunct.h"

#include <gtest/gtest.h>

#include <gmpxx.h>

#include <openssl/bn.h>

#include <iostream>
#include <cstdlib>
#include <random>
#include <chrono>
#include <fstream>

using namespace std::chrono;

constexpr size_t maxTestedBitsSize = 512;

TEST(BigIntFunct, SimpleStrings)
{
    gmp_randclass randomMachine(gmp_randinit_default);

    for (size_t i = 124; i < maxTestedBitsSize; ++i) {
        mpz_class gmpBigNum = randomMachine.get_z_bits(i);
        BigInt myBigNum(gmpBigNum.get_str(10), BigInt::Dec);

        ASSERT_EQ(gmpBigNum.get_str(2).size(), myBigNum.bitsLen());
        ASSERT_TRUE(std::string(gmpBigNum.get_str(2)) == myBigNum.getStr(BigInt::Bin));
        ASSERT_TRUE(std::string(gmpBigNum.get_str(16)) == myBigNum.getStr(BigInt::Hex));
        ASSERT_TRUE(std::string(gmpBigNum.get_str(10)) == myBigNum.getStr(BigInt::Dec));
    }
}

TEST(BigIntFunct, And)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    for (size_t i = 124; i < maxTestedBitsSize; ++i) {
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
    for (size_t i = 124; i < maxTestedBitsSize; ++i) {
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
    for (size_t i = 124; i < maxTestedBitsSize; ++i) {
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
    for (size_t i = 124; i < maxTestedBitsSize; ++i) {
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

    for (size_t i = 124; i < maxTestedBitsSize; ++i) {
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

    for (size_t i = 124; i < maxTestedBitsSize; ++i) {
        mpz_class gmpBigNum = randomMachine.get_z_bits(i);
        BigInt myBigNum(gmpBigNum.get_str(16));

        std::default_random_engine gen;
        std::uniform_int_distribution<size_t> distr(0, i);
        for (int j = 0; j < 10; ++j) {
            size_t randomShift = distr(gen);
            gmpBigNum <<= randomShift;
            myBigNum <<= randomShift;

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
    for (size_t i = 0; i < maxTestedBitsSize; ++i) {
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
    for (size_t i = 0; i < maxTestedBitsSize; ++i) {
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
    for (size_t i = 0; i < maxTestedBitsSize; ++i) {
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
    for (size_t i = 2; i < maxTestedBitsSize; ++i) {
        std::uniform_int_distribution<size_t> distr(1, i);
        mpz_class left = randomMachine.get_z_bits(distr(gen));
        mpz_class right = randomMachine.get_z_bits(distr(gen));
        if (right == 0)
            continue;
        mpz_class quotient, remainder;

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));

        mpz_fdiv_qr(quotient.get_mpz_t(), remainder.get_mpz_t(), left.get_mpz_t(), right.get_mpz_t());
        auto[myQuotient, myRemainder] = divisionRemainder(myLeft, myRight);

        ASSERT_TRUE(std::string(quotient.get_str(16)) == myQuotient.getStr(BigInt::Hex));
        ASSERT_TRUE(std::string(remainder.get_str(16)) == myRemainder.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, Multiplication)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    for (size_t i = 2; i < maxTestedBitsSize; ++i) {
        std::uniform_int_distribution<size_t> distr(1, i);
        mpz_class left = randomMachine.get_z_bits(distr(gen));
        mpz_class right = randomMachine.get_z_bits(distr(gen));

        mpz_class product = left * right;

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));
        BigInt myProduct = myLeft * myRight;

        ASSERT_TRUE(std::string(product.get_str(16)) == myProduct.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, GCD)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    for (size_t i = 2; i < maxTestedBitsSize; ++i) {
        std::uniform_int_distribution<size_t> distr(1, i);
        mpz_class left = randomMachine.get_z_bits(distr(gen));
        mpz_class right = randomMachine.get_z_bits(distr(gen));
        mpz_class g;
        mpz_class s;
        mpz_class t;

        mpz_gcdext(g.get_mpz_t(), s.get_mpz_t(), t.get_mpz_t(), left.get_mpz_t(), right.get_mpz_t());

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));
        BigInt myG = gcd(myLeft, myRight);

        ASSERT_TRUE(std::string(g.get_str(16)) == myG.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, Reduction)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    for (size_t i = 2; i < maxTestedBitsSize; ++i) {
        std::uniform_int_distribution<size_t> distr(1, i);
        mpz_class left = randomMachine.get_z_bits(distr(gen));
        mpz_class right = randomMachine.get_z_bits(distr(gen));

        // This checks needed to satisfy GMP lib constraints
        if (right == 0 or left == 0 or right == left) {
            --i;
            continue;
        }

        mpz_class product = left % right;

        BigInt myLeft(left.get_str(16));
        BigInt myRight(right.get_str(16));
        BigInt myProduct = myLeft % myRight;

        ASSERT_TRUE(std::string(product.get_str(16)) == myProduct.getStr(BigInt::Hex));
    }
}


TEST(BigIntFunct, binaryLRExp)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    constexpr word maxTestExponent = 3000;
    constexpr size_t iterations = 100;
    std::ofstream data("benaryLRExp.csv");
    for (size_t i = 1; i < iterations; ++i) {
        mpz_class base = randomMachine.get_z_bits(64);
        mpz_class result;
        std::uniform_int_distribution<size_t> expDistr(1, maxTestExponent);
        word exponent = expDistr(gen);

        mpz_pow_ui(result.get_mpz_t(), base.get_mpz_t(), exponent);

        BigInt myBase(base.get_str(16));
        auto myStart = high_resolution_clock::now();
        BigInt myResult = myBase.binaryLRExp(BigInt(exponent));
        auto myStop = high_resolution_clock::now();
        auto myDuration = duration_cast<microseconds>(myStop - myStart);
        data << myBase.bitsLen() << "," << exponent << "," << myDuration.count() << "," << std::endl;

        ASSERT_TRUE(std::string(result.get_str(16)) == myResult.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, binaryRLExp)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    constexpr word maxTestExponent = 3000;
    constexpr size_t iterations = 100;
    std::ofstream data("binaryRLExp.csv");
    for (size_t i = 1; i < iterations; ++i) {
        mpz_class base = randomMachine.get_z_bits(64);
        mpz_class result;
        std::uniform_int_distribution<size_t> expDistr(1, maxTestExponent);
        word exponent = expDistr(gen);

        mpz_pow_ui(result.get_mpz_t(), base.get_mpz_t(), exponent);

        BigInt myBase(base.get_str(16));
        auto myStart = high_resolution_clock::now();
        BigInt myResult = myBase.binaryRLExp(BigInt(exponent));
        auto myStop = high_resolution_clock::now();
        auto myDuration = duration_cast<microseconds>(myStop - myStart);
        data << myBase.bitsLen() << "," << exponent << "," << myDuration.count() << "," << std::endl;

        ASSERT_TRUE(std::string(result.get_str(16)) == myResult.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, slidingWindowExp)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    constexpr word maxTestExponent = 3000;
    constexpr size_t iterations = 100;
    mpz_class base = randomMachine.get_z_bits(64);
    std::ofstream data("slidingWindowExp.csv");
    for (size_t i = 1; i < iterations; ++i) {
        mpz_class result;
        std::uniform_int_distribution<size_t> expDistr(1, maxTestExponent);
        word exponent = expDistr(gen);
        mpz_pow_ui(result.get_mpz_t(), base.get_mpz_t(), exponent);

        BigInt myBase(base.get_str(16));
        auto myStart = high_resolution_clock::now();
        BigInt myResult = myBase.binarySWExp(BigInt(exponent));
        auto myStop = high_resolution_clock::now();
        auto myDuration = duration_cast<microseconds>(myStop - myStart);
        data << myBase.bitsLen() << "," << exponent << "," << myDuration.count() << "," << std::endl;

        ASSERT_TRUE(std::string(result.get_str(16)) == myResult.getStr(BigInt::Hex));
    }
}

TEST(BigIntFunct, kAryLRExp)
{
    gmp_randclass randomMachine(gmp_randinit_default);
    std::default_random_engine gen;
    constexpr word maxTestExponent = 3000;
    constexpr size_t iterations = 100;
    mpz_class base = randomMachine.get_z_bits(64);
    std::ofstream data("kAryLRExp.csv");
    for (size_t i = 1; i < iterations; ++i) {
        mpz_class result;
        std::uniform_int_distribution<size_t> expDistr(1, maxTestExponent);
        word exponent = expDistr(gen);
        mpz_pow_ui(result.get_mpz_t(), base.get_mpz_t(), exponent);

        BigInt myBase(base.get_str(16));
        auto myStart = high_resolution_clock::now();
        BigInt myResult = myBase.mAryLRExp(BigInt(exponent));
        auto myStop = high_resolution_clock::now();
        auto myDuration = duration_cast<microseconds>(myStop - myStart);
        data << myBase.bitsLen() << "," << exponent << "," << myDuration.count() << "," << std::endl;

        ASSERT_TRUE(std::string(result.get_str(16)) == myResult.getStr(BigInt::Hex));
    }
}

TEST(BigIntPerf, FixedExp)
{
    std::default_random_engine gen;
    std::uniform_int_distribution<word> distr(128, 1024);
    gmp_randclass randomMachine(gmp_randinit_default);
    word randConstExp = distr(gen);
    std::ofstream data("resultsFixedExp.csv");
    // Testing rising base
    for (size_t i = 1; i < maxTestedBitsSize; ++i) {
        // GNU MP implementation
        mpz_class gmpBase = randomMachine.get_z_bits(i);
        mpz_class gmpResult;
        // OpenSSL implementation
        BIGNUM* osslBase = BN_new();
        BIGNUM* osslResult = BN_new();
        BIGNUM* osslExp = BN_new();
        BN_CTX* ctx = BN_CTX_new();
        BN_hex2bn(&osslBase, gmpBase.get_str(16).c_str());
        BN_dec2bn(&osslExp, std::to_string(randConstExp).c_str());
        // Home-grown implementation :)
        BigInt myBase(gmpBase.get_str(16));

        auto gmpStart = high_resolution_clock::now();
        mpz_pow_ui(gmpResult.get_mpz_t(), gmpBase.get_mpz_t(), randConstExp);
        auto gmpStop = high_resolution_clock::now();
        auto gmpDuration = duration_cast<microseconds>(gmpStop - gmpStart);
        data << randConstExp << "," << myBase.bitsLen() << "," << gmpDuration.count() << ",";

        auto osslStart = high_resolution_clock::now();
        BN_exp(osslResult, osslBase, osslExp, ctx);
        auto osslStop = high_resolution_clock::now();
        auto osslDuration = duration_cast<microseconds>(osslStop - osslStart);
        data << osslDuration.count() << ",";

        auto mySwStart = high_resolution_clock::now();
        myBase.binaryLRExp(randConstExp);
        auto myStop = high_resolution_clock::now();
        auto myDuration = duration_cast<microseconds>(myStop - mySwStart);
        data << myDuration.count() << ",";

        auto myArStart = high_resolution_clock::now();
        myBase.binaryRLExp(randConstExp);
        auto myArStop = high_resolution_clock::now();
        auto myArDuration = duration_cast<microseconds>(myArStop - myArStart);
        data << myArDuration.count() << ",";

        auto myLrStart = high_resolution_clock::now();
        myBase.mAryLRExp(randConstExp);
        auto myLrStop = high_resolution_clock::now();
        auto myLrDuration = duration_cast<microseconds>(myLrStop - myLrStart);
        data << myLrDuration.count() << ",";

        auto myRLStart = high_resolution_clock::now();
        myBase.binarySWExp(randConstExp);
        auto myRLStop = high_resolution_clock::now();
        auto myRLDuration = duration_cast<microseconds>(myRLStop - myRLStart);
        data << myRLDuration.count() << "," << std::endl;

        BN_clear_free(osslBase);
        BN_clear_free(osslExp);
        BN_clear_free(osslResult);
        BN_CTX_free(ctx);
    }
}
