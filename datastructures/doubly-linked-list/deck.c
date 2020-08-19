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
int check_code(const char *code);
struct node * find_pos_left(struct node *deck, int num);
struct node * find_pos_right(struct node *deck, int num);
int push(struct deck *deck, const char *code, int num);
void push_left(struct deck *deck, struct node *cur, int num);
void push_right(struct deck *deck, struct node *cur, int num);
void print_deck(struct deck deck);
void free_deck(struct deck *deck);

int main(void) {
  struct deck deck;
  int num;
  init_deck(&deck);

  while (scanf("%d", &num) == 1)
    push(&deck, "rs", num);
  print_deck(deck);
  while (scanf("%d", &num) == 1)
    push(&deck, "ls", num);
  print_deck(deck);
  
  free_deck(&deck);
  return 0;
}

void init_deck(struct deck *deck) {
  deck->head = NULL;
  deck->tail = deck->head;
}

int push(struct deck *deck, const char *code, int num) {
  int check = check_code(code);
  struct node *pos;  

  switch (check) {
    case 108: /*l*/
      push_left(deck, deck->head, num);
      return 0;
    case 114: /*r*/
      push_right(deck, deck->tail, num);
      return 0;
    case 223: /*ls*/
      if (!deck->tail || deck->tail->data < num) {
        push_right(deck, deck->tail, num);
      }
      else {
        pos = find_pos_left(deck->head, num);
        push_left(deck, pos, num);
      }
      return 0;
    case 229: /*rs*/
      if (!deck->head || deck->head->data < num) {
        push_left(deck, deck->head, num);
      }
      else {
        pos = find_pos_right(deck->tail, num);
        push_right(deck, pos, num);
      }
      return 0;
    case -1:
      fprintf(stderr, "push error: code shall be of 1 or 2 chars!\n");
      return -1;
    default:
      fprintf(stderr, "push error: flags to use - l, r, sl or sr!\n");
      return -1;  
  }
}

struct node * find_pos_left(struct node *deck, int num) {
  if (!deck) {
    return deck;
  }
  while (deck) {
    if (deck->data >= num)
      break;
    deck = deck->next;
  }
  return deck;
}

struct node * find_pos_right(struct node *deck, int num) {
  if (!deck) {
    return deck;
  }
  while (deck) {
    if (deck->data >= num)
      break;
    deck = deck->prev;
  }
  return deck;
}

int check_code(const char *code) {
  int i = 0;
  int check = 0;
  for ( ; code[i]; i++) {
    check += code[i];
  }
  if (i != 1 && i != 2)
    return -1;
  return check;
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

void free_deck(struct deck *deck) {
  struct node *tmp;
  while (deck->head) {
    tmp = deck->head;
    deck->head = deck->head->next;
    free(tmp);
  }
  deck->head = NULL;
  deck->tail = NULL;
}
