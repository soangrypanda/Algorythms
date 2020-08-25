#include <stdio.h>

void print_arr(int *arr, int len);
void qs(int *arr, int lo, int hi);
int pivot(int *arr, int lo, int hi);
void swap(int *i, int *j);

int main(void) {
  //int arr[] = { 6, 2, 5, 0, 1, 10, -1, 3, 4, 9, 8, -2, 7 };
  int arr[] = { 2, 2, 2, 2, 1, 1, 1, 0, 0, -1 };
  int len_arr;

  len_arr = sizeof(arr) / sizeof(arr[0]);

  print_arr(arr, len_arr);
  printf("\n");
  qs(arr, 0, len_arr);

  print_arr(arr, len_arr);
}

void print_arr(int *arr, int len) {
  for (int i = 0; i < len; i++)
    printf("%d ", arr[i]);
  printf("\n");
}

void qs(int *arr, int lo, int hi) {
  if (lo >= hi) {
    ;
  }
  else {
    int p;
    p = pivot(arr, lo, hi);
    qs(arr, lo, p);
    qs(arr, p+1, hi);
  }
}

int pivot(int *arr, int lo, int hi) {
  int i, j, p;
  p = lo;
  i = p;
  j = p + 1;
  printf("lo is %d and hi is %d\n", lo, hi);

  for ( ; j < hi; j++) {
    printf("pivot is: %d and i is %d and j is %d\n", p, i, j);
    print_arr(arr, 10);
    if (arr[j] < arr[p]) {
      printf("SWAP %d and %d\n", j, i+1);
      swap(&arr[j], &arr[i+1]);
      i++;
    }
  }
  printf("pivot is: %d and i is %d\n", p, i);
  swap(&arr[p], &arr[i]);
  print_arr(arr, 10);
  p = i;
  printf("pivot is: %d and i is %d\n", p, i);
  printf("\n");
  return p;
}

void swap(int *i, int *j) {
  int tmp;
  tmp = *i;
  *i = *j;
  *j = tmp;
}
