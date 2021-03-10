#ifndef AVL_TREE_SENTRY
#define AVL_TREE_SENTRY


#include <inttypes.h>
#include <stddef.h>

#define	AVL_NODE			struct node
#define AVL_NODE_P 	  AVL_NODE *
#define AVL_TREE			AVL_NODE_P *
#define key_t			    int32_t
#define PRIKEY				PRId32

struct node;

enum print_tree_flags {PLAIN, VERBOSE};

#define		  new_avl_tree(X)	  AVL_TREE X = init_avl_tree(X)
AVL_TREE	  init_avl_tree	    (AVL_TREE tree);
void   		  insert_node		    (AVL_TREE tree, key_t key, void *data, 
							                uint32_t data_size, char *inserted);
void   		  delete_node		    (AVL_TREE tree, AVL_NODE_P np);
AVL_TREE	  delete_tree		    (AVL_TREE tree);
AVL_NODE_P	pop_node		      (AVL_TREE tree, AVL_NODE_P np);
AVL_NODE_P	get_max			      (AVL_TREE tree);
AVL_NODE_P	get_min			      (AVL_TREE tree);
AVL_NODE_P	get_node		      (AVL_TREE tree, key_t key);
AVL_NODE_P	find_next_larger  (AVL_NODE_P np);
AVL_NODE_P	find_next_lesser  (AVL_NODE_P np);
void		    print_tree		    (AVL_TREE tree, int32_t flag);
void		    check_invariant	  (AVL_TREE tree);
void		    insert_random_data(AVL_TREE tree, size_t size, int32_t min, int32_t max);


#endif
