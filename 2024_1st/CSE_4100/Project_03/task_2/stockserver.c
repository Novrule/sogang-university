/*
 * echoserveri.c - An iterative echo server
 */
/* $begin echoserverimain */
#include "csapp.h"

#define NTHREADS 128
#define SBUFSIZE 1024
#define STOCK_NUM 10000

typedef struct {
  int *buf;
  int n;
  int front;
  int rear;
  sem_t mutex;
  sem_t slots;
  sem_t items;
} sbuf_t;

typedef struct {
  int id;
  int count;
  int price;
} stock;

sbuf_t sbuf;

int client_cnt = 0;
int readcnt;
stock stocks[STOCK_NUM];
sem_t mutex, w;

void sbuf_init(sbuf_t *sp, int n);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);

void init_stock_server();
void stock_server(int connfd);
void *thread(void *vargp);
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

      for (int j = 0; j < STOCK_NUM; j++) {
        char temp[MAXLINE];

        if (stocks[j].id == -1) {
          break;
        }

        sprintf(temp, "%d %d %d\n", stocks[j].id, stocks[j].count,
                stocks[j].price);
        strcat(buf, temp);
      }

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

      for (int j = 0; j < STOCK_NUM; j++) {
        if (stocks[j].id == -1) {
          break;
        }

        if (stocks[j].id == id) {
          if (stocks[j].count >= count) {
            stocks[j].count -= count;
            sprintf(buf, "[buy] success\n");
          } else {
            sprintf(buf, "Not enough left stock\n");
          }
          Rio_writen(connfd, buf, MAXLINE);

          break;
        }
      }

      V(&w);
    } else if (buf[0] == 's') {
      int id, count;
      char temp[10];

      sscanf(buf, "%s %d %d\n", temp, &id, &count);

      P(&w);

      for (int j = 0; j < STOCK_NUM; j++) {
        if (stocks[j].id == -1) {
          break;
        }

        if (stocks[j].id == id) {
          stocks[j].count += count;
          sprintf(buf, "[sell] success\n");
          Rio_writen(connfd, buf, MAXLINE);

          break;
        }
      }

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

int read_stock() {
  int flag = 0;
  FILE *fp = fopen("stock.txt", "r");

  if (fp != NULL) {
    int i = 0;
    int stock_id, stock_count, stock_price;

    flag = 1;

    while (EOF !=
           fscanf(fp, "%d %d %d\n", &stock_id, &stock_count, &stock_price)) {
      stocks[i].id = stock_id;
      stocks[i].count = stock_count;
      stocks[i].price = stock_price;
      i++;
    }

    fclose(fp);

    for (int j = i; j < STOCK_NUM; j++) {
      stocks[j].id = -1;
    }
  }

  return flag;
}

void write_stock() {
  FILE *fp = fopen("stock.txt", "w");

  if (fp != NULL) {
    for (int i = 0; i < STOCK_NUM; i++) {
      if (stocks[i].id == -1) {
        break;
      }

      fprintf(fp, "%d %d %d\n", stocks[i].id, stocks[i].count, stocks[i].price);
    }

    fclose(fp);
  }
}

void sigint_handler(int sig) {
  write_stock();

  exit(0);
}
