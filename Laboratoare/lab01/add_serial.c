#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    schelet pentru exercitiul 5
*/

#define NUM_THREADS 6

int* arr;
int array_size;

int min(double a, double b) {
    return a < b ? a : b;
}

void *f(void *arg) {
    long id = *(long *)arg;
    int start = id * (double)array_size / NUM_THREADS;
    int end = min((id + 1) * (double)array_size / NUM_THREADS, array_size);

    for (int i = start; i < end; i++) {
        arr[i] += 100;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

    // TODO: aceasta operatie va fi paralelizata
  	// for (int i = 0; i < array_size; i++) {
    //     arr[i] += 100;
    // }

    void *status;
    long ids[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    for (long i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, f, &ids[i]);
    }

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], &status);
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

  	pthread_exit(NULL);
}
