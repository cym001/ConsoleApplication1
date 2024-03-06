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
#include <fstream>
#include "testCase.h"


// 生成浮点数类型的测试数据
double GenerateRandomDouble(double min, double max) {
    static mt19937 gen(time(nullptr)); 
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

void GenerateRandomAndBoundaryDataForParameter(Parameter& param, int numRandomDataSets, int numValuesPerDataSet) {
    for (int i = 0; i < numRandomDataSets; ++i) {
        TestData randomData;
        randomData.groupNumber = i + 1;
        randomData.description = "Random data set " + std::to_string(i + 1);
        for (int j = 0; j < numValuesPerDataSet; ++j) {
            double value = GenerateRandomDouble(param.min, param.max);
            randomData.values.push_back(value);
        }
        param.testData.push_back(randomData);
    }

    TestData boundaryData;
    boundaryData.groupNumber = numRandomDataSets + 1;
    boundaryData.description = "Boundary data set";
    double boundaryValue = (param.min + param.max) / 2;
    boundaryData.values.push_back(boundaryValue); 
    param.testData.push_back(boundaryData);
}

void GenerateRandomAndBoundaryDataForFunctionCall(FunctionCall function, int numRandomDataSets, int numValuesPerDataSet) {
    map<string, Parameter> parameters = function.parameters;
    for (auto& [name, param] : parameters) {
        if (param.type == "double") {
            GenerateRandomAndBoundaryDataForParameter(param, numRandomDataSets, numValuesPerDataSet);
        }

    }
}

void StoreGeneratedTestData(TestConfiguration& config, size_t functionCallIndex, int numRandomDataSets, int numValuesPerDataSet) {
    if (config.interfaceFunctionCallSequence.size() > functionCallIndex) {
        auto& parameters = config.interfaceFunctionCallSequence[functionCallIndex].parameters;

        for (auto& [name, param] : parameters) {
            if (param.type == "double") {
                GenerateRandomAndBoundaryDataForParameter(param, numRandomDataSets, numValuesPerDataSet);
            }
        }
    }
    else {
        cerr << "FunctionCall index out of range." << endl;
    }
}

void PrintDataInParameter(map<string, Parameter> parameters) {
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


/*int test_int(std::vector<Param>& params) {
    int* param1 = static_cast<int*>(params[0].data);
    double* param2 = static_cast<double*>(params[1].data);
    return *param1 + *param2;
}

double test_double(std::vector<Param>& params) {
    int* param1 = static_cast<int*>(params[0].data);
    double* param2 = static_cast<double*>(params[1].data);
    return *param1 + *param2;
}*/
