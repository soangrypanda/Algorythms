#ifndef HT_H_SENTRY
#define HT_H_SENTRY


#include <stddef.h>
#include <inttypes.h>


typedef struct ht       HT_S;
typedef HT_S *          HT;
typedef struct ht_item  HT_ITEM_S;
typedef HT_ITEM_S*      HT_ITEM_S_P;

#ifndef DEF_HT_SIZE
#define DEF_HT_SIZE     8
#endif


#define         new_ht(ht, size)    HT ht   = init_hash_table(size)
HT              init_hash_table     (size_t len);

#define         delete_ht(ht)       ht      = delete_ht_do(ht)
HT              delete_ht_do        (HT ht);

signed char     insert_data         (HT ht, void *data, size_t d_size, char *key);
HT_ITEM_S_P     find_item_by_key    (HT ht, char *key);
HT_ITEM_S_P     pop_item            (HT ht, char *key);
void            print_ht            (HT ht);
char **         fill_ht_with_r_d    (HT ht, size_t size, int32_t min, int32_t max);


#endif
