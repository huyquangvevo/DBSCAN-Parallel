#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "mpi.h"

#define nstrings 5
const char *const strings[nstrings] = {"Hello","world!","Bonjour","le","monde!"};

using namespace std;

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv); 
    const int ROOT_RANK = 0;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int A[6] = {0,1,2,3,4,5};
    int sendcounts[3] = {0,1,5};
    int displs[3] = {0,1,5};
    int *local_data = NULL;
    local_data = (int *)malloc(sizeof(int) * 10);
    MPI_Scatterv(&A, sendcounts, displs, MPI_INT,
            local_data, 5, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    
    if(rank != ROOT_RANK){
        cout << "rank : " << rank << endl;
        cout << local_data[0] << endl;        
    }

    MPI_Finalize();
    return 0;
}