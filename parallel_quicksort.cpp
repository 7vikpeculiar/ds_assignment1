/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <queue>
#include <fstream>

using namespace std;
long long int A[1000000];
template <typename T>
void pop_front(std::vector<T> &v)
{
    if (v.size() > 0)
    {
        v.erase(v.begin());
    }
}

struct heapItem
{
    long long int value;
    int pid;
    heapItem(long long int value, int pid) : value(value), pid(pid) {}
};

int cmpfunc(const void *a, const void *b)
{
    return (*(long long int *)a - *(long long int *)b);
}

bool operator>(const heapItem &lhs, const heapItem &rhs)
{
    return lhs.value > rhs.value;
}

int main(int argc, char **argv)
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    // /*synchronize all processes*/
    MPI_Barrier(MPI_COMM_WORLD);
    double tbeg = MPI_Wtime();
    // 7 -1 4 9 -8 0 6 1
    long long int* B;
    int n;
    MPI_Status status;
    int array_size, other_array_size;

    ifstream inFile;

    if (rank == 0)
    {

        // char f_input_name[100] = ""; //Storing File Path/Name of Image to Display
        // char f_output_name[100] = ""; //Storing File Path/Name of Image to Display

        inFile.open(argv[1]);
        if (!inFile)
        {
            cerr << "Unable to open input file";
            exit(1); // call system to stop
        }
        long long int x;
        int index = 0;
        while (inFile >> x)
        {
            A[index] = x;
            index++;
        }
        n = index;
        inFile.close();

        int procId = 0;
        array_size = n / numprocs + n % numprocs;
        other_array_size = n / numprocs;
        for (procId = 1; procId < numprocs; procId++)
        {
            MPI_Send(&other_array_size, 1, MPI_INT, procId, 0, MPI_COMM_WORLD);
            MPI_Send(&A[array_size + (procId - 1) * other_array_size], other_array_size, MPI_LONG_LONG_INT, procId, 0, MPI_COMM_WORLD);
        }
    }
    /* write your code here */
    if (rank != 0)
    {
        MPI_Recv(&array_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&A, array_size, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    // All do QS
    qsort(A, array_size, sizeof(long long int), cmpfunc);

    if (rank != 0)
    {
        MPI_Send(&A, array_size, MPI_LONG_LONG_INT, 0, 2, MPI_COMM_WORLD);
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
    {
        long long int B[10000];
        vector<vector<long long int>> sortedArray(numprocs);
        vector<long long int> tmp;
        for (int i = 0; i < array_size; i++)
        {
            tmp.push_back(A[i]);
        }
        sortedArray[0] = tmp;

        for (int procIde = 1; procIde < numprocs; procIde++)
        {
            MPI_Recv(&B, other_array_size, MPI_LONG_LONG_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
            int source_rank = status.MPI_SOURCE;
            vector<long long int> tmp;
            for (int i = 0; i < other_array_size; i++)
            {
                tmp.push_back(B[i]);
            }
            sortedArray[source_rank] = tmp;
        }

        priority_queue<heapItem, vector<heapItem>, greater<heapItem>> heap;
        for (int i = 0; i < numprocs; i++)
        {
            heap.push(heapItem(sortedArray[i][0], i));
            pop_front(sortedArray[i]);
        }

        long long int val;

        ofstream outfile;
        outfile.open(argv[2]);

        while (!heap.empty())
        {
            heapItem temp = heap.top();
            outfile << temp.value << " ";
            heap.pop();

            if (sortedArray[temp.pid].size() != 0)
            {
                val = sortedArray[temp.pid][0];
                pop_front(sortedArray[temp.pid]);

                heap.push(heapItem(val, temp.pid));
            }
        }
        outfile.close();
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