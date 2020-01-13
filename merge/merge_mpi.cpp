#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include <assert.h>

using namespace std;

const int SEPERATE = -1000;

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
    int *A = NULL;
    int *B = NULL;
    srand(time(NULL));
    int n = 10, i;
    int *C = NULL;
    // C = (int *)malloc(sizeof(int) * n * 2);

    int *B_indexs = NULL;
    int *local_data = NULL;
    local_data = (int *)malloc(sizeof(int) * n);
    int *data = NULL;
    int *send_counts = NULL;
    int *displs = NULL;
    int *group_size = NULL;

    // MPI

    MPI_Init(NULL, NULL);
    const int ROOT_RANK = 0;
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_rank == ROOT_RANK)
    {
        A = create_rand_nums(n);
        B = create_rand_nums(n);
        printArray("A", A, n);
        printArray("B", B, n);
        int size_block = n % world_size == 0 ? n / world_size : n / world_size + 1;
        cout << "root size block: " << size_block << endl;
        cout << "root world size: " << world_size << endl;
        data = (int *)malloc(sizeof(int) * 2 * n + world_size);
        send_counts = (int *)malloc(sizeof(int) * world_size);
        displs = (int *)malloc(sizeof(int) * world_size);

        B_indexs = (int *)malloc(sizeof(int) * size_block);

        int i_data = 0,offset = 0;
        for (i = 0; i < world_size; i++)
        {
            send_counts[i] = offset;
            displs[i] = offset;
            int end_block = (i + 1) * size_block - 1;
            B_indexs[i] = binarySearch(B, 0, n - 1, A[end_block]);
            for (int j = i * size_block; j < (i + 1) * size_block; j++)
            {
                data[i_data++] = A[j];
                offset++;
            }
            data[i_data++] = SEPERATE;
            offset ++;
            int i_prev = (i == 0) ? 0 : B_indexs[i - 1] + 1;
            for (int j = i_prev; j <= B_indexs[i]; j++)
            {
                data[i_data++] = B[j];
                offset ++;
            }
            group_size[i] = (i==0) ? offset : offset - group_size[i-1];
            cout << "offset : " << offset << endl;
            // send_counts[i] = (i==0) ? offset : send_counts[i-1] + offset;
            // displs[i] = send_counts[i]; //(i==0) ? offset : displs[i-1] + offset;
            // data[i_data++] = 12345678;
        }
        cout << "data: " << endl;
        for (int i = 0; i < i_data; i++)
        {
            cout << data[i] << endl;
        }
        cout << "end data!" << endl;
    }

    MPI_Scatterv(data, send_counts, displs, MPI_INT,
            local_data, n, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    
    if(world_rank != ROOT_RANK){
        cout << "local data 0: " << local_data[0] << endl;
        cout << "local data 1: " << local_data[1] << endl;
        cout << "local data 2: " << local_data[2] << endl;

    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}