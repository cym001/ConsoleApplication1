#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <string>
#include <map>
#include <random>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" 
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include <fstream>
#include "testCase.h"
#include "testData.h"


// 生成浮点数类型的测试数据
double GenerateRandomDouble(double min, double max) {
    static mt19937 gen(time(nullptr)); 
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

vector<double> GenerateRandomDataForParameter(Parameter& param,  int numValuesPerDataSet) {
    vector<double> values;
    for (int i = 0; i < numValuesPerDataSet; ++i) {
        double value = GenerateRandomDouble(param.min, param.max);
        values.push_back(value);
    }
    return values;
}

vector<double> GenerateRandomDataForParameterLimit(Parameter& param, int numValuesPerDataSet, vector<double> temp, vector<double> BW3dB) {
    vector<double> values;
    for (int i = 0; i < numValuesPerDataSet; ++i) {
        double min = temp[i] - 0.25 * BW3dB[i];
        double max = temp[i] + 0.25 * BW3dB[i];
        double value = GenerateRandomDouble(min, max);
        values.push_back(value);
    }
    return values;
}

void GenerateRandomDataForFunctionCall(FunctionCall& function, int numRandomDataSets, int numValuesPerDataSet) {
    map<string, Parameter> parameters = function.parameters;
    for (int i = 0; i < numRandomDataSets; i++) {
        TestDataComputeEIRP testData;
        testData.groupNumber = i + 1;
        testData.description = "Random data set " + to_string(i + 1);
        testData.dataFrequency = numValuesPerDataSet;
        for (auto& [name, param] : parameters) {
            if (param.type == "double") {
                vector<double> values = GenerateRandomDataForParameter(param, numValuesPerDataSet);
                testData.datas.insert(pair<string, vector<double>>(name, values));
            }
        }
        function.testData.push_back(testData);
    }
    
}

void GenerateRandomDataForFunctionCall1(FunctionCall& function, int numRandomDataSets, int numValuesPerDataSet) {
    map<string, Parameter> parameters = function.parameters;
    for (int i = 0; i < numRandomDataSets; i++) {
        TestDataComputeEIRP testData;
        testData.groupNumber = i + 1;
        testData.description = "Random data set " + to_string(i + 1);
        testData.dataFrequency = numValuesPerDataSet;
        Parameter PhiTarget, ThetaTarget;
        for (auto& [name, param] : parameters) {
            if (name == "PhiTarget") {
                PhiTarget = param;
            }
            else if (name == "ThetaTarget") {
                ThetaTarget = param;

            }
            else if (param.type == "double") {
                vector<double> values = GenerateRandomDataForParameter(param, numValuesPerDataSet);
                testData.datas.insert(pair<string, vector<double>>(name, values));
            }
        }
        vector<double> BW3dB = GetParamsValue(testData, "BW3dB");
        vector<double> ThetaBeam = GetParamsValue(testData, "ThetaBeam");
        vector<double> PhiBeam = GetParamsValue(testData, "PhiBeam");
        vector<double> values = GenerateRandomDataForParameterLimit(PhiTarget, numValuesPerDataSet, PhiBeam, BW3dB);
        testData.datas.insert(pair<string, vector<double>>("PhiTarget", values));
        values = GenerateRandomDataForParameterLimit(ThetaTarget, numValuesPerDataSet, ThetaBeam, BW3dB);
        testData.datas.insert(pair<string, vector<double>>("ThetaTarget", values));
        function.testData.push_back(testData);
    }

}

void StoreGeneratedTestData(TestConfiguration& config, string functionName, int numRandomDataSets, int numValuesPerDataSet) {
    for (FunctionCall &function : config.interfaceFunctionCallSequence) {
        if (function.functionName == functionName) {
            GenerateRandomDataForFunctionCall1(function, numRandomDataSets, numValuesPerDataSet);
            
            return;
        }
    }

    cerr << "FunctionCall is not exist." << endl;
    
}

void PrintDataInFunction(const FunctionCall& function) {
    cout << "Function Call Data:\n";
    cout << "Step: " << function.step << "\n";
    cout << "Function Name: " << function.functionName << "\n";
    cout << "Parameters:\n";

    // 打印参数
    for (const auto& [name, param] : function.parameters) {
        cout << "  Name: " << name
            << ", Type: " << param.type
            << ", Value: " << param.value
            << ", Min: " << param.min
            << ", Max: " << param.max << "\n";
    }

    cout << "Result Pointer: " << function.resultPointer << "\n";

    // 打印测试数据
    cout << "Test Data:\n";
    for (const auto& testData : function.testData) {
        cout << "  Group Number: " << testData.groupNumber << "\n";
        cout << "  Data Frequency: " << testData.dataFrequency << "\n";
        cout << "  Description: " << testData.description << "\n";
        cout << "  Datas:\n";

        for (const auto& [paramName, values] : testData.datas) {
            cout << "    " << paramName << ": ";
            for (const auto& value : values) {
                cout << value << " ";
            }
            cout << "\n";
        }
    }
}

void ExportTestDataToJson(const FunctionCall& function, const char* filePath) {
    // 创建一个RapidJSON的Document对象
    Document d;
    d.SetObject();

    Document::AllocatorType& allocator = d.GetAllocator();

    // 添加基础信息
    Value functionNameValue;
    functionNameValue.SetString(function.functionName.c_str(), allocator);
    d.AddMember("functionName", functionNameValue, allocator);

    Value parameters(kObjectType);
    // 遍历并添加参数信息
    for (const auto& [paramName, param] : function.parameters) {
        Value parameterObject(kObjectType);
        parameterObject.AddMember("type", Value().SetString(param.type.c_str(), allocator), allocator);
        parameterObject.AddMember("value", Value(param.value), allocator);
        parameterObject.AddMember("min", Value(param.min), allocator);
        parameterObject.AddMember("max", Value(param.max), allocator);
        parameters.AddMember(Value().SetString(paramName.c_str(), allocator), parameterObject, allocator);
    }
    d.AddMember("parameters", parameters, allocator);

    // 测试数据
    Value testDataArray(kArrayType);
    for (const auto& testData : function.testData) {
        Value testDataObject(kObjectType);
        testDataObject.AddMember("groupNumber", Value(testData.groupNumber), allocator);
        testDataObject.AddMember("dataFrequency", Value(testData.dataFrequency), allocator);
        testDataObject.AddMember("description", Value().SetString(testData.description.c_str(), allocator), allocator);

        Value datas(kObjectType);
        for (const auto& [paramName, values] : testData.datas) {
            Value valuesArray(kArrayType);
            for (double value : values) {
                valuesArray.PushBack(Value(value), allocator);
            }
            datas.AddMember(Value().SetString(paramName.c_str(), allocator), valuesArray, allocator);
        }
        testDataObject.AddMember("datas", datas, allocator);
        testDataArray.PushBack(testDataObject, allocator);
    }
    d.AddMember("testData", testDataArray, allocator);

    // 写入文件
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

/*void PrintDataInFunction(TestConfiguration& config) {
    for (const auto& [name, param] : parameters) {
        cout << "Parameter: " << name << endl;
        for (const auto& data : param.testData) {
            cout << "  Group " << data.groupNumber << " (" << data.description << "): ";
            for (const auto& value : data.values) {
                cout << value << " ";
            }
            cout << endl;
        }
    }
}

void ExportTestDataToJson(const map<string, Parameter>& parameters, const char* filePath) {
    Document d; 
    d.SetObject(); 

    Document::AllocatorType& allocator = d.GetAllocator();

    Value params(kArrayType); 

    for (const auto& [name, param] : parameters) {
        Value paramObj(kObjectType); 
        paramObj.AddMember("name", Value().SetString(name.c_str(), allocator), allocator);
        paramObj.AddMember("type", Value().SetString(param.type.c_str(), allocator), allocator);

        Value testDataArray(kArrayType); 
        for (const auto& testData : param.testData) {
            Value testDataObj(kObjectType);
            testDataObj.AddMember("groupNumber", Value(testData.groupNumber), allocator);
            testDataObj.AddMember("description", Value().SetString(testData.description.c_str(), allocator), allocator);

            Value valuesArray(kArrayType); 
            for (double val : testData.values) {
                valuesArray.PushBack(Value(val), allocator);
            }

            testDataObj.AddMember("values", valuesArray, allocator);
            testDataArray.PushBack(testDataObj, allocator);
        }

        paramObj.AddMember("testData", testDataArray, allocator);
        params.PushBack(paramObj, allocator);
    }

    d.AddMember("parameters", params, allocator);

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


int test_int(std::vector<Param>& params) {
    int* param1 = static_cast<int*>(params[0].data);
    double* param2 = static_cast<double*>(params[1].data);
    return *param1 + *param2;
}

double test_double(std::vector<Param>& params) {
    int* param1 = static_cast<int*>(params[0].data);
    double* param2 = static_cast<double*>(params[1].data);
    return *param1 + *param2;
}*/

vector<double> GetParamsValue(TestDataComputeEIRP testData, string paramName) {
    auto it = testData.datas.find(paramName);
    vector<double> null;
    if (it != testData.datas.end()) {
        const auto& values = it->second;
        return values;
    }
    else {
        cerr << "Parameter not found: " << paramName << endl;
    }
    return null;
}