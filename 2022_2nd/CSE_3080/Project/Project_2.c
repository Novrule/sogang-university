#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 1000000

typedef struct node* pointer;
typedef struct node {
	pointer left;
	int data;
	int level;
	pointer right;
}node; //Binary search tree를 구조체 배열 형식으로 정의

void preorder(pointer ptr) { //preorder방식으로 binary search tree를 읽어주는 함수
	if (ptr) {
		printf("%d", ptr->data);
		printf("(%d) ", ptr->level);
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

void insert(pointer* node, int data, int level) { //주어진 binary search tree에 node를 추가해주는 함수
	pointer ptr; 
	pointer temp = modifiedsearch(*node, data); //data의 parent node를 찾아줌

	if (temp || !(*node)) {
		ptr = (pointer)malloc(sizeof(*ptr));
		ptr->data = data;
		ptr->level = level;
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

void postorder_tree(pointer* tree, int A[], int N, int level) { //배열에 기록되어 있는 후위 순회 결과를 가지고 binary search tree를 구성하는 함수
	if (N == 1) { //배열의 원소가 하나라면 바로 binary search tree에 추가 해줌
		insert(tree, A[N - 1], level++);
	}
	else if (N > 1) { //배열의 원소가 하나가 아니라면 맨 마지막 원소는 binary search tree에 추가해주고
		int left = 0;
		int right = 0;
		int* rightarr = (int*)malloc(sizeof(int) * N);
		int* leftarr = (int*)malloc(sizeof(int) * N);

		insert(tree, A[N - 1], level++);

		for (int i = 0; i < N - 1; i++) { //배열에 있는 원소중 추가해준 원소보다 큰 원소는 추가해준 원소의 right child 작은 원소는 left child임
			if (A[i] > A[N - 1]) {
				rightarr[right++] = A[i];
			}
			else {
				leftarr[left++] = A[i];
			}
		}

		postorder_tree(tree, rightarr, right, level); //앞에서 나눈 right child들과 left child들로 함수를 한번 더 돌려줌
		postorder_tree(tree, leftarr, left, level);

		free(rightarr);
		free(leftarr);
	}
	else {
		printf("Error!");
	}
}

int main(void) {
	int M;
	int N = 0;
	int arr[MAX_SIZE] = { 0 };
	pointer tree = NULL;
	
	do { //후위 순회 결과를 배열에 저장해줌
		scanf("%d", &M);
		arr[N++] = M;
	} while (getc(stdin) == ' ');

	postorder_tree(&tree, arr, N, 1);

	printf("preorder : ");
	preorder(tree);

	return 0;
}
