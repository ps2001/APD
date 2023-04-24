#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 2

void *f(void *arg) {
  	long id = *(long*)arg;
	printf("Hello World din functia f apelata din threadul %ld!\n", id);
  	pthread_exit(NULL);
}

void *g(void *arg) {
  	long id = *(long*)arg;
	printf("Hello World din functia g apelata din threadul %ld!\n", id);
  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	long cores = 6;

	pthread_t threads[cores];
  	int r;
  	long id;
  	void *status;
	long ids[cores];

  	// for (id = 0; id < cores; id++) {
	// 	ids[id] = id; 
	// 	r = pthread_create(&threads[id], NULL, f, &ids[id]);

	// 	if (r) {
	//   		printf("Eroare la crearea thread-ului %ld\n", id);
	//   		exit(-1);
	// 	}
  	// }


	ids[0] = 0;
	ids[1] = 1;

	r = pthread_create(&threads[0], NULL, f, &ids[0]);
	if (r) {
		printf("Eroare la crearea thread-ului 0\n");
		exit(-1);
	}

	r = pthread_create(&threads[1], NULL, g, &ids[1]);
	if (r) {
		printf("Eroare la crearea thread-ului 1\n");
		exit(-1);
	}

  	for (id = 0; id < NUM_THREADS; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	pthread_exit(NULL);
}
