#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include "testcase.h"

#ifndef __TESTDATA_H__
#define __TESTDATA_H__

double GenerateRandomDouble(double minValue, double maxValue);
void GenerateRandomAndBoundaryDataForParameter(Parameter& param, int numRandomDataSets, int numValuesPerDataSet);
void PrintDataInParameter(map<string, Parameter> parameters);
//int test_int(std::vector<Param>& params);
//double test_double(std::vector<Param>& params);
#endif

