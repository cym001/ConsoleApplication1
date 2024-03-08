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
        return; // Èç¹ûinitÊ§°Ü£¬ÔòÖÕÖ¹²âÊÔ
    }

    

    // Ö´ÐÐfinalº¯Êý
    final(initResult);


    
    

    


}