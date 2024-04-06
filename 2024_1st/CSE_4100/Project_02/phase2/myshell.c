/* $begin shellmain */
#include <errno.h>

#include "csapp.h"
#define MAXARGS 128

/* Global varialbes */
int pipe_flag; /* 파이프 사용 여부 */

/* Function prototypes */
void eval(char *cmdline);
int builtin_command(char **argv, int argc);
int parseline(char *buf, char **argv, int *argc);

void split_pipe(char *argv[], char *cmd1[], char *cmd2[]);
void excute_pipe(char *cmd1[], char *cmd2[]);
void child_handler(int sig);

int main(void) {
  char cmdline[MAXLINE]; /* Command line */

  Signal(SIGCHLD, child_handler);  // 자식 프로세스가 종료되면 핸들러로 처리

  while (1) {
    pipe_flag = 0;  // 파이프 사용 여부 초기화

    /* Read */
    printf("CSE4100-SP-P2> ");

    char *temp = fgets(cmdline, MAXLINE, stdin);

    if (feof(stdin)) {
      exit(0);
    }

    /* Evaluate */
    eval(cmdline);
  }

  return 0;
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) {
  int bg;                  /* Should the job run in bg or fg? */
  int argc = 0;            /* args의 개수 */
  int single_cnt = 0;      /* '의 개수 */
  int double_cnt = 0;      /* "의 개수 */
  int back_cnt = 0;        /* `의 개수 */
  int single_idx[MAXARGS]; /* '의 위치 */
  int double_idx[MAXARGS]; /* "의 위치 */
  int back_idx[MAXARGS];   /* `의 위치 */
  char buf[MAXLINE];       /* Holds modified command line */
  char *argv[MAXARGS];     /* Argument list execve() */
  pid_t pid;               /* Process id */

  // 명령어에 ', ", `이 있는지 확인하고 위치를 저장
  for (int i = 0; i < strlen(cmdline); i++) {
    if (cmdline[i] == '|') {
      pipe_flag = 1;
    } else if (cmdline[i] == '\'') {
      single_idx[single_cnt++] = i;
    } else if (cmdline[i] == '\"') {
      double_idx[double_cnt++] = i;
    } else if (cmdline[i] == '`') {
      back_idx[back_cnt++] = i;
    }
  }

  // 명령어에 ', ", `이 있다면 제거
  if ((single_cnt % 2) == 0 && single_cnt != 0) {
    for (int i = 0; i < single_cnt; i++) {
      cmdline[single_idx[i]] = ' ';
    }
  }

  if ((double_cnt % 2) == 0 && double_cnt != 0) {
    for (int i = 0; i < double_cnt; i++) {
      cmdline[double_idx[i]] = ' ';
    }
  }

  if ((back_cnt % 2) == 0 && back_cnt != 0) {
    pipe_flag = 1;  // 파이프를 사용하는 것이므로 flag를 1(=true)로 설정

    for (int i = 0; i < back_cnt; i += 2) {
      cmdline[back_idx[i]] = '|';  // 첫 번째 `를 |로 변경

      for (int j = back_idx[i + 1] - 1; j > back_idx[i];
           j--) {  // ` 사이 명령어를 한 칸씩 뒤로 이동
        cmdline[j + 1] = cmdline[j];
      }

      cmdline[back_idx[i] + 1] = ' ';  // | 뒤에 공백 추가
    }
  }

  strcpy(buf, cmdline);
  bg = parseline(buf, argv, &argc);

  if (argv[0] == NULL) { /* Ignore empty lines */
    return;
  }

  if (!builtin_command(argv,
                       argc)) {  // quit -> exit(0), & -> ignore, other -> run
    char path[100] = "/bin/";    // 기본 디렉토리 경로

    strcat(path, argv[0]);  // 기본 디렉토리 경로에 명령어를 붙임

    if (pid = Fork() == 0) {  // 자식 프로세스인 경우
      if (pipe_flag == 1) {   // 파이프를 사용하는 경우
        char *cmd1[50] = {
            0,
        };
        char *cmd2[50] = {
            0,
        };

        split_pipe(argv, cmd1, cmd2);  // 파이프를 기준으로 명령어를 나눔
        excute_pipe(cmd1, cmd2);  // 나뉘어진 명령어 실행
      } else if (execve(path, argv, environ) < 0) {  // ex) /bin/ls ls -al &
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
      }
    } else {  // 부모 프로세스인 경우
      /* Parent waits for foreground job to terminate */
      if (!bg) {
        int status;

        Wait(&status);
      } else {  // when there is backgrount process!
        printf("%d %s", pid, cmdline);
      }
    }
  }

  return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv, int argc) {
  if (!strcmp(argv[0], "quit") || !strcmp(argv[0], "exit")) { /* quit command */
    exit(0);
  }

  if (!strcmp(argv[0], "&")) { /* Ignore singleton & */
    return 1;
  }

  if (!strcmp(argv[0], "cd")) { /* cd command */
    char *path;                 // 디렉토리의 경로

    if (argc == 1) {          // cd만 입력한 경우
      path = getenv("HOME");  // 디렉토리의 경로를 환경변수에서 가져옴

      if (path == NULL) {  // 환경변수에 HOME이 없는 경우
        path = ".";        // 현재 디렉토리로 설정
      }
    } else {           // cd 뒤에 디렉토리 경로가 있는 경우
      path = argv[1];  // 디렉토리 경로로 설정
    }

    if (chdir(path) != 0) {  // 디렉토리 변경에 실패한 경우
      printf("Invalid path : %s\n", path);  // 에러 메시지 출력
    }

    return 1;
  }

  return 0; /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv, int *argc) {
  char *delim; /* Points to first space delimiter */
  int bg;      /* Background job? */

  buf[strlen(buf) - 1] = ' '; /* Replace trailing '\n' with space */

  while (*buf && (*buf == ' ')) { /* Ignore leading spaces */
    buf++;
  }

  /* Build the argv list */
  *argc = 0;
  while ((delim = strchr(buf, ' '))) {
    argv[(*argc)++] = buf;
    *delim = '\0';
    buf = delim + 1;

    while (*buf && (*buf == ' ')) { /* Ignore spaces */
      buf++;
    }
  }

  argv[*argc] = NULL;

  if (*argc == 0) { /* Ignore blank line */
    return 1;
  }

  /* Should the job run in the background? */
  if ((bg = (*argv[*argc - 1] == '&')) != 0) {
    argv[--(*argc)] = NULL;
  }

  return bg;
}
/* $end parseline */

void split_pipe(char *argv[], char *cmd1[],
                char *cmd2[]) {  // 파이프를 기준으로 명령어를 나눔

  int idx;

  for (int i = 0; argv[i] != NULL; i++) {  // 파이프의 앞 부분을 cmd1에 저장
    if (!strcmp(argv[i], "|")) {           // 파이프를 만난 경우
      idx = i + 1;                         // 파이프의 위치를 저장

      break;
    }

    cmd1[i] = argv[i];
    cmd1[i + 1] = NULL;
  }

  for (int i = 0; argv[i] != NULL; i++) {  // 파이프의 뒷 부분을 cmd2에 저장
    cmd2[i] = argv[idx + i];
    cmd2[i + 1] = NULL;
  }

  return;

  /*
  int i = 0;
  int j = 0;

  while (strcmp(argv[i], "|") &&
         argv[i] != NULL) {  // 파이프를 기준으로 앞부분을 cmd1에 저장
    cmd1[i] = argv[i];
    i++;
  }

  cmd1[i++] = NULL;

  while (argv[i] != NULL) {  // 파이프를 기준으로 뒷부분을 cmd2에 저장
    cmd2[j] = argv[i];
    i++;
    j++;
  }

  cmd2[j] = NULL;

  return;
  */
}

void excute_pipe(char *cmd1[],
                 char *cmd2[]) {  // 파이프를 기준으로 나뉘어진 명령어 실행
  int fd[2];

  if (pipe(fd) == -1) {  // 파이프 생성에 실패한 경우
    printf("Create pipe error\n");
    exit(1);
  }

  if (Fork() == 0) {             // 자식 프로세스인 경우
    dup2(fd[1], STDOUT_FILENO);  // 표준 출력 변경
    close(fd[0]);                // 파이프의 쓰기 닫기

    if (execvp(cmd1[0], cmd1) == -1) {  // 첫 번째 명령어 실행에 실패한 경우
      printf("First execvp error\n");
      exit(1);
    }
  } else {  // 부모 프로세스인 경우
    char *temp1[50] = {
        0,
    };
    char *temp2[50] = {
        0,
    };

    dup2(fd[0], STDIN_FILENO);  // 표준 입력 변경
    close(fd[1]);               // 파이프의 읽기 닫기

    split_pipe(cmd2, temp1, temp2);  // 두 번째 명령어를 다시 나눔

    if (temp2[0] == NULL) {  // 두 번째 명령어가 없는 경우
      if (execvp(cmd2[0], cmd2) == -1) {  // 두 번째 명령어 실행에 실패한 경우
        printf("Second execvp error\n");
        exit(1);
      }
    } else {                      // 두 번째 명령어가 있는 경우
      excute_pipe(temp1, temp2);  // 재귀적으로 파이프 실행
    }
  }

  return;
}

void child_handler(int sig) {  // 자식 프로세스의 종료를 처리
  int status;

  pid_t id = waitpid(-1, &status, WNOHANG);  // 자식 프로세스의 종료를 기다림

  return;
}