/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <queue>
#include <limits>
#include <bits/stdc++.h>

using namespace std;

void min_array(int *dist1, int *dist2, int n)
{
    for (int i = 0; i < n; i++)
    {
        dist1[i] = min(dist1[i], dist2[i]);
    }
}

int main(int argc, char **argv)
{

    // int A[10][3] = {
    //     {0, 1, 1},
    //     {0, 2, 1},
    //     {0, 3, 3},
    //     {1, 3, 1},
    //     {2, 3, 1},

    //     {1, 0, 1},
    //     {2, 0, 1},
    //     {3, 0, 3},
    //     {3, 1, 1},
    //     {3, 2, 1}};
    int A[][3] = {{0, 1, -1}, {0, 2, 4}, {1, 2, 3}, {1, 3, 2}, {1, 4, 2}, {3, 2, 5}, {3, 1, 1}, {4, 3, -3}};
    MPI_Status status;
    int rank, numprocs;
    int vertices;
    int edges;
    int *dist_array_copy;
    int block_size;
    int src_vertex;
    int other_block_size;
    // int minVertex;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    double tbeg = MPI_Wtime();

    if (rank == 0)
    {
        vertices = 5;
        edges = 8;
        block_size = other_block_size = edges / numprocs;
        src_vertex = 0;
    }

    MPI_Bcast(&vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&edges, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&block_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        block_size = edges / numprocs + edges % numprocs;
    }
    int dist[block_size];

    if (rank == 0)
    {
        int procId;
        for (procId = 1; procId < numprocs; procId++)
        {

            MPI_Send(&A[block_size + other_block_size * (procId - 1)][0], other_block_size * 3, MPI_INT, procId, 1, MPI_COMM_WORLD);
        }
    }
    else
    {

        MPI_Recv(&A[0][0], block_size * 3, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        // MPI_Recv(&starting_vertex, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    }
    cout << "======================\n";
    for (int i = 0; i < block_size; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    cout << "======================\n";

    // Got the pieces and now source vertex also
    MPI_Bcast(&src_vertex, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Initialize
    for (int i = 0; i < vertices; i++)
    {
        dist[i] = INT_MAX;
    }

    dist[src_vertex] = 0;

    // cout << "Rank : " << rank << " dist : ";
    //     for (int i = 0; i < vertices; i++)
    //     {
    //         cout << dist[i] << " ";
    //     }
    //     cout << endl;

    if (rank == 0)
    {
        dist_array_copy = (int *)malloc(vertices * sizeof(int));
    }

    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < block_size; j++)
        {
            if (dist[A[j][0]] != INT_MAX && dist[A[j][0]] + A[j][2] <
                                                dist[A[j][1]])
            {
                dist[A[j][1]] =
                    dist[A[j][0]] + A[j][2];
            }
        }
        if (rank != 0)
        {
            MPI_Send(&dist[0], vertices, MPI_INT, 0, 3, MPI_COMM_WORLD);
        }
        else
        {

            for (int i = 1; i < numprocs; i++)
            {
                MPI_Recv(&dist_array_copy[0], vertices, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
                // for (int i = 0; i < vertices; i++)
                // {
                //     cout << dist_array_copy[i] << " ";
                // }
                // cout << endl;

                min_array(dist, dist_array_copy, vertices);
            }
        }
        MPI_Bcast(&dist[0], vertices, MPI_INT, 0, MPI_COMM_WORLD);
        // break;
    }

    if (rank == 0)
    {
        for (int i = 0; i < vertices; i++)
        {
            cout << dist[i] << " ";
        }
        cout << endl;
        // break;
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