#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 50

typedef struct stack* stack_pointer;
typedef struct stack {
	int item;
	stack_pointer link;
}stack; //링크드 리스트 스택 정의

stack_pointer top;

typedef struct graph_vertex {
    int vertex;
    struct graph_vertex* link;
} graph_vertex; //그래프의 vertex를 구조체 배열 형식으로 정의

typedef struct graph_edge {
    int n;
    graph_vertex* adj_list[MAX_SIZE];
} graph_edge; //그래프의 edge를 구조체 배열 형식으로 정의

int is_empty(stack_pointer* top) { //링크드 리스트 스택이 비어있는지 확인해주는 함수
	return(*top == NULL);
}

void push(stack_pointer* top, int item) { //링크드 리스트 스택에 아이템을 추가해주는 함수
	stack_pointer temp = (stack_pointer)malloc(sizeof(stack));
	temp->item = item;
	temp->link = *top;

	*top = temp;
}

int pop(stack_pointer* top) { //링크드 리스트 스택에서 아이탬을 pop해주는 함수
	if (is_empty(top)) {
		return -1;
	}

	stack_pointer temp = *top;
	int item = temp->item;
	*top = temp->link;

	free(temp);
	return item;
}

void init(graph_edge* g) { //앞서 정의한 그래프를 초기화해주는 함수
    g->n = 0;

    for (int i = 0; i < MAX_SIZE; i++)
        g->adj_list[i] = NULL;
}

void insert_vertex(graph_edge* g) { //앞서 정의한 그래프에 vertex를 하나 추가해주는 함수
    if (((g->n) + 1) > MAX_SIZE) { //더이상 vertex를 추가할 수 없으면 오류 처리
        printf("Overflow");
        return;
    }

    g->n++;
}

void insert_edge(graph_edge* g, int u, int v) { //그래프의 edge를 추가해주는 함수
    if (u >= g->n || v >= g->n) { //입력받은 vertex가 그래프에 존재한 vertex에 없으면 오류 처리
        fprintf(stderr, "Index error");
        return;
    }

    graph_vertex* temp = (graph_vertex*)malloc(sizeof(graph_vertex));
    temp->vertex = v;  //그래프에 있는 u의 링크드 리스트 스택에 v를 추가해줌
    temp->link = g->adj_list[u];
    g->adj_list[u] = temp;
}

void ascending_insert(graph_vertex** head, graph_vertex* node) { //앞서 정의한 그래프의 링크드 리스트 스택에 오름차순으로 삽입해주는 함수
    graph_vertex temp;
    graph_vertex* current = &temp;
    temp.link = *head;

    while (current->link != NULL && current->link->vertex < node->vertex) {
        current = current->link;
    }

    node->link = current->link;
    current->link = node;
    *head = temp.link;
}

void ascending_sort(graph_vertex** head) { //앞서 정의한 그래프의 vertex가 갖고 있는 링크드 리스트 스택을 오름차순으로 바꿔주는 함수
    graph_vertex* temp = NULL;
    graph_vertex* current = *head;
    graph_vertex* result = NULL;

    while (current != NULL) {
        temp = current->link;
        ascending_insert(&result, current);
        current = temp;
    }
    *head = result;

    free(current);
}

void DFS(graph_edge* g, int v, int n) { //DFS로 그래프를 탐색해주는 함수
    int a, b, c;
    int visit[MAX_SIZE] = { 0 };
    stack_pointer temp = (stack_pointer)malloc(sizeof(stack));
    graph_vertex* current = NULL;

    push(&temp, v);

    c = 0;

    while (!is_empty(&temp)) {
        if (c == 0) {
            v = pop(&temp);
        }
        else {
            v = b;
        }

        if (v <= 0 || v > n) {
            break;
        }

        if (visit[v] == 0) {
            visit[v] = 1;
            printf("%d ", v);
        }

        c = 0;
        for (current = g->adj_list[v]; current != NULL; current = current->link) {
            a = current->vertex;
            if (visit[a] == 0) {
                push(&temp, a);
                if (c == 0) {
                    b = a;
                    c++;
                }
            }
        }
    }
    printf("\n");
}

int main(int argc, char argv[]) {
    int a, n, u, v;
    char direction;
    graph_edge* g = (graph_edge*)malloc(sizeof(graph_edge));
    FILE* fp = fopen("graph.txt", "r");

    init(g);

    fscanf(fp, "%d\n", &n);

    for (int i = 0; i <= n; i++)
        insert_vertex(g);

    while (fscanf(fp, "%d %d %c\n", &u, &v, &direction) != EOF) { //주어진 데이터를 입력받아 앞서 정의한 그래프에 기록해줌
        if (direction == 'u') {
            insert_edge(g, u, v);
            insert_edge(g, v, u);
        }
        else {
            insert_edge(g, u, v);
        }
    }


    for (int i = 0; i <= n; i++) //그래프의 vertex가 갖고 있는 링크드 리스트 스택을 오름차순으로 정렬해줌
        ascending_sort(&(g->adj_list[i]));

    while (1) {
        scanf("%d", &a);

        if (a == -1) {
            printf("End\n");
            break;
        }
        else if (a>0 && a<=n) { //정렬한 그래프를 DFS로 탐색해줌
            DFS(g, a, n);
        }
        else {
            printf("Index error\n");
            break;
        }
    }

    free(g);
    return 0;
}
