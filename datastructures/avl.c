#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "avl.h"


/**********************************
 * INTERNAL INTERFACE DESCRIPTION *
 **********************************/

/*
 * NODE INTERNAL INTERFACE
 */
AVL_NODE_P	create_node		(int32_t d);
void     	set_height		(AVL_NODE_P np);
uint32_t	get_height		(AVL_NODE_P np);
int32_t		recalc_height	(AVL_NODE_P np);
void		set_as_parent	(AVL_NODE_P np);
int32_t 	get_balance		(AVL_NODE_P np);
AVL_NODE_P	handle_balance	(AVL_NODE_P np);
/*
 * TREE INTERNAL INTERFACE
 */
AVL_TREE	init_avl_tree	(AVL_TREE tree);
AVL_NODE_P	insert			(int32_t d, AVL_NODE_P np);
AVL_NODE_P	right_rotate	(AVL_NODE_P np);
AVL_NODE_P	left_rotate		(AVL_NODE_P np);
AVL_NODE_P	find_max		(AVL_NODE_P np);
AVL_NODE_P	find_min		(AVL_NODE_P np);
void		delete_tree_do	(AVL_NODE_P np);
AVL_NODE_P	pop_node_do(AVL_TREE tree, AVL_NODE_P np, int32_t rec_level);
void		handle_tree_recalc(AVL_NODE_P start, AVL_NODE_P end, AVL_TREE tree);
/*
 * MISCELANEOUS INTERNAL INTERFACE
 */
enum print_tree_flags {PLAIN, VERBOSE};
void		print_do		(AVL_NODE_P np, int32_t flag);
int32_t		invariant_holds	(AVL_NODE_P np);


/*******************
 * IMPLEMENTATIONS *
 *******************/

/*
 * NODE
 */

struct node {
	int32_t			data;
	int32_t 		height;
	AVL_NODE_P 		left;
	AVL_NODE_P		right;
	AVL_NODE_P		parent;
};

AVL_NODE_P 
create_node(int32_t d)
{
	#ifdef DEBUG
		printf("Creating node...\n");
	#endif
	AVL_NODE_P np = calloc(1, sizeof(*np));
	if(!np) {
		perror("create_node()");
		exit(EXIT_FAILURE);
	}
	np->data = d; np->left = np->right = np->parent = NULL; 
	return np;
}

void 
set_height(AVL_NODE_P np)
{
	int32_t lh = np->left  ? np->left->height  : -1;
	int32_t rh = np->right ? np->right->height : -1;	
	np->height = 1 + (lh > rh ? lh : rh);
}

uint32_t 
get_height(AVL_NODE_P np)
{
	return np->height;
}

int32_t
recalc_height(AVL_NODE_P np)
{
	if(!np) 	 return -1;
	int32_t lh = recalc_height(np->left);
	int32_t rh = recalc_height(np->right);
	return np->height = 1 + (lh > rh ? lh : rh);
}

void
set_as_parent(AVL_NODE_P np)
{
	if(!np)			return;
	if(np->left)	np->left->parent  = np;
	if(np->right)	np->right->parent = np;
}

int32_t
get_balance(AVL_NODE_P np)
{
	return (np->left ? np->left->height : -1) - (np->right ? np->right->height : -1);
}

AVL_NODE_P
handle_balance(AVL_NODE_P np)
{
	int32_t b = get_balance(np);
	#ifdef DEBUG
		printf("balance is - %" PRId32 "\n", b);
	#endif

	if 		( b >  1 ) {np = right_rotate(np); recalc_height(np); set_as_parent(np);}
	else if ( b < -1 ) {np = left_rotate(np);  recalc_height(np); set_as_parent(np);}

	return np;
}


/*
 * TREE
 */

/* EXTERNAL TREE INTERFACE IMPLEMENTATION */

void
insert_node(AVL_TREE tree, int32_t d)
{
	*tree = insert(d, *tree);
}

void
delete_node(AVL_TREE tree, AVL_NODE_P np)
{
	free(pop_node(tree, np));
}

void
delete_tree(AVL_TREE tree)
{
	delete_tree_do(*tree);
	*tree = NULL;
}

AVL_NODE_P
pop_node(AVL_TREE tree, AVL_NODE_P np)
{
	return pop_node_do(tree, np, 0);
}

AVL_NODE_P 
get_max(AVL_TREE tree)
{
	return find_max(*tree);
}

AVL_NODE_P 
get_min(AVL_TREE tree)
{
	return find_min(*tree);
}

AVL_NODE_P
get_node(AVL_TREE tree, int32_t key)
{
	AVL_NODE_P np = *tree;
	while(np) {
		if		(key < np->data)	np = np->left;
		else if	(key > np->data) 	np = np->right;
		else						break;
	}
	return np;
}

AVL_NODE_P 
find_next_larger(AVL_NODE_P np)
{
	if(np->right)	return find_min(np->right);	

	AVL_NODE_P	tmp_p = np;
	int32_t	    tmp_d  = np->data;
	
	while(tmp_p->parent) {
		tmp_p = tmp_p->parent;
		if(tmp_p->data > tmp_d)	return tmp_p;
	}

	return np;
}

AVL_NODE_P 
find_next_lesser(AVL_NODE_P np)
{
	if(np->left)	return find_max(np->left);	

	AVL_NODE_P	tmp_p = np;
	int32_t     tmp_d  = np->data;
	
	while(tmp_p->parent) {
		tmp_p = tmp_p->parent;
		if(tmp_p->data < tmp_d)	return tmp_p;
	}

	return np;
}

/* INTERNAL TREE INTERFACE IMPLEMENTATION */

AVL_TREE
init_avl_tree(AVL_TREE tree)
{
	/* allocates memory for initial AVL_NODE_P.
	 * just want to have one function to init
	 * new tree, and macro is not an option as
	 * do-while loop hides a tree's keyword. */
	AVL_TREE root = calloc(1, sizeof(*root));
	return root;
}

AVL_NODE_P 
insert(int32_t d, AVL_NODE_P np)
{
	#ifdef DEBUG
		printf("Entering insert...\n");
	#endif
	if		(!np)			np		  = create_node(d);	
	else if	(d < np->data)	np->left  = insert(d, np->left);
	else if	(d > np->data) 	np->right = insert(d, np->right);	
	else					;
	
	#ifdef DEBUG
		printf("insert() after first if - data is %" PRId32 "\n", np->data);
	#endif

	set_height(np);
	set_as_parent(np);
	np = handle_balance(np);

	#ifdef DEBUG
		int32_t b = get_balance(np);  
		printf("balance after rotation is - %" PRId32 "\n", b);
		printf("insert() end  - data is %" PRId32 "\n", np->data);
	#endif
	
	return np;
};

AVL_NODE_P 
right_rotate(AVL_NODE_P np)
{
	#ifdef DEBUG
		puts("Entering right_rotate...");
	#endif
	
	if(get_balance(np->left) < 0)
		np->left = left_rotate(np->left);
	
	AVL_NODE_P tmp	= np->left;
	np->left		= tmp->right;
	tmp->right		= np;
	tmp->parent		= np->parent; 
	np->parent		= tmp;
	set_as_parent(tmp);
	set_as_parent(np);
	
	return tmp;
};	

AVL_NODE_P 
left_rotate(AVL_NODE_P np)
{
	#ifdef DEBUG
		puts("Entering left_rotate...");
	#endif

	if(get_balance(np->right) > 0)
		np->right = right_rotate(np->right);
		
	AVL_NODE_P tmp	= np->right;
	np->right		= tmp->left;
	tmp->left		= np;
	tmp->parent		= np->parent;
	np->parent		= tmp;
	set_as_parent(tmp);
	set_as_parent(np);
	
	return tmp;
};

AVL_NODE_P 
find_max(AVL_NODE_P np)
{
	if(!np->right)	return np;
	else			return find_max(np->right);
}

AVL_NODE_P 
find_min(AVL_NODE_P np)
{
	if(!np->left)	return np;
	else			return find_min(np->left);
}

void
delete_tree_do(AVL_NODE_P np)
{
	if(!np)	return;
	
	delete_tree_do(np->left);
	delete_tree_do(np->right);
	free(np);
}

AVL_NODE_P 
pop_node_do(AVL_TREE tree, AVL_NODE_P np, int32_t rec_level)
{
	AVL_NODE_P np_par			= np->parent;
	AVL_NODE_P node_instead_np	= NULL;
	AVL_NODE_P next_larger		= NULL;
	AVL_NODE_P node_to_start_recalc_from = np_par;
	AVL_NODE_P node_to_end_recalc_at     = NULL;

	if     (!np->left && !np->right) {	
		node_instead_np = NULL;
	}
	else if(np->left && !np->right) {
		node_instead_np			= np->left;
		node_instead_np->parent = np_par;
	}
	else if(!np->left && np->right) {
		node_instead_np			= np->right;
		node_instead_np->parent = np_par;
	}
	else {
		next_larger		= find_next_larger(np);
		node_instead_np = create_node(0);
		memcpy(node_instead_np, next_larger, sizeof(node_instead_np));
		node_instead_np->left	= np->left;	
		node_instead_np->right	= np->right;	

		node_to_start_recalc_from = next_larger->parent;
	}

	set_as_parent(node_instead_np);

    if		(!np_par)					*tree	      = node_instead_np;
	else if	(np->data < np_par->data)	np_par->left  = node_instead_np;
	else								np_par->right = node_instead_np;	

    
	/* no way next_larger can be root, so won't bother with a NULL parent */
	if    	(next_larger)		free(pop_node_do(tree, next_larger, rec_level + 1));

	if		(rec_level != 0)	node_to_end_recalc_at = np_par;

	#ifdef DEBUG
		printf("recursion level is %" PRId32 "\n", rec_level);
	#endif
	
	handle_tree_recalc(node_to_start_recalc_from, node_to_end_recalc_at, tree);

	np->parent = np->left = np->right = NULL;
	return np;
}

void
handle_tree_recalc(AVL_NODE_P start, AVL_NODE_P end, AVL_TREE tree)
{
	#ifdef DEBUG
		printf("Entering handle_tree_recalc...\n");
	#endif
	
	AVL_NODE_P cur_np = start;
	while(cur_np != end) {
		AVL_NODE_P parent = cur_np->parent;
		set_height(cur_np);
		cur_np = handle_balance(cur_np);
		if		(!parent)						*tree	      = cur_np;
		else if	(cur_np->data < parent->data)	parent->left  = cur_np;
		else									parent->right = cur_np;	
		cur_np = parent;
	}
}


/*
 * MISCELANEOUS
 */
 
/* PRINT TREE */
 
void
print_tree(AVL_TREE tree, int32_t flag)
{
	if(!*tree) {
		puts("No tree to print!");
		return;
	}
	puts("Printing tree..."); 
	print_do(*tree, flag); 
	puts("");
}


void
print_do(AVL_NODE_P np, int32_t flag)
{
	if(!np) { return; }
	
	print_do(np->left, flag);
	
	printf("%" PRId32 " ", np->data);
	
	if(flag == VERBOSE) {
		printf("next min is %" PRId32 " and next max is %" PRId32 " ", 
				find_next_lesser(np)->data, find_next_larger(np)->data);
		if(np->parent)	printf("parent data is %" PRId32 "\n", np->parent->data);
		else			printf("root\n");
	}
	
	print_do(np->right, flag);
};

/* INVARIANT CHECK */

void
check_invariant(AVL_TREE tree)
{
	if(invariant_holds(*tree))	puts("INVARIANT HOLDS!");
	else						puts("INVARIAN DOES NOT HOLD!");		
}

int32_t
invariant_holds(AVL_NODE_P np)
{
	if(!np)				return 1;
	
	int32_t b = get_balance(np);
	if(b > 1 || b < -1) return 0;

	int32_t li = invariant_holds(np->left);
	int32_t ri = invariant_holds(np->right);

	if(li && ri)		return 1;
	
	return 0;	
}

/* RANDOM FUNCTION */

/* using a normal random function is not a purporse of
 * this educational module. don't use it in practice */
unsigned int seed;
void
init_rand() 
{
	seed = time(NULL);
}

int32_t
get_rand_num(int32_t l, int32_t h)
{
	return l + (unsigned int)rand_r(&seed) % h;
}






int main(void)
{
	init_rand();
	new_avl_tree(tree);

	uint32_t inp_nums[INPUT_SIZE];
	/* one good input set */
	//uint32_t inp_nums[] = {3, 66, 84, 87, 58, 1, 26, 52, 14, 37};
	for(size_t i = 0; i < INPUT_SIZE; ++i) {
		inp_nums[i] = get_rand_num(INPUT_MIN, INPUT_MAX);
		printf("rand num is %" PRIu32 "\n", inp_nums[i]);
		uint32_t n = inp_nums[i];
		insert_node(tree, n);
	}


	print_tree(tree, VERBOSE);
	printf("%" PRId32 "\n", (*tree)->data);
	check_invariant(tree);
	printf("%" PRId32 " %" PRId32 "\n", get_max(tree)->data, get_min(tree)->data);

	AVL_NODE_P  min = get_min(tree);
	delete_node(tree, min);
	print_tree(tree, PLAIN);
	check_invariant(tree);
		
	AVL_NODE_P  max = get_max(tree);
	delete_node(tree, max);
	print_tree(tree, PLAIN);
	check_invariant(tree);

	delete_node(tree, *tree);
	print_tree(tree, PLAIN);
	check_invariant(tree);
	
	AVL_NODE_P np = get_node(tree, 14);
	printf("Data is %" PRId32 "\n", np ? np->data : -1);
	np = get_node(tree, 97);
	printf("Data is %" PRId32 "\n", np ? np->data : -1);
	np = get_node(tree, 58);
	printf("Data is %" PRId32 "\n", np ? np->data : -1);
	
	delete_tree(tree);
	print_tree(tree, PLAIN);
}
