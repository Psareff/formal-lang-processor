#ifndef LIST_H_
#define LIST_H_

typedef struct list list_t;

typedef struct list
{
	void *data;
	list_t *next;
} list_t;

list_t *push(list_t *list, void *data);
list_t *create(void *data);
void iterate(list_t *list, void func(void *));
void delete(list_t *list, void func(void *));
#endif //LIST_H_
