#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include "testcase.h"

#ifndef __TESTDATA_H__
#define __TESTDATA_H__

int* generateRandomInt(int minValue, int maxValue);
double* generateRandomDouble(double minValue, double maxValue);
int test_int(std::vector<Param>& params);
double test_double(std::vector<Param>& params);
#endif

