#include <stdio.h>

void print_arr(int *arr, int len);
void qs(int *arr, int lo, int hi);
int partition(int *arr, int lo, int hi);
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
    p = partition(arr, lo, hi);
    qs(arr, lo, p);
    qs(arr, p+1, hi);
  }
}

int partition(int *arr, int lo, int hi) {
  int i, j, p;
  p = hi-1;
  i = lo;
  j = lo;

  for ( ; j < p; j++) {
    if (arr[j] <= arr[p]) {
      swap(&arr[j], &arr[i]);
      i++;
    }
  }
  swap(&arr[p], &arr[i]);
  p = i;
  return p;
}

void swap(int *i, int *j) {
  int tmp;
  tmp = *i;
  *i = *j;
  *j = tmp;
}
