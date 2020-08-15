#include <stdio.h>
#include <stdlib.h>

struct Stack {
  int data;
  struct Stack *next;
};

struct Stack * init_stack(struct Stack **stack);

void push_stack(const char code, struct Stack **stack);
void * determine_push(const char code);
void push(struct Stack **stack, int num); 
void push_sorted(struct Stack **stack, int num); 

void delete_negatives(struct Stack **stack);
void print_stack(struct Stack *stack);
struct Stack * free_stack(struct Stack *stack);

int main(void) {
  int num = 0;
  struct Stack *stack;

  init_stack(&stack);
  push_stack('p', &stack);
  print_stack(stack);
  push_stack('s', &stack);
  print_stack(stack);
  delete_negatives(&stack);
  print_stack(stack);
  stack = free_stack(stack);
  print_stack(stack);
}

struct Stack * init_stack(struct Stack **stack) {
  *stack = NULL;
}

void push_stack(const char code, struct Stack **stack) {
  int num;
  void (*push) (struct Stack **stack, int num);
  if (!(push = determine_push(code))) {
    printf("Cannot push - wrong push flag!\n");
    return;
  }
  while((scanf("%d", &num) != EOF)) {
   push(stack, num);
  }
}

void * determine_push(const char code) {
  switch(code) {
    case 'p':
      return &push;
      break;
    case 's':
      return &push_sorted;
      break;
    default:
      return NULL;
  }
}

void push(struct Stack **stack, int num) {
  struct Stack *tmp = malloc(sizeof(struct Stack));
  tmp->data = num;
  tmp->next = *stack;
  *stack = tmp;
  printf("ENTRY: %d\n", (*stack)->data);
}

void push_sorted(struct Stack **stack, int num) {
  struct Stack *tmp;
    if (((*stack) == NULL) || ((*stack)->data < num)) {
       tmp = malloc(sizeof(struct Stack));
       tmp->data = num;
       tmp->next = *stack;
       *stack = tmp;
       printf("ENTRY: %d\n", (*stack)->data);
       return;
    }
    else {
      push_sorted(&(*stack)->next, num);
    }
}

void print_stack(struct Stack *stack) {
  struct Stack *tmp = stack;
  while (tmp) {
    printf("%d ", tmp->data);
    tmp = tmp->next;
  }
  printf("\n");
}

struct Stack *  free_stack(struct Stack *stack) {
  struct Stack *tmp = stack;
  while (stack) {
    tmp = stack;
    stack = stack->next;
    free(tmp);
  }
  return NULL;
}

void delete_negatives(struct Stack **stack_ptr) {
  struct Stack *tmp = *stack_ptr;
  while ((*stack_ptr)) {
    if ((*stack_ptr)->data < 0) {
      tmp = *stack_ptr;
      *stack_ptr = (*stack_ptr)->next;
      free(tmp);
    } 
    else {
      stack_ptr = &(*stack_ptr)->next;
    }
  }
}
