#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <variant>
#include <ctime>
#include <fstream>
#include <Windows.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "testcase.h"

using namespace std;
using namespace rapidjson;


Document read_config(const char* path, char* readBuffer) {
    // 打开并读取配置文件
    FILE* fp;
    errno_t err = fopen_s(&fp, path, "rb"); 

    if (err != 0 || !fp) {
        cerr << "Failed to open config file" << '\n';
        return NULL;
    }

    // 使用动态内存分配来创建readBuffer
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(char) * 65536);

    Document d;
    d.ParseStream(is);
    fclose(fp);

    // 确保文档解析成功且为对象类型
    if (!d.IsObject()) {
        std::cerr << "JSON document is not an object." << '\n';
        return NULL;
    }

    return d;
}

HINSTANCE load_library(Document d){
    // 加载动态库
    // 确保配置中存在 "TestedLibraryPath"
    if (!d.HasMember("TestConfiguration") || !d["TestConfiguration"].IsObject() ||
        !d["TestConfiguration"].HasMember("TestedLibraryPath")) {
        std::cerr << "TestedLibraryPath not found in configuration." << '\n';
        return NULL;
    }

    const rapidjson::Value& config = d["TestConfiguration"];
    std::string libraryPath = config["TestedLibraryPath"].GetString();
    HINSTANCE hinstLib = LoadLibraryA(libraryPath.c_str());
    //HINSTANCE hinstLib = LoadLibrary(TEXT("G:\\final\\vs2010\\project\\computeEIRP\\x64\\Release\\computeEIRP.dll"));
    if (hinstLib == NULL) {
        std::cerr << "Cannot open library: " << libraryPath << '\n';
        return NULL;
    }
    return hinstLib;
}

TestConfiguration ParseTestConfig(const Document& d) {
    TestConfiguration config;

    const auto& testConfig = d["TestConfiguration"];
    config.testCaseFilePath = testConfig["TestCaseFilePath"].GetString();
    config.testedLibraryPath = testConfig["TestedLibraryPath"].GetString();
    config.concurrentUsers = testConfig["ConcurrentUsers"].GetInt();
    config.launchMode = testConfig["LaunchMode"].GetString();
    config.testRunningTime = testConfig["TestRunningTime"].GetInt();
    config.testExitCondition = testConfig["TestExitCondition"].GetString();

    for (const auto& m : d["InterfaceFunctionCallSequence"].GetArray()) {
        FunctionCall call;
        call.step = m["Step"].GetInt();
        call.functionName = m["FunctionName"].GetString();

        if (m.HasMember("Parameters")) {
            const auto& params = m["Parameters"];
            for (auto& p : params.GetObj()) {
                Parameter param;
                if (p.value.HasMember("Type")) {
                    param.type = p.value["Type"].GetString();
                }

                // 设置默认值，以便在字段缺失时使用
                param.value = p.value.HasMember("Value") ? p.value["Value"].GetDouble() : 0.0;
                param.min = p.value.HasMember("Min") ? p.value["Min"].GetDouble() : 0.0;
                param.max = p.value.HasMember("Max") ? p.value["Max"].GetDouble() : -1.0;

                call.parameters[p.name.GetString()] = param;
            }
        }
        config.interfaceFunctionCallSequence.push_back(call);
    }

    return config;
}

void PrintParameter(const Parameter& param) {
    if (param.type == "Result*") {
        cout << "Type: " << param.type << endl;
    }
    else {
        cout << "Type: " << param.type << ", Value: " << param.value
            << ", Min: " << param.min << ", Max: " << param.max << endl;
    }
}

void PrintFunctionCall(const FunctionCall& call) {
    cout << "Step: " << call.step << ", FunctionName: " << call.functionName << endl;

    cout << "Parameters:" << endl;
    for (const auto& param : call.parameters) {
        cout << param.first << " => ";
        PrintParameter(param.second);
    }

}

void PrintTestConfiguration(const TestConfiguration& config) {
    cout << "TestCaseFilePath: " << config.testCaseFilePath << endl;
    cout << "TestedLibraryPath: " << config.testedLibraryPath << endl;
    cout << "ConcurrentUsers: " << config.concurrentUsers << endl;
    cout << "LaunchMode: " << config.launchMode << endl;
    cout << "TestRunningTime: " << config.testRunningTime << " minutes" << endl;
    cout << "TestExitCondition: " << config.testExitCondition << endl;

    cout << "Interface Function Call Sequence:" << endl;
    for (const auto& call : config.interfaceFunctionCallSequence) {
        PrintFunctionCall(call);
    }
}