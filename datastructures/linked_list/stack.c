#include <stdio.h>
#include <stdlib.h>

struct Stack {
  int data;
  struct Stack *next;
};

struct Stack * init_stack(void);
void push(struct Stack **stack, int num); 
void print_stack(struct Stack *stack);
struct Stack *  free_stack(struct Stack *stack);

int main(void) {
  int num = 0;
  struct Stack *stack = init_stack();
  while((scanf("%d", &num) != EOF)) {
   push(&stack, num);
  }
  print_stack(stack);
  stack = free_stack(stack);
  print_stack(stack);
}

struct Stack * init_stack(void) {
  return NULL;
}

void push(struct Stack **stack, int num) {
  struct Stack *tmp = malloc(sizeof(struct Stack));
  tmp->data = num;
  tmp->next = *stack;
  *stack = tmp;
  printf("ENTRY: %d\n", (*stack)->data);
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
