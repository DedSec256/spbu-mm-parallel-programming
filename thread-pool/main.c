#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>

void leibniz_for_pi(void *arg) {
	double * pi = ((struct work_arg *)arg)->pi;
	int num = ((struct work_arg *)arg)->num;
	double part_sum = (num * 2) + 1;
	double sum = 4.0 / (num % 2 == 0 ? part_sum : -part_sum);
	*pi += sum;
	printf("thread id: %ld, num = %d, part_sum = %lf, sum = %lf, pi = %lf\n",
	      (long int)pthread_self(),
	      num,
	      part_sum,
	      sum,
	      *pi);
}

int main (int argc, char ** argv) {
	int work_count;
	struct thread_pool *tpool;
	double *pi = calloc(1, sizeof(double));
	struct work_arg *arg;
	int thread_count = 2;
	if (argc < 2) {
		printf ("usage: ./thread_pool <work count> <thread count>\n");
		return 0;
	}

	work_count = atoi(argv[1]);
	thread_count = atoi(argv[2]);
	arg = calloc(work_count, sizeof(struct work_arg));

	tpool = thread_pool_init(thread_count);

	for (int i = 0; i < work_count; i++) {
		arg[i].pi = pi;
		arg[i].num = i;
		work_add(&leibniz_for_pi, arg + i, tpool);
	}
	thread_pool_wait(tpool);


	printf("pi == %lf\n", *pi);
	free(pi);
	free(arg);
	thread_pool_destroy(tpool);
	return 0;
}
