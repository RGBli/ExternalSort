//
// Created by lbw on 2021/2/15.
//
#include <ctime>
#include <fstream>
#include <cstdlib>

using namespace std;

// 生成 [1, size] 的随机整数文件
const int size = 1000000;
int data[size];

int main()
{
    for(int i = 0; i < size; i++)
    {
        data[i] = i + 1;
    }
    ofstream output("../1m.txt");
    // 用时钟初始化随机数种子
    srand((unsigned)time(0));
    // 打乱数据
    for(int i = 0; i < size; i++)
    {
        int n1 = rand() % size;
        int n2 = rand() % size;
        swap(data[n1], data[n2]);
    }
    //输出到文件
    for(int i = 0; i < size; i++)
    {
        // 避免在末尾产生空行
        if (i == size - 1)
        {
            output << data[i];
        }
        else
        {
            output << data[i] << endl;
        }
    }
    output.close();
    return 0;
}
