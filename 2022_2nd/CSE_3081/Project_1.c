#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX_SIZE 100

FILE* fp2;

int maxsubsequencesum(int* list, int n) { // 1차원 배열에서 부분합의 최대값을 찾는 함수
	int thissum = 0;
	int maxsum = 0;
	int i;

	for (i = 0; i < n; i++) {
		thissum += list[i];
		if (thissum < 0) {
			thissum = 0;

		}
		else if (thissum > maxsum) {
			maxsum = thissum;
		}
	}
	return maxsum;
}

int index1(char fname[MAX_SIZE]) {
	char result[MAX_SIZE] = "result_";
	int row, column, thissum;
	int maxsum = 0;
	int index = 1;

	FILE* fp = fopen(fname, "r"); //파일 이름 입력 받기
	fscanf(fp, "%d %d", &row, &column); //2차원 배열 행과 열 크기 입력 받기

	int** list = malloc(sizeof(int*) * row);
	for (int i = 0; i < row; i++) {
		list[i] = malloc(sizeof(int) * column);
	} //2차원 배열 동적 할당

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			fscanf(fp, "%d", &list[i][j]);
		}
	} //2차원 배열 입력 받기

	for (int i1 = 0; i1 < row; i1++) {
		for (int i2 = i1; i2 < row; i2++) {
			for (int j1 = 0; j1 < column; j1++) {
				for (int j2 = j1; j2 < column; j2++) {
					thissum = 0;
					for (int k1 = i1; k1 <= i2; k1++) {
						for (int k2 = j1; k2 <= j2; k2++) {
							thissum += list[k1][k2];
						}
					}
					if (thissum > maxsum) {
						maxsum = thissum;
					}
				}
			}

		}
	} //부분합의 최대값 구하기

	strcat(result, fname);
	fp2 = fopen(result, "w");
	fprintf(fp2, "%s\n%d\n%d\n%d\n%d\n", fname, index, row, column, maxsum);

	for (int i = 0; i < row; i++) {
		free(list[i]);
	}
	free(list); //2차원 배열 공간 해제
	fclose(fp); //포인터 해제
	return 0;
}

int index2(char fname[MAX_SIZE]) {
	char result[MAX_SIZE] = "result_";
	int row, column, thissum;
	int temp = 0;
	int maxsum = 0;
	int index = 2;

	FILE* fp = fopen(fname, "r"); //파일 이름 입력 받기
	fscanf(fp, "%d %d", &row, &column); //2차원 배열 행과 열 크기 입력 받기
	
	int** list1 = malloc(sizeof(int*) * row);
	for (int i = 0; i < row; i++) {
		list1[i] = malloc(sizeof(int) * column);
	} //2차원 배열 동적 할당

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			fscanf(fp, "%d", &list1[i][j]);
		}
	} //2차원 배열 입력 받기

	int** list2 = malloc(sizeof(int*) * row);
	for (int i = 0; i < row; i++) {
		list2[i] = malloc(sizeof(int) * column);
	} //2차원 누적합 배열 동적 할당

	for (int i1 = 0; i1 < row; i1++) {
		for (int j1 = 0; j1 < column; j1++) {
			for (int i2 = 0; i2 <= i1; i2++) {
				for (int j2 = 0; j2 <= j1; j2++) {
					temp = temp + list1[i2][j2];
				}
			}
			list2[i1][j1] = temp;
			temp = 0;
		}
	} //2차원 누적합 배열 입력 받기

	for (int i1 = 0; i1 < row; i1++) {
		for (int i2 = i1; i2 < row; i2++) {
			for (int j1 = 0; j1 < column; j1++) {
				for (int j2 = j1; j2 < column; j2++) {
					if (i1 == 0 && j1 == 0) {
						thissum = list2[i2][j2];
					}
					else if (i1 == 0) {
						thissum = list2[i2][j2] - list2[i2][j1 - 1];
					}
					else if (j1 == 0) {
						thissum = list2[i2][j2] - list2[i1 - 1][j2];
					}
					else {
						thissum = list2[i2][j2] - list2[i1 - 1][j2] - list2[i2][j1 - 1] + list2[i1 - 1][j1 - 1];
					}

					if (thissum > maxsum) {
						maxsum = thissum;
					}
				}
			}

		}
	} //부분합의 최대값 구하기
	
	strcat(result, fname);
	fp2 = fopen(result, "w");
	fprintf(fp2, "%s\n%d\n%d\n%d\n%d\n", fname, index, row, column, maxsum);

	for (int i = 0; i < row; i++) {
		free(list1[i]);
	}
	free(list1); //2차원 배열 공간 해제
	for (int i = 0; i < row; i++) {
		free(list2[i]);
	}
	free(list2); //2차원 누적합 공간 해제
	fclose(fp); //포인터 해제
	return 0;
}

int index3(char fname[MAX_SIZE]) {
	char result[MAX_SIZE] = "result_";
	int row, column, thissum;
	int maxsum = 0;
	int index = 3;

	FILE* fp = fopen(fname, "r"); //파일 이름 입력 받기
	fscanf(fp, "%d %d", &row, &column); //2차원 배열 행과 열 크기 입력 받기

	int** list = malloc(sizeof(int*) * row);
	for (int i = 0; i < row; i++) {
		list[i] = malloc(sizeof(int) * column);
	} //2차원 배열 동적 할당

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			fscanf(fp, "%d", &list[i][j]);
		}
	} //2차원 배열 입력 받기

	int* temp = malloc(sizeof(int) * row); //1차원 배열 동적 할당

	for (int i = 0; i < column; i++) {
		memset(temp, 0, sizeof(int) * row);
		for (int j = i; j < column; j++) {
			for (int k = 0; k < row; k++) {
				temp[k] += list[k][j];
			}
			thissum = maxsubsequencesum(temp, row);
			if (thissum > maxsum) {
				maxsum = thissum;
			}
		}
	}
	
	strcat(result, fname);
	fp2 = fopen(result, "w");
	fprintf(fp2, "%s\n%d\n%d\n%d\n%d\n", fname, index, row, column, maxsum);

	for (int i = 0; i < row; i++) {
		free(list[i]);
	}
	free(list); //2차원 배열 공간 해제
	free(temp); //1차원 배열 공간 해제
	fclose(fp); //포인터 해제
	return 0;
}

int main(int argc, char* argv[]) {
	int index = atoi(argv[2]);
	clock_t start = clock();

	if (index == 1) {
		index1(argv[1]);
	}
	else if(index == 2) {
		index2(argv[1]);
	}
	else if (index == 3) {
		index3(argv[1]);
	}
	clock_t finish = clock();
	double time = ((double)(finish - start) / CLOCKS_PER_SEC);

	fprintf(fp2,"%f", time);

	fclose(fp2);
	return 0;
}
