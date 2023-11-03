#include "header.h"

/* 테스트 케이스가 몇 개 인지 입력 받고 그 횟수만큼 calculate함수와 print함수를 실행해주는 main함수 */

int main(void) {
	int T;

	scanf("%d", &T); //테스트 케이스를 입력 받아줌

	while (T--) { //입력 받은 테스트 케이스의 횟수만큼 반복해줌
		int arr[10] = { 0, }; //0~9까지 몇 번 씩 나오는지 저장해주는 배열 초기화

		calculate(arr); //1부터 N까지의 숫자를 이루는 숫자를 계산해 배열에 저장해주는 calculate함수를 호출함

		print(arr); //배열을 전달받아 그 배열 안에 있는 숫자들을 한칸씩 띄어서 출력해주는 print함수를 호출함
	}

	return 0; //종료
}

