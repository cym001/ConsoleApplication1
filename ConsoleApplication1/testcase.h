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

#ifndef __TESTCASE_H__
#define __TESTCASE_H__

using namespace std;
using namespace rapidjson;

struct Result {
    int INFO;
    double EIRP;
};

struct MyStruct {
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

// ����һ���࣬���������ӿں���������Ĺ�����ϵ
class InterfaceDescription {
public:
    // ���һ���ӿں���
    template<typename ReturnType>
    void addFunction(const Function& function, const FunctionType<ReturnType>& functionType) {
        functions.emplace_back(function, functionType);
    }

    // ִ��ָ���Ľӿں���
    template<typename ReturnType>
    ReturnType executeFunction(const string& functionName) {
        for (const auto& [function, functionType] : functions) {
            if (function.name == functionName) {
                return execute<ReturnType>(functionType, function.params);
            }
        }
        cerr << "�ӿں��� " << functionName << " δ�ҵ�\n";
        return ReturnType{}; 
    }

private:
    // �洢�ӿں��������Ӧ�ĺ�������
    vector<pair<Function, variant<FunctionType<Result*>, FunctionType<MyStruct*>, FunctionType<int>, FunctionType<double>>>> functions;
    // ����ģ�壬���ڱ���ʱ���
    template<typename T>
    static constexpr bool always_false() { return false; }
    // ��������ģ�壬ִ�нӿں���
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

Document read_config(const char* path, char* readBuffer);
HINSTANCE load_library(Document d);
#endif