#include "catch.hpp"

#include "rectangular.hpp"

using namespace gnb;

using R = rectangular<int>;

TEST_CASE("rectangular copy", "[rectangular]") {
    R i{2, 2, 7};

    REQUIRE(i.size() == 4);
    REQUIRE(i[0][0] == 7);

    R j{i};

    REQUIRE(i.size() == 4);
    REQUIRE(i[0][0] == 7);
    REQUIRE(j.size() == 4);
    REQUIRE(j[0][0] == 7);

    R k;
    k =  j;
     
    REQUIRE(j.size() == 4);
    REQUIRE(j[0][0] == 7);
    REQUIRE(k.size() == 4);
    REQUIRE(k[0][0] == 7);

}

TEST_CASE("rectangular move", "[rectangular]") {
    R i{2, 3, 7};

    REQUIRE(i.size() == 6);
    REQUIRE(i[0][0] == 7);

    R j{std::move(i)};

    REQUIRE(i.size() == 0);

    REQUIRE(i.height() == 0);
    REQUIRE(i.width() == 0);
    REQUIRE(i.invariants());

    REQUIRE(j.size() == 6);
    REQUIRE(j.invariants());
    REQUIRE(j.height() == 2);
    REQUIRE(j.width() == 3);
    REQUIRE(j[0][0] == 7);

    R k;
    k =  std::move(j);
     
    REQUIRE(j.size() == 0);
    REQUIRE(j.invariants());
    REQUIRE(j.height() == 0);
    REQUIRE(j.width() == 0);

    REQUIRE(k.size() == 6);
    REQUIRE(k.height() == 2);
    REQUIRE(k.width() == 3);
    REQUIRE(k.invariants());
    REQUIRE(k[0][0] == 7);

}

using CR = checked_rectangular<int>;

TEST_CASE("checked_rectangular copy", "[checked_rectangular]") {
    CR i{2, 2, 7};

    REQUIRE(i.size() == 4);
    REQUIRE(i[0][0] == 7);

    CR j{i};

    REQUIRE(i.size() == 4);
    REQUIRE(i[0][0] == 7);
    REQUIRE(j.size() == 4);
    REQUIRE(j[0][0] == 7);

    CR k;
    k =  j;
     
    REQUIRE(j.size() == 4);
    REQUIRE(j[0][0] == 7);
    REQUIRE(k.size() == 4);
    REQUIRE(k[0][0] == 7);

}

TEST_CASE("checked_rectangular move", "[checked_rectangular]") {
    CR i{2, 3, 7};

    REQUIRE(i.size() == 6);
    REQUIRE(i[0][0] == 7);

    CR j{std::move(i)};

    REQUIRE(i.size() == 0);

    REQUIRE(i.height() == 0);
    REQUIRE(i.width() == 0);
    REQUIRE(i.invariants());

    REQUIRE(j.size() == 6);
    REQUIRE(j.invariants());
    REQUIRE(j.height() == 2);
    REQUIRE(j.width() == 3);
    REQUIRE(j[0][0] == 7);

    CR k;
    k =  std::move(j);
     
    REQUIRE(j.size() == 0);
    REQUIRE(j.invariants());
    REQUIRE(j.height() == 0);
    REQUIRE(j.width() == 0);

    REQUIRE(k.size() == 6);
    REQUIRE(k.height() == 2);
    REQUIRE(k.width() == 3);
    REQUIRE(k.invariants());
    REQUIRE(k[0][0] == 7);

}
