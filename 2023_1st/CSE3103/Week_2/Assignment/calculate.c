#include "header.h"

/* 1부터 N까지의 숫자를 이루는 숫자를 계산해 배열에 저장해주는 calculate함수 */

void calculate(int* arr) {
	int N, temp;

	scanf("%d", &N); //N을 입력 받아줌

	for (int i = 1; i <= N; i++) { //1부터 N까지의 숫자를 세야하므로 1부터 N까지 반복시켜줌
		temp = i; // i로 계산을 진행하면 반복문이 망가지므로 temp를 도입해 i와 같게 해줌

		while (temp > 0) { //temp가 0보다 작아질때까지 반복 (각 자리수에 해당하는 index에 1만큼 더해주는 것을 완료 할 때까지 반복)
			arr[temp % 10]++; //temp를 10으로 나눈 나머지가 일의 자리수이므로 나머지에 해당하는 index를 1만큼 더해줌

			temp /= 10; //일의 자리 수를 구했으므로 그 수는 필요 없으니 10으로 나눠줌
		}
	}
}

