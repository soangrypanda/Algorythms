#include "hto.h"

struct ht_item_s {
    void *  data;
    char *  key;
    size_t  size;
    size_t  k_size;
};

struct ht_s {
    struct ht_item_s ** arr;
    size_t              size;
    size_t              prime;
    size_t              elems;
};

static HT_ITEM_S deleted_item_s = {0};
static HT_ITEM_S *deleted_item  = &deleted_item_s;

enum change_ht_len { INCREASE_HT, DECREASE_HT };

#define     HT_TRAVERSE_MODE_DECLARATIONS       HT_TRAVERSE_MODE(insert) \
                                                HT_TRAVERSE_MODE(delete) \
                                                HT_TRAVERSE_MODE(search) \
                                                
enum traverse_mods { 
#define HT_TRAVERSE_MODE(mode) ht_ ## mode ## _m ,
    HT_TRAVERSE_MODE_DECLARATIONS
#undef HT_TRAVERSE_MODE

};

#define HT_TRAVERSE_MODE(mode) static char if_found_for_ ## mode (HT ht, char *key, size_t i);
    HT_TRAVERSE_MODE_DECLARATIONS
#undef HT_TRAVERSE_MODE 


ht_traverse_cond_fp ht_traverse_jump_table[] =  {
#define HT_TRAVERSE_MODE(mode) if_found_for_ ## mode ,
    HT_TRAVERSE_MODE_DECLARATIONS
#undef HT_TRAVERSE_MODE 
};

#undef HT_TRAVERSE_MODE_DECLARATIONS


/**********************************
 * INTERNAL INTERFACE DESCRIPTION *
 **********************************/

/* HT_ITEM */
static HT_ITEM          create_item             (void *data, size_t data_size, char *key);
static void             delete_item             ( HT_ITEM * item_p);

/* HT */
static char *           get_bytes_as_string     (void *k, size_t k_size);
static char             change_ht_size          (HT ht, int change_how);
static HT_ITEM *        traverse_ht             (HT ht, char *key, int mode);
static char             if_found_for_insert     (HT ht, char *key, size_t i);
static char             if_found_for_delete     (HT ht, char *key, size_t i);
static char             if_found_for_search     (HT ht, char *key, size_t i);
static size_t           hash1                   (size_t size, uint_least64_t prehash);
static size_t           hash2                   (uint_least64_t prime, uint_least64_t prehash);
static uint_least64_t   prehash                 (char *key);

/* MISCELANEOUS */
static char             is_prime                (uint64_t x);
static uint64_t         next_prime              (uint64_t x);
static void             init_rand               (void);
static int32_t          get_rand_num            (int32_t l, int32_t h);


/*****************************
 * INTERFACE IMPLEMENTATIONS *
 *****************************/
 
/* 
 * HT_ITEM
 */

/* HT_ITEM INTERNAL IMPLEMENTATIONS */

static HT_ITEM
create_item(void *data, size_t data_size, char *key)
{
    HT_ITEM item     = calloc(1, sizeof(*item));
    if(NULL == item) {
        perror("create_item");
        exit(EXIT_FAILURE);
    }
    
    item->data =   data;
    item->size =   data_size;
    item->key  =   key;
    
    return item;
}

static void
delete_item( HT_ITEM * item_p)
{
    HT_ITEM item = *item_p;
    free(item->data);
    free(item->key);
    free(item);
    *item_p = deleted_item;
}

/*
 * HT
 */
 
/* HT EXTERNAL IMPLEMENTATIONS */

HT
init_ht(size_t size)
{
    HT ht            = calloc(1, sizeof(*ht));
    if(NULL == ht) {
        perror("init_ht - HT_S");
        exit(EXIT_FAILURE);
    }
    
    size_t prime_size = next_prime(size);
    HT_ITEM *ht_arr  = calloc(prime_size, sizeof(*ht_arr));
    if(NULL == ht_arr) {
        perror("init_ht - HT_ITEM arr");
        exit(EXIT_FAILURE);
    }
    
    ht->arr     = ht_arr;
    ht->size    = prime_size;
    ht->prime   = next_prime(prime_size / 2);
    ht->elems   = 0;
    
    return ht;
}

HT
delete_ht_do(HT ht)
{
    HT_ITEM *arr    = ht->arr;
    size_t size     = ht->size;
    
    for(size_t i = 0; i < size; ++i) {
        HT_ITEM item = arr[i];
        if(item != NULL && item != deleted_item) {
            delete_item(&item);   
        }
    }
    free(arr);
    free(ht);
    
    return NULL;
}

signed char
insert_ht_item_do(HT ht, void *data, size_t data_size, void *k, size_t k_size)
{
    #ifdef DEBUG
        printf("\nEntering insert_ht_item()!\n");
        printf("\t data size is %zu and key size is %zu\n", data_size, k_size);
    #endif
    
    signed char ret;
   
    char *key        = get_bytes_as_string(k, k_size); 
    #ifdef DEBUG
        printf("\tPrinting byte representation - %s\n", key);
    #endif

    HT_ITEM * item_p = traverse_ht(ht, key, ht_insert_m);
    
    if(item_p != NULL) {
        *item_p = create_item(data, data_size, key);
        ht->elems++;
        ret = 0;
        #ifdef DEBUG
            printf("\t item inserted into ht!\n");
        #endif
            
        if(ht->elems >= ht->size)
            change_ht_size(ht, INCREASE_HT);
    }
    else {
        ret = -1;  
        #ifdef DEBUG
            printf("\t item is already in ht!\n");
        #endif
    }
    
    return ret;
}

signed char
delete_ht_item_do(HT ht, void *k, size_t k_size)
{
    #ifdef DEBUG
        printf("\nEntering delete_ht_item()!\n");
    #endif
    
    signed char ret;
    
    char *key        = get_bytes_as_string(k, k_size); 

    HT_ITEM *item_p = traverse_ht(ht, key, ht_delete_m);
    if(item_p != NULL) {
        delete_item(item_p);
        ht->elems--;
        ret = 0;
        #ifdef DEBUG
            printf("\t item deleted from ht!\n");
        #endif
    }
    else {
        ret = -1;  
        #ifdef DEBUG
            printf("\t item is not in in ht!\n");
        #endif
    }
    
    if(ht->elems * 3 <= ht->size) {
        change_ht_size(ht, DECREASE_HT);
    }
    
    free(key); 
    return ret;
}

void *
search_ht_item_do(HT ht, void *k, size_t k_size, size_t *sp)
{
    #ifdef DEBUG
        printf("\nEntering search_ht_item()!\n");
    #endif  
    
    void *ret;
    
    char *key       = get_bytes_as_string(k, k_size); 
    HT_ITEM *item_p = traverse_ht(ht, key, ht_search_m);
    
    if(item_p != NULL) {
        HT_ITEM item = *item_p;
        size_t size  = item->size;
        void *tmp;
        int r = posix_memalign(&tmp, alignof(max_align_t), size);
        if(r != 0) {
            perror("search_ht_item - posix_memalign");
            return NULL;
        }
        memcpy(tmp, item->data, size);              
        /* this is one shady stuff, probably should store alignof item in item. */
        ret = tmp;
        *sp = size;
        #ifdef DEBUG
            printf("\t item found in ht!\n");
        #endif
    }
    else {
        ret = NULL;
        *sp = 0;
        #ifdef DEBUG
            printf("\t item is not in in ht!\n");
        #endif
    }

    free(key);    
    return ret;
}

void
print_ht(HT ht)
{
    #ifdef DEBUG
        printf("\nEntering print_ht()!\n");
    #endif
    
    if(ht == NULL) {
        printf("No ht to print!\n");
        return;
    }
    
    size_t size =   ht->size;
    HT_ITEM *arr =  ht->arr;
    
    for(size_t i = 0; i < size; ++i) {
        printf("%d ", i);
        HT_ITEM item = arr[i];
        if      (item != NULL && item != deleted_item)
            printf("key is %s and data is %s\n", item->key, item->data); 
        else if (item == deleted_item)
            printf("deleted item\n");
        
        else
            printf("\n");
    }
}

/* HT INTERNAL IMPLEMENTATIONS */

static char *
get_bytes_as_string(void *k, size_t k_size)
{
    #ifdef DEBUG
        printf("\nEntering get_bytes_as_string()!\n");
    #endif  
    char *ret   = calloc(k_size + 1, sizeof(*ret));
    assert(ret != NULL);
    
    char *key   = (char *) k;

    memmove(ret, key, k_size);
    ret[k_size] = 0;

    return ret;
}

static char
change_ht_size(HT ht, int change_how)
{
    #ifdef DEBUG
        printf("\nEntering change_ht_size()!\n");
    #endif  
    
    HT_ITEM *arr    = ht->arr;
    size_t old_s    = ht->size;
    size_t new_s    = 0;
    
    switch(change_how) {
        case(INCREASE_HT): new_s = next_prime(old_s * 2); break;
        case(DECREASE_HT): new_s = next_prime(old_s / 2); break;
        default: return -1;
    }
    
    HT_ITEM *new_arr = calloc(new_s, sizeof(*new_arr));
    if(NULL == new_arr) {
        perror("change_ht_size");
        return -1;
    }
    ht->arr         = new_arr;
    ht->size        = new_s;
    ht->prime       = old_s;
    
    for(size_t i = 0; i < old_s; ++i) {
        HT_ITEM item = arr[i];
        if(item == NULL || item == deleted_item)
            continue;
        HT_ITEM *new_place_p = traverse_ht(ht, item->key, ht_insert_m);
        *new_place_p         = item;
    }
    free(arr);
    
    return 0;
}

enum traverse_results { found, n_found, fail_and_return_null };

static HT_ITEM *
traverse_ht(HT ht, char *key, int mode)
{
    HT_ITEM *arr            = ht->arr;
    size_t table_size       = ht->size;
    size_t prime            = ht->prime;
    uint_least64_t ph       = prehash(key);
    size_t h1               = hash1(table_size, ph);
    size_t h2               = hash2(prime, ph);
    HT_ITEM *ret            = NULL;
    
    ht_traverse_cond_fp place_found = ht_traverse_jump_table[mode];
    
    size_t i = 0;
    do {
        size_t h    = (h1 + i * h2) % table_size;
        char cond   = place_found(ht, key, h);

        if      ( found == cond) {
            ret = &(arr[h]);
            break;
        }
        else if ( fail_and_return_null == cond) {
            ret = NULL;
            break;
        }
        else {
            ;   
        }
        
        ++i;    
    } while(1);
        
    return ret;
}

static char
if_found_for_insert(HT ht, char *key, size_t i)
{
    HT_ITEM item = ht->arr[i];
    if      (item == NULL || item == deleted_item)
        return found;
    else if (0 == strcmp(item->key, key))
        return fail_and_return_null;
    return n_found;
}

static char
if_found_for_delete(HT ht, char *key, size_t i)
{
    HT_ITEM item = ht->arr[i];
    if      (item == NULL)
        return fail_and_return_null;
    else if (item == deleted_item || 0 != strcmp(item->key, key))
        return n_found;
    return found;
}

static char
if_found_for_search(HT ht, char *key, size_t i)
{
    HT_ITEM item = ht->arr[i];
    if      (item == NULL)
        return fail_and_return_null;
    else if (item == deleted_item || 0 != strcmp(item->key, key))
        return n_found;
    return found;    
}

static size_t
hash1(size_t size, uint_least64_t prehash)
{
    return prehash % size; 
}

static size_t
hash2(uint_least64_t prime, uint_least64_t prehash)
{
    return prime - (prehash % prime);
}

static uint_least64_t
prehash(char *key)
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

    return sum;
}

/*
 * MISCELANEOUS
 */
 
/* MISCELANEOUS EXTERNAL IMPLEMENTATIONS */

unsigned int seed;
char **
fill_ht_with_r_d(HT ht, size_t size, int32_t min, int32_t max)
{
    if(!seed)   init_rand();
    
    if(max==0)  max = size;
    
    char **keyarr = calloc(size, sizeof(*keyarr));
    size_t index = 0;
    
    for(size_t i = 0; i < size; ++i) {
        int d_n   = get_rand_num(min, max);
        int key_n = ( (size_t)pow(get_rand_num(min, max), i) << sizeof(max) / 2) | d_n>>2;
        
        #ifdef DEBUG
            printf("\nrand num is %d for key and %d for data\n", key_n, d_n);
        #endif

        int32_t key_len = snprintf(NULL, 0, "%d", key_n) + 1;
        char *key       = calloc(1, key_len);
        snprintf(key, key_len, "%d", key_n);
        
        int32_t d_len   = snprintf(NULL, 0, "%d", d_n) + 1;
        char *data      = calloc(1, d_len);
        snprintf(data, d_len, "%d", d_n);

        #ifdef DEBUG
            printf("key is %s and data is %s\n\n", key, data);
        #endif
        
        char ret = insert_ht_item(ht, data, key);    
        if(ret == -1) {
            free(key);
            free(data);
        }
        else {
            keyarr[index] = key;
            index++;
        }
    }
    
    return keyarr;
}

/* MISCELANEOUS INTERNAL IMPLEMENTATIONS */

static char
is_prime(uint64_t x)
{
    for (uint64_t i = 3; i < x; i += 2)
    {
        uint64_t p_sqrt = x / i;
        if (p_sqrt < i)
            return 1;
        if(p_sqrt * i == x)
            return 0;
    }
    return 1;
}

static uint64_t
next_prime(uint64_t x)
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
