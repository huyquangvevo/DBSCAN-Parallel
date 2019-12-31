#include <stdio.h>
#include <mpi.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv){
    // initialize the mpi environment
    MPI_Init(NULL,NULL);

    // get the number of the processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // get the  rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    // get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name,&name_len);

    //print off a hello world message
    cout << "hello world from processor " << processor_name<<", rank " << world_rank << " out of "<< world_size <<" processors\n";
    MPI_Finalize();
}
