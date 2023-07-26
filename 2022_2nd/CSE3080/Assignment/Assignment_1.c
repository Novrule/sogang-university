#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
ㅁ
void insertion_sort(int N, int list[]) { //insertion_sort방식으로 입력받은 배열을 정렬해주는 함수
	int a, b, c;
	for (a = 1; a < N; a++) { //첫번째는 정렬되어 있으므로 바로 두번째로 감
		c = list[a]; //a번째 값을 c에 저장함
		for (b = a - 1; b >= 0 && list[b] > c; b--) { //b가 0보다 작아지거나 b번째 값이 앞에서 저장한 c보다 작아질때까지 1씩 줄이며 반복함
			list[b + 1] = list[b]; //c보다 b번째 값이 더 크다면 b번째 값을 한칸 뒤로 옮김
		}
		list[b + 1] = c; //c값을 c값보다 큰 값이 모두 뒤로가게 만든 위치에 저장함
	}
}

int main(void) {
	int a, b, c, d, e, g, n, m, o;
	double f, h;
	int* list;
	int* worst;
	int* backup;


	clock_t all = clock();

	FILE* fp = fopen("lab1.data", "r"); //lab1.data를 읽기 모드로 열기
	list = (int*)calloc(1, sizeof(int) * 2); //정수값 하나를 받을수 있을 만큼 동적 메모리 할당

	if (list != NULL) { //메모리 동적 할당에 성공하면
		a = 0;
		while (fscanf(fp, "%d", &list[a]) != EOF) { //lab1.data의 데이터가 끝날 때까지
			list = (int*)realloc(list, sizeof(int) * (a + 1)); //데이터가 입력될 때마다 동적메모리의 크기를 하나씩 키움
			a++;
		}
	}
	else { //메모리 동적 할당에 실패하면
		printf("Memory allocation error!"); //실패했다는 메시지 출력
		exit(-1); //코드 종료
	}
	
	rewind(fp); //동적 메모리를 할당하면서 뒤로 옮겨진 파일 포인터를 맨 앞으로 옮김
	for (g = 0; g < a; g++) {
		fscanf(fp, "%d", &list[g]); //데이터의 크기만큼 할당 된 동적메모리에 lab1.data.의 값을 입력함
	}

	printf("Input : ");
	for (b = 0; b < a; b++) { //입력받은 값을 출력
		printf("%d ", list[b]);
	}
	printf("\n");

	insertion_sort(a, list); //입력받은 값을 insertion sort로 정렬
	printf("Sorted : ");
	for (c = 0; c < a; c++) { //insertion sort로 정렬한 값을 출력
		printf("%d ", list[c]);
	}
	printf("\n");

	printf("  n repetitions     time\n");
	for (n = 0; n < 11; n++) { //0부터 100까지 총 11번 실행
		d = 0;
		o = 10 * n; //10의 간격으로 실행
		worst = (int*)calloc(o + 1, sizeof(int));
		backup = (int*)calloc(o + 1, sizeof(int)); //최악의 경우를 상정해 돌릴 크기만큼 동적메모리 할당
		for (m = 0; m < o; m++) {
			backup[m] = o - m; //최악의 경우를 상정해 동적메모리에 입력
		}

		clock_t start = clock();
		clock_t end;
		do {
			d++;
			for (e = 0; e < o; e++) {
				worst[e] = backup[e]; //정렬한 값을 다시 최악의 경우로 바꿔줌
			}
			insertion_sort(o, worst);
			end = clock();
		} while (end - start < 100); //정해진 시간 안에 몇번 도는지 계산
		f = ((double)(end - start)) / CLOCKS_PER_SEC;
		f /= d; //시간을 작동된 횟수로 나눠 한 번 작동하는데 걸리는 시간 계산
		printf("%3d %11d %f\n", o, d, f);
		
		free(worst);
		free(backup); //다음 단계에서 다른 동적 메모리를 할당해야 하므로 초기화
	}

	fclose(fp);
	free(list);

	clock_t finish = clock();
	h = ((double)(finish - all) / CLOCKS_PER_SEC); //코드 전체의 실행시간 계산
	printf("[Finished in %.1fs]", h);

	return 0;
}
