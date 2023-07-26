#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_SIZE 100ㅁ
#define COMPARE(x, y) ((x<y) ? -1 : (x==y) ? 0 : 1)

typedef struct {
	float coef;
	int expon;
} polynomial; //ADT poly를 구조체 배열 형식으로 정의

polynomial terms[MAX_SIZE];
FILE* A, * B, * C;
int avail = 0;
int starta, finisha, startb, finishb, startc, finishc, startd, finishd, starte, finishe;

void attach(float coefficient, int exponent) { //입력받은 계수와 지수를 구조체 배열 맨 뒤에 추가해주는 함수 
	if (avail >= MAX_SIZE) {
		fprintf(stderr, "Too many terms in the polynomial");
		exit(1);
	}
	terms[avail].coef = coefficient;
	terms[avail].expon = exponent;
	avail++;
}

void readPoly() { //A.txt, B.txt, C.txt를 입력받아 구조체 배열 형식으로 저장하는 함수
	int exponent;
	float coefficient;

	A = fopen("A.txt", "r");
	B = fopen("B.txt", "r");
	C = fopen("C.txt", "r"); //파일 3개를 입력받음

	starta = 0; //맨 처음 입력받을 A.txt의 시작을 0으로 정의
	while (fscanf(A, "%f %d", &coefficient, &exponent) != EOF) { //파일이 끝날때까지 계수와 지수를 입력받음
		terms[avail].coef = coefficient;
		terms[avail].expon = exponent;
		avail++;
	}
	finisha = avail - 1; //입력받은 곳까지를 다항식A의 끝으로 정의

	startb = avail;
	while (fscanf(B, "%f %d", &coefficient, &exponent) != EOF) { //B.txt를 입력받음
		terms[avail].coef = coefficient;
		terms[avail].expon = exponent;
		avail++;
	}
	finishb = avail - 1;

	startc = avail;
	while (fscanf(C, "%f %d", &coefficient, &exponent) != EOF) { //C.txt를 입력받음
		terms[avail].coef = coefficient;
		terms[avail].expon = exponent;
		avail++;
	}
	finishc = avail - 1;

	fclose(A);
	fclose(B);
	fclose(C); //파일을 닫아줌
}

void padd1(int starta, int finisha, int startb, int finishb, int* startd, int* finishd) { //A와 B를 더해 D에 저장하는 함수
	float coefficient;

	*startd = avail; //D에 저장해야 하므로 D의 시작을 맨 뒤로 정의
	while (starta <= finisha && startb <= finishb) //A 혹은 B가 끝날때까지 실행
		switch (COMPARE(terms[starta].expon, terms[startb].expon)) { //두 식의 지수를 각각 큰 것부터 비교함
		case -1: //B의 지수가 더 크면
			attach(terms[startb].coef, terms[startb].expon); //B를 D에 저장하고
			startb++; //다음으로 넘어감
			break;
		case 0: //둘의 지수가 같으면
			coefficient = terms[starta].coef + terms[startb].coef; //둘의 계수를 더해서
			if (coefficient)
				attach(coefficient, terms[starta].expon); //D에 저장하고
			starta++;
			startb++; //둘 다 다음으로 넘어감
			break;
		case 1: //A의 지수가 더 크면
			attach(terms[starta].coef, terms[starta].expon); //A를 D에 저장하고
			starta++; //다음으로 넘어감
			break;
		}

	for (; starta <= finisha; starta++)
		attach(terms[starta].coef, terms[starta].expon);
	for (; startb <= finishb; startb++) //while문이 다 끝나고 남은 A 혹은 B를 그대로 D에 저장함
		attach(terms[startb].coef, terms[startb].expon);
	*finishd = avail - 1; //저장된 곳까지를 D의 끝으로 정의
}

void padd2(int startc, int finishc, int startd, int finishd, int* starte, int* finishe) { //C와 D를 더해 E에 저장하는 함수
	float coefficient;

	*starte = avail;
	while (startc <= finishc && startd <= finishd)
		switch (COMPARE(terms[startc].expon, terms[startd].expon)) {
		case -1:
			attach(terms[startd].coef, terms[startd].expon);
			startd++;
			break;
		case 0:
			coefficient = terms[startc].coef + terms[startd].coef;
			if (coefficient)
				attach(coefficient, terms[startc].expon);
			startc++;
			startd++;
			break;
		case 1:
			attach(terms[startc].coef, terms[startc].expon);
			startc++;
			break;
		}

	for (; startc <= finishc; startc++)
		attach(terms[startc].coef, terms[startc].expon);
	for (; startd <= finishd; startd++)
		attach(terms[startd].coef, terms[startd].expon);
	*finishe = avail - 1;
}

void printPoly(int starte, int finishe) { //3개를 다 더한 E를 출력해주는 함수
	int i;

	for (i = starte; i < finishe; i++)
		printf("(%.0f,%d), ", terms[i].coef, terms[i].expon);
	printf("(%.0f,%d)\n", terms[finishe].coef, terms[finishe].expon);
}

int main(void) {
	double time;
	clock_t start = clock();

	readPoly();
	padd1(starta, finisha, startb, finishb, &startd, &finishd);
	padd2(startc, finishc, startd, finishd, &starte, &finishe);
	printPoly(starte, finishe);

	clock_t finish = clock();
	time = ((double)(finish - start) / CLOCKS_PER_SEC); //코드 전체의 실행시간 계산
	printf("[Finished in %.3fs]", time);

	return 0;
}
