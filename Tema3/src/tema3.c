#include<mpi.h>
#include<stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define CONVERGENCE_COEF 100
#define N_COORD 4

static int num_neigh;
static int *neigh;
static int coord;

// functie care citeste din fisier workerii pt fiecare coordonator
void read_neighbors(int rank) {
    // asiguram ca citirea nu se face decat de catre un coordonator
    if (rank > 3)
        return;

    FILE *fp;
    char file_name[15];
    sprintf(file_name, "./cluster%d.txt", rank);

    fp = fopen(file_name, "r");
    fscanf(fp, "%d", &num_neigh);

    // neigh = vectorul de workeri pt coordonatorul curent
    neigh = malloc(sizeof(int) * num_neigh);

    for (size_t i = 0; i < num_neigh; i++)
        fscanf(fp, "%d", &neigh[i]);
}

// fiecare coord isi anunta workerii despre faptul ca el este coord lor
void announce_workers(int rank) {
    if (rank <= 3) {
        for (int i = 0; i < num_neigh; i++) {
            MPI_Send(&rank, 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, neigh[i]);
        }
    } else {
        MPI_Recv(&coord, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void display_topology(int rank, int nProcesses, int **topology) {
    printf("%d -> ", rank);
    for (int i = 0; i < 4; i++) {
        int cnt = 0;
        for (int j = 4; j < nProcesses; j++) {
            if (topology[i][j] == 1) {
                cnt++;
            }
        }

        int cnt2 = 0;
        printf("%d:", i);
        for (int j = 4; j < nProcesses; j++) {
            if (topology[i][j] == 1) {
                printf("%d", j);
                if (cnt2 < cnt - 1) {
                    printf(",");
                }
                cnt2++;
            }
        }
        printf(" ");
    }
    printf("\n");
}

// calculeaza topologie
// topology[i][j] = 0, j nu este worker pt coord i
// tolopogy[i][j] = 1, j este worker pt coord i
int **get_topology(int rank, int nProcesses) {
    int ** topology = malloc(sizeof(int*) * N_COORD);
    int ** vTopology = malloc(sizeof(int*) * nProcesses);
	
	for (int i = 0; i < N_COORD; i++) {
		topology[i] = calloc(sizeof(int), 20);
		vTopology[i] = calloc(sizeof(int), 20);
	}

    // fiecare worker isi completeaza topologia cu workerii proprii
	for (int i = 0; i < num_neigh; i++) {
		topology[rank][neigh[i]] = 1;
	}

    // Are loc o "parcurgere" in sensul invers al acelor de ceasornic: 0->3->2->1
    // Coord 3, 2, 1 primesc de la coord de dinainte topologia, pe care o completeaza cu workerii lui
    // Dupa, trimit topologia coord urmator, care procedeaza la fel
    // La final, coord 1, dupa ce completeaza, trimite inapoi topologia completa:1->2->3->0
    if (rank == 0) {
        //trimite spre rank = 3 topologia
        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        }

        // primeste topologia completa de la 3
        for (int i = 0; i < 4; i++) {
            MPI_Recv(topology[i], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // afiseaza topologia
        display_topology(rank, nProcesses, topology);

    } else if (rank == 3) {
        // primeste topologia de la 0 si completeaza cu workerii proprii
        for (int i = 0; i < 4; i++) {
            MPI_Recv(vTopology[i], nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < nProcesses; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = vTopology[i][j];
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        }

        for (int i = 0; i < 4; i++) {
            MPI_Recv(topology[i], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("M(%d,%d)\n", rank, 0);
        }

        display_topology(rank, nProcesses, topology);

        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
        }

    } else if (rank == 2) {
        for (int i = 0; i < 4; i++) {
            MPI_Recv(vTopology[i], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < nProcesses; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = vTopology[i][j];
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], nProcesses, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
        }

        for (int i = 0; i < 4; i++) {
            MPI_Recv(topology[i], nProcesses, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        display_topology(rank, nProcesses, topology);

        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 3);
        }

    } else if (rank == 1) {
        for (int i = 0; i < 4; i++) {
            MPI_Recv(vTopology[i], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < nProcesses; j++) {
                if (topology[i][j] == 0) {
                    topology[i][j] = vTopology[i][j];
                }
            }
        }

        display_topology(rank, nProcesses, topology);

        for (int i = 0; i < 4; i++) {
            MPI_Send(topology[i], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
        }
    }

    return topology;
}

int min(double a, double b) {
    return a < b ? a : b;
}

void send_params_coord(int rank, int destination, int n, int nProcesses, int *start, int *end, int *v) {
    MPI_Send(v, n, MPI_INT, destination, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, destination);
        
    MPI_Send(start, nProcesses, MPI_INT, destination, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, destination);

    MPI_Send(end, nProcesses, MPI_INT, destination, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, destination);
}

void activate_workers(int rank, int n, int num_neigh, int *v, int *neigh, int *start, int *end) {
    for (int i = 0; i < num_neigh; i++) {
        MPI_Send(v, n, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, neigh[i]);

        MPI_Send(&start[neigh[i]], 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, neigh[i]);

        MPI_Send(&end[neigh[i]], 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, neigh[i]);
    }
}

// acelasi mecanism de comunicatie ca la get_topology
// vectorul v se imparte in mod egal la nr de workeri -> stanga, dreapta
// fiecare worker va primi acelasi vector v, dar si marginea stanga si dreapta
// fiecare worker inmulteste cu 5 elementele in [stanga, dreapta) 
void calculate(int rank, int n,  int nProcesses) {
    if (rank == 0) {
        int *v = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            v[i] = n - i - 1;
        }

        int nr_workers = nProcesses - 4;
        int start[nProcesses], end[nProcesses];

        for (int i = 0; i < nr_workers; i++) {
            start[i + 4] = i * (double) n / nr_workers;
            end[i + 4] = min(n, (i + 1) * (double) n / nr_workers);
        }

        // trimite parametrii: vectorul pt margini stanga, vectorul pt
        // margini dreapta(folositi de workeri), si vectorul v catre coord 3, pe care le trimite mai departe
        send_params_coord(rank, 3, n, nProcesses, start, end, v);

        // trimite fiecarui worker propriu : v, stanga, dreapta 
        activate_workers(rank, n, num_neigh, v, neigh, start, end);

        int *recv = malloc(n * sizeof(int));
        int *tmp = malloc(n * sizeof(int));

        for (int i = 0; i < n; i++) {
            tmp[i] = v[i];
        }

        // primeste de la workeri vectorul cu elementele din [stanga,dreapta) inmultite cu 5
        for (int i = 0; i < num_neigh; i++) {
            MPI_Recv(recv, n, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < n; j++) {
                if (tmp[j] != recv[j]) {
                    v[j] = recv[j];
                }
            }
        }

        // primeste vectorul complet de la coord 3
        MPI_Recv(recv, n, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < n; j++) {
            if (tmp[j] != recv[j]) {
                v[j] = recv[j];
            }
        }

        // primeste vectorul complet de la coord 2, care trece prin coord 3
        MPI_Recv(recv, n, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < n; j++) {
            if (tmp[j] != recv[j]) {
                v[j] = recv[j];
            }
        }

        // primeste vectorul complet de la coord 1, care trece prin coord 3
        MPI_Recv(recv, n, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int j = 0; j < n; j++) {
            if (tmp[j] != recv[j]) {
                v[j] = recv[j];
            }
        }

        printf("Rezultat: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");

    // aceeasi logica si pt rank = 3,2,1
    } else if (rank == 3) {
        int *v = malloc(n * sizeof(int));
        MPI_Recv(v, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int start[nProcesses], end[nProcesses];
        MPI_Recv(start, nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(end, nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        send_params_coord(rank, 2, n, nProcesses, start, end, v);

        activate_workers(rank, n, num_neigh, v, neigh, start, end);

        int *recv = malloc(n * sizeof(int));
        int *tmp = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            tmp[i] = v[i];
        }

        for (int i = 0; i < num_neigh; i++) {
            MPI_Recv(recv, n, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < n; j++) {
                if (tmp[j] != recv[j]) {
                    v[j] = recv[j];
                }
            }
        }

        MPI_Send(v, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 0);

        MPI_Recv(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(v, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 0);

        MPI_Recv(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(v, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 0);

    } else if (rank == 2) {
        int *v = malloc(n * sizeof(int));
        MPI_Recv(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int start[nProcesses], end[nProcesses];
        MPI_Recv(start, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(end, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        send_params_coord(rank, 1, n, nProcesses, start, end, v);

        activate_workers(rank, n, num_neigh, v, neigh, start, end);

        int *recv = malloc(n * sizeof(int));
        int *tmp = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            tmp[i] = v[i];
        }

        for (int i = 0; i < num_neigh; i++) {
            MPI_Recv(recv, n, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < n; j++) {
                if (tmp[j] != recv[j]) {
                    v[j] = recv[j];
                }
            }
        }

        MPI_Send(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 3);

        MPI_Recv(v, n, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(v, n, MPI_INT, 3, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 3);
    
    } else if (rank == 1) {
        int *v = malloc(n * sizeof(int));
        MPI_Recv(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int start[nProcesses], end[nProcesses];
        MPI_Recv(start, nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(end, nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        activate_workers(rank, n, num_neigh, v, neigh, start, end);

        int *recv = malloc(n * sizeof(int));
        int *tmp = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            tmp[i] = v[i];
        }

        for (int i = 0; i < num_neigh; i++) {
            MPI_Recv(recv, n, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < n; j++) {
                if (tmp[j] != recv[j]) {
                    v[j] = recv[j];
                }
            }
        }

        MPI_Send(v, n, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, 2);
    
    } else {
        int *recv = malloc(n * sizeof(int));
        MPI_Recv(recv, n, MPI_INT, coord, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


        int l_idx, r_idx;
        MPI_Recv(&l_idx, 1, MPI_INT, coord, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&r_idx, 1, MPI_INT, coord, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = l_idx; i < r_idx; i++) {
            recv[i] *= 5;
        }

        MPI_Send(recv, n, MPI_INT, coord, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, coord);
    }
}

int main(int argc, char *argv[]) {
    int rank, nProcesses;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

    int **topology;
    double n = atoi(argv[1]);

    read_neighbors(rank);
    announce_workers(rank);

    //calculez topologia
    topology = get_topology(rank, nProcesses);

    // fiecare coordonator isi calculeaza topologia si
    // dupa o transmite workerilor
    if (rank <= 3) {
        for (int i = 0; i < num_neigh; i++) {
            for (int j = 0; j < 4; j++) {
                // o transmit workerilor
                MPI_Send(topology[j], nProcesses, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, neigh[i]);
            }
        }
    } else {
        topology = malloc(sizeof(int*) * N_COORD);
	
        for (int i = 0; i < N_COORD; i++) {
            topology[i] = calloc(sizeof(int), 20);
        }

        // workerii primesc de la coordonatorul lor topologia
        for (int j = 0; j < 4; j++) {
            MPI_Recv(topology[j], nProcesses, MPI_INT, coord, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        display_topology(rank, nProcesses, topology);
    }

    calculate(rank, n, nProcesses);

    //display_topology(rank, nProcesses, topology);    

    MPI_Finalize();
    return 0;
}