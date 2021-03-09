#ifndef AVL_TREE_SENTRY
#define AVL_TREE_SENTRY


#define INPUT_SIZE	100
#define INPUT_MIN	0
#define INPUT_MAX	100

#define	AVL_NODE			struct node
#define AVL_NODE_P 			AVL_NODE *
#define AVL_TREE			AVL_NODE_P *



struct node;

#define		new_avl_tree(X)	AVL_TREE X = init_avl_tree(X)
AVL_TREE	init_avl_tree	(AVL_TREE tree);
void   		insert_node		(AVL_TREE tree, int32_t d);
void   		delete_node		(AVL_TREE tree, AVL_NODE_P np);
void		delete_tree		(AVL_TREE tree);
AVL_NODE_P	pop_node		(AVL_TREE tree, AVL_NODE_P np);
AVL_NODE_P	get_max			(AVL_TREE tree);
AVL_NODE_P	get_min			(AVL_TREE tree);
AVL_NODE_P	get_node		(AVL_TREE tree, int32_t key);
AVL_NODE_P	find_next_larger(AVL_NODE_P np);
AVL_NODE_P	find_next_lesser(AVL_NODE_P np);
/* print_tree flags are PLAIN, VERBOSE. */
void		print_tree		(AVL_TREE tree, int32_t flag);



#endif
