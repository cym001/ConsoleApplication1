#ifndef PSAPI_VERSION
#define PSAPI_VERSION 1
#endif
#include <optional>
#include <vector>
#include <iostream>
#include <functional>
#include <cstdlib>
#include <map>
#include <variant>
#include <ctime>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <Windows.h>
#include <psapi.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "testCase.h"
#include "testExecution.h"
#include "testdata.h"

#pragma  comment(lib,"Psapi.lib")


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


PerformanceTestReport PerformanceTestComputeEIRP(HINSTANCE hinstLib, const TestConfiguration& config) {
    Result* initResult = nullptr;
    PerformanceTestReport testReport;
    testReport.reportName = "performanceTest";
    PerformanceTestSuiteResult testSuiteResult1;

    HANDLE hProcess = GetCurrentProcess();
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD numProcessors = sysInfo.dwNumberOfProcessors;

    PROCESS_MEMORY_COUNTERS memCounter;
    SIZE_T memBefore, memAfter;


    auto init = reinterpret_cast<Result * (*)()>(GetProcAddress(hinstLib, "init"));
    auto computeEIRP = reinterpret_cast<void(*)(double, double, double, double, double, double, double, double, Result*)>(GetProcAddress(hinstLib, "ComputeEIRP"));
    auto final = reinterpret_cast<void(*)(Result*)>(GetProcAddress(hinstLib, "final"));


    if (!init || !computeEIRP || !final) {
        cerr << "Function cannot be loaded." << endl;
        bool success = false;
        string errorMessage = "Load function failed";

        PerformanceTestResult testResult1 = PerformanceTestResult("PerformanceTest", 0, 0, 0, success, errorMessage);
        testSuiteResult1.addTestResult(testResult1);
        testReport.addSuiteResult(testSuiteResult1);
        return testReport;
    }

    initResult = init();
    if (!initResult) {
        bool success = false;
        string errorMessage = "init function execution failed.";

        PerformanceTestResult testResult1 = PerformanceTestResult("PerformanceTest", 0, 0, 0, success, errorMessage);
        testSuiteResult1.addTestResult(testResult1);
        testReport.addSuiteResult(testSuiteResult1);
        return testReport;
    }

    FunctionCall computeEIRPFunction = GetFunction(config, "ComputeEIRP");

    int count = 0;
    // 遍历所有测试数据组
    for (const auto& testDataGroup : computeEIRPFunction.testData) {
        int dataFrequency = testDataGroup.dataFrequency;
        PerformanceTestSuiteResult testSuiteResult;
        testSuiteResult.suiteName = "PerformanceTest Group" + to_string(++count);

        for (int i = 0; i < dataFrequency; i++) {
            double EIRP0 = GetRaramValueEIRP(testDataGroup, "EIRP0", i);
            double BW3dB = GetRaramValueEIRP(testDataGroup, "BW3dB", i);
            double Augment = GetRaramValueEIRP(testDataGroup, "Augment", i);
            double Attenuation = GetRaramValueEIRP(testDataGroup, "Attenuation", i);
            double ThetaBeam = GetRaramValueEIRP(testDataGroup, "ThetaBeam", i);
            double PhiBeam = GetRaramValueEIRP(testDataGroup, "PhiBeam", i);
            double ThetaTarget = GetRaramValueEIRP(testDataGroup, "ThetaTarget", i);
            double PhiTarget = GetRaramValueEIRP(testDataGroup, "PhiTarget", i);

            GetProcessMemoryInfo(hProcess, &memCounter, sizeof(memCounter));
            memBefore = memCounter.WorkingSetSize;

            auto computeStart = std::chrono::high_resolution_clock::now();
            computeEIRP(EIRP0, BW3dB, Augment, Attenuation, ThetaBeam, PhiBeam, ThetaTarget, PhiTarget, initResult);
            auto computeEnd = std::chrono::high_resolution_clock::now();

            GetProcessMemoryInfo(hProcess, &memCounter, sizeof(memCounter));
            memAfter = memCounter.WorkingSetSize;
            SIZE_T memDiff = memAfter - memBefore;

            chrono::duration<double, std::milli> computeDuration = computeEnd - computeStart;
            string testName = "Case" + to_string(i + 1);
            double memUsage = memDiff / 1024.0;
            bool success = (initResult->INFO == 0);
            string errorMessage = success ? "" : "Test failed with INFO = 1";
            PerformanceTestResult testResult = PerformanceTestResult(testName, computeDuration.count(), memUsage, 0, success, errorMessage);
            testSuiteResult.addTestResult(testResult);
        }
        testReport.addSuiteResult(testSuiteResult);

    }


    // 执行final函数
    final(initResult);

    return testReport;

}

void PrintPerformanceTestReport(const PerformanceTestReport& report) {
    cout << "Report Name: " << report.reportName << "\n";
    cout << "-----------------------------------------\n";

    for (const auto& suite : report.suiteResults) {
        cout << "Suite Name: " << suite.suiteName << "\n";

        for (const auto& result : suite.testResults) {
            cout << "  Test Name: " << result.testName << "\n";
            cout << "    Execution Time: " << result.executionTime << " seconds\n";
            cout << "    Memory Usage: " << result.memoryUsage << " MB\n";
            cout << "    CPU Usage: " << result.cpuUsage << "%\n";
            cout << "    Success: " << (result.success ? "Yes" : "No") << "\n";
            if (!result.success) {
                cout << "    Error Message: " << result.errorMessage << "\n";
            }
            cout << "-----------------------------------------\n";
        }
    }
}

void ExportPerformanceTestReportToJson(const PerformanceTestReport& report, const char* filePath) {
    Document d;
    d.SetObject();

    Document::AllocatorType& allocator = d.GetAllocator();

    // 添加报告名称
    d.AddMember("reportName", Value().SetString(report.reportName.c_str(), allocator), allocator);

    // 创建测试套件结果的JSON数组
    Value suiteResultsArray(kArrayType);

    for (const auto& suiteResult : report.suiteResults) {
        Value suiteObject(kObjectType);
        suiteObject.AddMember("suiteName", Value().SetString(suiteResult.suiteName.c_str(), allocator), allocator);

        Value testResultsArray(kArrayType);
        for (const auto& testResult : suiteResult.testResults) {
            Value testObject(kObjectType);
            testObject.AddMember("testName", Value().SetString(testResult.testName.c_str(), allocator), allocator);
            testObject.AddMember("executionTime", Value(testResult.executionTime), allocator);
            testObject.AddMember("memoryUsage", Value(testResult.memoryUsage), allocator);
            testObject.AddMember("cpuUsage", Value(testResult.cpuUsage), allocator);
            testObject.AddMember("success", Value(testResult.success), allocator);

            if (!testResult.success) {
                testObject.AddMember("errorMessage", Value().SetString(testResult.errorMessage.c_str(), allocator), allocator);
            }

            testResultsArray.PushBack(testObject, allocator);
        }

        suiteObject.AddMember("testResults", testResultsArray, allocator);
        suiteResultsArray.PushBack(suiteObject, allocator);
    }

    d.AddMember("suiteResults", suiteResultsArray, allocator);

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    d.Accept(writer);

    // 写入文件
    ofstream file(filePath);
    if (file.is_open()) {
        file << buffer.GetString();
        file.close();
        cout << "Performance test data exported to: " << filePath << endl;
    }
    else {
        cerr << "Failed to open file for writing: " << filePath << endl;
    }
}