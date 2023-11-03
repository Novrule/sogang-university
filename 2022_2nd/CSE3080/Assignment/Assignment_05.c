#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100

typedef struct node* pointer;
typedef struct node {
	pointer left;
	int data;
	pointer right;
}node; //Binary search tree를 구조체 배열 형식으로 정의

void inorder(pointer ptr) { //Inorder방식으로 binary search tree를 읽어주는 함수
	if (ptr) {
		inorder(ptr->left);
		printf("%d ", ptr->data);
		inorder(ptr->right);
	}

}

void preorder(pointer ptr) { //preorder방식으로 binary search tree를 읽어주는 함수
	if (ptr) {
		printf("%d ", ptr->data);
		preorder(ptr->left);
		preorder(ptr->right);
	}

}

pointer modifiedsearch(pointer ptr, int data) { //주어진 binary search tree에서 삽입될 data의 parent node를 찾아주는 함수
	pointer temp = ptr;
	while (ptr) {
		temp = ptr;
		if (data < ptr->data) { //삽입될 node의 data가 현재의 node의 data보다 작으면 left child로 내려감
			ptr = ptr->left;
		}
		else if (data > ptr->data) { //삽입될 node의 data가 현재의 node의 data보다 크면 right child로 내려감
			ptr = ptr->right;
		}
		else {
			return NULL;
		}
	}
	return temp; //다음 node가 삽입될 곳의 주소값을 반환함
}

void insert(pointer* node, int data) { //주어진 binary search tree에 node를 추가해주는 함수
	pointer ptr;
	pointer temp = modifiedsearch(*node, data); //data의 parent node를 찾아줌

	if (temp || !(*node)) {
		ptr = (pointer)malloc(sizeof(*ptr));
		ptr->data = data;
		ptr->left = ptr->right = NULL;
		if (*node) {
			if (data < temp->data) { //parent node가 주어진 data보다 크면 data를 left child로 지정함
				temp->left = ptr;
			}
			else { //parent node가 주어진 data보다 작으면 data를 right child로 지정함
				temp->right = ptr;
			}
		}
		else { //주어진 binary search tree가 비어있다면 추가할 node를 root node로 지정
			*node = ptr; 
		}
	}
}

int main(void) {
	int N;
	int A[MAX_SIZE];
	pointer tree = NULL;
	FILE* fp = fopen("input.txt", "r");

	fscanf(fp, "%d", &N);
	for (int i = 0; i < N; i++) { //주어진 값들을 임시 배열에 저장함
		fscanf(fp, "%d", &A[i]);
	}

	for (int i = 0; i < N; i++) { //임시 배열에 저장된 값들을 구조체 배열 형식의 binary search tree에 다시 저장함
		insert(&tree, A[i]);
	}
	
	printf("inorder : ");
	inorder(tree);
	printf("\npreorder : ");
	preorder(tree);

	fclose(fp);

	return 0;
}
