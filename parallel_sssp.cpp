/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <queue>

using namespace std;

int minNode(int nporcs, int * allVertices)
{
    int vertex = INT_MAX; int dist = INT_MAX;
    for(int i=0; i < nporcs; i++)
    {
        if(allVertices[i] < dist)
        {
            dist = allVertices[i];
            vertex = i;
        }
    }
    return vertex;
}

int main(int argc, char **argv)
{

    int A[4][4] = {
        {0, 1, 1, 3},
        {1, 0, 1, 1},
        {1, 1, 0, 0},
        {3, 1, 0, 0}};
    MPI_Status status;
    // 7 -1 4 9 -8 0 6 1
    int rank, numprocs;
    int n;
    int m;
    int col_size;
    int src_vertex;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    double tbeg = MPI_Wtime();

    if (rank == 0)
    {
        n = 4;
        m = 5;
        col_size = n / numprocs;
    }
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&col_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int dist[n];
    bool visited[n];

    // cout << "Col_Size " << col_size << " ";
    if (rank == 0)
    {

        // cout << col_size << " ";
        int procId;
        for (procId = 1; procId < numprocs; procId++)
        {
            cout << rank << " " << col_size << " "
                 << "<-\n ";
            MPI_Send(&A[col_size * procId][0], col_size * n, MPI_INT, procId, 1, MPI_COMM_WORLD);
        }
    }
    else
    {
        cout << rank << " " << col_size << " "
             << "<-\n ";

        MPI_Recv(&A[0][0], col_size * n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    }
    cout << "======================\n";
    for (int i = 0; i < col_size; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    cout << "======================\n";

    // Got the pieces and now source vertex also
    MPI_Bcast(&src_vertex, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++)
    {
        dist[i] = INT_MAX;
        visited[i] = false;
    }

    dist[src] = 0;

    for (int i = 0; i < n - 1; i++)
    {
        // n iterations lo, SSP has to be done
        int minVertex = minDistance(dist, visited);
    }

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