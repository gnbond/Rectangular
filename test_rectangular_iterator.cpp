#include "catch.hpp"

#include "rectangular.hpp"

using namespace gnb;

using R = rectangular<int>;

TEST_CASE("basic iterator", "[rectangular]") {
    R i{2, 3};

    REQUIRE(i.size() == 6);
    auto b = i.begin();
    auto e = i.end();
    REQUIRE(e - b == 6); // RandomAccessIterator
    for (; b != e; ++b) {
        REQUIRE(*b == 0);
    }
}

TEST_CASE("const basic iterator", "[rectangular]") {
    const R i{2, 3};

    REQUIRE(i.size() == 6);
    auto b = i.begin();
    auto e = i.end();
    REQUIRE(e - b == 6); // RandomAccessIterator
    for (; b != e; ++b) {
        REQUIRE(*b == 0);
    }
}

TEST_CASE("fill from iterator", "[rectangular]") {
    R i{2,3};
    std::string s("012345");
    std::copy(s.begin(), s.end(), i.begin());

    REQUIRE(i[0][0] == '0');
    REQUIRE(i[0][2] == '2');
    REQUIRE(i[1][0] == '3');
    REQUIRE(i[1][2] == '5');
}
