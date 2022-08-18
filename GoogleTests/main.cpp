#include "gtest/gtest.h"
#include <vector>
#include <boost/uuid/detail/md5.hpp>

using boost::uuids::detail::md5;

int main(int argc, char **argv) {

    md5 hash;
    md5::digest_type digest;

    std::vector<char> data {'1', '2', '3'};
    hash.process_bytes(data.data(), data.size());
    hash.get_digest(digest);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}