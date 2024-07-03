/*
 * echoserveri.c - An iterative echo server
 */
/* $begin echoserverimain */
#include "csapp.h"

typedef struct pool {
  int maxfd;
  fd_set read_set;
  fd_set ready_set;
  int nready;
  int maxi;
  int clientfd[FD_SETSIZE];
  rio_t clientrio[FD_SETSIZE];
} pool;

typedef struct stock {
  int id;
  int count;
  int price;

  struct stock *left;
  struct stock *right;
} stock;

int client_cnt = 0;
stock *root;

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);
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

          show_tree(root, buf);
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

        } else if (buf[0] == 's') {
          int id, count;
          char temp[10];

          sscanf(buf, "%s %d %d\n", temp, &id, &count);

          stock *node = find_node(root, id);

          if (node == NULL) {
            sprintf(buf, "No such stock\n");
            Rio_writen(connfd, buf, MAXLINE);

            exit(0);
          }

          node->count += count;
          sprintf(buf, "[sell] success\n");

          Rio_writen(connfd, buf, MAXLINE);
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
