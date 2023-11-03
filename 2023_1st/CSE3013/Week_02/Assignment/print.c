#include "header.h"

/* 배열을 전달받아 그 배열 안에 있는 숫자들을 한칸씩 띄어서 출력해주는 print함수 */

void print(int* arr) {
	for (int i = 0; i < 10; i++) { //배열의 크기는 항상 10이므로 (0부터 9까지 세는 것이므로) 10번만 반복
		printf("%d ", arr[i]); //전달받은 수들을 한 칸씩 띄어서 출력
	}
	printf("\n"); //출력을 완료했으면 줄을 넘겨줌
}
