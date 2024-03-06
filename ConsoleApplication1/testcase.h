#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <map>
#include <variant>
#include <ctime>
#include <fstream>
#include <Windows.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#ifndef __TESTCASE_H__
#define __TESTCASE_H__

using namespace std;
using namespace rapidjson;

struct Result {
    int INFO;
    double EIRP;
};

struct TestData {
    int groupNumber;
    string description;
    vector<double> values; // 存储当前数据组的多个测试值
};

struct Parameter {
    string type;
    double value{ 0 };
    double min{ 0 };
    double max{ 0 };
    vector<TestData> testData;
};

struct FunctionCall {
    int step;
    string functionName;
    map<string, Parameter> parameters; 
    string resultPointer; 
};

struct TestConfiguration {
    string testCaseFilePath;
    string testedLibraryPath;
    int concurrentUsers{ 1 };
    string launchMode;
    int testRunningTime{ 10 };
    string testExitCondition;
    vector<FunctionCall> interfaceFunctionCallSequence;
};

Document read_config(const char* path, char* readBuffer);

HINSTANCE load_library(const Document& d);

TestConfiguration ParseTestConfig(const Document& d);

void PrintParameter(const Parameter& param);

void PrintFunctionCall(const FunctionCall& call);

void PrintTestConfiguration(const TestConfiguration& config);


/*struct MyStruct {
    int value1;
    double value2;

    MyStruct(int v1, double v2) : value1(v1), value2(v2) {}
};


struct Param {
    string type;
    string name;
    void* data;  
};


struct Function {
    string returnType;
    string name;
    vector<Param> params;
};


template<typename ReturnType>
using FunctionType = function<ReturnType(vector<Param>&)>;

// 定义一个类，用于描述接口函数与参数的关联关系
class InterfaceDescription {
public:
    // 添加一个接口函数
    template<typename ReturnType>
    void addFunction(const Function& function, const FunctionType<ReturnType>& functionType) {
        functions.emplace_back(function, functionType);
    }

    // 执行指定的接口函数
    template<typename ReturnType>
    ReturnType executeFunction(const string& functionName) {
        for (const auto& [function, functionType] : functions) {
            if (function.name == functionName) {
                return execute<ReturnType>(functionType, function.params);
            }
        }
        cerr << "接口函数 " << functionName << " 未找到\n";
        return ReturnType{}; 
    }

private:
    // 存储接口函数及其对应的函数类型
    vector<pair<Function, variant<FunctionType<Result*>, FunctionType<MyStruct*>, FunctionType<int>, FunctionType<double>>>> functions;
    // 辅助模板，用于编译时检查
    template<typename T>
    static constexpr bool always_false() { return false; }
    // 辅助函数模板，执行接口函数
    template<typename ReturnType>
    ReturnType execute(const variant<FunctionType<Result*>, FunctionType<MyStruct*>, FunctionType<int>, FunctionType<double>>& functionTypeVariant, vector<Param> params) {
        if constexpr (is_same<ReturnType, Result*>::value) {
            if (auto functionType = get_if<FunctionType<Result*>>(&functionTypeVariant)) {
                return (*functionType)(params);
            }
        }
        else if constexpr (is_same<ReturnType, MyStruct*>::value) {
            if (auto functionType = get_if<FunctionType<MyStruct*>>(&functionTypeVariant)) {
                return (*functionType)(params);
            }
        }
        else if constexpr (is_same<ReturnType, int>::value) {
            if (auto functionType = get_if<FunctionType<int>>(&functionTypeVariant)) {
                return (*functionType)(params);
            }
        }
        else if constexpr (is_same<ReturnType, double>::value) {
            if (auto functionType = get_if<FunctionType<double>>(&functionTypeVariant)) {
                return (*functionType)(params);
            }
        }
        else {
            static_assert(always_false<ReturnType>::value, "Unsupported return type");
        }

        cerr << "Unsupported return type or function not found.\n";
        return ReturnType{};
    }

};
*/
#endif