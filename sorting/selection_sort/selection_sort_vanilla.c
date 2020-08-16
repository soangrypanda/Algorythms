#include <stdio.h>

void selection_sort(int *arr, int arr_len);
void slcn_srt_loop(int *arr, int arr_len);

void print_arr(int *arr, int arr_len);
void swap(int *a, int *b); 

int main(void) {
  int arr[] = {1, 3, 10, 5, 4, 0, 9, 13, 67, -1 };
  int arr_len = sizeof(arr) / sizeof(arr[0]);
  print_arr(arr, arr_len);
  selection_sort(arr, arr_len);
  print_arr(arr, arr_len);
  return 0;
}

void selection_sort(int *arr, int arr_len) {
  slcn_srt_loop(arr, arr_len);
}

void slcn_srt_loop(int *arr, int arr_len) {
  int min = 0;
  int pos = 0;
  while (pos < arr_len - 1) {
    min = pos;
    for (int i = pos + 1; i < arr_len; i++) {
      if (arr[i] < arr[min])
	      min = i;	
    }
    if (arr[min] < arr[pos])
      swap(&arr[min], &arr[pos]);
    pos++;
  }
}

void print_arr(int *arr, int arr_len) {
  for (int i = 0; i < arr_len; i++) 
    printf("%i ", arr[i]);
  printf("\n");
}

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
