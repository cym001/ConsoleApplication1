#include <iostream>
#include <fstream>
#include <Windows.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "testcase.h"
#include "testdata.h"

using namespace std;
using namespace rapidjson;


int main() {
    char* readBuffer = new char[65536];
    const char* path = ".\\testcase\\test_cases.json";
    Document d = read_config(path, readBuffer);

    HINSTANCE hinstLib = load_library(d);



    // 获取函数指针
    auto init = (Result * (*)())GetProcAddress(hinstLib, "init");
    auto computeEIRP = (void (*)(double, double, double, double, double, double, double, double, Result*))GetProcAddress(hinstLib, "ComputeEIRP");
    auto final = (void (*)(Result*))GetProcAddress(hinstLib, "final");

    if (!init || !computeEIRP || !final) {
        cerr << "Function cannot be loaded" << '\n';
        FreeLibrary(hinstLib);
        return 1;
    }


    TestConfiguration config1 = ParseTestConfig(d);
    PrintTestConfiguration(config1);
    StoreGeneratedTestData(config1, 1, 3, 4);
    map<string, Parameter> parameters = config1.interfaceFunctionCallSequence[1].parameters;
    const char* json_path = ".\\testdata\\testdata.json";
    PrintDataInParameter(parameters);
    ExportTestDataToJson(parameters, json_path);
    // 卸载动态库
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
            break; // 退出循环
        }
    }
    else if (functionName == "ComputeEIRP" && resultEIRP != nullptr) {
        // 读取并设置参数
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
        delete resultEIRP; // 清理资源
        resultEIRP = nullptr;
    }
}*/