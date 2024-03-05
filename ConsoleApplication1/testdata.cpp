#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include "testcase.h"

// 生成整数类型的测试数据
int* generateRandomInt(int minValue, int maxValue) {
    int* result = new int(rand() % (maxValue - minValue + 1) + minValue);
    return result;
}

// 生成浮点数类型的测试数据
double* generateRandomDouble(double minValue, double maxValue) {
    double* result = new double(((double)rand() / RAND_MAX) * (maxValue - minValue) + minValue);
    return result;
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
