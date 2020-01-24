/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <queue>
#include <limits>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

void min_array(long long int *dist1, long long int *dist2, int n)
{
    for (int i = 0; i < n; i++)
    {
        dist1[i] = min(dist1[i], dist2[i]);
    }
}

long long int A[10000][3];
int main(int argc, char **argv)
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    double tbeg = MPI_Wtime();
    ifstream inFile;

    MPI_Status status;
    int vertices;
    int edges;
    long long int *dist_array_copy;
    int block_size;
    int src_vertex;
    int other_block_size;

    if (rank == 0)
    {
        inFile.open(argv[1]);
        if (!inFile)
        {
            cerr << "Unable to open input file";
            exit(1); // call system to stop
        }
        int index = 0;
        inFile >> vertices;
        inFile >> edges;
        for (int loop = 0; loop < edges; loop++)
        {
            inFile >> A[loop][0];
            inFile >> A[loop][1];
            inFile >> A[loop][2];
        }
        inFile >> src_vertex;
        inFile.close();

        block_size = other_block_size = edges / numprocs;
    }
    MPI_Bcast(&vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&edges, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&block_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        block_size = edges / numprocs + edges % numprocs;
    }
    long long int dist[vertices + 1];

    if (rank == 0)
    {
        int procId;
        for (procId = 1; procId < numprocs; procId++)
        {

            MPI_Send(&A[block_size + other_block_size * (procId - 1)][0], other_block_size * 3, MPI_LONG_LONG_INT, procId, 1, MPI_COMM_WORLD);
        }
    }
    else
    {

        MPI_Recv(&A[0][0], block_size * 3, MPI_LONG_LONG_INT, 0, 1, MPI_COMM_WORLD, &status);
        // MPI_Recv(&starting_vertex, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    }

    // Got the pieces and now source vertex also
    MPI_Bcast(&src_vertex, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Initialize
    for (int i = 0; i <= vertices; i++)
    {
        dist[i] = LLONG_MAX;
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
        dist_array_copy = (long long int *)malloc((vertices + 1) * sizeof(long long int));
    }

    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < block_size; j++)
        {
            if (dist[A[j][0]] != LLONG_MAX && dist[A[j][0]] + A[j][2] <
                                                  dist[A[j][1]])
            {
                dist[A[j][1]] =
                    dist[A[j][0]] + A[j][2];
            }
            if (dist[A[j][1]] != LLONG_MAX && dist[A[j][1]] + A[j][2] <
                                                  dist[A[j][0]])
            {
                dist[A[j][0]] =
                    dist[A[j][1]] + A[j][2];
            }
        }
        if (rank != 0)
        {
            MPI_Send(&dist[0], vertices + 1, MPI_LONG_LONG_INT, 0, 3, MPI_COMM_WORLD);
        }
        else
        {

            for (int i = 1; i < numprocs; i++)
            {
                MPI_Recv(&dist_array_copy[0], vertices + 1, MPI_LONG_LONG_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
                min_array(dist, dist_array_copy, vertices + 1);
            }
        }
        MPI_Bcast(&dist[0], vertices + 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
        ofstream outfile;
        outfile.open(argv[2]);
        for (int i = 1; i <= vertices; i++)
        {
            if (dist[i] == LLONG_MAX)
            {
                outfile << i << " " << -1 << endl;
            }
            else
            {
                outfile << i << " " << dist[i] << endl;
            }
        }
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