#include "result-tu01.h"

namespace rtt {
namespace batteries {
namespace testu01 {

std::unique_ptr<Result> Result::getInstance(
        const std::vector<ITest *> & tests) {
    if(tests.empty())
        raiseBugException("empty tests");

    std::unique_ptr<Result> r (new Result(
                                   tests.at(0)->getLogger(),
                                   tests.at(0)->getLogicName()));

    const static std::regex RE_SUBTEST {
        "\\w+? test:([^]*?)"
        "========= State of the binary file stream generator ========="
    };
    auto endIt = std::sregex_iterator();
    std::vector<result::SubTestResult> tmpSubTestResults;
    std::vector<result::PValueSet> tmpPValueSets;
    std::vector<std::string> tmpParamVec;

    /* Single test processing */
    for(const ITest * test : tests) {
        /* Single variant processing */
        for(IVariant * variant : test->getVariants()) {
            testu01::Variant * tu01Var =
                    dynamic_cast<Variant *>(variant);
            r->objectInfo = tu01Var->getObjectInfo() +
                            " (results)";

            /* Split log into subtests */
            auto variantLog = tu01Var->getBatteryOutput().getStdOut();

            auto subTestIt = std::sregex_iterator(
                                 variantLog.begin(),
                                 variantLog.end(),
                                 RE_SUBTEST);

            /* Single subtest processing */
            for(; subTestIt != endIt ; ++subTestIt) {
                std::smatch match = *subTestIt;
                std::string subTestLog = match[1].str();

                tmpPValueSets = r->extractPValueSets(
                                    subTestLog,
                                    tu01Var->getStatisticNames());

                /* Test settings extraction */
                tmpParamVec = r->extractTestParameters(
                                  subTestLog,
                                  tu01Var->getParamNames());

                auto tmpSubTestRes = result::SubTestResult::getInstance(
                                         tmpPValueSets);
                tmpSubTestRes.setTestParameters(tmpParamVec);
                tmpSubTestResults.push_back(std::move(tmpSubTestRes));
                tmpPValueSets.clear();
                tmpParamVec.clear();
            }
            r->varRes.push_back(result::VariantResult::getInstance(
                                    tmpSubTestResults,
                                    tu01Var->getUserSettings(),
                                    tu01Var->getBatteryOutput()));
            tmpSubTestResults.clear();
        }
    }
    return r;
}

std::vector<result::PValueSet> Result::extractPValueSets(
        const std::string & testLog, std::vector<std::string> statNames) {
    std::vector<result::PValueSet> rval;
    const static std::regex RE_PVALUES {
        "p-value of test {23}: *?("
        "eps|"                            /* Just "eps" */
        "1 - eps1|"                       /* Just "1 - eps1" */
        "0\\.\\d{2,4}|"                   /* Decimal rounded from 2 to 4 digits */
        "(1 -  ?)?\\d\\.\\de-\\d{1,3}"    /* Decimal in scientific notation that can be preceeded by "1 - " */
        ") *?(\\*\\*\\*\\*\\*)?\\n"       /* Capture ending "*****" - pvalue is suspect */
    };
    auto pValIt = std::sregex_iterator(
                      testLog.begin(),
                      testLog.end(),
                      RE_PVALUES);
    auto endIt = std::sregex_iterator();
    if(std::distance(pValIt, endIt) != statNames.size()) {
        statNames.clear();
        for(uint i = 0 ; i < std::distance(pValIt , endIt) ; ++i)
            statNames.push_back("Unknown " + Utils::itostr(i));
        logger->warn(objectInfo +
                     Strings::TEST_ERR_UNKNOWN_STATISTICS);
    }
    for(uint i = 0 ; pValIt != endIt ; ++i , ++pValIt) {
        std::smatch match = *pValIt;
        double pVal = convertStringToDouble(match[1].str(),
                                            match[2].str());
        rval.push_back(result::PValueSet::getInstance(
                       statNames.at(i),
                       pVal, { pVal }));
    }
    return rval;
}

double Result::convertStringToDouble(const std::string & num,
                                     const std::string & oneMinus) {
    if(num == "eps") {
        return 1.0E-300;
    } else if(num == "1 - eps1") {
        return 1 - 1.0E-15;
    } else if(num.find('e') == std::string::npos) {
        return Utils::strtod(num);
    } else {
        std::string tmp = num;
        tmp.erase(0 , oneMinus.length());
        std::vector<std::string> splitted =
                Utils::split(tmp , 'e');
        double base = Utils::strtod(splitted.at(0));
        double exp = Utils::strtod(splitted.at(1));
        base *= pow(10.0 , exp);
        if(oneMinus.empty()) {
            return base;
        } else {
            return 1 - base;
        }
    }
}

std::vector<std::string> Result::extractTestParameters(
        const std::string & testLog,
        std::vector<std::string> paramNames) {
    std::vector<std::string> rval;
    std::regex RE_PARAM = buildParamRegex(paramNames);
    auto paramIt = std::sregex_iterator(testLog.begin(),
                                        testLog.end(),
                                        RE_PARAM);
    auto endIt = std::sregex_iterator();
    if(std::distance(paramIt , endIt) != 1)
        throw RTTException(objectInfo,
                           "parameter extraction failed");
    std::smatch paramMatch = *paramIt;
    for(uint i = 0 ; i < paramNames.size() ; ++i) {
        rval.push_back(paramNames.at(i) + " = " +
                       paramMatch[i + 1].str());
    }
    return rval;
}

std::regex Result::buildParamRegex(
        std::vector<std::string> paramNames) {
    std::stringstream rval;
    for(uint i = 0 ; i < paramNames.size() ; ++i) {
        rval << "\\s+?" << paramNames.at(i) << " +?= +?([^\\s,]+?)";
        if(i + 1 != paramNames.size()) {
            rval << ",";
        } else {
            rval << "\\s";
        }
    }
    return std::regex(rval.str());
}


} // namespace testu01
} // namespace batteries
} // namespace rtt
