#include <inttypes.h>


#define	PQ_DATA		struct pq_data
#define PQ_DATA_P	PQ_DATA *
#define	PQ			struct pq
#define	PQ_P		PQ*
#define key_t		uint32_t
#define PRIKEY		PRIu32
#define	MIN_PRIOR	0
#define DEF_PQ_SIZE	100

struct pq_data;
struct pq;


#define		new_pq(pq, size)	PQ_P pq = init_pq((size), NULL)
PQ_P		init_pq				(size_t size, PQ_DATA_P *pq_a);

PQ_P		build_max_heap		(PQ_DATA_P *array, size_t arr_len);

PQ_P		insert_into_max_heap(PQ_P pqp, void *data, size_t data_size, key_t priority);
PQ_DATA_P	peek_max_pq			(PQ_P pqp);

char		is_empty			(PQ_P pqp);


PQ_P		max_heapify		(PQ_P pqp, size_t index);
