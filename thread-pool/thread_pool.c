#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>

struct thread_pool *thread_pool_init (int thread_count) {
	struct thread_pool *tpool;
	pthread_t  thread;

	tpool = calloc(1, sizeof(struct thread_pool));
	tpool->finish = 0;
	tpool->thread_count = thread_count;
	list_init(&work_list_head);
	pthread_mutex_init(&tpool->thread_pool_mutex, NULL);
	pthread_cond_init(&tpool->last_thread_cond, NULL);
	pthread_cond_init(&tpool->thread_add_work_cond, NULL);

	for (int i = 0; i < thread_count; i++) {
		pthread_create(&thread, NULL, thread_pool_worker, tpool);
		pthread_detach(thread);
	}

	return tpool;
}

void work_add (void (*tfunc) (void * arg), void *arg,
		 struct thread_pool *tpool) {
	struct work *thread_work;

	if (!tfunc)
		return;

	thread_work = calloc (1, sizeof (struct work));
	pthread_mutex_lock(&tpool->thread_pool_mutex);
	list_add(&thread_work->work_list,
		  work_list_head.prev,
		 &work_list_head);
	thread_work->thread_function = tfunc;
	thread_work->arg = arg;

	pthread_cond_broadcast(&tpool->thread_add_work_cond);
	pthread_mutex_unlock(&tpool->thread_pool_mutex);
	return;
}

void *thread_pool_worker (void *arg) {
	struct thread_pool *tpool= (struct thread_pool *)arg;
	struct work *wpool;
	printf("worker %ld started\n", (long)pthread_self());

	while (1) {
		pthread_mutex_lock(&tpool->thread_pool_mutex);

		while ((&work_list_head == work_list_head.prev) && !tpool->finish){
			pthread_cond_wait(&(tpool->thread_add_work_cond), &(tpool->thread_pool_mutex));
		}

		if (tpool->finish)
			break;

		if (&work_list_head != work_list_head.prev) {
			wpool = (struct work *) work_list_head.prev;
			list_del(work_list_head.prev->prev, &work_list_head);

			tpool->busy_worker_amount++;
			wpool->thread_function(wpool->arg);
			pthread_mutex_unlock(&tpool->thread_pool_mutex);
			free(wpool);

			pthread_mutex_lock(&tpool->thread_pool_mutex);
			tpool->busy_worker_amount--;
		}

		if ((!tpool->finish) && (tpool->busy_worker_amount == 0) &&
			(&work_list_head == work_list_head.prev)) {
			pthread_cond_signal(&tpool->last_thread_cond);
		}
		pthread_mutex_unlock(&tpool->thread_pool_mutex);
	}

	tpool->thread_count--;
	pthread_cond_signal(&tpool->last_thread_cond);
	pthread_mutex_unlock(&tpool->thread_pool_mutex);
	return NULL;
}

void thread_pool_wait (struct thread_pool *tpool) {
	if (tpool == NULL)
		return;
	pthread_mutex_lock(&tpool->thread_pool_mutex);

	while (1) {
		if (((!tpool->finish) && (tpool->busy_worker_amount != 0)) ||
			 ((tpool->finish) && (tpool->thread_count != 0))) {
			pthread_cond_wait(&tpool->last_thread_cond,
						  &tpool->thread_pool_mutex);
		} else {
			pthread_mutex_unlock(&tpool->thread_pool_mutex);
			if (&work_list_head != work_list_head.prev) {
				pthread_mutex_lock(&tpool->thread_pool_mutex);
				continue;
			}
			pthread_mutex_lock(&tpool->thread_pool_mutex);
			break;
		}
	}
	pthread_mutex_unlock(&tpool->thread_pool_mutex);
	return;

}
void thread_pool_destroy(struct thread_pool *tpool) {
	struct work *curr_work;

	if (tpool == NULL)
		return;
	pthread_mutex_lock(&tpool->thread_pool_mutex);

	while (&work_list_head != work_list_head.prev) {
		curr_work = (struct work *) work_list_head.prev;
		list_del(work_list_head.prev->prev, &work_list_head);
		free(curr_work);
	}
	tpool->finish = 1;
	pthread_cond_broadcast(&tpool->thread_add_work_cond);
	pthread_mutex_unlock(&tpool->thread_pool_mutex);

	thread_pool_wait(tpool);

	pthread_mutex_destroy(&tpool->thread_pool_mutex);
	pthread_cond_destroy(&tpool->last_thread_cond);
	pthread_cond_destroy(&tpool->thread_add_work_cond);

	free(tpool);
	return;
}
