#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define INPUT_SIZE	100
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
	struct node *n = calloc(1, sizeof(*n));
	n->data = d; n->left = n->right = n->parent = NULL; 
	return n;
};

int32_t
get_balance(struct node *n)
{
	return (n->left ? n->left->height : -1) - (n->right ? n->right->height : -1);
};

void 
set_height(struct node *n)
{
	int32_t lh = n->left  ? n->left->height  : -1;
	int32_t rh = n->right ? n->right->height : -1;	
	n->height = 1 + (lh > rh ? lh : rh);
};

int32_t
recalc_height(struct node *n)
{
	if(!n) return -1;
	int32_t lh = recalc_height(n->left);
	int32_t rh = recalc_height(n->right);
	return n->height = 1 + (lh > rh ? lh : rh);
};

uint32_t 
get_height(struct node *n)
{
	return n->height;
};

#define FIRST_ENTRY	1
#define SECOND_ENTRY	0

void
set_as_parent(struct node *n)
{
	if(n->left)	n->left->parent  = n;
	if(n->right)	n->right->parent = n;
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
	int32_t b = get_balance(np);  
	printf("balance is - %" PRId32 "\n", b);

	if 	( b >  1 ) {np = right_rotate(np, FIRST_ENTRY); recalc_height(np);}
	else if ( b < -1 ) {np = left_rotate(np , FIRST_ENTRY); recalc_height(np);}

	b = get_balance(np);  
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
	return tmp;
};

struct node *
find_max(struct node* np)
{
	if(!np->right)	return np;
	else		return find_max(np->right);
}

struct node *
find_min(struct node* np)
{
	if(!np->left)	return np;
	else		return find_min(np->left);
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

struct node *pop_node(struct node *np);
void
delete(struct node *np)
{
	free(pop_node(np));
}


//CREATE POINTER TO POINTER FOR ROOT NODE!!!


struct node*
pop_node(struct node *np)
{
	struct node *np_par  	     = np->parent;
	struct node *node_instead_np = NULL;

	if     (!np->left && !np->right)	
		node_instead_np = NULL;
	else if(np->left && !np->right) {
		node_instead_np 	= np->left;
		node_instead_np->parent = np_par;
	}
	else if(!np->left && np->right) {
		node_instead_np 	= np->right;
		node_instead_np->parent = np_par;
	}
	else {
		struct node *next_larger = find_next_larger(np);
		node_instead_np = calloc(1, sizeof(*node_instead_np));
		memcpy(node_instead_np, next_larger, sizeof(node_instead_np));
		node_instead_np->left	= np->left;	
		node_instead_np->right	= np->right;	

		delete(next_larger);
	}
	
	if(np_par->data > np->data)	np_par->left  = node_instead_np;
	else				np_par->right = node_instead_np;

	np->parent = np->left = np->right = NULL;
	return np;
}

void
print_all(struct node *np)
{
	if(!np) { return; }
	print_all(np->left);
	printf("%" PRId32 " ", np->data);
	printf("next min is %" PRId32 " and next max in %" PRId32 "\n", 
			find_next_lesser(np)->data, find_next_larger(np)->data);
	/*
	if(np->parent)	printf("parent data is %" PRId32 "\n", np->parent->data);
	else		printf("root\n");
	*/
	print_all(np->right);
};

int32_t
invariant_holds(struct node *np)
{
	if(!np)	return 1;
	int32_t b = get_balance(np);
	printf("balance - %" PRId32 "\n", b);
	if(b > 1 || b < -1) return 0;

	int32_t li = invariant_holds(np->left);
	int32_t ri = invariant_holds(np->right);

	if(li && ri)	return 1;
	else		return 0;	
}

void
check_invariant(struct node *np)
{
	if(invariant_holds(np)) puts("INVARIANT HOLDS!");
	else			puts("INVARIAN DOES NOT HOLD!");		
}

#define AVL_TREE 		struct node *
#define init_avl_tree(X) 	AVL_TREE X = NULL
#define insert_node(D, N) 	N = insert((D), N)
#define print_tree(X)		do { puts("Printing tree..."); print_all(X); printf("\n"); } while(0)

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

	uint32_t inp_nums[INPUT_SIZE];
	for(size_t i = 0; i < INPUT_SIZE; ++i) {
		inp_nums[i] = get_rand_num(INPUT_MIN, INPUT_MAX);
		printf("rand num is %" PRIu32 "\n", inp_nums[i]);
		uint32_t n = inp_nums[i];
		insert_node(n, tree);
	}


	print_tree(tree);
	printf("%" PRId32 "\n", tree->data);
	check_invariant(tree);
	printf("%" PRId32 " %" PRId32 "\n", find_max(tree)->data, find_min(tree)->data);

	delete(tree);
		

}
