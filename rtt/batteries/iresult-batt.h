#ifndef RTT_BATTERIES_IRESULT_H
#define RTT_BATTERIES_IRESULT_H

#include "rtt/batteries/itest-batt.h"
#include "rtt/batteries/result/variantresult-res.h"
#include "rtt/storage/istorage.h"

namespace rtt {
namespace batteries {

class IResult {
public:
    static std::unique_ptr<IResult> getInstance(
            const std::vector<ITest *> & tests);

    void writeResults(storage::IStorage * storage, int precision);

    std::vector<result::VariantResult> getResults() const;

    bool getPassed() const;

protected:
    /* Variables */
    Logger * logger;
    std::string objectInfo;
    std::string testName;
    std::vector<result::VariantResult> varRes;
    bool passed = true;

    /* Methods */
    IResult(Logger * logger , std::string testName)
        : logger(logger) , testName(testName)
    {}

    void evaluateSetPassed();
};







} // namespace batteries
} // namespace rtt

#endif // RTT_BATTERIES_IRESULT_H
