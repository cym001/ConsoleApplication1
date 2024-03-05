#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include "testcase.h"

// �����������͵Ĳ�������
int* generateRandomInt(int minValue, int maxValue) {
    int* result = new int(rand() % (maxValue - minValue + 1) + minValue);
    return result;
}

// ���ɸ��������͵Ĳ�������
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
