#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int rand_num;
    printf("Process [%d], before broadcast %d.\n", rank, rand_num);

    // Root process generates a random number.
    // Broadcasts to all processes.

    if (rank == 0) {
        srand(time(NULL));
        rand_num = rand() % 10;
    }

    //int MPI_Bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
    MPI_Bcast(&rand_num, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Process [%d], after broadcast %d.\n", rank, rand_num);

    MPI_Finalize();
    return 0;
}
