#ifndef UTILS_HT_SENTRY
#define UTILS_HT_SENTRY


#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdalign.h>

typedef struct list LIST_S;
typedef LIST_S *    LIST;
typedef struct node NODE_S;
typedef NODE_S *    NODE;


void*   get_item_data   (NODE item);
void    set_item_data   (NODE item, void *data);

LIST    create_list     (void);
void    delete_list     (LIST list);
/* from the head */
void    push_item       (LIST list, void *data);
void*	pop_item		(LIST list);
/* from the tail */
void    put_item        (LIST list, void *data);
void*	get_item		(LIST list);
NODE    pop_random_item (LIST list);
size_t  get_list_len    (LIST list);
void    set_list_len    (LIST list, size_t len);

size_t  get_rand_num    (size_t l, size_t h);


#endif
