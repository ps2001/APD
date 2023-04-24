#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0

int main (int argc, char *argv[])
{
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value = rank;
    MPI_Status status;

    for (int step = 2; step <= procs; step *= 2) {
        if (rank % step == 0) {
            int nr;
            int nr_rank = rank + (step / 2);
            MPI_Recv(&nr, 1, MPI_INT, nr_rank, 0, MPI_COMM_WORLD, &status);
            value += nr;
        } else if (rank % (step / 2) == 0) {
            //int MPI_Send(void* data, int count, MPI_Datatype datatype, int destination, int tag, MPI_Comm communicator)
            int nr_rank = rank - (step / 2);
            MPI_Send(&value, 1, MPI_INT, nr_rank, 0, MPI_COMM_WORLD);
        }
    }

    if (rank == MASTER) {
        printf("Result = %d\n", value);
    }

    MPI_Finalize();

}

