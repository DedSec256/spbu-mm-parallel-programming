#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include "list.h"

struct work_arg {
	double *pi;
	int num;
};

struct list_head work_list_head;

struct work {
	struct list_head work_list;
	struct work_arg *arg;
	void (*thread_function) (void * arg);
};

struct thread_pool {
	pthread_mutex_t thread_pool_mutex;
	pthread_cond_t thread_add_work_cond;
	pthread_cond_t last_thread_cond;
	int busy_worker_amount;
	int thread_count;
	int finish;
};

struct thread_pool *thread_pool_init (int thread_count);
void *thread_pool_worker (void *arg);
void thread_pool_wait (struct thread_pool *tpool);
void thread_pool_destroy(struct thread_pool *tpool);
void work_add (void (*tfunc) (void * arg), void * arg,
		struct thread_pool *tpool);

#endif

