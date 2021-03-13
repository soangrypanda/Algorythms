#ifndef MERGE_SORT_VANILLA_SENTRY
#define MERGE_SORT_VANILLA_SENTRY


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

void merge_sort	(int32_t *arr, uint32_t strt, uint32_t len);
void merge	(int32_t *arr, uint32_t strt, uint32_t mid, uint32_t len);


void merge_sort(int32_t *arr, uint32_t strt, uint32_t len) {
	if (len - strt > 1) {
	/* avoid potential arithmetic overflow on start+end
     	 * for more info see:
     	 * https://ai.googleblog.com/2006/06/extra-extra-read-all-about-it-nearly.html
     	 * https://thebittheories.com/the-curious-case-of-binary-search-the-famous-bug-that-remained-undetected-for-20-years-973e89fc212
	 */
		uint32_t mid = strt + (len - strt) / 2;
		merge_sort(arr, strt, mid);
		merge_sort(arr, mid, len);
		merge(arr, strt, mid, len); 
	}
}

void merge(int32_t *arr, uint32_t strt, uint32_t mid, uint32_t len) {
	int32_t *tmp_arr = malloc(sizeof(*tmp_arr) * (len - strt));
	uint32_t i = strt;
	uint32_t j = 0;
 	uint32_t k = mid;

	while (i < mid && k < len) {
 		if (arr[i] <= arr[k])	tmp_arr[j++] = arr[i++];
		else			tmp_arr[j++] = arr[k++];
	}
  	
	while (i < mid) {
		tmp_arr[j++] = arr[i++];
	}
	/* copying the remaining elements from the right half is not needed as they
	 * are already in the proper place
	while (k < end) {
		tmp_arr[j++] = arr[k++];
	} 
	 */
	for (size_t m = 0; m < j; m++) {
		arr[strt + m] = tmp_arr[m];
	}

	free(tmp_arr);
}


#endif
