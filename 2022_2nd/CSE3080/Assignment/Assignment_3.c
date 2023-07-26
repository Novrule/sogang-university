#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100

typedef struct {
	int row;
	int column;
	int value;
} matrix; //희소 행렬을 구조체 배열 형식으로 정의

int row, column;

int readmatrix(FILE* fp, matrix A[]) { //희소 행렬을 입력받아 구조체에 저장해주는 함수
	int avail = 0;

	fscanf(fp, "%d %d", &row, &column);

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
				fscanf(fp, "%d", &A[avail].value);
				A[avail].row = i;
				A[avail].column = j;
				avail++;
		}
	}
	return 0;
}

int matrixadd(matrix A[], matrix B[], matrix C[]) { //구조체에 저장되어있는 두 희소 행렬을 더해주는 함수
	int avail = 0;
	int nonzero = 0;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			C[avail].row = i;
			C[avail].column = j;
			C[avail].value = A[avail].value + B[avail].value; //두 행렬을 더한 값이 0이어도 상관 없이 구조체에 저장해 줌
			avail++;
		}
	}

	for (int i = 0; i < avail; i++) {
		if (C[i].value != 0) {
			nonzero++; //두 행렬을 더한 값이 0이 아닌 것들의 갯수를 세어줌
		}
	}
	C[0].row = row;
	C[0].column = column;
	C[0].value = nonzero;
	return 0;
}

int printmatrix(matrix A[]) { //구조체에 저장되어 있는 희소 행렬을 프린트 해주는 함수
	int avail = 0;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			if (A[avail].value != 0) { //값이 0이 아닌 것들만 읽어줌
				printf("%d %d %d\n", A[avail].row, A[avail].column, A[avail].value);
			}
			avail++;
		}
	}
	return 0;
}

int main(void) {
	matrix A[MAX_SIZE];
	matrix B[MAX_SIZE];
	matrix C[MAX_SIZE];

	FILE* fp1 = fopen("A.txt", "r");
	readmatrix(fp1, A);
	FILE* fp2 = fopen("B.txt", "r");
	readmatrix(fp2, B);

	matrixadd(A, B, C);
	printmatrix(C);

	fclose(fp1);
	fclose(fp2);
	return 0;
}
