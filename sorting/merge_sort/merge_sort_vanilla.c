#include <stdio.h>
#include <stdlib.h>

void merge_sort(int *arr, int strt, int end);
void merge(int *arr, int strt, int mid, int end);
void print_arr(int *arr, int arr_len);

int main(void) {
  int arr[] = { 8, 1, 10, 54, 2, 0, -3, 5, 70, 60, 11, 4 };
  int arr_len = sizeof(arr) / sizeof(arr[0]); 
  print_arr(arr, arr_len);
  merge_sort(arr, 0, arr_len);
  print_arr(arr, arr_len);
}

void print_arr(int *arr, int arr_len) {
  for (int i = 0; i < arr_len; i++) {
    printf("%i ", arr[i]);
  }
  printf("\n");
}

void merge_sort(int *arr, int strt, int end) {
  if (end - strt > 1) {
    // avoid potential arithmetic overflow on start+end
    //for more info see:
    //https://ai.googleblog.com/2006/06/extra-extra-read-all-about-it-nearly.html
    //https://thebittheories.com/the-curious-case-of-binary-search-the-famous-bug-that-remained-undetected-for-20-years-973e89fc212
    int mid = strt + (end - strt) / 2;
    merge_sort(arr, strt, mid);
    merge_sort(arr, mid, end);
    merge(arr, strt, mid, end); 
  }
}

void merge(int *arr, int strt, int mid, int end) {
  int *tmp_arr = malloc(sizeof(*tmp_arr) * (end - strt));
  //int tmp_arr[end - strt];
  int i = strt;
  int j = 0;
  int k = mid;
  while (i < mid && k < end) {
    if (arr[i] <= arr[k]) {
      tmp_arr[j++] = arr[i++];
    } else {
      tmp_arr[j++] = arr[k++];
    }
  }
  while (i < mid) {
      tmp_arr[j++] = arr[i++];
  }
  // copying the remaining elements from the right half is not needed as they
  // are already in the proper place
  //while (k < end) {
  //    tmp_arr[j++] = arr[k++];
  //}
  for (int m = 0; m < j; m++) {
    arr[strt + m] = tmp_arr[m];
  }
  free(tmp_arr);
}
