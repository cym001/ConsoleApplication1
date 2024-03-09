#include <optional>
#include <vector>
#include <iostream>
#include <functional>
#include <cstdlib>
#include <map>
#include <variant>
#include <ctime>
#include <fstream>
#include <cstdio>
#include <Windows.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "testCase.h"
#include "testExecution.h"
#include "testdata.h"

using namespace rapidjson;
using namespace std;


double GetRaramValueEIRP(TestDataComputeEIRP testData, string paramName, int index) {
    auto it = testData.datas.find(paramName);
    if (it != testData.datas.end()) {
        const auto& values = it->second; 
        if (index >= 0 && index < values.size()) {
            return values[index]; 
        }
        else {
            cerr << "Index out of range for parameter: " << paramName << endl;
        }
    }
    else {
        cerr << "Parameter not found: " << paramName << endl;
    }
    return 0.0; 
}

TestResult PerformAutomatedTestsComputeEIRP(HINSTANCE hinstLib, const TestConfiguration& config) {
    Result* initResult = nullptr;
    TestResult testResult;
    testResult.groupFrequency = 0;

    auto init = reinterpret_cast<Result * (*)()>(GetProcAddress(hinstLib, "init"));
    auto computeEIRP = reinterpret_cast<void(*)(double, double, double, double, double, double, double, double, Result*)>(GetProcAddress(hinstLib, "ComputeEIRP"));
    auto final = reinterpret_cast<void(*)(Result*)>(GetProcAddress(hinstLib, "final"));

    GroupTestResult groupTestResult1;



    if (!init || !computeEIRP || !final) {
        cerr << "Function cannot be loaded." << endl;
        bool success = false;
        string errorMessage = "Load function failed";

        SingleTestResult singleTestResult("load function", success, errorMessage);
        groupTestResult1.singleTestResult.push_back(singleTestResult);
        testResult.groupTestResult.push_back(groupTestResult1);
        return testResult;
    }

    initResult = init();
    if (!initResult) {
        bool success = false;
        string errorMessage = "init function execution failed.";

        SingleTestResult singleTestResult("init function execution", success, errorMessage);
        groupTestResult1.singleTestResult.push_back(singleTestResult);
        testResult.groupTestResult.push_back(groupTestResult1);
        return testResult;
    }

    FunctionCall computeEIRPFunction = GetFunction(config, "ComputeEIRP");


    // 遍历所有测试数据组
    for (const auto& testDataGroup : computeEIRPFunction.testData) {
        GroupTestResult groupTestResult;
        groupTestResult.testFrequency = testDataGroup.dataFrequency;
        testResult.groupFrequency++;
        int dataFrequency = testDataGroup.dataFrequency;

        for (int i = 0; i < dataFrequency; i++) {
            double EIRP0 = GetRaramValueEIRP(testDataGroup, "EIRP0", i);
            double BW3dB = GetRaramValueEIRP(testDataGroup, "BW3dB", i);
            double Augment = GetRaramValueEIRP(testDataGroup, "Augment", i);
            double Attenuation = GetRaramValueEIRP(testDataGroup, "Attenuation", i);
            double ThetaBeam = GetRaramValueEIRP(testDataGroup, "ThetaBeam", i);
            double PhiBeam = GetRaramValueEIRP(testDataGroup, "PhiBeam", i);
            double ThetaTarget = GetRaramValueEIRP(testDataGroup, "ThetaTarget", i);
            double PhiTarget = GetRaramValueEIRP(testDataGroup, "PhiTarget", i);
            computeEIRP(EIRP0, BW3dB, Augment, Attenuation, ThetaBeam, PhiBeam, ThetaTarget, PhiTarget, initResult);
            bool success = (initResult->INFO == 0);
            string errorMessage = success ? "" : "Test failed with INFO = 1";

            SingleTestResult singleTestResult(testDataGroup.description, success, errorMessage);
            groupTestResult.singleTestResult.push_back(singleTestResult);

        }
        
        testResult.groupTestResult.push_back(groupTestResult);


    }

    /*for (const auto& groupResult : testResult.groupTestResult) {
        cout << "Group Test Frequency: " << groupResult.testFrequency << "\n";
        for (const auto& result : groupResult.singleTestResult) {
            cout << "Function: " << result.functionName
                << ", Success: " << (result.success ? "Yes" : "No")
                << ", Error Message: " << result.errorMessage;
            cout << "\n";
        }
    }*/

    // 执行final函数
    final(initResult);

    return testResult;

}

void ExportTestResultsToJson(const vector<GroupTestResult>& groupTestResults, const char* filePath) {
    Document d;
    d.SetObject();

    Document::AllocatorType& allocator = d.GetAllocator();

    // 创建测试结果的JSON数组
    Value resultsArray(kArrayType);

    for (const auto& groupResult : groupTestResults) {
        Value groupObject(kObjectType);
        groupObject.AddMember("groupFrequency", Value().SetInt(groupResult.testFrequency), allocator);

        Value testResultsArray(kArrayType);
        for (const auto& result : groupResult.singleTestResult) {
            Value resultObject(kObjectType);
            resultObject.AddMember("functionName", Value().SetString(result.functionName.c_str(), allocator), allocator);
            resultObject.AddMember("success", Value().SetBool(result.success), allocator);
            resultObject.AddMember("errorMessage", Value().SetString(result.errorMessage.c_str(), allocator), allocator);

            testResultsArray.PushBack(resultObject, allocator);
        }

        groupObject.AddMember("tests", testResultsArray, allocator);
        resultsArray.PushBack(groupObject, allocator);
    }

    d.AddMember("testResults", resultsArray, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    d.Accept(writer);

    ofstream file(filePath);
    if (file.is_open()) {
        file << buffer.GetString();
        file.close();
        cout << "Test data exported to " << filePath << endl;
    }
    else {
        cerr << "Failed to open file for writing: " << filePath << endl;
    }
}