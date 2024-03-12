#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include "testCase.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" 
#include "rapidjson/stringbuffer.h"
#include <fstream>

#ifndef __TESTDATA_H__
#define __TESTDATA_H__

double GenerateRandomDouble(double minValue, double maxValue);

vector<double> GenerateRandomDataForParameter(Parameter& param, int numValuesPerDataSet);

void GenerateRandomDataForFunctionCall(FunctionCall& function, int numRandomDataSets, int numValuesPerDataSet);

void StoreGeneratedTestData(TestConfiguration& config, string functionName, int numRandomDataSets, int numValuesPerDataSet);

void PrintDataInFunction(const FunctionCall& function);

void ExportTestDataToJson(const FunctionCall& function, const char* filePath);

vector<double> GetParamsValue(TestDataComputeEIRP testData, string paramName);

void GenerateRandomDataForFunctionCall1(FunctionCall& function, int numRandomDataSets, int numValuesPerDataSet);

//int test_int(std::vector<Param>& params);
//double test_double(std::vector<Param>& params);
#endif

