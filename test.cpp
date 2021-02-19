//
// Created by lbw on 2021/2/18.
//
#include <iostream>
#include <regex>
#include <sstream>
#include <cmath>
#include <cfloat>
#include <ctime>

using namespace std;

int main()
{
    time_t start, end;
    start = clock();
    for (double i = 0; i < 1000000000; i++)
    {

    }
    end = clock();
    cout << double(end - start) / CLOCKS_PER_SEC << endl;
}
