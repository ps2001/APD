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

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;

    srand(time(NULL));

    if (rank == 0) {

        // Generate the random numbers.
        // Generate the random tags.
        // Sends the numbers with the tags to the second process.
        for (int i = 0; i < send_numbers; i++) {
            int nr = rand() % 100;
            int tag = rand() % 100;

            //int MPI_Send(void* data, int count, MPI_Datatype datatype, int destination, int tag, MPI_Comm communicator)
            MPI_Send(&nr, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        }
    } else {

        // Receives the information from the first process.
        // Prints the numbers with their corresponding tags.

        //int MPI_Recv(void* data, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm communicator, MPI_Status* status)
        for (int i = 0; i < send_numbers; i++) {
            int recv_nr;
            MPI_Status status;
            MPI_Recv(&recv_nr, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Process [%d] received the number %d with the tag %d\n", rank, recv_nr, status.MPI_TAG);
        }
    }

    MPI_Finalize();

}

