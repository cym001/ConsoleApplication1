#include <iostream>
#include <fstream>
#include <Windows.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "testcase.h"
#include "testdata.h"
#include "testExecution.h"
#include <string>

using namespace std;
using namespace rapidjson;


int main() {
    char* readBuffer = new char[65536];
    const char* path = ".\\testcase\\test_cases.json";
    Document d = read_file(path, readBuffer);

    HINSTANCE hinstLib = load_library(d);


    TestConfiguration config1 = ParseTestConfig(d);
    PrintTestConfiguration(config1);
    //StoreGeneratedTestData(config1, "ComputeEIRP", 3, 4);
    //const char* json_path = ".\\testdata\\testdata.json";
    ReadTestData(".\\testdata\\testdata.json", config1);
    FunctionCall function_EIRP = GetFunction(config1, "ComputeEIRP");
    PrintDataInFunction(function_EIRP);
    //string data_path = CurrentTimeForFilePath(".\\testdata\\testdata", ".json");
    //ExportTestDataToJson(function_EIRP, data_path);
    //PrintDataInParameter(parameters);
    //ExportTestDataToJson(parameters, json_path);

    //TestResult testResult = PerformAutomatedTestsComputeEIRP(hinstLib, config1);

    //ExportTestResultsToJson(testResult.groupTestResult, ".\\testresult\\test_result.json");

    //PerformanceTestReport testReport = PerformanceTestComputeEIRP(hinstLib, config1);

    //PrintPerformanceTestReport(testReport);

    //string performance_path = CurrentTimeForFilePath(".\\testresult\\performance_test_result", ".json");
    //ExportPerformanceTestReportToJson(testReport, performance_path);

    //GenerateHtmlReport(testReport, ".\\testresult\\performance_test_result1.html");

    FreeLibrary(hinstLib);

    delete[] readBuffer;

    return 0;
}


/*Result* resultEIRP = nullptr;

// 遍历并执行测试序列
const Value& steps = d["InterfaceFunctionCallSequence"];
for (auto& step : steps.GetArray()) {
    string functionName = step["FunctionName"].GetString();

    if (functionName == "init") {
        resultEIRP = init();
        if (!resultEIRP) {
            cerr << "Init function failed." << endl;
            break; 
        }
    }
    else if (functionName == "ComputeEIRP" && resultEIRP != nullptr) {
        
        double EIRP0 = step["Parameters"]["EIRP0"].GetDouble();
        double BW3dB = step["Parameters"]["BW3dB"].GetDouble();
        double Augment = step["Parameters"]["Augment"].GetDouble();
        double Attenuation = step["Parameters"]["Attenuation"].GetDouble();
        double ThetaBeam = step["Parameters"]["ThetaBeam"].GetDouble();
        double PhiBeam = step["Parameters"]["PhiBeam"].GetDouble();
        double ThetaTarget = step["Parameters"]["ThetaTarget"].GetDouble();
        double PhiTarget = step["Parameters"]["PhiTarget"].GetDouble();

        computeEIRP(EIRP0, BW3dB, Augment, Attenuation, ThetaBeam, PhiBeam, ThetaTarget, PhiTarget, resultEIRP);
        cout << "ComputeEIRP executed: INFO=" << resultEIRP->INFO << ", EIRP=" << resultEIRP->EIRP << endl;
    }
    else if (functionName == "final" && resultEIRP != nullptr) {
        final(resultEIRP);
        cout << "Final function executed." << endl;
        delete resultEIRP; 
        resultEIRP = nullptr;
    }
}*/