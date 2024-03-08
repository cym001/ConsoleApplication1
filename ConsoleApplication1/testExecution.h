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
    optional<Result> output;

    SingleTestResult(string name, bool succ, string error = "", optional<Result> out = nullopt)
        : functionName(name), success(succ), errorMessage(error), output(out) {}
};

struct GroupTestResult {
    int testFrequency;
    vector<SingleTestResult> singleTestResult;
};

struct TestResult {
    int groupFrequency;
    vector<GroupTestResult> groupTestResult;
};




