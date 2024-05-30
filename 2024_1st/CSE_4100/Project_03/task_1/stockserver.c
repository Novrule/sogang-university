/*
 * echoserveri.c - An iterative echo server
 */
/* $begin echoserverimain */
#include "csapp.h"

#define STOCK_NUM 10000

typedef struct {
  int maxfd;
  fd_set read_set;
  fd_set ready_set;
  int nready;
  int maxi;
  int clientfd[FD_SETSIZE];
  rio_t clientrio[FD_SETSIZE];
} pool;

typedef struct {
  int id;
  int count;
  int price;
} stock;

int client_cnt = 0;
stock stocks[STOCK_NUM];

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);
int read_stock();
void write_stock();
void sigint_handler(int sig);

int main(int argc, char **argv) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  /* Enough space for any address */  // line:netp:echoserveri:sockaddrstorage
  char client_hostname[MAXLINE], client_port[MAXLINE];
  static pool pool;

  Signal(SIGINT, sigint_handler);

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  listenfd = Open_listenfd(argv[1]);

  if (!read_stock()) {
    exit(0);
  }

  init_pool(listenfd, &pool);

  while (1) {
    pool.ready_set = pool.read_set;
    pool.nready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

    if (FD_ISSET(listenfd, &pool.ready_set)) {
      clientlen = sizeof(struct sockaddr_storage);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE,
                  client_port, MAXLINE, 0);
      printf("Connected to (%s, %s)\n", client_hostname, client_port);
      add_client(connfd, &pool);
    }

    check_clients(&pool);
  }

  write_stock();

  exit(0);
}
/* $end echoserverimain */

void init_pool(int listenfd, pool *p) {
  p->maxi = -1;

  for (int i = 0; i < FD_SETSIZE; i++) {
    p->clientfd[i] = -1;
  }

  p->maxfd = listenfd;
  FD_ZERO(&p->read_set);
  FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool *p) {
  int i;

  p->nready--;

  for (i = 0; i < FD_SETSIZE; i++) {
    if (p->clientfd[i] < 0) {
      p->clientfd[i] = connfd;
      Rio_readinitb(&p->clientrio[i], connfd);

      FD_SET(connfd, &p->read_set);

      if (connfd > p->maxfd) {
        p->maxfd = connfd;
      }

      if (i > p->maxi) {
        p->maxi = i;
      }

      client_cnt++;

      break;
    }
  }

  if (i == FD_SETSIZE) {
    app_error("add_client error: Too many clients");
  }
}

void check_clients(pool *p) {
  int connfd, n;
  char buf[MAXLINE];
  rio_t rio;

  for (int i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
    connfd = p->clientfd[i];
    rio = p->clientrio[i];

    if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
      p->nready--;

      if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("Server received %d bytes on fd %d\n", n, connfd);

        if (!strcmp(buf, "show\n")) {
          memset(buf, 0, MAXLINE);

          for (int j = 0; j < STOCK_NUM; j++) {
            char temp[MAXLINE];

            if (stocks[j].id == -1) {
              break;
            }

            sprintf(temp, "%d %d %d\n", stocks[j].id, stocks[j].count,
                    stocks[j].price);
            strcat(buf, temp);
          }

          Rio_writen(connfd, buf, MAXLINE);
        } else if (!strcmp(buf, "exit\n")) {
          Rio_writen(connfd, "exit\n", MAXLINE);

          Close(connfd);
          FD_CLR(connfd, &p->read_set);
          p->clientfd[i] = -1;

          client_cnt--;
          if (client_cnt == 0) {
            write_stock();
          }
        } else if (buf[0] == 'b') {
          int id, count;
          char temp[10];

          sscanf(buf, "%s %d %d\n", temp, &id, &count);

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
        } else if (buf[0] == 's') {
          int id, count;
          char temp[10];

          sscanf(buf, "%s %d %d\n", temp, &id, &count);

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
        }
      } else {
        Close(connfd);
        FD_CLR(connfd, &p->read_set);
        p->clientfd[i] = -1;

        client_cnt--;
        if (client_cnt == 0) {
          write_stock();
        }
      }
    }
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
