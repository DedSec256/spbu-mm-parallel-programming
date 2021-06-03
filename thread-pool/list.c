#include "list.h"

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

