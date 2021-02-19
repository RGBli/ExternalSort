//
// Created by lbw on 2021/2/15.
//
#include <iostream>
#include <fstream>
using namespace std;

#define SIZE 4
#define MAX_INT 0x7fffffff
#define MIN_INT -1

/* 缓冲区
 * 在生成顺序段的阶段充当缓冲区
 * 在多路归并阶段 buf 的每一部分充当 segment 的缓冲区*/
int buf[SIZE];

/* 多路归并阶段时的段*/
struct Segment
{
    // 指向 buf
    int *buffer;
    // 缓冲区的长度
    int length;
    // 当前下标
    int index;
};

/* 生成多个顺序段*/
int generateSegment(const char *inputFile)
{
    ifstream in(inputFile);
    ofstream out;
    char segmentOutputFile[20];
    // 记录最后一个 segment 截断的位置
    int cut = 0;
    int i = 0, count = 0;
    while (!in.eof())
    {
        while (i < SIZE && !in.eof())
        {
            in >> buf[i];
            i++;
            cut++;
        }
        sort(buf, buf + cut);
        count++;
        sprintf(segmentOutputFile, "%d.txt", count);
        out.open(segmentOutputFile);
        for (int j = 0; j < cut; j++)
        {
            out << buf[j] << endl;
        }
        out.close();
        i = 0;
        cut = 0;
    }
    return count;
}

/* 调整败者树
 * s 是需要调整的下标*/
void adjust(Segment *segments, int *ls, int n, int s)
{
    // 得到 s 的在败者树上面的父节点
    int t = (s + n) / 2;
    // 不断与父节点对比，直到败者树的根节点
    while (t > 0) {
        if(s == -1)
        {
            break;
        }
        // 如果当前节点 s（胜者）大于父节点，则交换
        if (ls[t] == -1 || segments[s].buffer[segments[s].index] > segments[ls[t]].buffer[segments[ls[t]].index])
        {
            swap(ls[t], s);
        }
        // 得到败者树的上一个父节点，继续与当前胜者 s 比较
        t /= 2;
    }
    // 最终的胜者记录于 ls[0]
    ls[0] = s;
}

/* 构建败者树*/
void buildLoserTree(Segment *segment, int *ls, int n)
{
    for (int i = 0; i < n; i++)
    {
        // 所有败者树初始化为 MIN 的下标
        ls[i] = MIN_INT;
    }
    for (int i = n - 1; i >= 0; i--)
    {
        // 依次调整即可完成初始化
        adjust(segment, ls, n, i);
    }
}

/* 多路归并排序
 * 利用顺序段来排序，生成最后的排序文件*/
void mergeSort(Segment *segments, int *ls, int numOfSegment, const char *sortedFile)
{
    // 缓冲区中为每个段分配的长度，即使除不尽也没关系，就少利用缓冲区中的几个空间而已
    int lengthPerSegment = SIZE / numOfSegment;
    // 在缓冲区中为每个段分配缓冲区
    for (int i = 0; i < numOfSegment; i++)
    {
        segments[i].buffer = buf + i * lengthPerSegment;
    }
    // 为每个 segment 创建一个 input stream 来从段文件中读取
    ifstream in[numOfSegment];
    char fileNames[20];
    for (int i = 0; i < numOfSegment; i++)
    {
        sprintf(fileNames, "%d.txt", i + 1);
        in[i].open(fileNames);
    }
    // 将顺串文件的数据读到缓冲区中
    for (int i = 0; i < numOfSegment; i++)
    {
        int j = 0;
        while(in[i] >> segments[i].buffer[j])
        {
            j++;
            if(j == lengthPerSegment)
            {
                break;
            }
        }
        segments[i].length = j;
        segments[i].index = 0;
    }
    buildLoserTree(segments, ls, numOfSegment);
    ofstream out(sortedFile);
    // 剩下的 segment 个数
    int aliveSegments = numOfSegment;
    while (aliveSegments > 0)
    {
        // 输出败者树的根节点 ls[0] 对应的数字到文件中
        out << segments[ls[0]].buffer[segments[ls[0]].index++] << endl;
        // 一个 segment 的缓冲区读完了，需要再读一批到该 segment 的缓冲区中
        if (segments[ls[0]].index == segments[ls[0]].length)
        {
            int j = 0;
            while (in[ls[0]] >> segments[ls[0]].buffer[j])
            {
                j++;
                if (j == lengthPerSegment)
                {
                    break;
                }
            }
            segments[ls[0]].length = j;
            segments[ls[0]].index = 0;
        }
        // 一个段真正读完了
        if (segments[ls[0]].length == 0)
        {
            segments[ls[0]].buffer[segments[ls[0]].index] = MAX_INT;
            aliveSegments--;
        }
        adjust(segments, ls, numOfSegment, ls[0]);
    }
}

int main()
{
    //int numOfSegment = generateSegment("test.txt");
    int numOfSegment = 3;
    const char *sortedFile = "output.txt";
    int *ls = new int[numOfSegment];
    Segment *segments = new Segment[numOfSegment];
    mergeSort(segments, ls, numOfSegment, sortedFile);
    return 0;
}