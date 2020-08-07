#include "catch.hpp"

#include "rectangular.hpp"

using namespace gnb;

using CR = checked_rectangular<int>;

TEST_CASE("Default constructor checked_rectangular", "[checked_rectangular]") {
    CR i;

    REQUIRE(i.height() == 0);
    REQUIRE(i.width() == 0);
    REQUIRE(i.size() == 0);
}

TEST_CASE("Can create checked_rectangular", "[checked_rectangular]") {
    CR i{2, 3};

    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 3);
}

TEST_CASE("create checked_rectangular with iterator", "[checked_rectangular]") {
    std::string s("012345");
    CR i{2, 3, s.begin(), s.end()};

    REQUIRE(i.size() == 6);
    REQUIRE(i[0][0] == '0');
    REQUIRE(i[0][2] == '2');
    REQUIRE(i[1][0] == '3');
    REQUIRE(i[1][2] == '5');
}

TEST_CASE("Create checked rectangular [0,0]", "[checked_rectangular]") {
    CR i{0,1};
    
    REQUIRE_THROWS_AS(i.at(0,0), std::out_of_range);
    REQUIRE_THROWS_AS(i[0][0], std::out_of_range);
    REQUIRE(i.size() == 0);
}

TEST_CASE("Create checked_rectangular from initializer list", "[checked_rectangular]") {
    CR i{3,2, {10, 11, 12, 13, 14, 15}};
 
    REQUIRE(i.height() == 3);
    REQUIRE(i.width() == 2);
    REQUIRE(i.at(0,0) == 10);
    REQUIRE(i.at(0,1) == 11);
    REQUIRE(i.at(1,0) == 12);
    REQUIRE(i.at(2,1) == 15);
}    

TEST_CASE("checked_rectangular initializer_list throws", "[checked_rectangular]") {
    REQUIRE_THROWS_AS( (CR{2,2, {0,1,2}} ), std::out_of_range);
    REQUIRE_THROWS_AS( (CR{2,2, {0,1,2,3,4}} ), std::out_of_range);
}

TEST_CASE("checked_rectangular access", "[checked_rectangular]") {
    CR i{2, 3};

    REQUIRE(i.at(0,0) == 0);
    REQUIRE(i.at(1,1) == 0);

    i.at(1,1) = 3;
    REQUIRE(i.at(1,1) == 3);

    REQUIRE_THROWS(i.at(2, 0));
    REQUIRE_THROWS(i.at(0, 3));
    REQUIRE_THROWS(i.at(-1, 0));
    REQUIRE_THROWS(i.at(0, -1));
}

TEST_CASE("checked_rectangular[]", "[checked_rectangular]") {
    CR i{2,3};

    REQUIRE(i[0][0] == 0);

    // This shouldn't compile, and doesn't in C++11 mode
    // but does compile in C++17 due to required Copy Elision
    // auto rp(i[0]);
    // REQUIRE(rp[0] == 0);

    i[1][1] = 7;
    REQUIRE(i[1][1] == 7);
}

TEST_CASE("checked_rectangular const", "[checked_rectangular]") {
    const CR i{2, 3};

    REQUIRE(i.at(0,0) == 0);
    REQUIRE(i.at(1,1) == 0);

    REQUIRE_THROWS(i.at(2, 0));
    REQUIRE_THROWS(i.at(0, 3));

    // Check that i.at(0,0)=7 does not compile, i.e. that the checked_rectangular is truly const
    static_assert(!std::is_assignable<decltype(i.at(0,0)), CR::value_type>::value
            || !std::is_reference<decltype(i.at(0,0))>::value, 
        "Const checked_rectangular contents should not be assignable");
}


TEST_CASE("const checked_rectangular[]", "[checked_rectangular]") {
    const CR i{2,3};

    REQUIRE(i[0][0] == 0);

    REQUIRE_THROWS_AS(i[2][0], std::out_of_range);
    REQUIRE_THROWS_AS(i[0][3], std::out_of_range);

    // Check that i[0][0]=7 does not compile, i.e. that the checked_rectangular is truly const
    static_assert(!std::is_assignable<decltype(i[0][0]), CR::value_type>::value
            || !std::is_reference<decltype(i[0][0])>::value, 
        "Const checked_rectangular contents should not be assignable");
}   

TEST_CASE("checked_rectangular swap", "[checked_rectangular]") {
    CR x{2,2};
    CR y{0,0};

    REQUIRE(x.size() == 4);
    REQUIRE(y.size() == 0);
    REQUIRE(x.invariants());
    REQUIRE(y.invariants());

    std::swap(x, y);

    REQUIRE(x.size() == 0);
    REQUIRE(y.size() == 4);
    REQUIRE(x.invariants());
    REQUIRE(y.invariants());
}
