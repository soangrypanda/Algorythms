#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "pq.h"

struct pq_data {
	void*	data;
	size_t	data_size;
	key_t	priority;
};

struct pq {
	PQ_DATA_P	*pq;
	size_t		pq_size;
	size_t		last_free;
};



PQ_DATA_P
create_data(void *data, size_t data_size, key_t priority)
{
	PQ_DATA_P data_p = calloc(1, sizeof(*data_p));
	if(NULL == data_p) {
		perror("create_data");
		exit(EXIT_FAILURE);
	}
	
	data_p->data		= data;
	data_p->data_size	= data_size;
	data_p->priority	= priority;
	
	return data_p;
}





PQ_P		
init_pq (size_t size)
{
	if(size == 0)	size = DEF_PQ_SIZE;
	
	PQ_DATA_P	*pq_a = calloc(size, sizeof(*pq_a));
	if(NULL == pq_a) {
		perror("init_pq");
		exit(EXIT_FAILURE);
	}
	
	PQ_P		pq   = calloc(1, sizeof(*pq));
	if(NULL == pq) {
		perror("init_pq");
		exit(EXIT_FAILURE);
	}
	
	pq->pq			= pq_a;
	pq->pq_size		= size;
	pq->last_free	= 0;
	
	return pq;
}

PQ_P
insert_into_max_heap(PQ_P pqp, void *data, size_t data_size, key_t priority)
{
	printf("\n   ENTERING insert_into_max_heap\n");
	PQ_DATA_P *pq_arr	= pqp->pq;
	size_t size			= pqp->pq_size;
	size_t lf			= pqp->last_free;

	//if(new_last >= size)	realloc_pq;
	
	pq_arr[lf]			= create_data(data, data_size, priority);
	lf 					= ++pqp->last_free;
	
	if(lf == 1)	return pqp;
	
	size_t current	= lf - 1;
	for(size_t parent = (current - 1) / 2; parent >= 0 && parent < lf; ) {
		printf("right before if statement\n");
		if(pq_arr[parent]->priority < priority) {
			printf("current is %zu and parent is %zu\n", current, parent);
			PQ_DATA_P tmp	= pq_arr[parent];
			pq_arr[parent]	= pq_arr[current];
			pq_arr[current]	= tmp;
			current			= parent;
			parent			= (current - 1) / 2;
			printf("end of if statement\n");
		}
		else {
			break;
		}
	}
	
	
}

PQ
build_max_heap(PQ_DATA_P array, size_t arr_len)
{
	
}

PQ
max_headpify(PQ_DATA_P dp, size_t parent)
{
	
}




/* PRINT PQ */

void
print_pq_old(PQ_P pqp)
{
	PQ_DATA_P *data_arr = pqp->pq;
	size_t size			= pqp->last_free;
	size_t cur_level	= 1;
	size_t next_level	= cur_level * 2;

	for(size_t i = 0; i < size; ++i) {
		if(cur_level == next_level)	{
			printf("\n");
			next_level *= 2;
		}
		
		printf("%" PRIKEY " ", data_arr[i]->priority);
		cur_level++;	
	}
	
	
	printf("\n");
}

void
print_pq(PQ_P pqp)
{
	PQ_DATA_P	*arr		= pqp->pq;
	size_t		elem_no		= pqp->last_free;
	uint32_t	max_level	= log2(elem_no) + 1;
	uint32_t	level		= 1;
	uint32_t	level_len	= pow(2, max_level - 1);
	uint32_t	offset		= level_len / 2 - 1;
	uint32_t	prev_offset = offset;
	
	uint32_t	elem_at_level = 1;
	
	printf("max level - %u\n", max_level);
	for(size_t i = 0; i < max_level; ++i) {
		
		for(size_t j = 0; j < offset; ++j) 
			printf(" ");
			
		
		for(size_t j = 0; j < elem_at_level; ++j) {
			//int32_t printed = printf("%" PRIKEY, arr[i+j]->priority);
			int32_t printed = printf("%s", arr[i+j]->data);
			printed = printed > 1 ? printed -= 1 : printed;
			int32_t blanks_to_print = 	prev_offset > printed ? 
										prev_offset - printed : 0;
			if(offset == 0) blanks_to_print = 1;
			for(size_t c = 0; c < blanks_to_print; ++c) 
				printf(" ");
		}
		
		prev_offset		= offset;
		elem_at_level	*= 2;
		offset			/= 2;
		printf("\n");
	}
	printf("\n");
	
	
}
/* RANDOM FUNCTION */

/* using a normal random function is not a purporse of
 * this educational module. don't use it in practice */
unsigned int seed;
static int32_t get_rand_num(int32_t l, int32_t h);
static void init_rand();

void
insert_random_data(PQ_P pqp, size_t size, int32_t min, int32_t max)
{
	if(!seed) init_rand();
	
	if(max==0) max = size;
	
	size_t input_size	= size;
	key_t *inp_nums		= calloc(1, input_size * sizeof(*inp_nums));
	
	/* one good input set */
	//int32_t inp_nums[] = {3, 66, 84, 87, 58, 1, 26, 52, 14, 37};
	//uint32_t input_size = 10;
	for(size_t i = 0; i < input_size; ++i) {
		inp_nums[i] 	= get_rand_num(min, max);
		key_t priority	= inp_nums[i];
		
		#ifdef DEBUG
			printf("rand num is %" PRIKEY "\n", key);
		#endif

		int32_t dummy_len = snprintf(NULL, 0, "%" PRIKEY, priority) + 1;
		char *	dummy	  = calloc(1, dummy_len);
		snprintf(dummy, dummy_len, "%" PRIKEY, priority);
		
		#ifdef DEBUG
			printf("dummy data is %s\n", dummy);
		#endif
		
		insert_into_max_heap(pqp, dummy, strlen(dummy)+1, priority);
	}
	free(inp_nums);
}

static void
init_rand() 
{
	seed = time(NULL);
}

static int32_t
get_rand_num(int32_t l, int32_t h)
{
	return l + rand_r(&seed) % h;
}






int main(void)
{
	new_pq(pq, 0);
	
	insert_random_data(pq, 100, 0, 100);
	
	print_pq_old(pq);
	
	
	
	
}
