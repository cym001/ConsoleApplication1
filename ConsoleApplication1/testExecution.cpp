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
#include "testExecution.h"
#include "testdata.h"


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

void PerformAutomatedTestsComputeEIRP(HINSTANCE hinstLib, const TestConfiguration& config) {
    std::vector<SingleTestResult> testResults;
    Result* initResult = nullptr;
    int testFrequency = 0;
    int groupFrequency = 0;

    auto init = reinterpret_cast<Result * (*)()>(GetProcAddress(hinstLib, "init"));
    auto computeEIRP = reinterpret_cast<void(*)(double, double, double, double, double, double, double, double, Result*)>(GetProcAddress(hinstLib, "ComputeEIRP"));
    auto final = reinterpret_cast<void(*)(Result*)>(GetProcAddress(hinstLib, "final"));

    if (!init || !computeEIRP || !final) {
        cerr << "Function cannot be loaded." << endl;
        testResults.emplace_back("null", false, "load function failed");
        return;
    }

    initResult = init();
    if (!initResult) {
        cerr << "init function execution failed." << endl;
        return; // 如果init失败，则终止测试
    }

    FunctionCall computeEIRPFunction = GetFunction(config, "ComputeEIRP");

    TestResult testResult;
    testResult.groupFrequency = 0;

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

    for (const auto& groupResult : testResult.groupTestResult) {
        cout << "Group Test Frequency: " << groupResult.testFrequency << "\n";
        for (const auto& result : groupResult.singleTestResult) {
            cout << "Function: " << result.functionName
                << ", Success: " << (result.success ? "Yes" : "No")
                << ", Error Message: " << result.errorMessage;
            cout << "\n";
        }
    }

    // 执行final函数
    final(initResult);



}