/*
 * Some macros for simple C++ unit tests for C++03.
 * Catch2 needs C++11 or better.
 */

#include <iostream>

#define TEST_CASE_BEGIN(case) int nfailed(0), npassed(0); const char* case_name = case; std::cout << "Test case " << case

#define TEST_CASE_END() std::cout << std::endl << case_name << ": " << nfailed << " tests failed, " << npassed << " tests passed." << std::endl; \
    return nfailed

#define REQUIRE(expr) if (expr) { npassed++; std::cout << '.'; } \
    else {nfailed++; std::cout << std::endl << "Line " << __LINE__ << ": Test failed: " << #expr << std::endl; }

// requires expr to be convertable to boolean
// This is so the "if (_x) break;" line can suppress unused expression warning
#define REQUIRE_THROWS_AS(expr, exc) \
    do { \
        bool has_thrown(false); \
        int _x; \
        try { \
            _x = expr; \
        } \
        catch (const exc& e) { npassed++; has_thrown = true; std::cout << '.'; } \
        catch(...) { nfailed++; has_thrown = true; \
            std::cout << std::endl << "Line "<< __LINE__ << ": test failed: threw wrong exception" << std::endl;\
        } \
        if (!has_thrown) { nfailed++; \
            std::cout << std::endl << "Line "<< __LINE__ << ": test failed: no exception thrown" << std::endl;\
        } \
        if (_x) break; \
    } while (0)

