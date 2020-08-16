#include <stdio.h>
#include <stdlib.h>

struct node {
  int data;
  struct node* next;
};

struct Q {
  struct node* head;
  struct node* tail;
};

void init_q(struct Q *q);
void put(struct Q *q, int num);
void put_s(struct Q *q, int num);
void put_q(struct Q *q, const char code);
void *determine_put(const char code, struct Q *q);

int get(struct Q *q);
void print_q(struct Q q);
void delete_negatives(struct Q *q);
void free_q(struct Q *q);

int main(void) {
  int num;
  struct Q q;
  init_q(&q);

  put_q(&q, 's');
  print_q(q);

  delete_negatives(&q);
  print_q(q);

  put_q(&q, 'p');
  print_q(q);
  
  get(&q);
  print_q(q);
  get(&q);
  print_q(q);

  free_q(&q);
  print_q(q);
}

void init_q(struct Q *q) {
  q->head = NULL;
}

void put_q(struct Q *q, const char code) {
  int num;
  void (*put) (struct Q *q, int num);
  put = determine_put(code, q);
  if (!put) {
    printf("Error - wrong put code!\n");
    return;
  }
  while (scanf("%d",&num) != EOF) {
    put(q, num);
  }
}

void *determine_put(const char code, struct Q *q) {
  switch(code) {
    case 'p':
      return &put;
    case 's':
      return &put_s;
    default:
      return NULL;
  }
}

void put(struct Q *q, int num) {
  struct node *tmp;
  if (q->head == NULL) {
    tmp = malloc(sizeof(struct node));
    q->head = tmp;
  }
  else {
    tmp = malloc(sizeof(struct node));
    q->tail->next = tmp;
  }
  q->tail = tmp;
  tmp->data = num;
  tmp->next = NULL;
}

void put_s(struct Q *q, int num) {
  if (q->head == NULL) {
    put(q, num);
    return;
  }
  struct node **pp = &q->head;
  struct node *tmp;
  while (1) {
    if (((*pp) == NULL) || ((*pp)->data > num)) {
      tmp = malloc(sizeof(struct node));
      tmp->data = num;
      tmp->next = (*pp);
      (*pp) = tmp;
      return;
    }
    else {
      pp = &(*pp)->next;
    }
  }
}

int get(struct Q *q) {
  int num;
  struct node *tmp;
  num = q->head->data;
  tmp = q->head;
  q->head = q->head->next;
  free(tmp);
  return num;
}

void print_q(struct Q q) {
  while(q.head) {
    printf("%d ", q.head->data);
    q.head = q.head->next;
  }
  printf("\n");
}

void delete_negatives(struct Q *q) {
  struct node **tmp_q = &q->head;
  while(*tmp_q) {
    if ((*tmp_q)->data < 0) {
      struct node *tmp = (*tmp_q);
      (*tmp_q) = (*tmp_q)->next;
      free(tmp);
    }
    else {
      tmp_q = &(*tmp_q)->next;
    }
  }
}

void free_q(struct Q *q) {
  struct node *tmp;
  while (q->head) {
    tmp = q->head;
    q->head = q->head->next;
    free(tmp);
  }
  q->head = NULL;
}
