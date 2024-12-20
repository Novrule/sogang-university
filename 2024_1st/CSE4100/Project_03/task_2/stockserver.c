/*
 * echoserveri.c - An iterative echo server
 */
/* $begin echoserverimain */
#include "csapp.h"

#define NTHREADS 128
#define SBUFSIZE 1024
#define STOCK_NUM 10000

typedef struct sbuf_t {
  int *buf;
  int n;
  int front;
  int rear;
  sem_t mutex;
  sem_t slots;
  sem_t items;
} sbuf_t;

typedef struct stock {
  int id;
  int count;
  int price;

  struct stock *left;
  struct stock *right;
} stock;

sbuf_t sbuf;

int client_cnt = 0;
stock *root;
int readcnt;
sem_t mutex, w;

void sbuf_init(sbuf_t *sp, int n);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);

void init_stock_server();
void stock_server(int connfd);
void *thread(void *vargp);
void show_tree(stock *node, char *buf);
stock *find_node(stock *node, int id);
int read_stock();
void write_stock();
void sigint_handler(int sig);

int main(int argc, char **argv) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  /* Enough space for any address */  // line:netp:echoserveri:sockaddrstorage
  char client_hostname[MAXLINE], client_port[MAXLINE];
  pthread_t tid;

  Signal(SIGINT, sigint_handler);

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  listenfd = Open_listenfd(argv[1]);

  if (!read_stock()) {
    exit(0);
  }

  sbuf_init(&sbuf, SBUFSIZE);

  for (int i = 0; i < NTHREADS; i++) {
    Pthread_create(&tid, NULL, thread, NULL);
  }

  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE,
                client_port, MAXLINE, 0);
    printf("Connected to (%s, %s)\n", client_hostname, client_port);
    sbuf_insert(&sbuf, connfd);
  }

  write_stock();

  exit(0);
}
/* $end echoserverimain */

void sbuf_init(sbuf_t *sp, int n) {
  sp->buf = Calloc(n, sizeof(int));
  sp->n = n;
  sp->front = sp->rear = 0;

  Sem_init(&sp->mutex, 0, 1);
  Sem_init(&sp->slots, 0, n);
  Sem_init(&sp->items, 0, 0);
}

void sbuf_insert(sbuf_t *sp, int item) {
  P(&sp->slots);
  P(&sp->mutex);
  sp->buf[(++sp->rear) % (sp->n)] = item;
  V(&sp->mutex);
  V(&sp->items);
}

int sbuf_remove(sbuf_t *sp) {
  int item;

  P(&sp->items);
  P(&sp->mutex);
  item = sp->buf[(++sp->front) % (sp->n)];
  V(&sp->mutex);
  V(&sp->slots);

  return item;
}

void init_stock_server() {
  readcnt = 0;

  Sem_init(&mutex, 0, 1);
  Sem_init(&w, 0, 1);
}

void stock_server(int connfd) {
  int n;
  char buf[MAXLINE];
  rio_t rio;
  static pthread_once_t once = PTHREAD_ONCE_INIT;

  client_cnt++;

  pthread_once(&once, init_stock_server);
  Rio_readinitb(&rio, connfd);

  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("Server received %d bytes on fd %d\n", n, connfd);

    if (!strcmp(buf, "show\n")) {
      memset(buf, 0, MAXLINE);

      P(&mutex);
      readcnt++;
      if (readcnt == 1) {
        P(&w);
      }
      V(&mutex);

      show_tree(root, buf);

      P(&mutex);
      readcnt--;
      if (readcnt == 0) {
        V(&w);
      }
      V(&mutex);

      Rio_writen(connfd, buf, MAXLINE);
    } else if (!strcmp(buf, "exit\n")) {
      Rio_writen(connfd, "exit\n", MAXLINE);

      break;
    } else if (buf[0] == 'b') {
      int id, count;
      char temp[10];

      sscanf(buf, "%s %d %d\n", temp, &id, &count);

      P(&w);

      stock *node = find_node(root, id);

      if (node == NULL) {
        sprintf(buf, "No such stock\n");
        Rio_writen(connfd, buf, MAXLINE);

        exit(0);
      }

      if (node->count >= count) {
        node->count -= count;
        sprintf(buf, "[buy] success\n");
      } else {
        sprintf(buf, "Not enough left stock\n");
      }

      Rio_writen(connfd, buf, MAXLINE);

      V(&w);
    } else if (buf[0] == 's') {
      int id, count;
      char temp[10];

      sscanf(buf, "%s %d %d\n", temp, &id, &count);

      P(&w);

      stock *node = find_node(root, id);

      if (node == NULL) {
        sprintf(buf, "No such stock\n");
        Rio_writen(connfd, buf, MAXLINE);

        exit(0);
      }

      node->count += count;
      sprintf(buf, "[sell] success\n");

      Rio_writen(connfd, buf, MAXLINE);

      V(&w);
    }
  }

  client_cnt--;
  if (client_cnt == 0) {
    write_stock();
  }
}

void *thread(void *vargp) {
  Pthread_detach(pthread_self());

  while (1) {
    int connfd = sbuf_remove(&sbuf);

    stock_server(connfd);
    Close(connfd);
  }
}

void show_tree(stock *node, char *buf) {
  if (node != NULL) {
    char temp[MAXLINE];

    show_tree(node->left, buf);
    sprintf(temp, "%d %d %d\n", node->id, node->count, node->price);
    strcat(buf, temp);
    show_tree(node->right, buf);
  }
}

stock *find_node(stock *node, int id) {
  if (node == NULL) {
    return NULL;
  } else if (node->id == id) {
    return node;
  } else if (id < node->id) {
    return find_node(node->left, id);
  } else {
    return find_node(node->right, id);
  }
}

stock *add_tree(stock *node, int id, int count, int price) {
  if (node == NULL) {
    node = (stock *)malloc(sizeof(stock));
    node->id = id;
    node->count = count;
    node->price = price;
    node->left = NULL;
    node->right = NULL;
  } else {
    if (id < node->id) {
      node->left = add_tree(node->left, id, count, price);
    } else {
      node->right = add_tree(node->right, id, count, price);
    }
  }

  return node;
}

int read_stock() {
  FILE *fp = fopen("stock.txt", "r");

  if (fp != NULL) {
    int stock_id, stock_count, stock_price;

    while (EOF !=
           fscanf(fp, "%d %d %d\n", &stock_id, &stock_count, &stock_price)) {
      root = add_tree(root, stock_id, stock_count, stock_price);
    }

    fclose(fp);

    return 1;
  } else {
    return 0;
  }
}

void print_tree(stock *node, FILE *fp) {
  if (node != NULL) {
    print_tree(node->left, fp);
    fprintf(fp, "%d %d %d\n", node->id, node->count, node->price);
    print_tree(node->right, fp);
  }
}

void write_stock() {
  FILE *fp = fopen("stock.txt", "w");

  if (fp != NULL) {
    print_tree(root, fp);

    fclose(fp);
  }
}

void sigint_handler(int sig) {
  write_stock();

  exit(0);
}
