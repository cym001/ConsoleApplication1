#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include "testcase.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" 
#include "rapidjson/stringbuffer.h"
#include <fstream>

#ifndef __TESTDATA_H__
#define __TESTDATA_H__

double GenerateRandomDouble(double minValue, double maxValue);
void GenerateRandomAndBoundaryDataForParameter(Parameter& param, int numRandomDataSets, int numValuesPerDataSet);
void PrintDataInParameter(map<string, Parameter> parameters);
void ExportTestDataToJson(const map<string, Parameter>& parameters, const char* filePath);
//int test_int(std::vector<Param>& params);
//double test_double(std::vector<Param>& params);
#endif

