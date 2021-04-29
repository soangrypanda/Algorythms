#include "utils.h"


#define LIST_S      struct list
#define NODE_S      struct node

NODE_S {
    #undef NODE_S
    void*           data;
    NODE            next;
    size_t          type_size; 
    size_t          type_align;
};

LIST_S {
    #undef LIST_S
    NODE*           head;
    NODE*           tail;
    NODE            dummy;
    size_t          len;
    size_t          type_size; 
    size_t          type_align;
};


static NODE     create_node     (void *data);
static void     delete_list_do  (LIST list);
static void     init_rand       (void);


/* NODE */

void*
get_item_data(NODE item)                { return item->data; }
void
set_item_data(NODE item, void *data)    { item->data = data; }

static NODE
create_node(void *data)
{
    NODE item = calloc(1, sizeof(*item));
    assert(item != NULL);
    item->data  = data; 
    item->type_size  = sizeof(*item);
    item->type_align = alignof(*item);
    return item;
}


/* LIST */

LIST
create_list(void)
{
    LIST list   = calloc(1, sizeof(*list));
    assert(list != NULL);       

    list->dummy = NULL;
    list->head  = &list->dummy;
    list->tail  = &list->dummy;
    list->len   = 0;
    list->type_size  = sizeof(*list);
    list->type_align = alignof(*list);
    return list;
}

void
delete_list(LIST list)
{
    delete_list_do(list);
    free(list);
}

void
push_item(LIST list, void *data)
{
    NODE tmp    = *list->head;
    *list->head = calloc(1, sizeof(**list->head));
    NODE item   = *list->head;
    assert(item != NULL);

    item->data  = data;
    item->next  = tmp;
    
    list->len++;
}

void *
pop_item(LIST list)
{
    NODE *head  = list->head;
    NODE ret    = *head;
    if(ret == NULL)
        return ret;
    *head       = ret->next;
    ret->next   = NULL;
    
    void *ret_data    = ret->data;
    free(ret);
    
    list->len--; 
    return ret_data;
}

void
put_item(LIST list, void *data)
{
    *list->tail = calloc(1, sizeof(**list->tail));
    NODE item = *list->tail;
    assert(item != NULL);
    
    item->data  = data;
    item->next  = NULL;

    list->tail  = &(item->next);

    list->len++;
}
/*
void *
get_item(LIST list)
{
    NODE *head  = list->list;
    NODE ret    = *head;
    *head       = ret->next;
    ret->next   = NULL;
    
    void *ret_data    = ret->data;
    free(ret);
    
    return ret_data;
}
*/
NODE
pop_random_item(LIST list)
{   
    size_t node_n = get_rand_num(0, list->len);
    NODE *tmp = list->head;
    while(node_n != 0) {
        tmp = &((*tmp)->next);
        node_n--;
    }
    NODE ret = *tmp;
    *tmp = ret->next;
    ret->next = NULL;
    
    list->len--;
    return ret;
}

size_t
get_list_len(LIST list)             { return list->len; }
void
set_list_len(LIST list, size_t len) { list->len = len; }

static void
delete_list_do(LIST list)
{
    NODE head = *list->head;
    while(head != NULL) {
        NODE tmp = head;
        head     = head->next;
        free(tmp->data);
        free(tmp);
    }
}

/* RANDOM NUMBER GENERATOR */
/* maybe sometimes I will write a normal one */

unsigned int seed = 0;

size_t
get_rand_num(size_t l, size_t h)        
{
    if(seed == 0)   init_rand();
    return l + rand_r(&seed) % h;
}

static void
init_rand(void) 
{ 
    seed = time(NULL);
}
