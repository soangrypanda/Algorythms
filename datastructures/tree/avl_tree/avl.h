#ifndef AVL_TREE_SENTRY
#define AVL_TREE_SENTRY


#include <inttypes.h>
#include <stddef.h>

typedef struct node     AVL_NODE;
typedef AVL_NODE *      AVL_NODE_P;
typedef AVL_NODE_P *    AVL_TREE;   
typedef int32_t         nkey_t;
#define PRINKEY         PRId32

enum print_tree_flags {PLAIN, VERBOSE};

#define     new_avl_tree(X)     AVL_TREE X = init_avl_tree(X)
AVL_TREE    init_avl_tree       (AVL_TREE tree);
void        insert_node         (AVL_TREE tree, nkey_t key, void *data, 
                                uint32_t data_size, char *inserted);
void        delete_node         (AVL_TREE tree, AVL_NODE_P np);
AVL_TREE    delete_tree         (AVL_TREE tree);
AVL_NODE_P  pop_node            (AVL_TREE tree, AVL_NODE_P np);
AVL_NODE_P  get_max             (AVL_TREE tree);
AVL_NODE_P  get_min             (AVL_TREE tree);
AVL_NODE_P  get_node            (AVL_TREE tree, nkey_t key);
AVL_NODE_P  find_next_larger    (AVL_NODE_P np);
AVL_NODE_P  find_next_lesser    (AVL_NODE_P np);
void        print_tree          (AVL_TREE tree, int32_t flag);
void        check_invariant     (AVL_TREE tree);
void        insert_random_data  (AVL_TREE tree, size_t size, int32_t min, int32_t max);


#endif
