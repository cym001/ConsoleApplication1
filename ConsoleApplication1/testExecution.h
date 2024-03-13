#include <optional>
#include <vector>
#include <iostream>
#include <functional>
#include <cstdlib>
#include <map>
#include <variant>
#include <ctime>
#include <fstream>
#include <Windows.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "testCase.h"


struct SingleTestResult {
    string functionName;
    bool success;
    string errorMessage;

};

struct GroupTestResult {
    int testFrequency;
    vector<SingleTestResult> singleTestResult;
};

struct TestResult {
    int groupFrequency;
    vector<GroupTestResult> groupTestResult;
};

struct PerformanceTestResult {
    string testName; 
    double executionTime; 
    double memoryUsage; 
    double cpuUsage; 
    bool success; 
    string errorMessage; 

    PerformanceTestResult(string name, double execTime, double memUsage, double cpu, bool pass, string errMsg = "")
        : testName(name), executionTime(execTime), memoryUsage(memUsage), cpuUsage(cpu), success(pass), errorMessage(errMsg) {}
};


struct PerformanceTestSuiteResult {
    string suiteName; 
    vector<PerformanceTestResult> testResults; 

    void addTestResult(const PerformanceTestResult& result) {
        testResults.push_back(result);
    }
};


struct PerformanceTestReport {
    string reportName; 
    vector<PerformanceTestSuiteResult> suiteResults;

    void addSuiteResult(const PerformanceTestSuiteResult& suiteResult) {
        suiteResults.push_back(suiteResult);
    }
};


double GetParamValueEIRP(TestDataComputeEIRP testData, string paramName, int index);

TestResult PerformAutomatedTestsComputeEIRP(HINSTANCE hinstLib, const TestConfiguration& config);

void ExportTestResultsToJson(const vector<GroupTestResult>& groupTestResults, const char* filePath);

PerformanceTestReport PerformanceTestComputeEIRP(HINSTANCE hinstLib, const TestConfiguration& config);

void PrintPerformanceTestReport(const PerformanceTestReport& report);

void ExportPerformanceTestReportToJson(const PerformanceTestReport& report, const char* filePath);

void GenerateHtmlReport(const PerformanceTestReport& report, const std::string& filePath);