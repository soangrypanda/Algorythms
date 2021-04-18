#include "rh.h"

struct rh_s {
    char *           str;
    char *           pat;
    uint64_t         mod;
    uint64_t         hash;
    uint64_t         p_hash;
    uint64_t         base;
    uint64_t         b2wmodm;
    uint64_t         m_inv;
    char             old, new;
};

/* INTERNAL INTERFACE DESCRIPTION */
static uint64_t    multiplicative_inverse   (uint64_t what, uint64_t what_of);
static uint64_t    gcd_extended             (uint64_t a, uint64_t b, uint64_t* x, uint64_t* y);
static char        is_prime                 (uint64_t x);
static uint64_t    next_prime               (uint64_t x);
static uint64_t    get_mod                  (void);


/* EXTERNAL INTERFACE IMPLEMENTATIONS */

RH
init_rh(uint64_t base, uint64_t mod)
{
    RH_S *rh_p  = calloc(1, sizeof(*rh_p));
    if(rh_p == NULL) {
        perror("init_rh");
        exit(EXIT_FAILURE);
    }

    rh_p->base      = base;
    rh_p->mod       = mod;
    rh_p->hash      = 0;
    rh_p->p_hash    = 0;
    rh_p->b2wmodm   = 1; 
    rh_p->m_inv     = multiplicative_inverse(base, mod);
    if(rh_p->m_inv == -1) {
        free(rh_p);
        return NULL;
    }
    
    rh_p->str = rh_p->pat = NULL;
    rh_p->old = rh_p->new = 0;
    
    return rh_p;
}

RH
delete_rh_do(RH rh)
{
    free(rh);
    return NULL;
}

void
rh_set_pattern(RH rh, char *pat)
{
    size_t len = strlen(pat);
    rh->pat    = pat;
    for(size_t i = 0; i < len; ++i) {
        rh->new = pat[i];
        append_to_rh(rh);
    }
    rh->p_hash  = rh->hash;
    rh->hash    = 0;
    rh->b2wmodm = 1;
}

void
rh_set_string(RH rh, char *str)
{
    rh->str    = str;
}

char **
rh_find_pat_in_str(char *pat, char *str, uint64_t base)
{
    if(base == 0)   base = 10;
    
    new_rh(rh, base, get_mod());
    if(rh == NULL) {
        perror("rh_find_pat_in_str() - rh not initialized");
        return NULL;
    }   
    
    rh_set_pattern(rh, pat);
    rh_set_string(rh, str);
    uint64_t p_hash = rh->p_hash;
    size_t patlen   = strlen(pat);
    size_t str_len  = strlen(str);
    
    size_t matches  = 0;
    size_t matchlen = patlen * 2;
    char **matcharr = calloc(matchlen, sizeof(*matcharr));
    assert(matcharr != NULL);
    
    size_t i = 0;
    for(; i < patlen; ++i) {
        rh->new = str[i];
        append_to_rh(rh);
    }
    if(p_hash == rh->hash && 0 == strncmp(pat, &str[i - patlen], patlen)) {
        matcharr[matches] = &str[i - patlen];
        matches++;
    }
   
    for(; i < str_len; ++i) {
        rh->old = str[i-patlen];
        rh->new = str[i];
        append_to_rh(rh);
        skip(rh);
        if(p_hash == rh->hash && 0 == strncmp(pat, &str[i - patlen+1], patlen)) {
            matcharr[matches] = &str[i - patlen+1];
            matches++;
            if(matches == matchlen - 2) {
                matchlen *= 2;
                size_t new_len = matchlen * sizeof(*matcharr);
                matcharr = realloc(matcharr, new_len);
                assert(matcharr != NULL);
            }
        }
    }
    matcharr[matches] = NULL;
    
    delete_rh_do(rh);
    
    return matcharr;
}

void
append_to_rh(RH rh)
{
    uint64_t hash    = rh->hash;
    uint64_t base    = rh->base;
    uint64_t mod     = rh->mod;
    uint64_t b2wmodm = rh->b2wmodm;
    char     new     = rh->new;
    
    hash             = ( hash * base + new ) % mod;    
    b2wmodm          = ( b2wmodm * base ) % mod;
    
    rh->hash    = hash;
    rh->b2wmodm = b2wmodm;
}

void
skip(RH rh)
{
    uint64_t hash       = rh->hash;
    uint64_t base       = rh->base;
    uint64_t mod        = rh->mod;
    uint64_t b2wmodm    = rh->b2wmodm;
    uint64_t m_inv      = rh->m_inv;
    char     old        = rh->old;
    uint64_t correction = old * mod;
   
    b2wmodm = ( b2wmodm * (m_inv % mod) ) % mod;
    hash    = ( hash - old * b2wmodm + correction ) % mod;

    rh->hash    = hash;
    rh->b2wmodm = b2wmodm;
}

void
slide_window(RH rh)
{
    uint64_t base    = rh->base;
    uint64_t mod     = rh->mod;
    uint64_t hash    = rh->hash;
    uint64_t b2wmodm = rh->b2wmodm;
    char old = rh->old;
    char new = rh->new;

    hash     = ( hash * base - old * b2wmodm + new ) % mod;
    rh->hash = hash;
}


/* INTERNAL INTERFACE IMPLEMENTATIONS */

static uint64_t
multiplicative_inverse(uint64_t what, uint64_t what_of)
{
    uint64_t x, y;
    uint64_t g = gcd_extended(what, what_of, &x, &y);
    if (g != 1)
        return -1;
        
    uint64_t res = (x % what_of + what_of) % what_of;
    return res;
}

static uint64_t
gcd_extended(uint64_t a, uint64_t b, uint64_t* x, uint64_t* y)
{
    if (a == 0)
    {
        *x = 0, *y = 1;
        return b;
    }
 
    uint64_t x1, y1;
    uint64_t gcd = gcd_extended(b % a, a, &x1, &y1);
 
    *x = y1 - (b / a) * x1;
    *y = x1;
 
    return gcd;
}

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

static uint64_t
get_mod(void)
{
    /* so that would probably be a good idea to hardcode different
     * primes large enough to be used in different systems, if needed. */
    
    return 14316593;
}
