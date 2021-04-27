#ifndef HTO_H_SENTRY
#define HTO_H_SENTRY


#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdalign.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <math.h>


typedef     struct ht_s         HT_S;
typedef     HT_S *              HT;
typedef     struct ht_item_s    HT_ITEM_S;
typedef     HT_ITEM_S *         HT_ITEM;

typedef     char (*ht_traverse_cond_fp) (HT ht, char *key, size_t i);

#define     new_ht(ht, size) HT ht = init_ht(size);
HT          init_ht         (size_t size);
#define     delete_ht(ht)    ht = delete_ht_do(ht);
HT          delete_ht_do    (HT ht);

#define     SIZE_OF(obj_p) ( sizeof(*obj_p) != 1 ? sizeof(*obj_p) : strlen(obj_p) )

#define     insert_ht_item(ht, dp, kp) insert_ht_item_do(ht, dp, SIZE_OF(dp), kp, SIZE_OF(kp))
signed char insert_ht_item_do  (HT ht, void *data, size_t data_size, void *k, size_t k_size);

#define     delete_ht_item(ht, kp) delete_ht_item_do(ht, kp, SIZE_OF(kp))
signed char delete_ht_item_do  (HT ht, void *k, size_t k_size);

#define     search_ht_item(ht, kp, sp) search_ht_item_do(ht, kp, SIZE_OF(kp), sp)
void *      search_ht_item_do  (HT ht, void *k, size_t k_size, size_t *sp);

void        print_ht        (HT ht);

char **     fill_ht_with_r_d(HT ht, size_t size, int32_t min, int32_t max);


#endif
