#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <stdatomic.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>

struct list_head {
	struct list_head *next, *prev;
};

struct resource {
	struct list_head res_list;
	int val;
};

pthread_t *tid;
volatile sig_atomic_t flag = 0;
struct list_head res_list_head;
pthread_mutex_t res_mutex;

void terminate()
{
      	flag = 1;
}

void list_init(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

void list_add(struct list_head *new,
	      struct list_head *prev,
	      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void list_del(struct list_head * prev,
			    struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

void* producer ()
{
	while (!flag) {
		struct resource *res = (struct resource *)
				       calloc(1, sizeof(struct resource));
		res->val = rand() % 1000;
		pthread_mutex_lock(&res_mutex);
		list_add(&res->res_list, res_list_head.prev, &res_list_head);
		pthread_mutex_unlock(&res_mutex);
		printf("ADD | thread id: %li, val: %i, res: %p\n", (long)pthread_self(), res->val, res);
		sleep(1);
	}
	printf("thread id: %li finished\n", (long)pthread_self());
	return NULL;
}

void* consumer ()
{
	int val;
	while(!flag) {
		struct resource *res;

		pthread_mutex_lock(&res_mutex);
		if (res_list_head.next == &res_list_head) {
			pthread_mutex_unlock(&res_mutex);
			sleep(1);
			continue;
		}

		res = (struct resource *) res_list_head.prev;
		val = res->val;

		list_del(res_list_head.prev->prev, &res_list_head);
		free(res);
		pthread_mutex_unlock(&res_mutex);

		printf("DEL | thread id: %li, val: %i, res: %p\n", (long)pthread_self(), val, res);
		sleep(1);
	}
	printf("thread id: %li finished\n", (long)pthread_self());
	return NULL;
}

int main (int argc, char **argv)
{
	int i, err = 0;
	int prod, cons;
	signal(SIGINT, terminate);

	if (argc < 2 ) {
		printf("usage: ./main <prod cons>\n");
		return 0;
	}
	prod = atoi(argv[1]);
	cons = atoi(argv[2]);
	printf("prod: %i, cons: %i\n", prod, cons);

	srand(time(NULL));

	pthread_mutex_init(&res_mutex, NULL);
	tid = calloc(prod + cons, sizeof(pthread_t));
	list_init(&res_list_head);

	i = 0;
	while (i < prod) {
		err = pthread_create(&(tid[i]), NULL, &producer, NULL);
		if (err != 0) {
			printf("can't create thread :[%i]\n", err);
			return 0;
		}
		i++;
	}
	while (i < prod + cons) {
		err = pthread_create(&(tid[i]), NULL, &consumer, NULL);
		if (err != 0) {
			printf("can't create thread :[%i]\n", err);
			return 0;
		}
		i++;
	}

	i = 0;
	while (i < (prod + cons)) {
		pthread_join(tid[i], NULL);
		i++;
	}

	pthread_mutex_destroy(&res_mutex);
	printf("List remained:\n");
	while (res_list_head.next != &res_list_head) {
		int val;
		struct resource *res;

		res = (struct resource *) res_list_head.prev;
		val = res->val;
		list_del(res_list_head.prev->prev, &res_list_head);

		printf("LEFT: val: %i, res: %p\n",  val, res);
	}
	return 0;
}

