#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <chrono>

// #include "mpi.h"
#include <assert.h>

using namespace std;
using namespace std::chrono;

int *create_rand_nums(int num_elements)
{
    int *rand_nums = (int *)malloc(sizeof(int) * num_elements);
    assert(rand_nums != NULL);
    int i, prev_num = 0;
    for (i = 0; i < num_elements; i++)
    {
        rand_nums[i] = rand() % 5 + prev_num;
        rand_nums[i] = rand_nums[i] == prev_num ? prev_num + 1 : rand_nums[i];
        prev_num = rand_nums[i];
    }
    return rand_nums;
}

int *merge(int *A, int *B, int n)
{
    int *C = (int *)malloc(sizeof(int) * 2 * n);
    assert(C != NULL);
    int i = 1, j = 1, k = 1;
    while (k <= 2 * n)
    {
        if (A[i] < B[j])
        {
            C[k] = A[i];
            i = i + 1;
        }
        else
        {
            C[k] = B[j];
            j = j + 1;
        };
        k = k + 1;
    }
    return C;
}

void printArray(string s, int *arr, int size)
{
    cout << s << " : ";
    int i;
    for (i = 0; i < size; i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int binarySearch(int arr[], int l, int r, int x)
{
    if (r >= l)
    {
        int mid = l + (r - l) / 2;
        if ((arr[mid] < x && arr[mid + 1] > x) || (arr[mid] == x))
            return mid;
        if (arr[mid] > x)
            return binarySearch(arr, l, mid - 1, x);
        return binarySearch(arr, mid + 1, r, x);
    }
    return -1;
}

int main()
{
    auto start = high_resolution_clock::now();
    int *A = NULL;
    int *B = NULL;
    srand(time(NULL));
    int n = 20000;
    int *C = NULL;
    // C = (int *)malloc(sizeof(int) * n * 2);
    A = create_rand_nums(n);
    B = create_rand_nums(n);
    C = merge(A, B, n);
    printArray("A",A,n);
    printArray("B",B,n);
    printArray("C",C,n);

    // int i;
    // printArray("A", A, n);
    // printArray("B", B, n);

    // int world_size = 4;
    // int size_block = n % world_size == 0 ? n / world_size : n / world_size + 1;
    // int *B_indexs = NULL;
    // cout << "size block: " << size_block << endl;
    // B_indexs = (int *)malloc(sizeof(int) * size_block);
    // for (i = 0; i < size_block; i++)
    // {
    //     int end_block = (i + 1) * size_block - 1;
    //     B_indexs[i] = binarySearch(B, 0, n - 1, A[end_block]);
    // }
    // cout << size_block << endl;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Execution time: " << duration.count() << endl;
    return 0;
}