#include "ht_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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


HT_ITEM_S_P
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
            table_double(ht);
    }

    #ifdef DEBUG
        printf("\n");
    #endif

    return ret;
}

signed char
insert_into_slot(HT_ITEM_S_P *ht_arr, void *data, size_t d_size, char *key, size_t h_i)
{
    HT_ITEM_S_P *pp     = &ht_arr[h_i]; 

    while(*pp != NULL) {
        if ( strcmp((*pp)->key, key) == 0 ) {

            #ifdef DEBUG
                printf("\tkey %s is already inserted!\n", key);
            #endif

            (*pp)->data     = data;
            (*pp)->d_size   = d_size;
            return 1; 
        }
        pp = &(*pp)->next;
    }
    *pp                 = create_ht_item(data, d_size, key);

    return 0;
}

void
table_double(HT ht)
{

}

size_t
hash(char *key, uint32_t n)
{
    uint_least64_t pre_h = 0;
    prehash(key, &pre_h);

    #ifdef DEBUG
        printf("\tprehash value is %" PRIuLEAST64 ".\n", pre_h);
    #endif

    return pre_h % n;
}

void
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

void
print_ht(HT ht)
{
    printf("Printing ht...\n");

    HT_ITEM_S_P *arr    = ht->arr;
    size_t ht_len       = ht->len;

    for(size_t i = 0; i < ht_len; ++i) {
        printf("\t%zu \n", i);
        HT_ITEM_S_P i_i = arr[i];
        while(i_i != 0) {
            printf("\tkey is %s and data is %d\n", i_i->key, *((int*)i_i->data));
            i_i = i_i->next;
        }
    }
}

int main(void)
{
    new_ht(ht,1);
    int a = 30;
    void *data = &a;
    size_t d_size = sizeof(a); 
    char *key = "key1";
    insert_data(ht, data, d_size, key);    
    int a15 = 50;
    void * data15 = &a15;
    insert_data(ht, data15, d_size, key);    

    int a2 = 32;
    void *data2 = &a2;    
    char *key2 = "key2";
    insert_data(ht, data2, d_size, key2);    

    print_ht(ht);

/*
    char *s1 = "lalala";
    char *s2 = "alalal";
    char *s5 = "lllaaa";
    char *s3 = "tsaet";
    char *s4 = "teast";
    char *s6 = "etats";
    uint32_t n = 11;
    printf("key - %s, hash - %zu\n", s1, hash(s1, n));
    printf("key - %s, hash - %zu\n", s2, hash(s2, n));
    printf("key - %s, hash - %zu\n", s5, hash(s5, n));
    printf("key - %s, hash - %zu\n", s3, hash(s3, n));
    printf("key - %s, hash - %zu\n", s4, hash(s4, n));
    printf("key - %s, hash - %zu\n", s6, hash(s6, n));
*/
}
