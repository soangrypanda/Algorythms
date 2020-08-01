#include <stdio.h>

int insertion_sort(int *arr, int arr_len);
void ins_srt_rec(int *arr, int arr_len, int key);
void ins_srt_loop(int *arr, int arr_len, int key); 

void swap(int *a, int *b); 
void print_arr(int *arr, int arr_len);

int main(void) {
  int arr[] = {1, 3, 10, 5, 4, 0, 9, 13, 67, -1 };
  int arr_len = sizeof(arr) / sizeof(arr[0]);
  print_arr(arr, arr_len);
  if (!insertion_sort(arr, arr_len)) {
    printf("Cannot sort!\n");
    return 1;
  }
  print_arr(arr, arr_len);
  return 0;
}

void print_arr(int *arr, int arr_len) {
  for (int i = 0; i < arr_len; i++) {
    printf("%i ", arr[i]);
  }
  printf("\n");
}

int insertion_sort(int *arr, int arr_len) {
  //ins_srt_rec(arr, arr_len, 1);
  ins_srt_loop(arr, arr_len, 1);
  return 1;
}

void ins_srt_rec(int *arr, int arr_len, int key) {
  if (key < arr_len) {
    for (int i = key - 1; i >= 0; i--) {
      if (arr[i+1] < arr[i])
	swap(&arr[i+1], &arr[i]);
      else 
	ins_srt_rec(arr, arr_len, key+1);
    }
  }
}

void ins_srt_loop(int *arr, int arr_len, int key) {
  for (; key < arr_len; key++) {
    for (int i = key - 1; i >= 0; i --) {
      if (arr[i+1] < arr[i])
	swap(&arr[i+1], &arr[i]);
    }
  }
}

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
