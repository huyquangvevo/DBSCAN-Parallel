#include <iostream>
#include <stdio.h>

#include <vector>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <bits/stdc++.h>
// #include <assert.h>
#include <chrono>

double t_comunication = 0;


using namespace std;
using namespace std::chrono;

const int SEPERATE = -1000;

int *create_rand_nums(int num_elements)
{
    int *rand_nums = (int *)malloc(sizeof(int) * num_elements);
    // assert(rand_nums != NULL);
    int i, prev_num = 0;
    for (i = 0; i < num_elements; i++)
    {
        rand_nums[i] = rand() % 5 + prev_num;
        rand_nums[i] = rand_nums[i] == prev_num ? prev_num + 1 : rand_nums[i];
        prev_num = rand_nums[i];
    }
    return rand_nums;
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

int *merge(int *A_, int *B_, int size_A, int size_B)
{
    int *C_ = (int *)malloc(sizeof(int) * (size_A + size_B));
    // assert(C_ != NULL);
    int i = 0, j = 0, k = 0;
    while (k < size_A + size_B)
    {
        // if (i == size_A)
        // {
        //     C_[k++] = B_[j++];
        //     continue;
        // }
        if(size_B == 0){
            C_[k++] = A_[i++];
            continue;
        }
        if (j == size_B)
        {
            C_[k++] = A_[i++];
            continue;
        }
        if (A_[i] < B_[j])
        {
            C_[k++] = A_[i++];
        }
        else
        {
            C_[k++] = B_[j++];
        };
    }
    return C_;
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

int main(int argc,char** argv)
{
    auto start = high_resolution_clock::now();
    
    const int ROOT_RANK = 0;
    int n = atoi(argv[1]);
    int isShow = atoi(argv[2]);

    double start_com,end;


    MPI_Init(NULL, NULL);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    int *A = NULL;
    int *B = NULL;
    srand(time(NULL));
    int i;
    
    int *C = NULL;

    int *B_indexs = NULL;
    int *local_data = NULL;
    int *data = NULL;
    int *send_info = NULL;
    int *recv_info = NULL;
    recv_info = (int *)malloc(sizeof(int) * 2);
    int *displs = NULL;
    int *group_size = NULL;

    int *A_local = NULL;
    int *B_local = NULL;
    int *C_local = NULL;


    // MPI

   
    if (world_rank == ROOT_RANK)
    {
        A = create_rand_nums(n);
        B = create_rand_nums(n);
        A[n - 1] = B[n - 1] >= A[n - 1] ? B[n - 1] + rand() % 10 : A[n - 1];
        if(isShow == 1){
            printArray("A", A, n);
            printArray("B", B, n);
        }
        int size_block = (n % world_size == 0 ) ? n / world_size : n / world_size + 1;
        // data chua cai gi? : 2 mang, them cac phan tu ngan cach
        data = (int *)malloc(sizeof(int) * 2 * n + world_size);
        // khong dung
        // 4
        displs = (int *)malloc(sizeof(int) * world_size);
        // 4
        group_size = (int *)malloc(sizeof(int) * world_size);
        // 8
        send_info = (int *)malloc(sizeof(int) * world_size * 2);
        int ids = 0;
        // 4
        B_indexs = (int *)malloc(sizeof(int) * world_size);

        int i_data = 0, offset = 0;
        for (i = 0; i < world_size; i++)
        {
            displs[i] = offset;
            // end_block: chi so 
            int end_block = (i == world_size - 1) ? n - 1 : (i + 1) * size_block - 1;
            B_indexs[i] = (i == world_size - 1) ? n - 1 : binarySearch(B, 0, n - 1, A[end_block]);
            for (int j = i * size_block; j <= end_block; j++)
            {
                data[i_data++] = A[j];
                offset++;
            }
            data[i_data++] = SEPERATE;
            send_info[ids++] = (i == 0) ? offset : offset - displs[i]; //group_size[i - 1];
            offset++;
            int i_prev = (i == 0) ? 0 : B_indexs[i - 1] + 1;
            for (int j = i_prev; j <= B_indexs[i]; j++)
            {
                data[i_data++] = B[j];
                offset++;
            }
            group_size[i] = (i == 0) ? offset : offset - displs[i]; //group_size[i - 1];
            send_info[ids++] = group_size[i];
        }

        C = (int *)malloc(sizeof(int) * n * 2);
    }



    start_com = MPI_Wtime();
    MPI_Scatter(send_info, 2, MPI_INT, recv_info,
                2, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    end = MPI_Wtime();
    t_comunication += end - start_com;


    local_data = (int *)malloc(sizeof(int) * recv_info[1]);

    start_com = MPI_Wtime();
    MPI_Scatterv(data, group_size, displs, MPI_INT,
                 local_data, recv_info[1], MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    end = MPI_Wtime();
    t_comunication += end - start_com;


    A_local = (int *)malloc(sizeof(int) * recv_info[0]);
    if((recv_info[1] - recv_info[0] - 1)!=0)
    B_local = (int *)malloc(sizeof(int) * (recv_info[1] - recv_info[0] - 1));

    
    for (i = 0; i < recv_info[1]; i++)
    {
        if (i == recv_info[0])
            continue;
        if (i < recv_info[0])
        {
            A_local[i] = local_data[i];
        }
        else
        {
            B_local[i - recv_info[0] - 1] = local_data[i];
        }
    }
    
    C_local = merge(A_local, B_local, recv_info[0], recv_info[1] - recv_info[0] - 1);


    if(world_rank == ROOT_RANK){
         for (i = 0; i < world_size; i++)
        {
            displs[i] = (i == 0) ? displs[i]: displs[i] - i;
            group_size[i] = group_size[i] - 1;
        }
    }

    start_com = MPI_Wtime();
    MPI_Gatherv(
        C_local,
        recv_info[1] - 1,
        MPI_INT,
        C,
        group_size,
        displs, MPI_INT, ROOT_RANK,
        MPI_COMM_WORLD);

    end = MPI_Wtime();
    t_comunication += end - start_com;

    if (world_rank == ROOT_RANK)
    {
        if(isShow == 1)
            printArray("C", C, 2 * n);
        free(A);
        free(B);
        free(B_indexs);
        free(data);
        free(send_info);
        free(displs);
        free(group_size);
        free(C);

    }

    free(recv_info);
    free(local_data);
    free(A_local);
    if(B_local != NULL)
        free(B_local);
    free(C_local);


    auto stop = high_resolution_clock::now();
    if (world_rank == ROOT_RANK)
    {
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Total time: " << duration.count() << endl;
        cout << "Comunication time: " << t_comunication * 1000000 << endl;
        cout << "Execution time: " << duration.count() - t_comunication * 1000000 << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}