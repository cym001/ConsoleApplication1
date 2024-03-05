#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <string>
#include <map>
#include <random>
#include "testcase.h"


// 生成浮点数类型的测试数据
double GenerateRandomDouble(double min, double max) {
    static mt19937 gen(time(nullptr)); // 随机数生成器
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
