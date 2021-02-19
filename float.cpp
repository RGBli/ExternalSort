//
// Created by lbw on 2021/2/17.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cmath>
#include <cfloat>

using namespace std;

/* 缓冲区大小，每个8bytes*/
#define SIZE 100000

/* 最大的浮点数*/
#define MAX DBL_MAX

/* 缓冲区
 * 在生成顺序段的阶段充当缓冲区
 * 在多路归并阶段 buf 的每一部分充当 segment 的缓冲区*/
double buf[SIZE];

/* 非法条目数量*/
int illegal = 0;

/* 多路归并阶段时的段*/
struct Segment
{
    // 指向 buf
    double *buffer;
    // 缓冲区的长度
    int length;
    // 当前下标
    int index;
};

/* 判断字符串是否能转为合法的 double*/
bool isValid(string s)
{
    if (!regex_match(s, regex("[\\.\\deE-]+"))
            || (s.find('-') != -1 && s.find_last_of('-') != 0)
            || (s.find('.') != -1 && s.find_last_of('.') != s.find('.'))
            || (s.find('e') != -1 && s.find_last_of('e') != s.find('e'))
            || (s.find('E') != -1 && s.find_last_of('E') != s.find('E')))
    {
        // 显示非法条目
        cout << s << endl;
        illegal++;
        return false;
    }
    return true;
}

/* 将一行字符串转为 double*/
double stringToDouble(string s)
{
    double res;
    // 科学计数法，需要提取出 e 或 E 之前和之后的部分
    if (s.find('e') != -1 || s.find('E') != -1)
    {
        // 判断正负号
        bool flag = (s[0] != '-');
        // 找到 e 或 E 的位置
        int posE = (s.find('e') == -1) ? s.find('E') : s.find('e');
        double part1;
        int part2;
        stringstream ss1(s.substr(0, posE));
        ss1 >> part1;
        stringstream ss2(s.substr(posE + 2));
        ss2 >> part2;
        res = part1 * pow(10, part2);
        if (!flag)
        {
            res = -res;
        }
    }
    // 非科学计数法，直接转数字
    else
    {
        stringstream ss(s);
        ss >> res;
    }
    return res;
}

/* 将 double 转为10位有效数字的科学计数法的字符串*/
string doubleToString(double n)
{
    string s = to_string(n);
    bool positiveFlag = (s[0] != '-');
    // 找到第一个非零数字的位置
    int firstNonZeroPos = -1;
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] != '0' && s[i] != '-' && s[i] != '.')
        {
            firstNonZeroPos = i;
            break;
        }
    }
    // 如果没有非零数字
    if (firstNonZeroPos == -1)
    {
        return "0.000000000E+000";
    }
    // 找到小数点的位置
    int pointPos = s.find('.');
    // 如果没有小数点
    if (pointPos == -1)
    {
        pointPos = s.length();
    }
    // 计算指数
    int exp;
    if (pointPos - firstNonZeroPos >= 1)
    {
        exp = pointPos - firstNonZeroPos - 1;
    }
    else
    {
        exp = pointPos - firstNonZeroPos;
    }
    // 从第一个非零字符开始截取
    string baseStr = s.substr(firstNonZeroPos);
    // 删除小数点
    if (baseStr.find('.') != -1)
    {
        baseStr.erase(baseStr.find('.'), 1);
    }
    // 不足十位，补零
    if (baseStr.length() <= 10)
    {
        baseStr.insert(baseStr.length(), 10 - baseStr.length(), '0');
    }
        // 十位以上，四舍五入
    else
    {
        // 四舍
        if (baseStr[10] < '5')
        {
            baseStr = baseStr.substr(0, 10);
        }
        // 五入
        else
        {
            bool bitFlag = true;
            for (int i = 9; i >= 0; i--)
            {
                if (bitFlag)
                {
                    if (baseStr[i] == '9')
                    {
                        baseStr[i] = '0';
                        bitFlag = true;
                    }
                    else
                    {
                        baseStr[i]++;
                        bitFlag = false;
                    }
                }
                else
                {
                    bitFlag = false;
                    break;
                }
            }
            if (bitFlag)
            {
                baseStr.insert(0, 1, '1');
            }
            baseStr = baseStr.substr(0, 10);
        }
    }
    // 加上小数点
    if (baseStr.length() > 1)
    {
        baseStr.insert(1, 1, '.');
    }
    string expStr;
    string tmp = to_string(exp);
    if (exp >= 0)
    {
        if (tmp.length() < 3)
        {
            tmp.insert(0, 3 - tmp.length(), '0');
        }
        expStr = "E+" + tmp;
    }
    else
    {
        if (tmp.length() < 3)
        {
            tmp.insert(1, 3 - tmp.length() + 1, '0');
        }
        expStr = "E" + tmp;
    }
    if (!positiveFlag)
    {
        return "-" + baseStr + expStr;
    }
    else
    {
        return baseStr + expStr;
    }
}

/* 生成多个顺序段*/
int generateSegment(string inputFile)
{
    ifstream in(inputFile);
    ofstream out;
    string segmentOutputFile;
    stringstream ss;
    // 记录最后一个 segment 截断的位置
    int cut = 0;
    int i = 0, count = 0;
    string tmp;
    while (!in.eof())
    {
        while (i < SIZE && !in.eof())
        {
            in >> tmp;
            // 判断是否有效
            if (!isValid(tmp))
            {
                continue;
            }
            // 将字符串转为 double
            buf[i] = stringToDouble(tmp);
            i++;
            cut++;
        }
        // 对一个段排序
        sort(buf, buf + cut);
        count++;
        // 将顺序段写入文件
        ss.str("");
        ss << count << ".txt";
        segmentOutputFile = ss.str();
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
        if (s == -1)
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
        ls[i] = -1;
    }
    for (int i = n - 1; i >= 0; i--)
    {
        // 依次调整即可完成初始化
        adjust(segment, ls, n, i);
    }
}

/* 多路归并排序
 * 利用顺序段来排序，生成最后的排序文件*/
void mergeSort(Segment *segments, int *ls, int numOfSegment, string sortedFile)
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
    string fileName;
    stringstream ss;
    for (int i = 0; i < numOfSegment; i++)
    {
        ss.str("");
        ss << i + 1 << ".txt";
        fileName = ss.str();
        in[i].open(fileName);
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
        out << doubleToString(segments[ls[0]].buffer[segments[ls[0]].index++]);
        //out << segments[ls[0]].buffer[segments[ls[0]].index++] << endl;
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
            segments[ls[0]].buffer[segments[ls[0]].index] = MAX;
            aliveSegments--;
        }
        adjust(segments, ls, numOfSegment, ls[0]);
        // 避免在末尾产生空行
        if (aliveSegments > 0)
        {
            out << endl;
        }
    }
}

int main()
{
    // 输入输出文件路径
    string inputFile;
    string outputFile;
    // 读取参数
    ifstream in("Sort.param");
    string line;
    while (!in.eof())
    {
        in >> line;
        if (line.find("path_input") == 0)
        {
            inputFile = line.substr(11);
        }
        else if (line.find("path_output") == 0)
        {
            outputFile = line.substr(12);
        }
    }
    in.close();

    time_t start1, end1, start2, end2;
    start1 = clock();
    int numOfSegment = generateSegment(inputFile);
    //int numOfSegment = 3;
    end1 = clock();
    cout << "生成顺序段完成，共" << numOfSegment << "个顺序段" << "用时: " << double(end1 - start1) / CLOCKS_PER_SEC << "秒" << endl;

    start2 = clock();
    int *ls = new int[numOfSegment];
    Segment *segments = new Segment[numOfSegment];
    mergeSort(segments, ls, numOfSegment, outputFile);
    end2 = clock();
    cout << "归并顺序段完成，" << "用时: " << double(end2 - start2) / CLOCKS_PER_SEC << "秒" << endl;
    cout << "总用时: " <<  double(end2 - start1) / CLOCKS_PER_SEC << "秒" << endl;

    // 删除顺序段
    cout << "正在删除临时文件..." << endl;
    for (int i = 1; i <= numOfSegment; i++)
    {
        string cmd = "rm " + to_string(i) + ".txt";
        // 将 string 转为 system 函数要求的 char*
        system(cmd.data());
    }
    cout << "临时文件完成删除完成，退出程序" << endl;
    return 0;
}
