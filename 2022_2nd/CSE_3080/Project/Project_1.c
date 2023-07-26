#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>ㅁ
#include <stdlib.h>
#include <string.h>

typedef struct stack *stack_pointer;
typedef struct stack {
	char item;
	stack_pointer link;
}stack; //링크드 리스트 스택 정의

stack_pointer top;

void push(stack_pointer *top, char item) { //스택의 맨 위에 item을 넣는 함수 정의
	stack_pointer temp = (stack_pointer)malloc(sizeof(stack));
	temp->item = item;
	temp->link = *top;
	*top = temp;
}

int is_empty(stack_pointer *top) { //스택이 비어있으면 1을 반환하는 함수 정의
	return(*top == NULL);
}

char pop(stack_pointer *top) { //스택에 맨 위에 있는 item을 반환하는 함수 정의
	if (is_empty(top)) {
		return -1;
	}
	stack_pointer temp = *top;
	char item = temp->item;
	*top = temp->link;
	free(temp);
	return item;
}

void main(void) {
	int value = 0;
	int j = 1;
	int count = 0;
	char symbol[30]; //입력받을 문자열의 최대 크기가 30이므로 크기를 30으로 지정
	int subsymbol[30] = { 0 }; //subsymbol을 0으로 초기화
	
	scanf("%s", symbol); //괄호값을 계산할 문자열 입력 받기

	for (int i = 0; symbol[i] != '\0'; i++) { 
		if (symbol[i] == '(' || symbol[i] == '[') { //(이나 [이 들어오면 스택에 저장
			push(&top, symbol[i]);
		}
		else if (symbol[i] == ')') { //)이 들어왔을 때
			if (pop(&top) == '(') { //스택에 저장되어 있던 맨 위의 값이 (일 때만 올바른 괄호임
				value++;
			}
			else { //스택에 저장되어 있던 맨 위의 값이 [이거나 스택이 끝난 것이므로 올바르지 않은 괄호
				value = 0;
				break;
			}
		}
		else {
			if (pop(&top) == '[') {
				value++;
			}
			else {
				value = 0;
				break;
			}
		}
	}
	if (!(is_empty(&top))) { //스택에 남아있는 괄호가 있어도 올바르지 않는 괄호
		value = 0;
	}

	if (value == 0) { //올바르지 않는 괄호인 경우 0을 출력함
		printf("%d", value);
	}
	else { //올바른 괄호인 경우
		for (int i = 0; symbol[i] != '\0'; i++) { //괄호는 char형 인자이고 괄호 값은 int형 인자이므로 괄호를 각각 -1, -2, -3, -4로 int형 배열에 저장함
			if (symbol[i] == '(') {
				subsymbol[i] = -1;
			}
			else if (symbol[i] == ')') {
				subsymbol[i] = -2;
			}
			else if (symbol[i] == '[') {
				subsymbol[i] = -3;
			}
			else {
				subsymbol[i] = -4;
			}
		}
		
		value = 0;

		while (1) {
			for (int i = 0; i < 30; i++) {
				if (subsymbol[i] > 0) { //저장된 값이 0보다 클 경우 괄호값임
					j++; //j는 괄호와 그 이전에 있는 괄호 사이의 거리임
					for (int k = i - 1; k > i - j + 1; k--) { //괄호 사이에 두 개의 괄호값이 있는 경우 두 괄호값을 더해줌
						if (subsymbol[k] < 0) {
							break;
						}
						else if (subsymbol[k] > 0) {
							subsymbol[i] += subsymbol[k];
							subsymbol[k] = 0;
						}
					}
				}
				else if (subsymbol[i] == -1) {
					j = 1;
				}
				else if (subsymbol[i] == -2) {
					if (subsymbol[i - j] == -1) { //바로 이전에 있는 괄호가 (일 때
						for (int k = i - j + 1; k <= i; k++) {
							if (subsymbol[k] > 0) { //사이에 계산된 괄호 값이 있으면 2를 곱해서 저장함
								subsymbol[i - j] = 2 * subsymbol[k];
								subsymbol[k] = 0;
								break;
							}
							else { //없으면 2를 더해서 저장함
								subsymbol[i - j] = 2;
							}
						}
						subsymbol[i] = 0;
					}
					j = 1;
				}
				else if (subsymbol[i] == -3) {
					j = 1;
				}
				else if (subsymbol[i] == -4) {
					if (subsymbol[i - j] == -3) {
						for (int k = i - j + 1; k <= i; k++) {
							if (subsymbol[k] > 0) {
								subsymbol[i - j] = 3 * subsymbol[k];
								subsymbol[k] = 0;
								break;
							}
							else {
								subsymbol[i - j] = 3;
							}
						}
						subsymbol[i] = 0;
					}
					j = 1;
				}
				else {
					j++;
				}
			}

			for (int i = 0; i < 30; i++) { //계산을 다 한 후 배열에 존재하는 양수의 값이 몇 개인지 계산함
				if (subsymbol[i] > 0) {
					count++;
				}
				else if (subsymbol[i] < 0) { //음수 값이 존재하면 한 번 더 계산해야 함
					count = 100;
				}
				value += subsymbol[i];
			}

			if (count == 1) { //양수의 값이 1개이면 계산이 완료된 것이므로 그때의 값을 출력함
				printf("%d", value);
				break;
			}
			else {
				count = 0;
				value = 0;
			}
		}
	}
}
