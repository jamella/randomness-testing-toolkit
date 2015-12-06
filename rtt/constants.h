#ifndef RTT_CONSTANTS_H
#define RTT_CONSTANTS_H

#include <string>

namespace rtt {

class Constants {
public:
    ///////////////////////////////////////////////
    /*********Toolkit global constants************/
    ///////////////////////////////////////////////

    /* Statistical batteries constants */
    static const int BATTERY_DIEHARDER =        1;
    static const int BATTERY_NIST_STS =         2;
    static const int BATTERY_TU01_SMALLCRUSH =  3;
    static const int BATTERY_TU01_CRUSH =       4;
    static const int BATTERY_TU01_BIGCRUSH =    5;
    static const int BATTERY_EACIRC =           6;

    /* Default files locations */
    static const std::string FILE_DEFAULT_CFG_PATH;
};

} // namespace rtt

#endif // RTT_CONSTANTS_H
