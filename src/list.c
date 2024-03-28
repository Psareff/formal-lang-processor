#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"
#include <string.h>

list_t *create(void *data)
{
	list_t *list = malloc(sizeof(list_t));
	list->data = data;
	list->next = NULL;
	return list;
}


list_t *push(list_t *list, void *data)
{
	if (list == NULL)
		return create(data);
	list->next = push(list->next, data);
	return list;
}
void iterate(list_t *list, void func(void *))
{
	if (list == NULL)
		return;
	func(list->data);
	iterate(list->next, func);
}

void delete(list_t *list, void func(void *))
{
	if(list == NULL)
		return;
	delete(list->next, func);
	func(list->data);
	free(list);
}
