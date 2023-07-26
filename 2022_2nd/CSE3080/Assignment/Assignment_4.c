#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 100

int nfind(char* string, char* pattern) { //주어진 문자열의 처음부터 하나하나씩 대조해가며 주어진 문자열에서 패턴을 찾는 함수
	int i;
	int lasts = strlen(string);
	int lastp = strlen(pattern) - 1;
	int endmatch = lastp;
	int start = 0;
	int j = 0;
	int k = 0;

	for (; endmatch < lasts; endmatch++, start++) {
		k++;
		if (string[endmatch] == pattern[lastp]) { //주어진 문자열과 패턴의 끝부분이 같으면
			for (i = start, j = 0; j < lastp && string[i] == pattern[j]; i++, j++) { //패턴의 맨 처음부터 하나하나씩 대조해보기 시작함
				k++;
			}
		}
		if (j == lastp) { //문자열이 끝나기 전에 패턴의 끝까지 매칭이 완료되면 패턴 매칭에 성공
			printf("nfind : YES ");
			break;
		}
	}
	if (j != lastp) { //패턴의 끝까지 가기 전에 문자열이 끝나면 패턴 매칭에 실패
		printf("nfind : NO ");
	}
	printf("%d\n", k);

	return 0;
}

int KMP(char* string, char* pattern) { //KMP알고리즘을 통해서 주어진 문자열에서 패턴을 찾는 함수
	int i, j;
	char failure[MAX_SIZE];
	int k = 0;
	int lens = strlen(string);
	int lenp = strlen(pattern);

	failure[0] = -1;
	for (j = 1; j < lenp; j++) { //패턴을 읽으면서 실패 문자열을 작성함 
		k++;
		i = failure[j - 1];
		while ((pattern[j] != pattern[i + 1]) && (i >= 0)) {
			i = failure[i];
		}
		if (pattern[j] == pattern[i + 1]) {
			failure[j] = i + 1;
		}
		else {
			failure[j] = -1;
		}
	}

	i = 0;
	j = 0;
	while (i < lens && j < lenp) {
		k++;
		if (string[i] == pattern[j]) { //주어진 문자열과 패턴이 같으면 한칸씩 뒤로감
			i++;
			j++;
		}
		else if (j == 0) { //패턴의 맨 앞까지 가면 문자열만 한칸 뒤로감
			i++;
		}
		else {
			j = failure[j - 1] + 1; //나머지의 경우에는 실패 문자열을 통해 대조할 필요가 없는 부분을 뛰어 넘어줌
		}
	}

	if (j == lenp) { //패턴의 끝까지 매칭이 완료되면 패턴 매칭에 성공
		printf("kmp : YES ");
	}
	else { //패턴의 끝까지 매칭이 완료되지 못하면 패턴 매칭에 실패
		printf("kmp : NO ");
	}
	printf("%d\n", k);

	return 0;
}

int main(void) {
	char string[MAX_SIZE], pattern[MAX_SIZE];

	scanf("%s", &string);
	scanf("%s", &pattern);

	nfind(string, pattern);
	KMP(string, pattern);

	return 0;
}
