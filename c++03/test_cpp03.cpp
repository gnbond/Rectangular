/*
 * catch2 only works with C++11, so hardwire some tests for C++03
 */

#include "rectangular.hpp"

#include "test_macros.hpp"

   
typedef gnb::rectangular<int> R;
typedef gnb::checked_rectangular<int> CR;


static int test_create() {
    TEST_CASE_BEGIN("rectangular create");

    R i(2, 2);

    REQUIRE(i.size() == 4);
    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 2);
    REQUIRE(i.at(0,0) == 0);
    REQUIRE(i.at(1,1) == 0);
    REQUIRE(i[0][0] == 0);
    REQUIRE(i[0][0] == 0);
    REQUIRE(i[1][1] == 0);

    TEST_CASE_END();
}

static int test_create_checked() {
   TEST_CASE_BEGIN("checked create");

    CR i(2, 2);
    
    REQUIRE(i.size() == 4);
    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 2);
    REQUIRE(i.at(1,1) == 0);
    REQUIRE(i.at(0,0) == 0);
    REQUIRE(i[0][0] == 0);
    REQUIRE(i[1][1] == 0);

    TEST_CASE_END();
}

static int test_create_iterators() {
   TEST_CASE_BEGIN("create iterator");

    std::string s("123456");
    R i(2, 3, s.begin(), s.end());
    
    REQUIRE(i.size() == 6);
    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 3);
    REQUIRE(i.at(0,0) == '1');
    REQUIRE(i.at(1,1) == '5');
    REQUIRE(i[0][1] == '2');
    REQUIRE(i[1][2] == '6');

    TEST_CASE_END();
}

static int test_create_vector() {
   TEST_CASE_BEGIN("create vector");

    std::vector<int> v;
    for (int i = '1'; i <= '6'; ++i)
        v.push_back(i);

    REQUIRE(v.size() == 6);

    R i(2, 3, v);
    
    REQUIRE(i.size() == 6);
    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 3);
    REQUIRE(i.at(0,0) == '1');
    REQUIRE(i.at(1,1) == '5');
    REQUIRE(i[0][1] == '2');
    REQUIRE(i[1][2] == '6');

    REQUIRE(v.size() == 0);

    TEST_CASE_END();
}

static int test_create_checked_iterators() {
   TEST_CASE_BEGIN("checked create iterator");

    std::string s("123456");
    CR i(2, 3, s.begin(), s.end());
    
    REQUIRE(i.size() == 6);
    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 3);
    REQUIRE(i.at(0,0) == '1');
    REQUIRE(i.at(1,1) == '5');
    REQUIRE(i[0][1] == '2');
    REQUIRE(i[1][2] == '6');

    TEST_CASE_END();
}

static int test_create_iterator_throws() {
    TEST_CASE_BEGIN("iterator create throws");

    std::string s("1234");

    REQUIRE_THROWS_AS(R(1,2, s.begin(), s.end()).size(), std::out_of_range);
    REQUIRE_THROWS_AS(R(3,2, s.begin(), s.end()).size(), std::out_of_range);

    TEST_CASE_END();
}

static int test_throws() {
    TEST_CASE_BEGIN("rectangular throws");
 
    R i(2, 2);

    REQUIRE_THROWS_AS(i.at(2,0), std::out_of_range);
    REQUIRE_THROWS_AS(i.at(0,2), std::out_of_range);
    REQUIRE_THROWS_AS(i.at(-2,0), std::out_of_range);
    REQUIRE_THROWS_AS(i.at(0,-2), std::out_of_range);
    
    TEST_CASE_END();
}

static int test_checked_throws() {
    TEST_CASE_BEGIN("rectangular checked throws");
 
    CR i(2, 2);

    REQUIRE_THROWS_AS(i[2][0], std::out_of_range);
    REQUIRE_THROWS_AS(i[0][2], std::out_of_range);
    REQUIRE_THROWS_AS(i[-2][0], std::out_of_range);
    REQUIRE_THROWS_AS(i[0][-2], std::out_of_range);
    
    TEST_CASE_END();
}

static int test_resize() {
    TEST_CASE_BEGIN("resize");
    std::string s("123456");

    R i(1,6, s.begin(), s.end());

    i.resize(2, 3);
    REQUIRE(i.height() == 2);
    REQUIRE(i.width() == 3);

    TEST_CASE_END();
}

static int test_swap() {
    TEST_CASE_BEGIN("swap");

    R x(1,6);
    R y;

    REQUIRE(x.size() == 6);
    REQUIRE(y.size() == 0);

    std::swap(x, y);

    REQUIRE(x.size() == 0);
    REQUIRE(y.size() == 6);

    TEST_CASE_END();
}


static int test_fill() {
    TEST_CASE_BEGIN("swap");

    R x(1,6);

    REQUIRE(x.size() == 6);
    REQUIRE(x[0][0] == 0);
    REQUIRE(x[0][5] == 0);

    x.fill(1);

    REQUIRE(x[0][0] == 1);
    REQUIRE(x[0][5] == 1);
 
    TEST_CASE_END();
}

static int test_iterators() {
    TEST_CASE_BEGIN("iterators");

    R i(3,2);
    R::iterator b = i.begin();
    R::iterator e = i.end();

    REQUIRE(e - b == 6); // RandomAccessIterator

    *b = 5;
    REQUIRE(i[0][0] == 5);

    TEST_CASE_END();
}

static int test_const_iterators() {
    TEST_CASE_BEGIN("const_iterators");

    const R i(3,2);
    R::const_iterator b = i.begin();
    R::const_iterator e = i.end();

    REQUIRE(e - b == 6); // RandomAccessIterator

    REQUIRE(*b == 0);

    TEST_CASE_END();
}

int main(int, char**) {
    int ret(0);

    ret += test_create();
    ret += test_create_checked();
    ret += test_create_iterators();
    ret += test_create_vector();
    ret += test_create_checked_iterators();
    ret += test_create_iterator_throws();
    ret += test_throws();
    ret += test_checked_throws();
    ret += test_resize();
    ret += test_swap();
    ret += test_fill();
    ret += test_iterators();
    ret += test_const_iterators();

    if (ret) 
        std::cout << "** " << ret << " tests failed! **" << std::endl;
    else
        std::cout << "All passed OK" << std::endl;

    return !!ret;
}
