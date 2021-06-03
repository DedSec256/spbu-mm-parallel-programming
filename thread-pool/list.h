#ifndef __LIST_H__
#define __LIST_H__

struct list_head {
	struct list_head *next, *prev;
};

void list_init(struct list_head *list);

void list_add(struct list_head *new,
	      struct list_head *prev,
	      struct list_head *next);

void list_del(struct list_head * prev,
			    struct list_head * next);

#endif
