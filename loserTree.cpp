//
// Created by lbw on 2021/2/15.
//
#include <iostream>

using namespace std;

// 败者树容量，即多路归并数目
#define LEN 5
// 所有数据的可能最小值
#define MIN -1

// 败者树，即非叶子节点，ls[0] 存放胜者，其余存放败者
int ls[LEN + 1];
// 叶子节点，存放多路归并的头元素值，多出来的一位放 MIN
int buf[LEN + 1];

/*调整败者树 buf
 * s 是需要调整的 buf 的下标*/
void adjust(int s, int *buf) {
    // 得到 s 的在败者树上面的父节点
    int t = (s + LEN) / 2;
    // 不断与父节点对比，直到败者树的根节点
    while (t > 0) {
        // 如果当前节点s（胜者）大于父节点，则交换
        if (buf[s] > buf[ls[t]]) {
            swap(ls[t], s);
        }
        // 得到败者树的上一个父节点，继续与当前胜者 s 比较
        t /= 2;
    }
    // 最终的胜者记录于ls[0]
    ls[0] = s;
}

/*构建败者树*/
void build(int *buf) {
    for (int & l : ls)
    {
        l = -1;
    }
    for (int i = 0; i < LEN; i++)
    {
        // 依次调整即可完成初始化
        adjust(i, buf);
    }
}

int main() {
    int tmp[5] = {18, 21, 16, 11, 19};
    // 给 buf 数组赋值
    memcpy(buf, tmp, LEN * sizeof(int));
    build(buf);
    // 输出11
    cout << buf[ls[0]] << endl;

    // 取出11后，buf[3] = 17
    int tmp1[5] = {18, 21, 16, 17, 19};
    // 改动 buf[3]，给 buf 数组赋新值
    memcpy(buf, tmp1, LEN * sizeof(int));
    adjust(3, buf);
    // 输出16
    cout << buf[ls[0]] << endl;
    return 0;
}
