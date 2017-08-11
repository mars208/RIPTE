#include "list.h"
#include <stdlib.h>

int check(const list_t *list);

int check(const list_t *list){
	if(list == NULL) return(-1);
	//if(list->head == NULL) return(-1);
	return 0;
}
// Assume list is malloced or freed outside this lib.
void list_init(list_t *list){
	if(check(list) == -1) return;
	list->head = (elem_t *)malloc(sizeof(elem_t));
	list->head->next = NULL;
	list->head->data = NULL;
	list->last = list->head;
}
void list_finalize(list_t *list){
	if(check(list) == -1) return;
	while(!list_is_empty(list)){
		list_pop(list);
	}
	free(list->head);
	list->head = NULL;
	list->last = NULL;
}

void *list_head(const list_t *list){
	if(check(list) == -1) return NULL;
	if(list -> head == NULL) return NULL;
	if(list -> head -> next == NULL) return NULL;
	return list->head->next->data;
}
void *list_last(const list_t *list){
	if(check(list) == -1) return NULL;
	if(list->last == NULL) return NULL;
	return list->last->data;
}
bool list_is_empty(const list_t *list){
	if(check(list) == -1) return true;
	if(list->head == list->last)
		return true;
	else
		return false;
}

void list_append(list_t *list, void* data){
	elem_t *newnode;
	if(check(list) == -1) return ;
	newnode = (elem_t*)malloc(sizeof(elem_t));
	newnode->next = NULL;
	newnode->data = data;
	list->last->next = newnode;
	list->last = newnode;
}
void *list_remove(list_t *list){
	iter_t *iter;
	elem_t *last_node;
	void *data;
	if(check(list) == -1) return NULL;
	if(list_is_empty(list))return NULL;
	iter = list_iter(list);
	while(iter->curr->next != list->last)
		list_next(iter);
	last_node = list->last;
	data = last_node->data;
	iter->curr->next = NULL;
	list->last = iter->curr;
	free(last_node);
	return data;
}

void list_push(list_t *list, void* data){
	elem_t *newnode;
	if(check(list) == -1) return;
	newnode = (elem_t*)malloc(sizeof(elem_t));
	newnode->data = data;
	newnode->next = list->head->next;
	list->head->next = newnode;
	if(list->head == list->last)list->last = newnode;
}
void *list_pop(list_t *list){
	elem_t *first_node;
	void *data;
	if(check(list) == -1) return NULL;
	if(list_is_empty(list))return NULL;
	first_node = list->head->next;
	data = first_node->data;
	list->head->next = first_node->next;
	if(first_node == list->last)list->last = list->head;
	free(first_node);
	return data;
}

iter_t *list_iter(list_t *list){
	iter_t *iter;
	if(check(list) == -1) return 0;
	iter = (iter_t*)malloc(sizeof(iter_t));
	iter->curr = list->head;
	return iter;
}
void *list_next(iter_t *iter){
	if(iter == NULL) return NULL;
	// If iter->curr is NULL, error happens.
	if(iter->curr == NULL) return NULL;
	if(iter->curr->next == NULL){
		return NULL;
	}else{
		iter->curr = iter->curr->next;
		return iter->curr->data;
	}
	return NULL;
}