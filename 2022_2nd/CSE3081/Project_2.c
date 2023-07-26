#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_SIZE 100
#define SWAP(a,b) {int temp = a; a = b; b = temp;}

int partition(int list[], int left, int right) { //주어진 배열을 배열의 맨 오른쪽의 원소보다 작은 값과 큰 값, 총 2개의 배열로 나누는 함수
	int pivot;

	pivot = left;
	for (int i = left; i < right; i++) {
		if (list[i] < list[right]) { //배열의 맨 왼쪽값보다 작으면 그 값을 배열의 오른쪽으로 보내줌
			SWAP(list[i], list[pivot]);
			pivot++;
		}
	}

	SWAP(list[right], list[pivot]); //배열의 맨 값의 위치를 옮겨 왼쪽에는 그 값보다 작은 값들만, 오른쪽에는 큰 값들만 남겨줌
	return (pivot);
}

void merge(int list[], int left, int mid, int right, int N) { //두개의 배열을 입력받아 오름차순으로 정렬된 하나의 배열로 만들어주는 함수
	int a = left;
	int b = mid + 1;
	int c = left;

	int* list2 = (int*)malloc(sizeof(int) * (N + 1)); //두 배열을 합칠 임시 배열을 생성

	while (a <= mid && b <= right) { //두 배열의 처음부터 끝까지 돌면서 둘 중 더 작은값을 임시 배열에 넣어줌
		if (list[a] < list[b]) {
			list2[c] = list[a];
			a++;
			c++;
		}
		else {
			list2[c] = list[b];
			b++;
			c++;
		}
	}

	if (a > mid) { //남은 값들을 임시 배열에 넣어줌
		for (int i = b; i <= right; i++) {
			list2[c++] = list[i];
		}
	}
	else {
		for (int i = a; i <= mid; i++) {
			list2[c++] = list[i];
		}
	}

	for (int i = left; i <= right; i++) { //임시 배열을 그대로 원래의 배열에 넣어줌
		list[i] = list2[i];
	}

	free(list2);
}

int middle(int list[], int left, int right) { //주어진 배열에서 맨 왼쪽, 맨 오른쪽, 가운데 총 3개의 값 중 중간에 있는 값을 찾는 함수
	int middle = (left + right) / 2;

	if (list[left] > list[right]) {
		if (list[middle] > list[left]) {
			return (left);
		}
		else {
			if (list[middle] > list[right]) {
				return (middle);
			}
			else
				return (right);
		}
	}
	else {
		if (list[middle] > list[right]) {
			return (right);
		}
		else {
			if (list[middle] > list[right]) {
				return (middle);
			}
			else {
				return (left);
			}
		}
	}
}

int custom_partition(int list[], int left, int right) { //주어진 배열을 앞서 middle함수에서 찾은 값보다 작은 값과 큰 값, 총 2개의 배열로 나누는 함수
	int pivot;
	int a = middle(list, left, right);

	SWAP(list[a], list[right]);

	pivot = left;
	for (int i = left; i < right; i++) {
		if (list[i] < list[right]) { //middle 함수에서 찾은 값보다 작으면 그 값을 배열의 오른쪽으로 보내줌
			SWAP(list[i], list[pivot]);
			pivot++;
		}
	}

	SWAP(list[right], list[pivot]); //middle 함수에서 찾은 값의 위치를 옮겨 왼쪽에는 그 값보다 작은 값들만, 오른쪽에는 큰 값들만 남겨줌
	return (pivot);
}

void insertion_sort(int list[], int left, int right) { //insertion sort방식으로 배열을 정렬하는 함수
	int a, b, c;

	for (a = left; a <= right; a++) {
		c = list[a];
		for (b = a - 1; b >= 0 && list[b] > c; b--) {
			list[b + 1] = list[b];
		}
		list[b + 1] = c;
	}
}

void quick_sort(int list[], int left, int right) { //quick sort방식으로 배열을 정렬하는 함수
	int pivot;

	if (left < right) {
		pivot = partition(list, left, right);

		quick_sort(list, left, pivot - 1);
		quick_sort(list, pivot + 1, right);
	}
}

void merge_sort(int list[], int left, int right, int N) { //merge sort방식으로 배열을 정렬하는 함수
	int mid;

	if (left < right) {
		mid = (left + right) / 2;

		merge_sort(list, left, mid, N);
		merge_sort(list, mid + 1, right, N);
		merge(list, left, mid, right, N);
	}
}

void custom_sort(int list[], int left, int right) { //내가 생각해 낸 방식으로 배열을 정렬하는 함수
	int pivot;

	if (left < right) {
		if (right - left < 10) {
			insertion_sort(list, left, right);
		}
		else {
			pivot = custom_partition(list, left, right);

			custom_sort(list, left, pivot - 1);
			custom_sort(list, pivot + 1, right);
		}
	}
}

int main(int argc, char* argv[]) {
	int index = atoi(argv[2]);
	char result[MAX_SIZE] = "result_";
	int N;
	FILE* fp2;
	clock_t start = clock();

	FILE* fp1 = fopen(argv[1], "r");

	fscanf(fp1, "%d", &N);
	int* list = malloc(sizeof(int) * N);
	for (int i = 0; i < N; i++) { //데이터를 배열에 저장해줌
		fscanf(fp1, "%d ", &list[i]);
	}

	if (index == 1) {
		insertion_sort(list, 0, N - 1);
		strcat(result, "1_");
	}
	else if (index == 2) {
		quick_sort(list, 0, N - 1);
		strcat(result, "2_");
	}
	else if (index == 3) {
		merge_sort(list, 0, N - 1, N);
		strcat(result, "3_");
	}
	else if (index == 4) {
		custom_sort(list, 0, N - 1);
		strcat(result, "4_");
	}
	
	clock_t finish = clock();
	double time = ((double)(finish - start) / CLOCKS_PER_SEC);

	strcat(result, argv[1]);
	fp2 = fopen(result, "w");
	fprintf(fp2, "%s\n%d\n%d\n%f\n", argv[1], index, N, time);
	for (int i = 0; i < N; i++) {
		fprintf(fp2, "%d ", list[i]);
	}

	free(list);
	fclose(fp1);
	fclose(fp2);
	return 0;
}
