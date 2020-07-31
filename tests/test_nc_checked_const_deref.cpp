#include "rectangular.hpp"

/* 
 * Check we cannot alter const via the proxy class
 */
void foo() { 
    const gnb::checked_rectangular<int> cri{3,3};
    cri[0][0] =  8;   
}
