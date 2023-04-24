#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int Recv_nr;

    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number.
        // Send the number to the next process.
        srand(time(NULL));
        int random_nr = rand() % 10;

        //int MPI_Send(void* data, int count, MPI_Datatype datatype, int destination, int tag, MPI_Comm communicator);
        MPI_Send(&random_nr, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        MPI_Status status;
        MPI_Recv(&Recv_nr, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process with rank %d received number %d\n", rank, Recv_nr);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the first process.

        MPI_Status status;
        MPI_Recv(&Recv_nr, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process with rank %d received number %d\n", rank, Recv_nr);

        Recv_nr += 2;
        MPI_Send(&Recv_nr, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    } else {
        // Middle process.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the next process.

        int nr;
        MPI_Status status;
        MPI_Recv(&Recv_nr, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process with rank %d received number %d\n", rank, Recv_nr);

        Recv_nr += 2;
        MPI_Send(&Recv_nr, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}