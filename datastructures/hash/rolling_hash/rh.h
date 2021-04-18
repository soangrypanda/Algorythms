#ifndef RH_H_SENTRY
#define RH_H_SENTRY

#include <math.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>


typedef     struct rh_s     RH_S;
typedef     RH_S *          RH;


#define     new_rh(rh, base, mod) RH rh = init_rh( (base), (mod) )
RH          init_rh             (uint64_t base, uint64_t mod);
#define     delete_rh(rh)       rh = delete_rh_do(rh)
RH          delete_rh_do        (RH rh);

void        rh_set_pattern      (RH rh, char *pat);
void        rh_set_string       (RH rh, char *str);
char **     rh_find_pat_in_str  (char *pat, char *str, uint64_t base);
void        append_to_rh        (RH rh);
void        skip                (RH rh);
void        slide_window        (RH rh);


#endif
