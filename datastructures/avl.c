#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define INPUT_SIZE	10
#define INPUT_MIN	0
#define INPUT_MAX	100

struct node *left_rotate( struct node *np, uint32_t entry);
struct node *right_rotate(struct node *np, uint32_t entry);

struct node {
	int32_t 	data;
	int32_t 	height;
	struct node 	*left, *right, *parent;
};

struct node *
create_node(int32_t d)
{
	printf("Creating node...\n");
	struct node *np = calloc(1, sizeof(*np));
	np->data = d; np->left = np->right = np->parent = NULL; 
	return np;
};

int32_t
get_balance(struct node *np)
{
	return (np->left ? np->left->height : -1) - (np->right ? np->right->height : -1);
};

void 
set_height(struct node *np)
{
	int32_t lh = np->left  ? np->left->height  : -1;
	int32_t rh = np->right ? np->right->height : -1;	
	np->height = 1 + (lh > rh ? lh : rh);
};

int32_t
recalc_height(struct node *np)
{
	if(!np) return -1;
	int32_t lh = recalc_height(np->left);
	int32_t rh = recalc_height(np->right);
	return np->height = 1 + (lh > rh ? lh : rh);
};

uint32_t 
get_height(struct node *np)
{
	return np->height;
};

#define FIRST_ENTRY	1
#define SECOND_ENTRY	0

void
set_as_parent(struct node *np)
{
	if(!np)		return;
	if(np->left)	np->left->parent  = np;
	if(np->right)	np->right->parent = np;
}

struct node*
handle_balance(struct node *np)
{
	int32_t b = get_balance(np);  
	printf("balance is - %" PRId32 "\n", b);

	if 	( b >  1 ) {np = right_rotate(np, FIRST_ENTRY); recalc_height(np); set_as_parent(np);}
	else if ( b < -1 ) {np = left_rotate(np , FIRST_ENTRY); recalc_height(np); set_as_parent(np);}

	return np;
}

struct node *
insert(int32_t d, struct node *np)
{
	printf("Entering insert\n");
	if	(!np)		np = create_node(d);	
	else if	(d < np->data)	np->left  = insert(d, np->left);
	else if	(d > np->data) 	np->right = insert(d, np->right);	
	else			;

	printf("insert() after first if - data is %" PRId32 "\n", np->data);

	set_height(np);
	set_as_parent(np);
	np = handle_balance(np);

	int32_t b = get_balance(np);  
	printf("balance after rotation is - %" PRId32 "\n", b);


	printf("insert() end  - data is %" PRId32 "\n", np->data);
	return np;
};

struct node *
right_rotate(struct node *np, uint32_t entry)
{
	puts("ENTER right_rotate");
	//if(entry == FIRST_ENTRY && !np->left->left && np->left->right)
	if(get_balance(np->left) < 0)
		np->left = left_rotate(np->left, SECOND_ENTRY);	
	struct node *tmp = np->left;
	np->left = tmp->right;
	tmp->right = np;
	tmp->parent = np->parent; np->parent = tmp;
	set_as_parent(tmp);
	set_as_parent(np);
	return tmp;
};	

struct node *
left_rotate(struct node *np, uint32_t entry)
{
	puts("ENTER left_rotate");
	//if(entry == FIRST_ENTRY && !np->right->right && np->right->left)
	if(get_balance(np->right) > 0)
		np->right = right_rotate(np->right, SECOND_ENTRY);
	struct node *tmp = np->right;
	np->right = tmp->left;
	tmp->left = np;
	tmp->parent = np->parent; np->parent = tmp;
	set_as_parent(tmp);
	set_as_parent(np);
	return tmp;
};

struct node *
find_max(struct node* np)
{
	if(!np->right)	return np;
	else		return find_max(np->right);
}

struct node*
get_max(struct node** npp)
{
	return find_max(*npp);
}

struct node *
find_min(struct node* np)
{
	if(!np->left)	return np;
	else		return find_min(np->left);
}

struct node*
get_min(struct node** npp)
{
	return find_min(*npp);
}

struct node*
find_next_larger(struct node *np)
{
	if(np->right)	return find_min(np->right);	

	int32_t	    tmp_d  = np->data;
	struct node *tmp_p = np;
	while(tmp_p->parent) {
		tmp_p	  = tmp_p->parent;
		if(tmp_p->data > tmp_d)	return tmp_p;
	}

	return np;
}

struct node*
find_next_lesser(struct node *np)
{
	if(np->left)	return find_max(np->left);	

	int32_t     tmp_d  = np->data;
	struct node *tmp_p = np;
	while(tmp_p->parent) {
		tmp_p	  = tmp_p->parent;
		if(tmp_p->data < tmp_d)	return tmp_p;
	}

	return np;
}

struct node *pop_node(struct node **npp);
struct node* pop_node_do(struct node **npp, int32_t rec_level);
void
delete(struct node **npp)
{
	free(pop_node(npp));
}

void
handle_tree_recalc(struct node* start, struct node *end)
{
	printf("Entering handle_tree_recalc\n");
	while(start != end) {
		set_height(start);
		start = handle_balance(start);
		start = start->parent;
	}
}

struct node*
pop_node(struct node **npp)
{
	return pop_node_do(npp, 0);
}

struct node*
pop_node_do(struct node **npp, int32_t rec_level)
{
	struct node *np		     = *npp;
	struct node *np_par  	     = np->parent;
	struct node *node_instead_np = NULL;
	struct node *next_larger     = NULL;
	struct node *node_to_start_recalc_from = np_par;
	struct node *node_to_end_recalc_at     = NULL;

	if     (!np->left && !np->right) {	
		node_instead_np = NULL;
	}
	else if(np->left && !np->right) {
		node_instead_np 	= np->left;
		node_instead_np->parent = np_par;
	}
	else if(!np->left && np->right) {
		node_instead_np 	= np->right;
		node_instead_np->parent = np_par;
	}
	else {
		next_larger = find_next_larger(np);
		node_instead_np = calloc(1, sizeof(*node_instead_np));
		memcpy(node_instead_np, next_larger, sizeof(node_instead_np));
		node_instead_np->left	= np->left;	
		node_instead_np->right	= np->right;	

		node_to_start_recalc_from = next_larger->parent;
	}
	
	set_as_parent(node_instead_np);

	if	(!np_par)			*npp	      = node_instead_np;
	else if	(np_par->data > np->data)	np_par->left  = node_instead_np;
	else					np_par->right = node_instead_np;

	/* no way next_larger can be root, so won't bother with a NULL parent */
	if    	(next_larger)		free(pop_node_do(&next_larger, rec_level + 1));

	if	(rec_level != 0)	node_to_end_recalc_at = np_par;

	printf("recursion level is %" PRId32 "\n", rec_level);
	handle_tree_recalc(node_to_start_recalc_from, node_to_end_recalc_at);

	np->parent = np->left = np->right = NULL;
	return np;
}

void
print_all(struct node *np)
{
	if(!np) { return; }
	print_all(np->left);
	printf("%" PRId32 " ", np->data);
	printf("next min is %" PRId32 " and next max is %" PRId32 " ", 
			find_next_lesser(np)->data, find_next_larger(np)->data);

	if(np->parent)	printf("parent data is %" PRId32 "\n", np->parent->data);
	else		printf("root\n");

	print_all(np->right);
};

int32_t
invariant_holds(struct node *np)
{
	if(!np)	return 1;
	int32_t b = get_balance(np);
	//printf("balance - %" PRId32 "\n", b);
	if(b > 1 || b < -1) return 0;

	int32_t li = invariant_holds(np->left);
	int32_t ri = invariant_holds(np->right);

	if(li && ri)	return 1;
	else		return 0;	
}

void
check_invariant(struct node **npp)
{
	if(invariant_holds(*npp))	puts("INVARIANT HOLDS!");
	else				puts("INVARIAN DOES NOT HOLD!");		
}

#define AVL_TREE 		struct node *
#define init_avl_tree(X) 	AVL_TREE X_root = NULL; AVL_TREE *X = &X_root
#define insert_node(D, N) 	*N = insert((D), *N)
#define print_tree(X)		do { puts("Printing tree..."); print_all(*X); printf("\n"); } while(0)


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
	init_avl_tree(tree);

	//uint32_t inp_nums[INPUT_SIZE];
	/* one good input set */
	uint32_t inp_nums[] = {3, 66, 84, 87, 58, 1, 26, 52, 14, 37};
	for(size_t i = 0; i < INPUT_SIZE; ++i) {
		//inp_nums[i] = get_rand_num(INPUT_MIN, INPUT_MAX);
		printf("rand num is %" PRIu32 "\n", inp_nums[i]);
		uint32_t n = inp_nums[i];
		insert_node(n, tree);
	}


	print_tree(tree);
	printf("%" PRId32 "\n", (*tree)->data);
	check_invariant(tree);
	printf("%" PRId32 " %" PRId32 "\n", get_max(tree)->data, get_min(tree)->data);

	struct node* min = get_min(tree);
	delete(&min);
	print_tree(tree);
	check_invariant(tree);
		
	struct node* max = get_max(tree);
	delete(&max);
	print_tree(tree);
	check_invariant(tree);

	delete(tree);
	print_tree(tree);
	check_invariant(tree);

}
/*
 * INTERFACE FOR THIS PROGRAM IS THE FOLLOWING:
 * init_avl_tree(TREE_NAME) - initializes tree. It is a macro, just give a keyword as a parameter.
 * insert_node(DATA, TREE)  - inserts spedified data(int32_t in this program) into a given tree.
 * print_tree(TREE)	    - print specified tree's data.	
 * get_max(TREE)	    - returns specified tree's node with max data without deleting it.
 * get_min(TREE)	    - returns specified tree's node with min data without deleting it.
*/
