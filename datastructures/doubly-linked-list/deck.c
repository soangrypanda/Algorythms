#include <stdio.h>
#include <stdlib.h>

struct node {
  int data;
  struct node *next;
  struct node *prev;
};

struct deck {
  struct node *head;
  struct node *tail;
};

void init_deck(struct deck *deck);
void push_left(struct deck *deck, struct node *cur, int num);
void push_right(struct deck *deck, struct node *cur, int num);
void print_deck(struct deck deck);


int main(void) {
  struct deck deck;
  int num;
  init_deck(&deck);

  while (scanf("%d", &num) == 1)
    push_left(&deck, deck.head, num);
  print_deck(deck);
  while (scanf("%d", &num) == 1)
    push_right(&deck, deck.tail, num);
  print_deck(deck);
}

void init_deck(struct deck *deck) {
  deck->head = NULL;
  deck->tail = deck->head;
}

void push_left(struct deck *deck, struct node *cur, int num) {
  struct node *tmp = malloc(sizeof(struct node));
  tmp->data = num;
  tmp->next = cur;
  if (!cur)
    tmp->prev = NULL;
  else
    tmp->prev = cur->prev;
  if(!tmp->prev)
    deck->head = tmp;
  else 
    tmp->prev->next = tmp;
  if (!tmp->next)
    deck->tail = tmp;
  else
    tmp->next->prev = tmp;
}

void push_right(struct deck *deck, struct node *cur, int num) {
  struct node *tmp = malloc(sizeof(struct node));
  tmp->data = num;
  tmp->prev = cur;
  if (!cur)
    tmp->next = NULL;
  else
    tmp->next = cur->next;
  if (!tmp->next)
    deck->tail = tmp;
  else
    tmp->next->prev = tmp;
  if (!tmp->prev)
    deck->head = tmp;
  else
    tmp->prev->next = tmp;
}

void print_deck(struct deck deck) {
  FILE *f = fopen("out.txt", "w");

  while(deck.head != NULL) {
    fprintf(f, "%d ", deck.head->data);
    deck.head = deck.head->next;
  }
  printf("\n");
  fclose(f);
}
