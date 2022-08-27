#include "pch.h"
#include "cryptopp/cryptlib.h"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/md5.h"

#include <iostream>

TEST(TestCaseName, TestName) {
    using namespace CryptoPP;
    Weak::MD5 hash;
    std::cout << "Name: " << hash.AlgorithmName() << std::endl;
    std::cout << "Digest size: " << hash.DigestSize() << std::endl;
    std::cout << "Block size: " << hash.BlockSize() << std::endl;

    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}
