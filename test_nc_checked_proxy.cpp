#include "rectangular.hpp"

/* 
 * Check we cannot keep copies of the row proxy
 * Alas, in C++17 we can't avoid this due to copy elision rules
 * 
 * Clang as provided with XCode 10 (clang 6.0) does the copy elision
 * but also doesn't provide __cpp_guaranteed_copy_elision, so need to 
 * skip if we are on Clang 6 and compiling in C++17 mode
 */
#if __cpp_guaranteed_copy_elision >= 201606 || (__cplusplus == 201703L && __APPLE_CC__ >= 6000)
#   define SKIP 1
#else
#   define SKIP 0
#endif

int foo() {
#if SKIP
    // C++17, pick some random code that wil not compile
    7 = 7;
#else
    gnb::checked_rectangular<int> cri{3,3};
    // C++11 - C++14
    auto rp{cri[1]};
    return rp[1];
#endif
}
