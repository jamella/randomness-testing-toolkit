#include "filestorage.h"

namespace rtt {
namespace storage {

const std::string FileStorage::STRING_PASSED_PROP   = "Passed/Total test statistics: ";

const size_t FileStorage::MISC_TAB_SIZE     = 4;
const size_t FileStorage::MISC_COL_WIDTH    = 30;

std::unique_ptr<FileStorage> FileStorage::getInstance(const GlobalContainer & container) {
    std::unique_ptr<FileStorage> s (new FileStorage());
    s->cliOptions      = container.getCliOptions();
    s->toolkitSettings = container.getToolkitSettings();
    s->creationTime    = container.getCreationTime();
    s->battId          = s->cliOptions->getBatteryId();
    s->inFilePath      = s->cliOptions->getBinFilePath();

    /* Getting file name for main output file */
    s->mainOutFilePath = s->toolkitSettings->getRsFileOutFile();

    /* Creating file name for test report file */
    std::string binFileName = Utils::getLastItemInPath(s->inFilePath);
    std::string datetime = Utils::formatRawTime(s->creationTime , "%Y%m%d%H%M%S");
    s->outFilePath = s->toolkitSettings->getRsFileBatteryDir(s->battId);
    s->outFilePath.append(datetime + "-" + binFileName + ".report");

    s->makeReportHeader();

    return s;
}

void FileStorage::addNewTest(const std::string & testName) {
    report << "-----------------------------------------------------------" << std::endl;
    report << testName << " test results:" << std::endl;
    ++indent;
}

void FileStorage::finalizeTest() {
    report << "-----------------------------------------------------------"
           << std::endl << std::endl;
    --indent;
    currentVariant = 0;
}

void FileStorage::addVariant() {
    ++currentVariant;
    report << doIndent() << "Variant " << currentVariant << ":" << std::endl;
    ++indent;
}

void FileStorage::finalizeVariant() {
    --indent;
    report << doIndent() << "^^^^^^^^^^^^" << std::endl;
    currentSubtest = 0;
    report << std::endl;
}

void FileStorage::addSubTest() {
    ++currentSubtest;
    report << doIndent() << "Subtest " << currentSubtest << ":" << std::endl;
    ++indent;
}

void FileStorage::finalizeSubTest() {
    --indent;
    report << doIndent() << "############" << std::endl;
    report << std::endl;
}

void FileStorage::setTestResult(bool passed) {
    report << doIndent() << "Result: ";
    ++totalTestsCount;
    if(passed) {
        ++passedTestsCount;
        report << "Passed" << std::endl;
    }
    else
        report << "FAILED" << std::endl;
    report << std::endl;
}

void FileStorage::setUserSettings(const std::vector<std::string> & options) {
    report << doIndent() << "User settings: " << std::endl;
    ++indent;
    std::string spaces = doIndent();
    for(const std::string & i : options)
        report << spaces << i << std::endl;
    --indent;
    report << doIndent() << "************" << std::endl;
    report << std::endl;
}

void FileStorage::setTestParameters(const std::vector<std::string> & options) {
    if(options.empty())
        return;

    report << doIndent() << "Test parameters: " << std::endl;
    ++indent;
    std::string spaces = doIndent();
    for(const std::string & i : options)
        report << spaces << i << std::endl;
    --indent;
    report << doIndent() << "%%%%%%%%%%" << std::endl;
    report << std::endl;
}

void FileStorage::setRuntimeIssues(const std::string & stdErr,
                               const std::vector<std::string> & errors,
                               const std::vector<std::string> & warnings) {
    if(stdErr.empty() && errors.empty() && warnings.empty())
        return;

    bool first = true;
    report << doIndent() << "Test runtime issues: " << std::endl;
    ++indent;
    /* Reporting error output */
    if(!stdErr.empty()) {
        first = false;
        auto stdErrStrings = Utils::split(stdErr , '\n');
        report << doIndent() << "Standard error output: " << std::endl;
        ++indent;
        auto spaces = doIndent();
        for(const auto & i : stdErrStrings)
            report << spaces << i << std::endl;
        --indent;
        report << doIndent() << "!!!!!!!!!!!!" << std::endl;
    }

    /* Reporting strings containing error */
    if(!errors.empty()) {
        if(!first)
            report << std::endl;
        else
            first = false;

        report << doIndent() << "Errors in log: " << std::endl;
        ++indent;
        auto spaces = doIndent();
        for(const auto & i : errors)
            report << spaces << i << std::endl;
        --indent;
        report << doIndent() << "!!!!!!!!!!!!" << std::endl;
    }

    /* Reporting strings containing warning */
    if(!warnings.empty()) {
        if(!first)
            report << std::endl;
        else
            first = false;

        report << doIndent() << "Warnings in log: " << std::endl;
        ++indent;
        auto spaces = doIndent();
        for(const auto & i : warnings)
            report << spaces << i << std::endl;
        --indent;
        report << doIndent() << "!!!!!!!!!!!!" << std::endl;
    }

    --indent;
    report << doIndent() << "!!!!!!!!!!!!" << std::endl << std::endl;

}

void FileStorage::addStatisticResult(const std::string & statName,
                                     double value, int precision, bool passed) {
    std::stringstream tmp;
    tmp << doIndent() << statName <<" statistic p-value: "
        << std::setprecision(precision) << std::fixed << value;
    report << std::setw(58) << std::left << tmp.str();

    if(passed)
        report << " Passed" << std::endl;
    else
        report << " FAILED!!!" << std::endl;
}

void FileStorage::addPValues(const std::vector<double> & pvals , int precision) {
    report << doIndent() << "p-values: " << std::endl;
    ++indent;
    std::string spaces = doIndent();

    size_t pvalPerRow = 5;
    for(size_t ctr = 0 ; ctr < pvals.size() ; ) {
        report << spaces;
        for(size_t col = 0 ; col < std::min(pvalPerRow , pvals.size() - ctr) ; ++col) {
            report << std::setprecision(precision)
                   << std::fixed << pvals.at(ctr + col) << " ";
        }
        report << std::endl;
        ctr += pvalPerRow;
    }

    --indent;
    report << doIndent() << "============" << std::endl;
}

void FileStorage::finalizeReport() {
    /* Add passed tests proportion at the end of report */
    passedTestProp = { Utils::itostr(passedTestsCount) + "/" + Utils::itostr(totalTestsCount) };
    std::string reportStr = report.str();
    size_t pos = reportStr.find(STRING_PASSED_PROP);
    reportStr.insert(pos + STRING_PASSED_PROP.length(), passedTestProp);

    /* Storing report */
    Utils::createDirectory(Utils::getPathWithoutLastItem(outFilePath));
    Utils::saveStringToFile(outFilePath , reportStr);
    /* Adding result into table file */
    /* Files with same name as the file processed in
     * this run will be assigned new results */
    addResultToTableFile();
}

void FileStorage::makeReportHeader() {
    report << "***** Randomness Testing Toolkit data stream analysis report *****" << std::endl;
    report << "Date:    " << Utils::formatRawTime(creationTime , "%d-%m-%Y") << std::endl;
    report << "File:    " << inFilePath << std::endl;
    report << "Battery: " << Constants::batteryToString(battId) << std::endl;
    report << std::endl;
    report << "Alpha:   " << std::setw(3) << Constants::MATH_ALPHA << std::endl;
    report << "Epsilon: " << Constants::MATH_EPS << std::endl;
    report << std::endl;
    report << STRING_PASSED_PROP << std::endl;
    report << std::endl << std::endl;
}

std::string FileStorage::doIndent() const {
    if(indent > 0)
        return std::string(indent * MISC_TAB_SIZE, ' ');
    else
        return "";
}

void FileStorage::addResultToTableFile() const {
    if(Utils::fileExist(mainOutFilePath)) {
        /* Table file already exist */
        tStringVector header;
        tStringVector fileNames;
        std::vector<tStringVector> tableData;
        /* Loading file into table variables */
        loadMainTable(header , fileNames , tableData);

        /* Looking for file name - if table already has row
         * with same file name as I am processing here,
         * only new information is added to this row.
         * Otherwise new row is added. */
        std::vector<std::string>::iterator fileRow =
                std::find(fileNames.begin() , fileNames.end() , inFilePath);
        if(fileRow != fileNames.end()) {
            /* Row with same filename is present, modifying this row */
            int rowIndex = std::distance(fileNames.begin() , fileRow);
            tableData.at(rowIndex).at(0) =
                    Utils::formatRawTime(creationTime , "%Y-%m-%d %H:%M:%S");
            tableData.at(rowIndex).at(static_cast<int>(battId)) = passedTestProp;
        } else {
            /* Adding new row */
            addNewRow(fileNames , tableData);
        }

        /* Saving table variables into file. */
        saveMainTable(header , fileNames , tableData);
    } else {
        /* File doesn't exist, create brand new table file with single new result */
        tStringVector header;
        tStringVector fileNames;
        std::vector<tStringVector> tableData;

        /* Creating header */
        header.push_back("Input file path");
        header.push_back("Time of last update");
        for(int i = 1 ; i < static_cast<int>(Constants::Battery::LAST_ITEM) ; ++i)
            header.push_back(
                        Constants::batteryToString(static_cast<Constants::Battery>(i)));

        /* Adding new row */
        addNewRow(fileNames , tableData);

        /* Table variables are now complete, save into file */
        saveMainTable(header , fileNames , tableData);
    }
}

void FileStorage::loadMainTable(tStringVector & header,
                            tStringVector & fileNames,
                            std::vector<tStringVector> & tableData) const {
    std::string table = std::move(Utils::readFileToString(mainOutFilePath));
    tStringVector lines = Utils::split(table , '\n');
    if(lines.empty())
        throw std::runtime_error("can't load table: file is empty");

    /* Parsing header */
    header = std::move(Utils::split(lines.at(0) , '^'));

    /* Parsing rows */
    tStringVector row;
    for(size_t i = 1 ; i < lines.size() ; ++i) {
        row = std::move(Utils::split(lines.at(i) , '^'));
        if(row.size() != 1)
            throw std::runtime_error("input table corrupted: "
                                     "non header row contains more than one \"^\"");

        row = std::move(Utils::split(row.at(0) , '|'));
        if(row.size() != header.size())
            throw std::runtime_error("input table corrupted: "
                                     "row " + Utils::itostr(i) + " has "
                                     "different number of columns than header");
        fileNames.push_back(stripSpacesFromString(row.at(0)));
        tableData.push_back({row.begin() + 1 , row.end()});
    }
}

void FileStorage::saveMainTable(const tStringVector & header,
                            const tStringVector & fileNames,
                            const std::vector<tStringVector> & tableData) const {
    if(fileNames.size() != tableData.size())
        throw std::runtime_error("can't create output table: number of filenames and"
                                 " number of table data rows does not match");

    /* First columnt is aligned to left, rest right */
    bool first = true;
    std::stringstream table;

    /* Creating header */
    for(const std::string & val : header) {
        table << "^";
        table << std::setw(MISC_COL_WIDTH);
        if(first){
            table << std::left;
            first = false;
        }
        else /* This else is maybe useless */
            table << std::right;
        table << val;
    }
    table << "^" << std::endl;

    /* Inserting rows */
    for(size_t i = 0 ; i < fileNames.size() ; ++i) {
        table << "^";
        table << std::setw(MISC_COL_WIDTH) << std::left;
        table << fileNames.at(i);
        table << std::right;
        for(const std::string & val : tableData.at(i)) {
            table << "|";
            table << std::setw(MISC_COL_WIDTH);
            table << val;
        }
        table << "|" << std::endl;
    }

    /* Saving table to file */
    Utils::createDirectory(Utils::getPathWithoutLastItem(mainOutFilePath));
    Utils::saveStringToFile(mainOutFilePath , table.str());
}

void FileStorage::addNewRow(tStringVector & fileNames,
                        std::vector<tStringVector> & tableData) const {
    /* Add file path to file names column */
    fileNames.push_back(inFilePath);
    /* Add data into corresponding row */
    /* Column count is total batteries count + first column for last update info */
    tStringVector row(static_cast<int>(Constants::Battery::LAST_ITEM) , "");
    row.at(0) = Utils::formatRawTime(creationTime , "%Y-%m-%d %H:%M:%S");
    row.at(static_cast<int>(battId)) = passedTestProp;
    tableData.push_back(std::move(row));
}

std::string FileStorage::stripSpacesFromString(const std::string & str) {
    size_t start = str.find_first_not_of(" ");
    size_t end = str.find_last_not_of(" ");
    return str.substr(start , end + 1);
}

} // namespace storage
} // namespace rtt
