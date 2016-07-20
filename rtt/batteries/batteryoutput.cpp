#include "batteryoutput.h"

namespace rtt {
namespace batteries {

void BatteryOutput::appendStdOut(const std::string & stdOut) {
    errsWarnsDetected = false;

    this->stdOut.append(stdOut);
}

void BatteryOutput::appendStdErr(const std::string & stdErr) {
    this->stdErr.append(stdErr);
}

std::string BatteryOutput::getStdOut() const {
    return stdOut;
}

std::string BatteryOutput::getStdErr() const {
    return stdErr;
}

std::vector<std::string> BatteryOutput::getErrors() {
    if(!errsWarnsDetected)
        detectErrsWarnsInStdOut();

    return errors;
}

std::vector<std::string> BatteryOutput::getWarnings() {
    if(!errsWarnsDetected)
        detectErrsWarnsInStdOut();

    return warnings;
}

void BatteryOutput::detectErrsWarnsInStdOut() {
    if(errsWarnsDetected)
        return;

    /* Detect warnings and errors here.
     * Detection happens only in stdOut variable. */
    static const std::regex RE_ERR ("(.*?error.*?)\\n", std::regex::icase);
    static const std::regex RE_WARN ("(.*?warning.*?)\\n", std::regex::icase);

    std::smatch match;
    auto end =       std::sregex_iterator();
    auto errBegin =  std::sregex_iterator(stdOut.begin() , stdOut.end(), RE_ERR);
    auto warnBegin = std::sregex_iterator(stdOut.begin() , stdOut.end(), RE_WARN);

    errors.clear();
    warnings.clear();

    for( ; errBegin != end ; ++errBegin) {
        match = *errBegin;
        errors.push_back(match[1].str());
    }

    for( ; warnBegin != end ; ++ warnBegin) {
        match = *warnBegin;
        warnings.push_back(match[1].str());
    }

    errsWarnsDetected = true;
}





} // namespace batteries
} // namespace rtt
