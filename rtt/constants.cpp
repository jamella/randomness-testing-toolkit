#include "rtt/constants.h"

namespace rtt {

/* Constant definition */
const std::string Constants::FILE_DEFAULT_CFG_PATH = "./config.json";
const std::string Constants::FILE_TOOLKIT_SETTINGS = "./rtt-settings.json";

const double Constants::MATH_ALPHA = 0.01;
const double Constants::MATH_EPS   = 1e-8;

std::string Constants::batteryToString(Battery batteryConstant) {
    switch(batteryConstant) {
        case Battery::DIEHARDER:
            return "Dieharder";
        case Battery::NIST_STS:
            return "NIST Statistical Test Suite";
        case Battery::TU01_SMALLCRUSH:
            return "TestU01 Small Crush";
        case Battery::TU01_CRUSH:
            return "TestU01 Crush";
        case Battery::TU01_BIGCRUSH:
            return "TestU01 Big Crush";
        case Battery::TU01_RABBIT:
            return "TestU01 Rabbit";
        case Battery::TU01_ALPHABIT:
            return "TestU01 Alphabit";
        case Battery::TU01_BLOCK_ALPHABIT:
            return "TestU01 Block Alphabit";
        default:
            raiseBugException(Strings::ERR_INVALID_BATTERY);
    }
}

std::string Constants::batteryToStringShort(Battery battId) {
    switch(battId) {
        case Battery::DIEHARDER:
            return "dieharder";
        case Battery::NIST_STS:
            return "nist_sts";
        case Battery::TU01_SMALLCRUSH:
            return "tu01_smallcrush";
        case Battery::TU01_CRUSH:
            return "tu01_crush";
        case Battery::TU01_BIGCRUSH:
            return "tu01_bigcrush";
        case Battery::TU01_RABBIT:
            return "tu01_rabbit";
        case Battery::TU01_ALPHABIT:
            return "tu01_alphabit";
        case Battery::TU01_BLOCK_ALPHABIT:
            return "tu01_blockalphabit";
        default:
            raiseBugException(Strings::ERR_INVALID_BATTERY);
    }
}

bool Constants::isInTU01Family(Constants::Battery battId) {
    switch(battId) {
        case Battery::NIST_STS:
        case Battery::DIEHARDER:
            return false;
        case Battery::TU01_SMALLCRUSH:
        case Battery::TU01_CRUSH:
        case Battery::TU01_BIGCRUSH:
        case Battery::TU01_RABBIT:
        case Battery::TU01_ALPHABIT:
        case Battery::TU01_BLOCK_ALPHABIT:
            return true;
        default:
            raiseBugException(Strings::ERR_INVALID_BATTERY);
    }
}

bool Constants::isInTU01CrushFamily(Constants::Battery battId) {
    switch(battId) {
        case Battery::DIEHARDER:
        case Battery::NIST_STS:
        case Battery::TU01_RABBIT:
        case Battery::TU01_ALPHABIT:
        case Battery::TU01_BLOCK_ALPHABIT:
            return false;
        case Battery::TU01_SMALLCRUSH:
        case Battery::TU01_CRUSH:
        case Battery::TU01_BIGCRUSH:
            return true;
        default:
            raiseBugException(Strings::ERR_INVALID_BATTERY);
    }
}

bool Constants::isInTU01BitFamily(Constants::Battery battId) {
    switch(battId) {
        case Battery::DIEHARDER:
        case Battery::NIST_STS:
        case Battery::TU01_SMALLCRUSH:
        case Battery::TU01_CRUSH:
        case Battery::TU01_BIGCRUSH:
            return false;
        case Battery::TU01_RABBIT:
        case Battery::TU01_ALPHABIT:
        case Battery::TU01_BLOCK_ALPHABIT:
            return true;
        default:
            raiseBugException(Strings::ERR_INVALID_BATTERY);
    }
}

bool Constants::isinTU01AlphabitFamily(Constants::Battery battId) {
    switch(battId) {
        case Battery::DIEHARDER:
        case Battery::NIST_STS:
        case Battery::TU01_SMALLCRUSH:
        case Battery::TU01_CRUSH:
        case Battery::TU01_BIGCRUSH:
        case Battery::TU01_RABBIT:
            return false;
        case Battery::TU01_ALPHABIT:
        case Battery::TU01_BLOCK_ALPHABIT:
            return true;
        default:
            raiseBugException(Strings::ERR_INVALID_BATTERY);
    }
}

} // namespace rtt
