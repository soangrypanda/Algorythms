/* comments are for future me that whould work on clav */

#include <stdio.h>
#include <stdlib.h>

struct node {
  int data;
  struct node *left;
  struct node *right;
};

/* consider using const ptrptr in functions,
at least at fill_tree and is_in_tree */

void init_tree(struct node **root);
struct node ** search_tree(struct node **node, int num);
int push_tree(struct node **node_ptr, int num);
void fill_tree(struct node **node_ptr);
void print_tree(struct node *node);
void print_tr_recur(struct node *node);
void free_tree(struct node **node_ptr);
void is_in_tree(struct node **node_ptr);

int main(void) {
  struct node *root;
  int num;
  init_tree(&root);
  
  fill_tree(&root);
  print_tree(root);

  is_in_tree(&root);

  free_tree(&root);
  print_tree(root);
/* always altering between root and &root is messy */
}

void init_tree(struct node **root) {
  *root = NULL;
}

struct node ** search_tree(struct node **node, int num) {
  if (!(*node) || (*node)->data == num) {
    return node;
  }
  else if ((*node)->data < num) {
    return search_tree(&(*node)->left,  num);
  }
  else {
    return search_tree(&(*node)->right,  num);
  }
}

int push_tree(struct node **node_ptr, int num) {

  if (!(*node_ptr)) {
    (*node_ptr) = malloc(sizeof(struct node));
    (*node_ptr)->data = num;
    (*node_ptr)->left = NULL;
    (*node_ptr)->right = NULL;
    return 0;
  }
  else {
    return -1;
  }
}

void fill_tree(struct node **node_ptr) {
  struct node **node_pos;
  int num;

  while(scanf("%d", &num) == 1) {
    node_pos = search_tree(node_ptr, num);
    if (push_tree(node_pos, num) == -1)
      printf("push_tree error: data already in a tree\n"); 
  }
}

void is_in_tree(struct node **node_ptr) {
  struct node **in_tree;
  int num;

  while(scanf("%d", &num) == 1) {
    in_tree = search_tree(node_ptr, num);
    if ((*in_tree))
      printf("Yes!\n");
    else
      printf("No!\n");
  }
}

void print_tree(struct node *node) {
  if (node)
    print_tr_recur(node);
  printf("\n");
}

void print_tr_recur(struct node *node) {
  if (!node) {
    return;
  }
  else {
    print_tr_recur(node ->left);
    printf("%d ", node->data);
    print_tr_recur(node ->right); 
  }
}

void free_tree(struct node **node_ptr) {
  if (!(*node_ptr)) {
    return;
  }
  else {
    free_tree(&(*node_ptr)->left);
    free_tree(&(*node_ptr)->right);
    free((*node_ptr));
    (*node_ptr) = NULL;
  }
}
