#include "catch.hpp"

#include "rectangular.hpp"

using namespace gnb;

using R = rectangular<unsigned char>;

TEST_CASE("Default constructor rectangular", "[rectangular]") {
    R i;

    REQUIRE(i.height() == 0);
    REQUIRE(i.width() == 0);
    REQUIRE(i.size() == 0);
}

TEST_CASE("Can create rectangular", "[rectangular]") {
    R i{2, 3};

    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 3);
}

TEST_CASE("Create [0,0]", "[rectangular]") {
    R i{0,1};
    
    REQUIRE_THROWS_AS(i.at(0,0), std::out_of_range);
    REQUIRE(i.size() == 0);
}

TEST_CASE("Create from initializer list", "[rectangular]") {
    R i{3,2, {10, 11, 12, 13, 14, 15}};
 
    REQUIRE(i.height() == 3);
    REQUIRE(i.width() == 2);
    REQUIRE(i.at(0,0) == 10);
    REQUIRE(i.at(0,1) == 11);
    REQUIRE(i.at(1,0) == 12);
    REQUIRE(i.at(2,1) == 15);
}    

TEST_CASE("initializer_list throws", "[rectangular]") {
    REQUIRE_THROWS_AS( (R{2,2, {0,1,2}} ), std::out_of_range);
    REQUIRE_THROWS_AS( (R{2,2, {0,1,2,3,4}} ), std::out_of_range);
}

TEST_CASE("Create from vector", "[rectangular]") {
    std::vector<unsigned char> vec{10, 11, 12, 13, 14, 15};
    R i{3,2, vec};

    REQUIRE(i.size() == 6);
    REQUIRE(i.height() == 3);
    REQUIRE(i.width() == 2);
    REQUIRE(i.at(0,0) == 10);
    REQUIRE(i.at(0,1) == 11);
    REQUIRE(i.at(1,0) == 12);
    REQUIRE(i.at(2,1) == 15);

    REQUIRE(vec.size() == 0);
}

TEST_CASE("Create from vector throws", "[rectangular]") {
    std::vector<unsigned char> vec{10, 11, 12, 13, 14, 15};
    REQUIRE_THROWS_AS( (R{2,2, vec} ), std::out_of_range);
    REQUIRE_THROWS_AS( (R{4,4, vec} ), std::out_of_range);
    REQUIRE(vec.size() == 6); // Untouched by failed constructor
}

TEST_CASE("rectangular access", "[rectangular]") {
    R i{2, 3};

    REQUIRE(i.at(0,0) == 0);
    REQUIRE(i.at(1,1) == 0);

    i.at(1,1) = 3;
    REQUIRE(i.at(1,1) == 3);

    REQUIRE_THROWS(i.at(2, 0));
    REQUIRE_THROWS(i.at(0, 3));
    REQUIRE_THROWS(i.at(-1, 0));
    REQUIRE_THROWS(i.at(0, -1));
}

TEST_CASE("rectangular[]", "[rectangular]") {
    R i{2,3};

    REQUIRE(i[0][0] == 0);

    // Can still do this, but be careful of dangling references
    auto rp = i[0];
    REQUIRE(rp[1] == 0);

    rp[1] = 9;
    REQUIRE(i[0][1] == 9);
    REQUIRE(rp[1] == 9);

    i[1][1] = 7;
    REQUIRE(i[1][1] == 7);
}

TEST_CASE("rectangular const", "[rectangular]") {
    const R i{2, 3};

    REQUIRE(i.at(0,0) == 0);
    REQUIRE(i.at(1,1) == 0);

    REQUIRE_THROWS(i.at(2, 0));
    REQUIRE_THROWS(i.at(0, 3));

    // References are never considered const, so remove reference first
    static_assert(std::is_const<std::remove_reference<decltype(i.at(0,0))>::type>::value,
        "Const rectangular.at() should return const reference");
    static_assert(!std::is_assignable<decltype(i.at(0,0)), R::value_type>::value
            || !std::is_reference<decltype(i.at(0,0))>::value, 
        "Const rectangular contents should not be assignable");
}


TEST_CASE("const rectangular[]", "[rectangular]") {
    const R i{2,3};

    REQUIRE(i[0][0] == 0);

    // Can still do this, but be careful of dangling references
    auto rp = i[0];
    REQUIRE(rp[1] == 0);

    // Check that i[0][0]=7 does not compile, i.e. that the rectangular is truly const
    static_assert(!std::is_assignable<decltype(i[0][0]), R::value_type>::value
            || !std::is_reference<decltype(i[0][0])>::value, 
        "Const rectangular contents should not be assignable");
}

TEST_CASE("rectangular fill", "[rectangular]") {
    R i{2,3};
    i.fill(8);

    REQUIRE(i.size() == 6);
    REQUIRE(i[0][0] == 8);
    REQUIRE(i[1][2] == 8);

}

TEST_CASE("create with iterator", "[rectangular]") {
    std::string s("012345");
    R i{2, 3, s.begin(), s.end()};

    REQUIRE(i.size() == 6);
    REQUIRE(i[0][0] == '0');
    REQUIRE(i[0][2] == '2');
    REQUIRE(i[1][0] == '3');
    REQUIRE(i[1][2] == '5');
}

TEST_CASE("create with empty iterator", "[rectangular]") {
    std::string s("");
    R i{0, 0, s.begin(), s.end()};

    REQUIRE(i.size() == 0);
}

TEST_CASE("iterator create throws", "[rectangular]") {
    std::string s("012345");

    // Needs (R{...}) due to the commas confusing preprocessor macro arg handling
    // See https://github.com/catchorg/Catch2/blob/master/docs/assertions.md#expressions-with-commas
    REQUIRE_THROWS_AS( (R{2, 2, s.begin(), s.end()}), std::out_of_range);
    REQUIRE_THROWS_AS( (R{3, 3, s.begin(), s.end()}), std::out_of_range);
}

TEST_CASE("rectangular swap", "[rectangular]") {
    R x{2,2};
    R y;

    REQUIRE(x.size() == 4);
    REQUIRE(y.size() == 0);

    std::swap(x, y);

    REQUIRE(x.size() == 0);
    REQUIRE(y.size() == 4);

}

TEST_CASE("rectangular move", "[rectangular]") {
    R x{2,3};
    R y;

    REQUIRE(x.size() == 6);
    REQUIRE(y.size() == 0);

    y = std::move(x);

    REQUIRE(x.size() == 0);
    REQUIRE(y.size() == 6);
    REQUIRE(y.height() == 2);
    REQUIRE(y.width() == 3);
}
