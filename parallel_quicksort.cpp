/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <queue>

using namespace std;

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
    int value;
    int pid;
    heapItem(int value, int pid) : value(value), pid(pid) {}
};

int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// struct compareHeap
// { // defining the comparemarks structure
//     bool operator()(heapItem const &p1, heapItem const &p2)
//     {
//         return p1.value < p2.value;
//     }
// };
bool operator>(const heapItem &lhs, const heapItem &rhs)
{
    return lhs.value > rhs.value;
}

int main(int argc, char **argv)
{

    // 7 -1 4 9 -8 0 6 1
    int n;
    int A[100];
    MPI_Status status;
    int rank, numprocs;
    int array_size, other_array_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (rank == 0)
    {
        n = 9;
        A[0] = 7;
        A[1] = -1;
        A[2] = 4;
        A[3] = 9;
        A[4] = -8;
        A[5] = 0;
        A[6] = 6;
        A[7] = 1;
        A[8] = 11;
        /////
        /// 9 / 4 -> 9%4 = 1
        int procId = 0;
        array_size = n / numprocs + n % numprocs;
        other_array_size = n / numprocs;
        for (procId = 1; procId < numprocs; procId++)
        {
            MPI_Send(&other_array_size, 1, MPI_INT, procId, 0, MPI_COMM_WORLD);
            MPI_Send(&A[array_size + (procId-1) * other_array_size], other_array_size, MPI_INT, procId, 0, MPI_COMM_WORLD);
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
        MPI_Send(&A, array_size, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
    {
        int B[100];
        vector<vector<int>> sortedArray(numprocs);
        vector<int> tmp;
        for (int i = 0; i < array_size; i++)
        {
            tmp.push_back(A[i]);
        }
        sortedArray[0] = tmp;

        for (int procIde = 1; procIde < numprocs; procIde++)
        {
            MPI_Recv(&B, other_array_size, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
            int source_rank = status.MPI_SOURCE;
            vector<int> tmp;
            for (int i = 0; i < other_array_size; i++)
            {
                tmp.push_back(B[i]);
            }
            sortedArray[source_rank] = tmp;
        }

        // Print Matrix
        // for (auto vec : sortedArray)
        // {
        //     for (auto x : vec)
        //         std::cout << x << " , ";
        //     std::cout << std::endl;
        // }

        priority_queue<heapItem, vector<heapItem>, greater<heapItem>> heap;
        for (int i = 0; i < numprocs; i++)
        {
            heap.push(heapItem(sortedArray[i][0], i));
            pop_front(sortedArray[i]);
            // if (sortedArray[i].size() != 0)
            // {                                                 //check if there any elements in the vector array
            //     sortedArray[i].erase(sortedArray[i].begin()); //erase the firs element
            // }
        }

        // cout << "==================================" << endl;
        // for (auto vec : sortedArray)
        // {
        //     for (auto x : vec)
        //         std::cout << x << " , ";
        //     std::cout << std::endl;
        // }
        // cout << "==================================" << endl;

        int val;
        while (!heap.empty())
        {
            heapItem temp = heap.top();
            cout << temp.value << " ";
            heap.pop();

            if (sortedArray[temp.pid].size() != 0)
            {
                val = sortedArray[temp.pid][0];
                pop_front(sortedArray[temp.pid]);
                // cout << "Val :: " << val << " " << temp.pid << endl;
                // cout << "==================================" << endl;
                // for (auto vec : sortedArray)
                // {
                //     for (auto x : vec)
                //         std::cout << x << " , ";
                //     std::cout << std::endl;
                // }
                // cout << "==================================" << endl;

                heap.push(heapItem(val, temp.pid));
            }
        }
        cout << endl;
        // cout << "==================================" << endl;
        // for (auto vec : sortedArray)
        // {
        //     for (auto x : vec)
        //         std::cout << x << " , ";
        //     std::cout << std::endl;
        // }
        // cout << "==================================" << endl;
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