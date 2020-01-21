/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <queue>

using namespace std;

struct heapItem
{
    int value;
    int pid;

    bool operator<(heapItem const &obj)
    {
        heapItem res;
        if (value <= obj.value)
        {
            return true;
        }
        return false;
    }
};

int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int main(int argc, char **argv)
{

    // 7 -1 4 9 -8 0 6 1
    int n;
    int A[100];
    MPI_Status status;
    int rank, numprocs;
    int array_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (rank == 0)
    {
        n = 8;
        A[0] = 7;
        A[1] = -1;
        A[2] = 4;
        A[3] = 9;
        A[4] = -8;
        A[5] = 0;
        A[6] = 6;
        A[7] = 1;
        /////
        array_size = n / numprocs;
        int procId = 0;
        for (procId = 1; procId < numprocs; procId++)
        {
            MPI_Send(&array_size, 1, MPI_INT, procId, 0, MPI_COMM_WORLD);
            MPI_Send(&A[procId * array_size], array_size, MPI_INT, procId, 0, MPI_COMM_WORLD);
        }
    }
    // /*synchronize all processes*/
    MPI_Barrier(MPI_COMM_WORLD);
    double tbeg = MPI_Wtime();
    /* write your code here */
    if (rank != 0)
    {
        MPI_Recv(&array_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&A, array_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    // All do QS
    qsort(A, array_size, sizeof(int), cmpfunc);

    if (rank != 0)
    {
        MPI_Send(&A, array_size, MPI_INT, rank, 2, MPI_COMM_WORLD);
        cout << "Sent";
    }
    
    // MPI_Barrier(MPI_COMM_WORLD);
    // if(rank == 0)
    // {
    //     int B[10];
    //     for (int procIde = 1; procIde < numprocs; procIde++)
    //     {
    //         cout << "ProcId:: " << procIde << endl;
    //         cout << "PID :: " << procIde << " ASZ:: " << array_size <<  endl;
    //         cout << "Blahhh1" << endl;
    //         // MPI_Recv( &partial_sum, 1, MPI_LONG, MPI_ANY_SOURCE,return_data_tag, MPI_COMM_WORLD, &status);
    //         // MPI_Recv(&A[procIde*array_size], array_size, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
    //         MPI_Recv(B, array_size, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
    //         cout << "Blahhh2" << endl;
    //         {
    //         for(int i=0;i<array_size;i++)
    //             A[procIde*array_size + i] = B[i];
    //         }
    //         // cout << "ProcId:: " << procId << endl;
    //     }
    //     cout << "Blahhh" << endl;
    // }

    // if (rank == 0)
    // {
    //     vector<vector<int>> sortedArray(numprocs);
    //     for(int i=0; i < numprocs ; i++)
    //     {
    //         sortedArray[i] = vector<int>(array_size); 
    //         for(int j=0; j < array_size; j++)
    //         {
    //             sortedArray[i][j] = numprocs*array_size + j;
    //         }
    //     }
    //     //
    //     for(int i=0; i < numprocs ; i++)
    //     {
    //         for(int j=0; j < array_size; j++)
    //         {
    //             cout << sortedArray[i][j] << " ";                
    //         }
    //         cout << endl;
    //     }
    // }

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce(&elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Total time (s): %f\n", maxTime);
    }

    /* shut down MPI */
    MPI_Finalize();
    return 0;
}