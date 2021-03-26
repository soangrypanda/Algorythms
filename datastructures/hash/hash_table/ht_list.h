#ifndef HT_H_SENTRY
#define HT_H_SENTRY

#include <stddef.h>
#include <inttypes.h>

typedef struct ht       HT_S;
typedef HT_S *          HT;
typedef struct ht_item  HT_ITEM_S;
typedef HT_ITEM_S*      HT_ITEM_S_P;

#define DEF_HT_SIZE     8


#define     new_ht(ht, size)    HT ht   = init_hash_table(size)
#define     delete_ht(ht)       ht      = delete_ht_do(ht)

signed char
insert_into_slot(HT_ITEM_S_P *ht_arr, void *data, size_t d_size, char *key, size_t h_i);

enum { INCREASE_HT, DECREASE_HT };
void
change_ht_len(HT ht, int change_how);


size_t next_prime(size_t x);
size_t prev_prime(size_t n);


size_t
hash(char *key, uint32_t n);

void
prehash(char *key, uint_least64_t *prehash);





#endif
