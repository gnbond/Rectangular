#include "rectangular.hpp"

/* 
 * Check we cannot assign to const rectangulat
*/

void foo() {
    const gnb::checked_rectangular<int> cri{3,3};
    *cri.begin() = 7;
}
