#include <stdio.h>
#include <stdlib.h>

void merge_sort(int* arr, int strt, int end);
void merge(int* arr, int strt, int mid, int end);

void print_arr(int *arr, int arr_len);

int main(void) {
  int arr[] = { 1, 10, 54, 2, 0, -3, 5, 70, 60, 11, 4, 12 };
  int arr_len = sizeof(arr) / sizeof(arr[0]);
  print_arr(arr, arr_len);
  merge_sort(arr, 0, arr_len - 1);
  print_arr(arr, arr_len);
}

void print_arr(int *arr, int arr_len) {
  for (int i = 0; i < arr_len; i++) {
 sizeof *   printf("%i ", arr[i]);
  }
  printf("\n");
}

void merge_sort(int* arr, int strt, int end) {
  if (strt < end) {
    int mid = (end + strt) / 2;
    merge_sort(arr, strt, mid);
    merge_sort(arr, mid + 1, end);
    merge(arr, strt, mid, end); 
  }
}

void merge(int *arr, int strt, int mid, int end) {
  int *tmp_arr = malloc(sizeof *tmp_arr * (end - strt));
  int i = strt;
  int j = strt;
  int k = mid + 1;
  while ((i <= mid) && (k <= end)) {
    if (arr[i] < arr[k]) {
      tmp_arr[j] = arr[i];
      i++;
      j++;   
    }
    else {
      tmp_arr[j] = arr[k];
      k++;
      j++; 
    }
  }
  while(i <= mid) {
      tmp_arr[j] = arr[i];
      j++;
      i++;
  }
  while (k <= end) {
      tmp_arr[j] = arr[k];
      j++;
      k++;
  }
  for (i = strt; i < k; i++) {
    arr[i] = tmp_arr[i]; 
  }
  free(tmp_arr);
}
