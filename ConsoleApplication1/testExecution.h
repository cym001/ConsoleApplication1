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



double GetRaramValueEIRP(TestDataComputeEIRP testData, string paramName, int index);

TestResult PerformAutomatedTestsComputeEIRP(HINSTANCE hinstLib, const TestConfiguration& config);

void ExportTestResultsToJson(const vector<GroupTestResult>& groupTestResults, const char* filePath);