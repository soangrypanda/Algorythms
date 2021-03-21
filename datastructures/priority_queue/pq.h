#include <inttypes.h>
#include <stddef.h>


typedef	struct pq_data  PQ_DATA;
typedef	PQ_DATA *       PQ_DATA_P;
typedef	struct pq       PQ;
typedef	PQ*             PQ_P;
typedef	uint32_t        pri_t;
#define	PRIPRI          PRIu32
#define	MIN_PRIOR       0


#define     new_pq(pq, size)    PQ_P pq = init_pq((size), NULL)
PQ_P        init_pq             (size_t size, PQ_DATA_P *pq_a);
#define     delete_pq(pq)       do { delete_pq_do(pq); pq = NULL; } while(0)
void        delete_pq_do        (PQ_P pqp);

PQ_P        insert_into_max_heap(PQ_P pqp, void *data, size_t data_size, pri_t priority);
PQ_P        build_max_heap      (PQ_DATA_P *array, size_t arr_len);
PQ_DATA_P   peek_pq             (PQ_P pqp);
PQ_DATA_P   pull_high_elem      (PQ_P pqp);
PQ_P        build_max_heap      (PQ_DATA_P *array, size_t arr_len);
PQ_DATA_P*  heap_sort			(PQ_DATA_P *array, size_t arr_len);

void        print_pq            (PQ_P pqp);

void        insert_random_data  (PQ_P pqp, size_t size, int32_t min, int32_t max);
PQ_DATA_P*  get_random_pqpdata_arr(size_t size, int32_t min, int32_t max);
