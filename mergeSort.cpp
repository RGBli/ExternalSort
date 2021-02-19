#include <iostream>

using namespace std;

/* 把 arr 数组的 [low, mid] 看做一个有序区，[mid + 1, high] 看作一个有序区
 * 功能是将这两个有序区合并成一个有序区*/
void merge(int arr[], int low, int mid, int high)
{
    // 创建临时动态数组，容量为 high - low + 1
    int *tmp = new int[high - low + 1];
    // i 表示第一段 [low, mid] 的下标
    int i = low;
    // j 表示第二段 [mid + 1, high] 的下标
    int j = mid + 1;
    // k 表示 tmp 数组中的下标
    int k = 0;
    // 把较小的数先移到新数组 tmp 中
    while (i <= mid && j <= high)
    {
        // 归并排序是稳定排序，这里一定是小于等于
        // 因为 i 在 j 的前面，稳定排序后也应该在 j 的前面
        if (arr[i] <= arr[j]) {
            tmp[k++] = arr[i++];
        } else {
            tmp[k++] = arr[j++];
        }
    }
    // 把左边剩余的数移入数组
    while (i <= mid)
    {
        tmp[k++] = arr[i++];
    }
    // 把右边边剩余的数移入数组
    while (j <= high)
    {
        tmp[k++] = arr[j++];
    }
    // 把新数组中的数覆盖 arr 数组
    for (int x = 0; x < high - low + 1; x++)
    {
        arr[low + x] = tmp[x];
    }
}

/* 递归排序入口
 * 使用了递归*/
void mergeSort(int arr[], int low, int high)
{
    // 递归出口，直到 low 与 high 相等，即只有一个元素时结束递归
    if (low < high)
    {
        int mid = (low + high) / 2;
        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);
        merge(arr, low, mid, high);
    }
}

void sort(int arr[], int length)
{
    mergeSort(arr, 0, length - 1);
}

int main()
{
    int arr[] = {1, 3, 2, 5, 4, 10, 9, 6, 8, 7};
    sort(arr, 10);
    for (int i : arr)
    {
        cout << i;
    }
    return 0;
}