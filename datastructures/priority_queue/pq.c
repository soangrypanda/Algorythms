#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pq.h"

struct pq_data {
    void*   data;
    size_t  data_size;
    pri_t   priority;
};

struct pq {
    PQ_DATA_P   *pq;
    size_t      pq_size;
    size_t      last_free;
};

enum {  DEF_PQ_SIZE     = 100, 
        REALLOC_STRIDE  = 3
};

/**********************************
 * INTERNAL INTERFACE DESCRIPTION *
 **********************************/

/*
 * PQ DATA
 */
static PQ_DATA_P    create_data     (void *data, size_t data_size, pri_t priority);
/*
 * PQ
 */
static void         realloc_pq      (PQ_P pqp);
static PQ_DATA_P*   heap_sort_do    (PQ_P pqp);
static PQ_P         max_heapify     (PQ_P pqp, size_t index);
/*
 * MISCELANEOUS INTERNAL INTERFACE
 */
static int32_t      get_rand_num    (int32_t l, int32_t h);
static void         init_rand       (void);


/*
 * PQ DATA
 */
 
 /* PQ DATA INTERNAL IMPLEMENTATIONS */
 
static PQ_DATA_P
create_data(void *data, size_t data_size, pri_t priority)
{
	PQ_DATA_P data_p = calloc(1, sizeof(*data_p));
	if(NULL == data_p) {
		perror("create_data");
		exit(EXIT_FAILURE);
	}
	
	data_p->data        = data;
	data_p->data_size   = data_size;
	data_p->priority    = priority;
	
	return data_p;
}


/*
 * PQ
 */

/* PQ EXTERNAL IMPLEMENTATIONS */

PQ_P		
init_pq (size_t size, PQ_DATA_P *pq_a)
{
	PQ_P pq	= calloc(1, sizeof(*pq));
	if(NULL == pq) {
		perror("init_pq");
		exit(EXIT_FAILURE);
	}
	
	size_t last_free = size;
	if(pq_a == NULL || size == 0) {
		if(size == 0)   size = DEF_PQ_SIZE;
		
		pq_a = calloc(size, sizeof(*pq_a));
		if(NULL == pq_a) {
			perror("init_pq");
			exit(EXIT_FAILURE);
		}
		
		last_free = 0;
	}
	
	pq->pq          = pq_a;
	pq->pq_size     = size;
	pq->last_free   = last_free;
	
	return pq;
}

void
delete_pq_do(PQ_P pqp)
{
	PQ_DATA_P *pq_a     = pqp->pq;
	size_t len          = pqp->last_free;
	
	for(size_t i = 0; i < len; ++i) {
		free(pq_a[i]->data);
		free(pq_a[i]);
	}
	
	free(pq_a);
	free(pqp);
}

PQ_P
insert_into_max_heap(PQ_P pqp, void *data, size_t data_size, pri_t priority)
{
    #ifdef DEBUG
        printf("\n   ENTERING insert_into_max_heap\n");
    #endif

	size_t size         = pqp->pq_size;
	size_t lf           = pqp->last_free;

	if(lf >= size)      realloc_pq(pqp);
	
	PQ_DATA_P *pq_arr   = pqp->pq;
	pq_arr[lf]          = create_data(data, data_size, priority);
	lf                  = ++pqp->last_free;
	
	if(lf == 1)	return pqp;
	
	size_t current	= lf - 1;
	for(size_t parent = (current - 1) / 2; parent >= 0 && parent < lf; ) {
        #ifdef DEBUG
            printf("right before if statement\n");
        #endif

        if(pq_arr[parent]->priority < priority) {
            #ifdef DEBUG
                printf("current is %zu and parent is %zu\n", current, parent);
            #endif

			PQ_DATA_P tmp   = pq_arr[parent];
			pq_arr[parent]  = pq_arr[current];
			pq_arr[current] = tmp;
			current         = parent;
			parent          = (current - 1) / 2;

            #ifdef DEBUG
                printf("end of if statement\n");
            #endif
		}
		else {
			break;
		}
	}
	return pqp;
}

PQ_DATA_P
peek_pq(PQ_P pqp) 
{
	return pqp->pq[0];
}

PQ_DATA_P
pull_high_elem(PQ_P pqp)
{
	PQ_DATA_P *pq_a = pqp->pq;
	size_t last_e_i = pqp->last_free - 1;
	PQ_DATA_P ret   = pq_a[0];
	
	pq_a[0]         = pq_a[last_e_i];
	pq_a[last_e_i]  = NULL;
	pqp->last_free--;
	
	max_heapify(pqp, 0);
	
	return ret;
}

PQ_P
build_max_heap(PQ_DATA_P *array, size_t arr_len)
{
    #ifdef DEBUG
        printf("Entering build_max_heap\n");
    #endif
	
	PQ_P pqp            = init_pq(arr_len, array);
	
	for(size_t cur_i    = (arr_len - 1) / 2; cur_i >= 0 && cur_i < arr_len; --cur_i)
		max_heapify(pqp, cur_i);
		
	return pqp;
}

PQ_DATA_P*
heap_sort(PQ_DATA_P *array, size_t arr_len)
{
	PQ_P pqp = build_max_heap(array, arr_len);
	return heap_sort_do(pqp);
}

/* PQ DATA INTERNAL IMPLEMENTATIONS */

static void
realloc_pq(PQ_P pqp)
{
    #ifdef DEBUG
        printf("\n\nENTERING realloc_pq\n\n");
    #endif

    PQ_DATA_P *tmp_arr  = pqp->pq;
    size_t new_size     = pqp->pq_size * REALLOC_STRIDE;

    tmp_arr             = reallocarray(tmp_arr, new_size, sizeof(*tmp_arr));   
    if(NULL == tmp_arr) {
        perror("realloc_pq");
        exit(EXIT_FAILURE);
    }

    pqp->pq             = tmp_arr;
    pqp->pq_size        = new_size; 
}

static PQ_DATA_P*
heap_sort_do(PQ_P pqp)
{
    #ifdef DEBUG
        printf("Entering heap_sort\n");
    #endif
	
	PQ_DATA_P *pq_a	= pqp->pq;
	size_t lf       = pqp->last_free;
	
	while(lf != 0) {
		size_t last_i   = lf - 1;
		PQ_DATA_P tmp   = pq_a[0];
		pq_a[0]         = pq_a[last_i];
		pq_a[last_i]    = tmp;
		
		lf              = --pqp->last_free;
		
		max_heapify(pqp, 0);	
	}
	
	free(pqp);
	return pq_a;
}

static PQ_P
max_heapify(PQ_P pqp, size_t index)
{
    #ifdef DEBUG
        printf("Entering max_heapify\n");
    #endif

	PQ_DATA_P *data_arr = pqp->pq;
	size_t size         = pqp->pq_size;
	size_t lf           = pqp->last_free;
	size_t i            = index;
	
	while(i < lf) {
        size_t ic_one   = i * 2 + 1;
        size_t ic_two   = i * 2 + 2;
		
        pri_t p_one     = ic_one < lf ? data_arr[ic_one]->priority : MIN_PRIOR;
        pri_t p_two     = ic_two < lf ? data_arr[ic_two]->priority : MIN_PRIOR;
        pri_t p_i       = data_arr[i]->priority;
		
        size_t ic_max   = (p_one >  p_two) * ic_one + 
                          (p_two >  p_one) * ic_two + 
                          (p_one == p_two) * ic_one * (ic_one < lf);
		
        #ifdef DEBUG
            printf(	"i is %zu and lf is %zu and ic_max is %zu "
                    "and ic_one is %zu(%"PRIPRI") and ic_two is %zu(%"PRIPRI")\n", 
                    i, lf, ic_max, ic_one, p_one, ic_two, p_two);
        #endif
		
        if(ic_max != 0 && data_arr[ic_max]->priority > p_i) {
            PQ_DATA_P tmp       = data_arr[ic_max];
            data_arr[ic_max]    = data_arr[i];
            data_arr[i]         = tmp;
            i                   = ic_max;
        }
        else {
            break;
        }	
    }
	
    return pqp;
}


/*
 * MISCELANEOUS IMPLEMENTATIONS
 */


/* PRINT PQ */

void
print_pq(PQ_P pqp)
{
	if(pqp == NULL) {
		printf("No PQ to print!\n");
		return;
	}
	
	printf("Printing PQ...\n");
	
	PQ_DATA_P *data_arr = pqp->pq;
	size_t size         = pqp->last_free;
	size_t cur_level    = 1;
	size_t next_level   = cur_level * 2;

	for(size_t i = 0; i < size; ++i) {
		if(cur_level == next_level)	{
			printf("\n");
			next_level *= 2;
		}
		
		printf("%" PRIPRI " ", data_arr[i]->priority);
		cur_level++;	
	}
	
	
	printf("\n");
}

/* RANDOM FUNCTION */

/* using a normal random function is not a purporse of
 * this educational module. don't use it in practice */
unsigned int seed;

void
insert_random_data(PQ_P pqp, size_t size, int32_t min, int32_t max)
{
	if(!seed) init_rand();
	
	if(max==0) max = size;
	
	size_t input_size   = size;
	pri_t *inp_nums     = calloc(1, input_size * sizeof(*inp_nums));
	
	/* one good input set */
	//int32_t inp_nums[] = {3, 66, 84, 87, 58, 1, 26, 52, 14, 37};
	//uint32_t input_size = 10;
	for(size_t i = 0; i < input_size; ++i) {
		inp_nums[i]     = get_rand_num(min, max);
		pri_t priority  = inp_nums[i];
		
        #ifdef DEBUG
            printf("rand num is %" PRIPRI "\n", priority);
        #endif

		int32_t dummy_len   = snprintf(NULL, 0, "%" PRIPRI, priority) + 1;
		char *	dummy       = calloc(1, dummy_len);
		snprintf(dummy, dummy_len, "%" PRIPRI, priority);
		
        #ifdef DEBUG
            printf("dummy data is %s\n", dummy);
        #endif
		
		insert_into_max_heap(pqp, dummy, strlen(dummy)+1, priority);
	}
	free(inp_nums);
}

PQ_DATA_P*
get_random_pqpdata_arr(size_t size, int32_t min, int32_t max)
{
	if(!seed) init_rand();
	
	if(max==0) max = size;
	
	PQ_DATA_P *array    = calloc(size, sizeof(*array));
	
	for(size_t i = 0; i < size; ++i) {
		pri_t priority  = get_rand_num(min, max);
		
        #ifdef DEBUG
            printf("rand num is %" PRIPRI "\n", priority);
        #endif

		int32_t dummy_len   = snprintf(NULL, 0, "%" PRIPRI, priority) + 1;
		char *	dummy       = calloc(1, dummy_len);
		snprintf(dummy, dummy_len, "%" PRIPRI, priority);
		
        #ifdef DEBUG
            printf("dummy data is %s\n", dummy);
        #endif
		
		array[i]            = create_data(dummy, dummy_len, priority);
	}
	
	return array;
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
