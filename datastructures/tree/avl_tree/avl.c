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
static AVL_NODE_P   create_node     (nkey_t key, void *data, uint32_t data_size);
static void         set_height      (AVL_NODE_P np);
static uint32_t     get_height      (AVL_NODE_P np);
static int32_t      recalc_height   (AVL_NODE_P np);
static void         set_as_parent   (AVL_NODE_P np);
static int32_t      get_balance     (AVL_NODE_P np);
static AVL_NODE_P   handle_balance  (AVL_NODE_P np);
/*
 * TREE INTERNAL INTERFACE
 */
static AVL_NODE_P   insert          (nkey_t key, void *data, uint32_t data_size, 
                                    AVL_NODE_P np, char *inserted);
static AVL_NODE_P   right_rotate    (AVL_NODE_P np);
static AVL_NODE_P   left_rotate     (AVL_NODE_P np);
static AVL_NODE_P   find_max        (AVL_NODE_P np);
static AVL_NODE_P   find_min        (AVL_NODE_P np);
static void         delete_tree_do  (AVL_NODE_P np);
static AVL_NODE_P   pop_node_do     (AVL_TREE tree, AVL_NODE_P np, int32_t rec_level);
static void         handle_tree_recalc(AVL_NODE_P start, AVL_NODE_P end, AVL_TREE tree);
/*
 * MISCELANEOUS INTERNAL INTERFACE
 */
static void         print_do        (AVL_NODE_P np, int32_t flag);
static int32_t      invariant_holds (AVL_NODE_P np);
static void         init_rand       (void);
static int32_t      get_rand_num    (int32_t l, int32_t h);



/*******************
 * IMPLEMENTATIONS *
 *******************/

/*
 * NODE
 */

struct node {
    nkey_t      key;
    int32_t     height;
    uint32_t    data_size;
    void        *data;
    AVL_NODE_P  left;
    AVL_NODE_P  right;
    AVL_NODE_P  parent;
};

static AVL_NODE_P 
create_node(nkey_t key, void *data, uint32_t data_size)
{
    #ifdef DEBUG
        printf("Creating node...\n");
    #endif
    
    AVL_NODE_P np = calloc(1, sizeof(*np));
    if(!np) {
        perror("create_node()");
        exit(EXIT_FAILURE);
    }
    np->key     = key; np->data = data; np->data_size = data_size;
    np->left    = np->right = np->parent = NULL; 
    return np;
}

static void 
set_height(AVL_NODE_P np)
{
    int32_t lh = np->left  ? np->left->height  : -1;
    int32_t rh = np->right ? np->right->height : -1;    
    np->height = 1 + (lh > rh ? lh : rh);
}

static uint32_t 
get_height(AVL_NODE_P np)
{
    return np->height;
}

static int32_t
recalc_height(AVL_NODE_P np)
{
    if(!np) return -1;
    int32_t lh          = recalc_height(np->left);
    int32_t rh          = recalc_height(np->right);
    return np->height   = 1 + (lh > rh ? lh : rh);
}

static void
set_as_parent(AVL_NODE_P np)
{
    if(!np)         return;
    if(np->left)    np->left->parent  = np;
    if(np->right)   np->right->parent = np;
}

static int32_t
get_balance(AVL_NODE_P np)
{
    return (np->left ? np->left->height : -1) - (np->right ? np->right->height : -1);
}

static AVL_NODE_P
handle_balance(AVL_NODE_P np)
{
    int32_t b = get_balance(np);
    
    #ifdef DEBUG
        printf("balance is - %" PRId32 "\n", b);
    #endif

    if  ( b >  1 )      {np = right_rotate(np); recalc_height(np); set_as_parent(np);}
    else if ( b < -1 )  {np = left_rotate(np);  recalc_height(np); set_as_parent(np);}

    return np;
}


/*
 * TREE
 */

/* EXTERNAL TREE INTERFACE IMPLEMENTATION */

AVL_TREE
init_avl_tree(AVL_TREE tree)
{
    /* use new_avl_tree() for new tree instead!
     *
     * allocates memory for initial AVL_NODE_P.
     * just want to have one function to init
     * new tree, and macro is not an option as
     * do-while loop hides a tree's keyword. */
    AVL_TREE root = calloc(1, sizeof(*root));
    return root;
}

void
insert_node(AVL_TREE tree, nkey_t key, void *data, uint32_t data_size, char *inserted)
{
    *tree = insert(key, data, data_size, *tree, inserted);
}

void
delete_node(AVL_TREE tree, AVL_NODE_P np)
{
    AVL_NODE_P tmp = pop_node(tree, np);
    free(tmp->data);
    free(tmp);
}

AVL_TREE
delete_tree(AVL_TREE tree)
{
    #ifdef DEBUG
        printf("Entering delete_tree()...\n");
    #endif
    
    delete_tree_do(*tree);
    free(tree);
    return NULL;
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
get_node(AVL_TREE tree, nkey_t key)
{
    AVL_NODE_P np = *tree;
    while(np) {
        if      (key < np->key) np = np->left;
        else if (key > np->key) np = np->right;
        else                    break;
    }
    return np;
}

AVL_NODE_P 
find_next_larger(AVL_NODE_P np)
{
    if(np->right)   return find_min(np->right); 

    AVL_NODE_P  tmp_p = np;
    int32_t     tmp_k = np->key;
    
    while(tmp_p->parent) {
        tmp_p = tmp_p->parent;
        if(tmp_p->key > tmp_k)  return tmp_p;
    }

    return np;
}

AVL_NODE_P 
find_next_lesser(AVL_NODE_P np)
{
    if(np->left)    return find_max(np->left);  

    AVL_NODE_P  tmp_p = np;
    int32_t     tmp_k  = np->key;
    
    while(tmp_p->parent) {
        tmp_p = tmp_p->parent;
        if(tmp_p->key < tmp_k)  return tmp_p;
    }

    return np;
}

/* INTERNAL TREE INTERFACE IMPLEMENTATION */

static AVL_NODE_P 
insert(nkey_t key, void *data, uint32_t d_size, AVL_NODE_P np, char *inserted)
{
    #ifdef DEBUG
        printf("Entering insert...\n");
    #endif
    
    if      (!np)           {np   = create_node(key, data, d_size); *inserted = 1;}
    else if (key < np->key) np->left  = insert(key, data, d_size, np->left,  inserted);
    else if (key > np->key) np->right = insert(key, data, d_size, np->right, inserted); 
    else                    *inserted = 0;
    
    #ifdef DEBUG
        printf("insert() after first if - key is %" PRINKEY "\n", np->key);
    #endif

    set_height(np);
    set_as_parent(np);
    np = handle_balance(np);

    #ifdef DEBUG
        int32_t b = get_balance(np);  
        printf("balance after rotation is - %" PRId32 "\n", b);
        printf("insert() end  - key is %" PRINKEY "\n", np->key);
    #endif
    
    return np;
};

static AVL_NODE_P 
right_rotate(AVL_NODE_P np)
{
    #ifdef DEBUG
        puts("Entering right_rotate...");
    #endif
    
    if(get_balance(np->left) < 0)
        np->left = left_rotate(np->left);
    
    AVL_NODE_P tmp  = np->left;
    np->left        = tmp->right;
    tmp->right      = np;
    tmp->parent     = np->parent; 
    np->parent      = tmp;
    set_as_parent(tmp);
    set_as_parent(np);
    
    return tmp;
};  

static AVL_NODE_P 
left_rotate(AVL_NODE_P np)
{
    #ifdef DEBUG
        puts("Entering left_rotate...");
    #endif

    if(get_balance(np->right) > 0)
        np->right = right_rotate(np->right);
        
    AVL_NODE_P tmp  = np->right;
    np->right       = tmp->left;
    tmp->left       = np;
    tmp->parent     = np->parent;
    np->parent      = tmp;
    set_as_parent(tmp);
    set_as_parent(np);
    
    return tmp;
};

static AVL_NODE_P 
find_max(AVL_NODE_P np)
{
    if(!np->right)  return np;
    else            return find_max(np->right);
}

AVL_NODE_P 
find_min(AVL_NODE_P np)
{
    if(!np->left)   return np;
    else            return find_min(np->left);
}

static void
delete_tree_do(AVL_NODE_P np)
{
    if(!np) return;
    
    delete_tree_do(np->left);
    delete_tree_do(np->right);
    free(np->data);
    free(np);
}

static AVL_NODE_P 
pop_node_do(AVL_TREE tree, AVL_NODE_P np, int32_t rec_level)
{
    AVL_NODE_P np_par           = np->parent;
    AVL_NODE_P node_instead_np  = NULL;
    AVL_NODE_P next_larger      = NULL;
    AVL_NODE_P node_to_start_recalc_from = np_par;
    AVL_NODE_P node_to_end_recalc_at     = NULL;

    if     (!np->left && !np->right) {  
        node_instead_np = NULL;
    }
    else if(np->left && !np->right) {
        node_instead_np         = np->left;
        node_instead_np->parent = np_par;
    }
    else if(!np->left && np->right) {
        node_instead_np         = np->right;
        node_instead_np->parent = np_par;
    }
    else {
        next_larger     = find_next_larger(np);
        node_instead_np = create_node(0, NULL, 0);  
        memmove(node_instead_np, next_larger, sizeof(*node_instead_np));
        node_instead_np->left   = np->left; 
        node_instead_np->right  = np->right;
        node_instead_np->parent = np_par;   
    }

    set_as_parent(node_instead_np);

    if      (!np_par)               *tree         = node_instead_np;
    else if (np->key < np_par->key) np_par->left  = node_instead_np;
    else                            np_par->right = node_instead_np;    

    /* no way next_larger can be root, so won't bother with a NULL parent */
    if(next_larger) {
        node_to_start_recalc_from   = next_larger->parent;
        AVL_NODE_P tmp          = pop_node_do(tree, next_larger, rec_level + 1);
        /* DON'T FREE THE DATA! IT IS IN ANOTHER NODE NOW!
         * free(tmp->data);
         */
        free(tmp);
    }

    if(rec_level != 0)  node_to_end_recalc_at = np_par;

    #ifdef DEBUG
        printf("recursion level is %" PRId32 "\n", rec_level);
    #endif
    
    handle_tree_recalc(node_to_start_recalc_from, node_to_end_recalc_at, tree);

    np->parent = np->left = np->right = NULL;
    return np;
}

static void
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
        if      (!parent)                   *tree         = cur_np;
        else if (cur_np->key < parent->key) parent->left  = cur_np;
        else                                parent->right = cur_np; 
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
    if(!tree) {
        puts("No tree to print!");
        return;
    }
    puts("Printing tree..."); 
    print_do(*tree, flag); 
    puts("");
}


static void
print_do(AVL_NODE_P np, int32_t flag)
{
    if(!np) { return; }
    
    print_do(np->left, flag);
    
    printf("Key - %" PRINKEY " Data - %s ", np->key, np->data);
    
    if(flag == VERBOSE) {
        printf("next min is %" PRINKEY " and next max is %" PRINKEY " ", 
                find_next_lesser(np)->key, find_next_larger(np)->key);
        if(np->parent)  printf("parent key is %" PRINKEY " ", np->parent->key);
        else            printf("root");
    }
    
    printf("\n");
    
    print_do(np->right, flag);
};

/* INVARIANT CHECK */

void
check_invariant(AVL_TREE tree)
{
    if(invariant_holds(*tree))  puts("INVARIANT HOLDS!");
    else                        puts("INVARIAN DOES NOT HOLD!");        
}

static int32_t
invariant_holds(AVL_NODE_P np)
{
    if(!np)             return 1;
    
    int32_t b = get_balance(np);
    if(b > 1 || b < -1) return 0;

    int32_t li = invariant_holds(np->left);
    int32_t ri = invariant_holds(np->right);

    if(li && ri)        return 1;
    
    return 0;   
}

/* RANDOM FUNCTION */

/* using a normal random function is not a purporse of
 * this educational module. don't use it in practice */
unsigned int seed;

void
insert_random_data(AVL_TREE tree, size_t size, int32_t min, int32_t max)
{
    if(!seed) init_rand();
    
    if(max==0) max = size;
    
    size_t input_size   = size;
    nkey_t *inp_nums    = calloc(1, input_size * sizeof(*inp_nums));
    
    /* one good input set */
    //int32_t inp_nums[] = {3, 66, 84, 87, 58, 1, 26, 52, 14, 37};
    //uint32_t input_size = 10;
    for(size_t i = 0; i < input_size; ++i) {
        inp_nums[i] = get_rand_num(min, max);
        nkey_t key  = inp_nums[i];
        
        #ifdef DEBUG
            printf("rand num is %" PRINKEY "\n", key);
        #endif

        int32_t dummy_len = snprintf(NULL, 0, "%" PRINKEY, key) + 1;
        char *  dummy     = calloc(1, dummy_len);
        snprintf(dummy, dummy_len, "%" PRINKEY, key);
        
        #ifdef DEBUG
            printf("dummy data is %s\n", dummy);
        #endif
        
        char inserted;
        insert_node(tree, key, dummy, strlen(dummy)+1, &inserted);
        if(!inserted) free(dummy);
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
