﻿#ifndef RTT_BATTERIES_NISTSTS_TEST_H
#define RTT_BATTERIES_NISTSTS_TEST_H

#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <tuple>

#include "rtt/batteries/itest-batt.h"

namespace rtt {
namespace batteries {
namespace niststs {

class Test : public ITest {
public:
    static std::unique_ptr<Test> getInstance(int testId ,
                                             const GlobalContainer & cont);

private:
    /* Variables */
    std::unique_ptr<std::mutex> testDir_mux;

    /* Methods */
    Test(int testIndex , const GlobalContainer & container)
        : ITest(testIndex , container)
    {}
};

} // namespace niststs
} // namespace batteries
} // namespace rtt

#endif // RTT_BATTERIES_NISTSTS_TEST_H
