#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include <assert.h>

using namespace std;

const int SEPERATE = -1000;

vector<int> create_rand_nums(int num_elements)
{
    vector<int> rand_nums;// = (vector<int> )malloc(sizeof(int) * num_elements);
    // assert(rand_nums != NULL);
    int i, prev_num = 0;
    for (i = 0; i < num_elements; i++)
    {
        int r = rand() % 5 + prev_num;
        r = (r == prev_num) ? prev_num + 1 : r;
        prev_num = r;
        rand_nums.push_back(r);
    }
    return rand_nums;
}

vector<int> merge(vector<int> &A, vector<int> &B, int n)
{
    // vector<int> C = (vector<int> )malloc(sizeof(int) * n);
    vector<int> C;
    int i = 0, j = 0, k = 0;
    while (k < n)
    {
        if (A[i] < B[j])
        {
            C.push_back(A[i]);
            i = i + 1;
        }
        else
        {
            C.push_back(B[j]);
            j = j + 1;
        };
        k = k + 1;
    }
    return C;
}

void printArray(string s, vector<int> &arr)
{
    cout << s << " : ";
    int i;
    for (i = 0; i < arr.size(); i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int binarySearch(vector<int> &arr, int l, int r, int x)
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
    vector<int> A = NULL;
    vector<int> B = NULL;
    srand(time(NULL));
    int n = 10, i;
    // vector<int> C = NULL;
    // C = (vector<int> )malloc(sizeof(int) * n * 2);

    // vector<int> B_indexs = NULL;
    // vector<int> local_data = NULL;
    // vector<int> data = NULL;
    // vector<int> send_info = NULL;
    // vector<int> recv_info = (vector<int> )malloc(sizeof(int) * 2);;
    // vector<int> send_counts = NULL;
    // vector<int> displs = NULL;
    // vector<int> group_size = NULL;

    vector<int> A;
    vector<int> B;
    vector<int> C;
    srand(time(NULL));
    int n = 10, i;

    vector<int> B_indexs = NULL;
    vector<int> local_data = NULL;
    vector<int> data = NULL;
    vector<int> send_info = NULL;
    vector<int> recv_info = NULL;
    vector<int> send_counts = NULL;
    vector<int> displs = NULL;
    vector<int> group_size = NULL;

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
        A[n-1] = B[n-1] >= A[n-1] ? B[n-1] + rand() % 10 : A[n-1];
        // C = merge(A,B,2*n);
        printArray("A", A);
        printArray("B", B);
        // printArray("C", C, 2*n);
        // return 0;
        int size_block = n % world_size == 0 ? n / world_size : n / world_size + 1;
        cout << "root size block: " << size_block << endl;
        cout << "root world size: " << world_size << endl;
        // data = (vector<int> )malloc(sizeof(int) * 2 * n + world_size);
        // send_counts = (vector<int> )malloc(sizeof(int) * world_size);
        // displs = (vector<int> )malloc(sizeof(int) * world_size);
        // group_size = (vector<int> )malloc(sizeof(int) * world_size);
        // send_info = (vector<int> )malloc(sizeof(int) * world_size * 2);
        int ids = 0; 

        // B_indexs = (vector<int> )malloc(sizeof(int) * size_block);

        int i_data = 0,offset = 0;
        for (i = 0; i < world_size; i++)
        {
            send_counts.push_back(offset); // send_counts[i] = offset;
            displs.push_back(offset); //displs[i] = offset;
            int end_block = (i + 1) * size_block - 1;
            B_indexs.push_back(binarySearch(B, 0, n - 1, A[end_block]));
            for (int j = i * size_block; j < (i + 1) * size_block; j++)
            {
                data.push_back(A[j]); //data[i_data++] = A[j];
                offset++;
            }
            data.push_back(SEPERATE); //data[i_data++] = SEPERATE;
            send_info.push_back((i==0) ? offset : offset - group_size[i-1]); //send_info[ids++] = (i==0) ? offset : offset - group_size[i-1];
            offset ++;
            int i_prev = (i == 0) ? 0 : B_indexs[i - 1] + 1;
            for (int j = i_prev; j <= B_indexs[i]; j++)
            {
                data.push_back(B[j]);// data[i_data++] = B[j];
                offset ++;
            }
            group_size.push_back((i==0) ? offset : offset - group_size[i-1]); //group_size[i] = (i==0) ? offset : offset - group_size[i-1];

            send_info.push_back(group_size[i]);
            // send_info[ids++] = group_size[i];
            cout << "offset : " << offset << endl;
            // send_counts[i] = (i==0) ? offset : send_counts[i-1] + offset;
            // displs[i] = send_counts[i]; //(i==0) ? offset : displs[i-1] + offset;
            // data[i_data++] = 12345678;
        }
        printArray("send couts",send_counts);
        printArray("displs",displs);
        // cout << "send_info: " << endl;
        // for (int i = 0; i < i_data; i++)
        // {
        //     cout << data[i] << endl;
        // }
        // cout << "end send_info!" << endl;
    }

    // local_data = (vector<int> )malloc(sizeof(int) * 2 * n);

    MPI_Scatter(send_info, 2, MPI_FLOAT, recv_info,
            2, MPI_FLOAT, 0, MPI_COMM_WORLD);
    // cout << "rect info " << world_rank << " : " << recv_info[1] << endl;


    MPI_Scatterv(data, group_size, displs, MPI_INT,
            local_data, n, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

    vector<int> A_local = (vector<int> )malloc(sizeof(int) * recv_info[0]);
    vector<int> B_local = (vector<int> )malloc(sizeof(int) * (recv_info[1] -recv_info[0]-1));
    for(i=0;i<recv_info[1];i++){
        if(i==recv_info[0])
            continue;
        if(i<recv_info[0]){
            A_local[i] = local_data[i];
        } else {
            B_local[i-recv_info[0]-1] = local_data[i];
        }
    }
    vector<int> C_local = NULL;
    C_local = merge(A_local,B_local,recv_info[1]-1);
    printArray("local data",local_data,recv_info[1]);
    printArray("A local",A_local,recv_info[0]);
    printArray("B local",B_local,recv_info[1] - recv_info[0] - 1);
    printArray("C local",C_local,recv_info[1]-1);

    cout << "Rank " << world_rank << " : " << recv_info[1] << endl; 
    

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}