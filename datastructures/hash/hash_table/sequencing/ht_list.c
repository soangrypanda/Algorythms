#include "ht_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

struct ht {
    struct ht_item **arr; 
    size_t          len;
    size_t          n_elem;
};

struct ht_item {
    void            *data;
    char            *key;
    struct ht_item  *next;
    size_t          d_size;
};

enum { HT_DECREASE_TRESHOLD = 3 }; 

/**********************************
 * INTERNAL INTERFACE DESCRIPTION *
 **********************************/
 
/* HT_ITEM */
static HT_ITEM_S_P  create_ht_item      (void *data, size_t d_size, char *key);

/* HT */
static void         delete_ht_arr       (HT_ITEM_S_P *arr, size_t arr_len);
static void         delete_ht_list      (HT_ITEM_S_P list);
static HT_ITEM_S_P* find_place_in_slot  (HT_ITEM_S_P *pp, char *key, char *ret);
static signed char  insert_into_slot    (HT_ITEM_S_P *ht_arr, void *data, size_t d_size, 
                                        char *key, size_t h_i);
static void         replace_data        (HT_ITEM_S_P item, void *data, size_t d_size);
static void         change_ht_len       (HT ht, int change_how);
enum                                    { INCREASE_HT, DECREASE_HT };

/* MISCELANEOUS */
static char         is_prime            (size_t x);
static size_t       next_prime          (size_t x);
static size_t       hash                (char *key, uint32_t n);
static void         prehash             (char *key, uint_least64_t *prehash);
static void         init_rand           (void);
static int32_t      get_rand_num        (int32_t l, int32_t h);


/*****************************
 * INTERFACE IMPLEMENTATIONS *
 *****************************/
 
/* 
 * HT_ITEM
 */

/* HT_ITEM INTERNAL IMPLEMENTATIONS */

static HT_ITEM_S_P
create_ht_item(void *data, size_t d_size, char *key)
{
    HT_ITEM_S_P lnsp = calloc(1, sizeof(*lnsp));
    if(NULL == lnsp) {
        perror("create_ht_node");
        exit(EXIT_FAILURE);
    }

    lnsp->data      = data;
    lnsp->d_size    = d_size;
    lnsp->key       = key;
    lnsp->next      = NULL;
}

/*
 * HT
 */
 
/* HT EXTERNAL IMPLEMENTATIONS */

HT
init_hash_table(size_t len)
{
    if(len == 0)
        len = DEF_HT_SIZE;
    
    HT new_ht       = calloc(1, sizeof(*new_ht));
    if(NULL == new_ht) {
        perror("init_hash_table");
        exit(EXIT_FAILURE);
    }

    HT_ITEM_S_P *arr = calloc(len, sizeof(*arr));
    if(NULL == arr) {
        perror("init_hash_table");
        exit(EXIT_FAILURE);
    }

    new_ht->arr         = arr;
    new_ht->len         = len;
    new_ht->n_elem      = 0;

    return new_ht;
}

HT
delete_ht_do(HT ht)
{
    delete_ht_arr(ht->arr, ht->len);
    free(ht);
    return NULL;
}

signed char
insert_data(HT ht, void *data, size_t d_size, char *key)
{
    #ifdef DEBUG
        printf("Inserting key %s...\n", key);
    #endif

    HT_ITEM_S_P *ht_arr = ht->arr;

    size_t h_i          = hash(key, ht->len);

    #ifdef DEBUG
        printf("\thash value is %zu.\n", h_i);
    #endif

    signed char ret = insert_into_slot(ht_arr, data, d_size, key, h_i);

    if(ret == 0) {
        size_t ht_len       = ht->len;
        size_t ht_n_elem    = ++ht->n_elem; 
        if(ht_n_elem >= ht_len)
            change_ht_len(ht, INCREASE_HT);
    }

    #ifdef DEBUG
        printf("\n");
    #endif

    return ret;
}

HT_ITEM_S_P
find_item_by_key(HT ht, char *key)
{
    HT_ITEM_S_P *arr    = ht->arr;
    size_t len          = ht->len;
    size_t h_i          = hash(key, len);

    if(h_i >= len)
        return NULL;

    HT_ITEM_S_P *pp     = &arr[h_i];
    pp                  = find_place_in_slot(pp, key, NULL);

    return *pp;
}

HT_ITEM_S_P
pop_item(HT ht, char *key)
{
    HT_ITEM_S_P *arr    = ht->arr;
    size_t len          = ht->len;
    size_t h_i          = hash(key, len);
    HT_ITEM_S_P *pp     = &arr[h_i];
    pp                  = find_place_in_slot(pp, key, NULL);

    HT_ITEM_S_P ret     = *pp;
    if(ret != NULL) {
        *pp = ret->next;
        ht->n_elem--; 
    }
    
    if(ht->n_elem * HT_DECREASE_TRESHOLD <= len) {
        change_ht_len(ht, DECREASE_HT);
    }
    
    return ret;
}

void
print_ht(HT ht)
{
    if(ht != NULL)
        printf("Printing ht...\n");
    else {
        printf("No ht to print!\n");
        return;
    }

    HT_ITEM_S_P *arr    = ht->arr;
    size_t ht_len       = ht->len;

    for(size_t i = 0; i < ht_len; ++i) {
        printf("\t%zu \n", i);
        HT_ITEM_S_P i_i = arr[i];
        while(i_i != NULL) {
            printf("\tkey is %s and data is %s\n", i_i->key, i_i->data);
            i_i = i_i->next;
        }
    }
}

/* HT INTERNAL IMPLEMENTATIONS */

static void
delete_ht_arr(HT_ITEM_S_P *arr, size_t arr_len)
{
    for(size_t i = 0; i < arr_len; ++i) 
        delete_ht_list(arr[i]);
    free(arr);
}

static void
delete_ht_list(HT_ITEM_S_P list)
{
    while(list != NULL) {
        HT_ITEM_S_P next = list->next;
        free(list->data);
        free(list->key);
        free(list);
        list = next;
    }
}

static HT_ITEM_S_P *
find_place_in_slot(HT_ITEM_S_P *pp, char *key, char *ret)
{
    char status = 0;

    while(*pp != NULL) {
        if ( key != NULL && strcmp((*pp)->key, key) == 0 ) {
            #ifdef DEBUG
                printf("\tkey %s is found in the ht!\n", key);
            #endif
            status = 1;
            break;
        }
        pp = &(*pp)->next;
    }

    if(ret != NULL) *ret = status;
    return pp;
}

static signed char
insert_into_slot(HT_ITEM_S_P *ht_arr, void *data, size_t d_size, char *key, size_t h_i)
{
    HT_ITEM_S_P *pp      = &ht_arr[h_i]; 
    char x;
    char ret;
    pp                   = find_place_in_slot(pp, key, &x);

    if(x == 0) {
        *pp = create_ht_item(data, d_size, key);
        ret = 0;
    }
    else {
        replace_data(*pp, data, d_size); 
        ret = 1; 
    }

    return ret;
}

static void
replace_data(HT_ITEM_S_P item, void *data, size_t d_size)
{
    free(item->data);
    item->data      = data;
    item->d_size    = d_size;
}


static void
change_ht_len(HT ht, int change_how)
{
    #ifdef DEBUG
        printf("\nEntering change_ht_len{}...\n");
    #endif

    HT_ITEM_S_P *arr    = ht->arr;
    size_t old_len      = ht->len;
    size_t new_len      = 0;

    switch(change_how) {
        case(INCREASE_HT): new_len = next_prime(old_len);      break;
        case(DECREASE_HT): new_len = next_prime(old_len >> 2); break;
        default: return;
    }

    HT_ITEM_S_P *new_arr = calloc(new_len, sizeof(*new_arr));

    for(size_t i = 0; i < old_len; ++i) {
        HT_ITEM_S_P item = arr[i];
        while(item != NULL) {
            size_t h_i      = hash(item->key, new_len);
            HT_ITEM_S_P *pp = &new_arr[h_i];
            pp              = find_place_in_slot(pp, NULL, NULL);
            *pp             = item; 
            item = item->next;
            (*pp)->next     = NULL;
        }
    } 
    
    #ifdef DEBUG
        printf("\tOld len is %zu...\n", ht->len);
    #endif

    ht->len     = new_len;

    #ifdef DEBUG
        printf("\tNew len is %zu...\n", ht->len);
    #endif
    
    ht->arr     = new_arr;
    free(arr);
}

/*
 * MISCELANEOUS
 */
 
/* MISCELANEOUS EXTERNAL IMPLEMENTATIONS */

unsigned int seed;
char **
fill_ht_with_r_d(HT ht, size_t size, int32_t min, int32_t max)
{
    if(!seed) init_rand();
    
    if(max==0) max = size;

    char **arr_of_keys = calloc(size, sizeof(*arr_of_keys));

    for(size_t i = 0; i < size; ++i) {
        int d_n   = get_rand_num(min, max);
        int key_n = ( (size_t)pow(get_rand_num(min, max), i) << sizeof(max) / 2) | d_n>>2;
        
        #ifdef DEBUG
            printf("rand num is %d for key and %d for data\n", key_n, d_n);
        #endif

        int32_t key_len = snprintf(NULL, 0, "%d", key_n) + 1;
        char *key       = calloc(1, key_len);
        snprintf(key, key_len, "%d", key_n);
        
        int32_t d_len   = snprintf(NULL, 0, "%d", d_n) + 1;
        char *data      = calloc(1, d_len);
        snprintf(data, d_len, "%d", d_n);

        #ifdef DEBUG
            printf("key is %s and data is %s\n", key, data);
        #endif
        
        char ret = insert_data(ht, data, d_len, key);    
        if(ret == 1)
            free(key);
        else
            arr_of_keys[i] = key;
    }

    return arr_of_keys;
}

/* MISCELANEOUS INTERNAL IMPLEMENTATIONS */

static char
is_prime(size_t x)
{
    for (size_t i = 3; i < x; i += 2)
    {
        size_t p_sqrt = x / i;
        if (p_sqrt < i)
            return 1;
        if(p_sqrt * i == x)
            return 0;
    }
    return 1;
}

static size_t
next_prime(size_t x)
{
    #ifdef DEBUG
        printf("\nEntering next_prime with value %zu\n", x);
    #endif

    if (x < 2)      return 2;

    if (!(x & 1))   ++x;
    else            x += 2;

    for (; !is_prime(x); x += 2)    ;

    #ifdef DEBUG
        printf("\tNext prime is %zu\n\n", x);
    #endif

    return x;
}

static size_t
hash(char *key, uint32_t n)
{
    uint_least64_t pre_h = 0;
    prehash(key, &pre_h);

    #ifdef DEBUG
        printf("\tprehash value is %" PRIuLEAST64 ".\n", pre_h);
    #endif

    return pre_h % n;
}

static void
prehash(char *key, uint_least64_t *prehash)
{
    size_t key_len      = strlen(key);
    uint_least64_t sum  = 0;
    char prev           = 0;
    char next           = 0;
    size_t i            = 0;
    for( ; i < key_len - 1; ++i) {
        next = key[i+1];
        if(i % 2 == 0)  sum += (key[i] + prev) * next; 
        else            sum += key[i] * prev + next;
        prev = key[i];
    }
    sum += key[i] + sum & next;

    *prehash            = sum;
}

static void
init_rand(void) 
{
    seed = time(NULL);
}

static int32_t
get_rand_num(int32_t l, int32_t h)
{
    return l + rand_r(&seed) % h;
}
