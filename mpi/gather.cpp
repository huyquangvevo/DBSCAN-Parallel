#include<mpi.h>
#include<iostream>
#include <assert.h>
#include<vector>

using namespace std; 

int main(int argc,char** argv){

    MPI_Init(NULL,NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    // int r[3] = {10,20,30};
    vector<int> r;
        r.push_back(5);
        r.push_back(10);
        r.push_back(15);
    // int *master = NULL;

    // if(world_rank == 0){
    //     master = (int *)malloc(sizeof(int));
    //     assert(master != NULL);
    // };
    cout << "world rank: " << world_rank << endl;
    cout << "world size: " << world_size << endl;
    int *master = NULL;
    if(world_rank == 0){
        master = (int *)malloc(sizeof(int));
        assert(master != NULL);
    }
    

    MPI_Gather(
        r.data(),
        1,
        MPI_INT,
        master,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    if(world_rank==0){
        cout << "master : " << master[0] << endl;
    } else
    {
        cout << "slave : " << r[0] << endl;
    }

    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}